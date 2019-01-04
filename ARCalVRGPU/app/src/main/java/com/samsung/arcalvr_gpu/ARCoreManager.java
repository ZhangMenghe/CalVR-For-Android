package com.samsung.arcalvr_gpu;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.media.Image;
import android.support.v8.renderscript.Allocation;
import android.support.v8.renderscript.Element;
import android.support.v8.renderscript.RenderScript;
import android.support.v8.renderscript.Type;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.ImageView;

import com.google.ar.core.Anchor;
import com.google.ar.core.ArCoreApk;
import com.google.ar.core.Camera;
import com.google.ar.core.Config;
import com.google.ar.core.Frame;
import com.google.ar.core.HitResult;
import com.google.ar.core.Plane;
import com.google.ar.core.Point;
import com.google.ar.core.PointCloud;
import com.google.ar.core.Session;
import com.google.ar.core.Trackable;
import com.google.ar.core.TrackingState;
import com.google.ar.core.exceptions.CameraNotAvailableException;
import com.google.ar.core.exceptions.NotYetAvailableException;
import com.google.ar.core.exceptions.UnavailableApkTooOldException;
import com.google.ar.core.exceptions.UnavailableArcoreNotInstalledException;
import com.google.ar.core.exceptions.UnavailableDeviceNotCompatibleException;
import com.google.ar.core.exceptions.UnavailableSdkTooOldException;
import com.google.ar.core.exceptions.UnavailableUserDeclinedInstallationException;
import com.samsung.arcalvr_gpu.helpers.CameraPermissionHelper;
import com.samsung.arcalvr_gpu.helpers.DisplayRotationHelper;
import com.samsung.arcalvr_gpu.helpers.TapHelper;
import com.samsung.arcalvr_gpu.rendering.BackgroundRenderer;
import com.samsung.arcalvr_gpu.rendering.PlaneRenderer;
import com.samsung.arcalvr_gpu.rendering.PointCloudRenderer;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import static org.opencv.imgproc.Imgproc.cvtColor;

//import com.samsung.arcalvr_gpu.ScriptC_yuv2rgb;

public class ARCoreManager {
    private static final String TAG = "ARCore Manager Java side";
    private Context context;
    private boolean installRequested;
    private Session session;
    private TapHelper tapHelper;
    private DisplayRotationHelper displayRotationHelper;
    private int _frameCount = 0;

    private final BackgroundRenderer backgroundRenderer = new BackgroundRenderer();

    private final PlaneRenderer planeRenderer = new PlaneRenderer();
    private final PointCloudRenderer pointCloudRenderer = new PointCloudRenderer();

    // Temporary matrix allocated here to reduce number of allocations for each frame.
    private final float[] anchorMatrix = new float[16];
    private static final float[] DEFAULT_COLOR = new float[] {0f, 0f, 0f, 0f};

    // This lock prevents changing resolution as the frame is being rendered. ARCore requires all
    // cpu images to be released before changing resolution.
    private final Object frameImageInUseLock = new Object();

    // Used to load the 'native-lib' library on application startup.
    // Anchors created from taps used for object placing with a given color.
    private static class ColoredAnchor {
        public final Anchor anchor;
        public final float[] color;

        public ColoredAnchor(Anchor a, float[] color4f) {
            this.anchor = a;
            this.color = color4f;
        }
    }
    // TWO approaches to obtaining image data accessible on CPU:
    // 1. Access the CPU image directly from ARCore. This approach delivers a frame without latency
    //    (if available), but currently is lower resolution than the GPU image.
    // 2. Download the texture from GPU. This approach incurs a 1-frame latency, but allows a high
    //    resolution image.
    private enum ImageAcquisitionPath {
        CPU_DIRECT_ACCESS,
        GPU_DOWNLOAD
    };

    private final ImageAcquisitionPath imageAcquisitionPath = ImageAcquisitionPath.CPU_DIRECT_ACCESS;

    private final ArrayList<ColoredAnchor> anchors = new ArrayList<>();

    RenderScript rs;
    ScriptC_yuv2rgb script;

