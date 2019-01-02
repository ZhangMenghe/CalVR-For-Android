package com.samsung.arcalvr;

import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Size;
import android.util.SizeF;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

import java.io.File;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity
    implements DisplayManager.DisplayListener,
    CVandCGViewBase.CvCameraViewListener2{

    final static String TAG = "ARCalVR_Activity";
    final static boolean skipLoadingResource = false;
    private CVandCGViewBase surfaceView;
    private long controllerAddr;

    //Surface view
//    private GLSurfaceView surfaceView;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    // Resource
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    String resourceDest = null;

    //For touch event
    private GestureDetectorCalVR gestureDetector;

    //Label
    TextView FPSlabel;
    MovableFloatingActionButton track_bnt;

    //Sensor pixel 2 meter
    ArrayList<Size> pixel_arr_size = new ArrayList<>();
    private BaseLoaderCallback baseLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                    surfaceView.enableView();
                    break;
                default:
                    super.onManagerConnected(status);
                    break;
            }
            super.onManagerConnected(status);
        }
    };

    @Override
    public void onCameraViewStarted(int width, int height) {
    }

    @Override
    public void onCameraViewStopped() {

    }
    @Override
    public Mat onCameraFrame(CVandCGViewBase.CvCameraViewFrame inputFrame){
        Log.e(TAG, "onCameraFrame: ====here!!!!" );
        return inputFrame.rgba();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JniInterface.assetManager = getAssets();
        controllerAddr = JniInterface.JNIcreateController(JniInterface.assetManager);

        setupLabelandButton();
        setupResource();
        setupSurfaceView();

        setupTouchDetector();
        JNIOnMainActivityCreated();
    }
    @Override
    protected void onResume(){
        super.onResume();

        //Request for camera permission, which may be used in ARCore
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            CameraPermissionHelper.requestCameraPermission(this);
            return;
        }

        surfaceView.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, baseLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            baseLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        JniInterface.JNIonResume(getApplicationContext(), this);
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterface.JNIonPause();
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
        if (surfaceView != null)
            surfaceView.disableView();
    }
    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        if(isFinishing()) {
            synchronized (this) {
                JniInterface.JNIonDestroy();
                controllerAddr = 0;
            }
        }
        if (surfaceView != null)
            surfaceView.disableView();
    }
    private void setupLabelandButton(){
// add button actions
//        final Button restart_bnt = (Button)findViewById(R.id.restart_button);
//        restart_bnt.setOnClickListener(new View.OnClickListener(){
//            public void onClick(View v){
//                Intent restartIntent = getBaseContext().getPackageManager()
//                        .getLaunchIntentForPackage(getBaseContext().getPackageName());
//                restartIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
//                finish();
//                JniInterface.JNIonDestroy();
//                startActivity(restartIntent);
//            }
//        });
        track_bnt = findViewById(R.id.main_button);
        track_bnt.bringToFront();

        track_bnt.addSubButton(findViewById(R.id.moveButton), -300f, -200f);
        track_bnt.addSubButton(findViewById(R.id.rotateButton), 300f, -200f);

        final FloatingActionButton trans_bnt = findViewById(R.id.moveButton);
        final FloatingActionButton move_bnt = findViewById(R.id.rotateButton);
        trans_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                trans_bnt.setAlpha(1.0f);
                move_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(3,view.getX(), view.getY());
            }
        });


        move_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                move_bnt.setAlpha(1.0f);
                trans_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(4, view.getX(), view.getY());
            }
        });

        FPSlabel = (TextView) findViewById(R.id.textViewFPS);
    }
    private void setupSurfaceView(){
        surfaceView = (CVandCGViewBase) findViewById(R.id.camera_view);

        surfaceView.setCameraIndex(CVandCGJavaCamera2View.CAMERA_ID_ANY);
        surfaceView.setCvCameraViewListener(this);
        surfaceView.disableView();

        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new MainActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private void setupTouchDetector(){
        gestureDetector = new GestureDetectorCalVR(this);

        surfaceView.setOnTouchListener(
                new View.OnTouchListener(){
                    @Override
                    public boolean onTouch(View v, MotionEvent e){
                        return gestureDetector.onTouchEvent(e);
                    }
                }
        );
    }
    private void setupResource(){
        resourceDest = getFilesDir().getAbsolutePath() + "/";
        copyFromAssets();
    }
    private void copyFromAssets(){
        calvr_dest = resourceDest + calvr_folder;
        //Skip copying if files exist
        if(skipLoadingResource){
            File destDir = new File(calvr_dest);
            if(destDir.exists())
                return;
        }
        try{
            fileUtils.copyFromAsset(getAssets(), calvr_folder, calvr_dest);
        }catch (Exception e){
            Log.e(TAG, "copyFromAssets: Failed to copy from asset folder");
        }
    }
    private class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES30.glClearColor(1.0f, .0f, .0f, 1.0f);
            JniInterface.JNIonGlSurfaceCreated(calvr_dest);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            viewportWidth = width;
            viewportHeight = height;
            viewportChanged = true;
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            // Synchronized to avoid racing onDestroy.
            synchronized (this) {
                if (controllerAddr == 0) {
                    return;
                }
                if (viewportChanged) {
                    int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                    JniInterface.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);

                    viewportChanged = false;
                }
                JniInterface.JNIdrawFrame();
                updateFPS(JniInterface.JNIgetFPS());
            }
        }

    }
    // DisplayListener methods
    @Override
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {viewportChanged = true;}

            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//                holder.setFormat(PixelFormat.TRANSPARENT);
            }

            public void surfaceCreated (SurfaceHolder holder) {
//                holder.setFormat(PixelFormat.TRANSPARENT);
            }

            public void surfaceDestroyed (SurfaceHolder holder) {

            }
    public void updateFPS(final float fFPS)
    {
        if( FPSlabel == null )
            return;
        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                FPSlabel.setText(String.format("%2.2f FPS", fFPS));

            }});
    }
    public void popButtons(){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                track_bnt = findViewById(R.id.main_button);
                track_bnt.StartLockAnimation();
            }
        });
    }
    public float[] getPixelSize(){
        float[] arr = new float[pixel_arr_size.size() * 2];
        int i=0;
        for(Size sz:pixel_arr_size){
            arr[2*i] = sz.getWidth();
            arr[2*i+1] = sz.getHeight();
            i++;
        }
        return arr;
    }

    public native void JNIOnMainActivityCreated();
}
