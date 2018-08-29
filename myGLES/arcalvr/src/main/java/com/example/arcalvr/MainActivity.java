package com.example.arcalvr;

import android.content.Intent;
import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.widget.Button;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity
        implements DisplayManager.DisplayListener{
    final static String TAG = "ARCalVR_Activity";
    private long controllerAddr;
    private GLSurfaceView surfaceView;
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    String resourceDest = null;
    //For touch event
    private calvrGestureDetector gestureDetector;

    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    private boolean moveCalvrCam = true;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);

        JniInterfaceCalVR.assetManager = getAssets();

        controllerAddr = JniInterfaceCalVR.JNIcreateController(JniInterfaceCalVR.assetManager);

        setupButtons();
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
        JniInterfaceCalVR.JNIonResume(getApplicationContext(), this);
        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterfaceCalVR.JNIonPause();
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }
    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        if(isFinishing()) {
            synchronized (this) {
                JniInterfaceCalVR.JNIonDestroy();
                controllerAddr = 0;
            }
        }
    }
    private void setupButtons(){
        // add button actions
        final Button restart_bnt = (Button)findViewById(R.id.restart_button);
        restart_bnt.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                Intent restartIntent = getBaseContext().getPackageManager()
                        .getLaunchIntentForPackage(getBaseContext().getPackageName());
                restartIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(restartIntent);
            }
        });

        final Button debug_bnt = (Button)findViewById(R.id.debug_button);
        debug_bnt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                moveCalvrCam=!moveCalvrCam;
            }
        });
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
        gestureDetector = new calvrGestureDetector(this);

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
        File destDir = new File(calvr_dest);
        if(destDir.exists())
            return;
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
            JniInterfaceCalVR.JNIonGlSurfaceCreated(calvr_dest);
//            JniInterfaceOSG.JNIonGlSurfaceCreated(resourceDest);
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
                    JniInterfaceCalVR.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);
//                    JniInterfaceOSG.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);
                    viewportChanged = false;
                }
//                JniInterfaceOSG.JNIdrawFrame(true);
                JniInterfaceCalVR.JNIdrawFrame(moveCalvrCam);
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