    public void onCreate(Context ctx){
        context = ctx;
        displayRotationHelper = new DisplayRotationHelper(/*context=*/ context);
        installRequested = false;
        // Set up tap listener.
        tapHelper = new TapHelper(/*context=*/ context);

        rs = RenderScript.create(context);
        script = new ScriptC_yuv2rgb(rs);
    }
    public void onResume(Activity activity){
        if (session == null) {
            Exception exception = null;
            String message = null;
            try {
                switch (ArCoreApk.getInstance().requestInstall(activity, !installRequested)) {
                    case INSTALL_REQUESTED:
                        installRequested = true;
                        return;
                    case INSTALLED:
                        break;
                }

                // ARCore requires camera permissions to operate. If we did not yet obtain runtime
                // permission on Android M and above, now is a good time to ask the user for it.
                if (!CameraPermissionHelper.hasCameraPermission(activity)) {
                    CameraPermissionHelper.requestCameraPermission(activity);
                    return;
                }

                // Create the session.
                session = new Session(/* context= */ context);

            } catch (UnavailableArcoreNotInstalledException
                    | UnavailableUserDeclinedInstallationException e) {
                message = "Please install ARCore";
                exception = e;
            } catch (UnavailableApkTooOldException e) {
                message = "Please update ARCore";
                exception = e;
            } catch (UnavailableSdkTooOldException e) {
                message = "Please update this app";
                exception = e;
            } catch (UnavailableDeviceNotCompatibleException e) {
                message = "This device does not support AR";
                exception = e;
            } catch (Exception e) {
                message = "Failed to create AR session";
                exception = e;
            }

            if (message != null) {
                Log.e(TAG, "Exception creating session", exception);
                return;
            }
        }

        // Note that order matters - see the note in onPause(), the reverse applies here.
        try {
            session.resume();
            Config config = new Config(session);
            config.setFocusMode(Config.FocusMode.AUTO);
            config.setUpdateMode(Config.UpdateMode.LATEST_CAMERA_IMAGE);
            session.configure(config);
        } catch (CameraNotAvailableException e) {
            session = null;
            return;
        }
        displayRotationHelper.onResume();
    }
    public void onPause(){
        if (session != null) {
            // Note that the order matters - GLSurfaceView is paused first so that it does not try
            // to query the session. If Session is paused before GLSurfaceView, GLSurfaceView may
            // still call session.update() and get a SessionPausedException.
            displayRotationHelper.onPause();
            session.pause();
        }
    }
    public void onDestroy(){
        script.destroy();
    }
    public void onSurfaceCreated(){
        // Prepare the rendering objects. This involves reading shaders, so may throw an IOException.
        try {
            // Create the texture and pass it to ARCore session to be filled during update().
            backgroundRenderer.createOnGlThread(/*context=*/ context);
            planeRenderer.createOnGlThread(/*context=*/ context, "models/trigrid.png");
            pointCloudRenderer.createOnGlThread(/*context=*/ context);

        } catch (IOException e) {
            Log.e(TAG, "Failed to read an asset file", e);
        }
    }
    public void onSurfaceChanged(int width, int height) {displayRotationHelper.onSurfaceChanged(width, height);}

