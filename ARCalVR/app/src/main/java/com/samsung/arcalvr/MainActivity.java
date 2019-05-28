package com.samsung.arcalvr;

import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import java.io.File;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import android.view.MotionEvent;
import android.view.View;

import com.samsung.arcalvr.UI.GestureDetectorCalVR;
import com.samsung.arcalvr.Utils.CameraPermissionHelper;
import com.samsung.arcalvr.Utils.fileUtils;

public class MainActivity extends AppCompatActivity
    implements DisplayManager.DisplayListener{
    final static String TAG = "ARCalVR_Activity";
    final static boolean skipLoadingResource = false;

    private long controllerAddr;

    //Surface view
    private GLSurfaceView surfaceView;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    // Resource
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    String resourceDest = null;

    //For touch event
    private GestureDetectorCalVR gestureDetector;

    protected UIsController uiController;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JniInterface.assetManager = getAssets();
        controllerAddr = JniInterface.JNIcreateController(JniInterface.assetManager);
        uiController = new UIsController(this);

        setupResource();
        setupSurfaceView();
        setupTouchDetector();
    }
    @Override
    protected void onResume(){
        super.onResume();
        surfaceView.onResume();
        //Request for camera permission, which may be used in ARCore
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            CameraPermissionHelper.requestCameraPermission(this);
            return;
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
    }
    private void setupSurfaceView(){
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
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
                uiController.updateFPS(JniInterface.JNIgetFPS());
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
}