    public void onDrawFrame(){
        if (session == null) {
            return;
        }
        _frameCount++;
        // Notify ARCore session that the view size changed so that the perspective matrix and
        // the video background can be properly adjusted.
        displayRotationHelper.updateSessionIfNeeded(session);

        try {
            session.setCameraTextureName(backgroundRenderer.getTextureId());

            // Obtain the current frame from ARSession. When the configuration is set to
            // UpdateMode.BLOCKING (it is by default), this will throttle the rendering to the
            // camera framerate.
            Frame frame = session.update();
            if(_frameCount %50 == 0){
                switch (imageAcquisitionPath) {
                    case CPU_DIRECT_ACCESS:
                        renderProcessedImageCpuDirectAccess(frame);
                        break;
                    case GPU_DOWNLOAD:
//                    renderProcessedImageGpuDownload(frame);
                        break;
                }
            }

            Camera camera = frame.getCamera();

            // Handle one tap per frame.
            handleTap(frame, camera);

            // Draw background.
            backgroundRenderer.draw(frame);

            // If not tracking, don't draw 3d objects.
            if (camera.getTrackingState() == TrackingState.PAUSED) {
                return;
            }

            // Get projection matrix.
            float[] projmtx = new float[16];
            camera.getProjectionMatrix(projmtx, 0, 0.1f, 100.0f);

            // Get camera matrix and draw.
            float[] viewmtx = new float[16];
            camera.getViewMatrix(viewmtx, 0);

            // Compute lighting from average intensity of the image.
            // The first three components are color scaling factors.
            // The last one is the average pixel intensity in gamma space.
            final float[] colorCorrectionRgba = new float[4];
            frame.getLightEstimate().getColorCorrection(colorCorrectionRgba, 0);

            // Visualize tracked points.
            PointCloud pointCloud = frame.acquirePointCloud();
            pointCloudRenderer.update(pointCloud);
            pointCloudRenderer.draw(viewmtx, projmtx);

            // Application is responsible for releasing the point cloud resources after
            // using it.
            pointCloud.release();
            // Visualize planes.
            planeRenderer.drawPlanes(
                    session.getAllTrackables(Plane.class), camera.getDisplayOrientedPose(), projmtx);

            // Visualize anchors created by touch.
            float scaleFactor = 1.0f;
            for (ColoredAnchor coloredAnchor : anchors) {
                if (coloredAnchor.anchor.getTrackingState() != TrackingState.TRACKING) {
                    continue;
                }
                // Get the current pose of an Anchor in world space. The Anchor pose is updated
                // during calls to session.update() as ARCore refines its estimate of the world.
                coloredAnchor.anchor.getPose().toMatrix(anchorMatrix, 0);
            }

        } catch (Throwable t) {
            // Avoid crashing the application due to unhandled exceptions.
            Log.e(TAG, "Exception on the OpenGL thread", t);
        }
    }
    public TapHelper getTapListener(){return tapHelper;}
    // Handle only one tap per frame, as taps are usually low frequency compared to frame rate.
    private void handleTap(Frame frame, Camera camera) {
        MotionEvent tap = tapHelper.poll();
        if (tap != null && camera.getTrackingState() == TrackingState.TRACKING) {
            for (HitResult hit : frame.hitTest(tap)) {
                // Check if any plane was hit, and if it was hit inside the plane polygon
                Trackable trackable = hit.getTrackable();
                // Creates an anchor if a plane or an oriented point was hit.
                if ((trackable instanceof Plane
                        && ((Plane) trackable).isPoseInPolygon(hit.getHitPose())
                        && (PlaneRenderer.calculateDistanceToPlane(hit.getHitPose(), camera.getPose()) > 0))
                        || (trackable instanceof Point
                        && ((Point) trackable).getOrientationMode()
                        == Point.OrientationMode.ESTIMATED_SURFACE_NORMAL)) {
                    // Hits are sorted by depth. Consider only closest hit on a plane or oriented point.
                    // Cap the number of objects created. This avoids overloading both the
                    // rendering system and ARCore.
                    if (anchors.size() >= 20) {
                        anchors.get(0).anchor.detach();
                        anchors.remove(0);
                    }

                    // Assign a color to the object for rendering based on the trackable type
                    // this anchor attached to. For AR_TRACKABLE_POINT, it's blue color, and
                    // for AR_TRACKABLE_PLANE, it's green color.
                    float[] objColor;
                    if (trackable instanceof Point) {
                        objColor = new float[] {66.0f, 133.0f, 244.0f, 255.0f};
                    } else if (trackable instanceof Plane) {
                        objColor = new float[] {139.0f, 195.0f, 74.0f, 255.0f};
                    } else {
                        objColor = DEFAULT_COLOR;
                    }

                    // Adding an Anchor tells ARCore that it should track this position in
                    // space. This anchor is created on the Plane to place the 3D model
                    // in the correct position relative both to the world and to the plane.
                    anchors.add(new ColoredAnchor(hit.createAnchor(), objColor));
                    break;
                }
            }
        }
    }

    private void renderProcessedImageCpuDirectAccess(Frame frame) {
        // Lock the image use to avoid pausing & resuming session when the image is in use. This is
        // because switching resolutions requires all images to be released before session.resume() is
        // called.
        synchronized (frameImageInUseLock) {
            try (Image image = frame.acquireCameraImage()) {
                if (image.getFormat() != ImageFormat.YUV_420_888) {
                    throw new IllegalArgumentException(
                            "Expected image in YUV_420_888 format, got format " + image.getFormat());
                }
                ConvertToRGBImage(image);
            } catch (NotYetAvailableException e) {
                Log.e(TAG, "Fail to get image from current frame");
            }
        }
    }
    private void ConvertToRGBImage(Image image){
        byte[] yuv = YUV_420_888toNV21(image);
        Mat mRGB = getYUV2Mat(yuv, image.getWidth(), image.getHeight());
        JNIPassSingleFrame(mRGB.getNativeObjAddr());
//        DEBUG-VIEW: SEE RUNTIME BACKGROUND IMAGE
//        Bitmap debugBitmap = Bitmap.createBitmap(currentImg.width(), currentImg.height(), Bitmap.Config.ARGB_8888);
//        org.opencv.android.Utils.matToBitmap(currentImg, debugBitmap);
//        MainActivity.imgView.setImageBitmap(debugBitmap);

//        Mat mYuvMat = imageToMat(image);

//        int imageWidth  = image.getWidth();
//        int imageHeight = image.getHeight();
//
//        Type.Builder yuvBlder = new Type.Builder(rs, Element.U8(rs))
//                .setX(imageWidth).setY(imageHeight*3/2);
//        Allocation allocIn = Allocation.createTyped(rs,yuvBlder.create(),Allocation.USAGE_SCRIPT);
//        Type rgbType = Type.createXY(rs, Element.RGBA_8888(rs), imageWidth, imageHeight);
//        Allocation allocOut = Allocation.createTyped(rs,rgbType,Allocation.USAGE_SCRIPT);
//
//        allocIn.copyFrom(yuv);
//        script.set_gW(imageWidth);
//        script.set_gH(imageHeight);
//        script.set_gYUV(allocIn);
//        script.forEach_YUV2RGB(allocOut);
//        allocIn.destroy();
//
//        currentBmp = Bitmap.createBitmap(imageWidth, imageHeight, Bitmap.Config.ARGB_8888);
//        allocIn.copyTo(currentBmp);
//        currentImg= new Mat(imageHeight, imageWidth, CvType.CV_8UC1);
//        Utils.bitmapToMat(currentBmp, currentImg);

    }
    private static byte[] YUV_420_888toNV21(Image image) {
        byte[] nv21;
        ByteBuffer yBuffer = image.getPlanes()[0].getBuffer();
        ByteBuffer uBuffer = image.getPlanes()[1].getBuffer();
        ByteBuffer vBuffer = image.getPlanes()[2].getBuffer();

        int ySize = yBuffer.remaining();
        int uSize = uBuffer.remaining();
        int vSize = vBuffer.remaining();

        nv21 = new byte[ySize + uSize + vSize];

        //U and V are swapped
        yBuffer.get(nv21, 0, ySize);
        vBuffer.get(nv21, ySize, vSize);
        uBuffer.get(nv21, ySize + vSize, uSize);
        return nv21;
    }

    public Mat getYUV2Mat(byte[] data, int width, int height) {
        Mat mYuv = new Mat(height + height / 2, width, CvType.CV_8UC1);
        mYuv.put(0, 0, data);
        Mat mRGB = new Mat();
        cvtColor(mYuv, mRGB, Imgproc.COLOR_YUV2RGB_NV21, 3);
        return mRGB;
    }

    public static native void JNIPassSingleFrame(long matAddr);

    //    private void renderProcessedImageGpuDownload(Frame frame) {
//        // If there is a frame being requested previously, acquire the pixels and process it.
//        if (gpuDownloadFrameBufferIndex >= 0) {
//            TextureReaderImage image = textureReader.acquireFrame(gpuDownloadFrameBufferIndex);
//
//            if (image.format != TextureReaderImage.IMAGE_FORMAT_I8) {
//                throw new IllegalArgumentException(
//                        "Expected image in I8 format, got format " + image.format);
//            }
//
//            ByteBuffer processedImageBytesGrayscale =
//                    edgeDetector.detect(image.width, image.height, /* stride= */ image.width, image.buffer);
//
//            // You should always release frame buffer after using. Otherwise the next call to
//            // submitFrame() may fail.
//            textureReader.releaseFrame(gpuDownloadFrameBufferIndex);
//
//            cpuImageRenderer.drawWithCpuImage(
//                    frame,
//                    IMAGE_WIDTH,
//                    IMAGE_HEIGHT,
//                    processedImageBytesGrayscale,
//                    cpuImageDisplayRotationHelper.getViewportAspectRatio(),
//                    cpuImageDisplayRotationHelper.getCameraToDisplayRotation());
//
//        } else {
//            cpuImageRenderer.drawWithoutCpuImage();
//        }
//
//        // Submit request for the texture from the current frame.
//        gpuDownloadFrameBufferIndex =
//                textureReader.submitFrame(cpuImageRenderer.getTextureId(), TEXTURE_WIDTH, TEXTURE_HEIGHT);
//    }


}
