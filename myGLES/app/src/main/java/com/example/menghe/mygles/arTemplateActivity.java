package com.example.menghe.mygles;

import android.hardware.display.DisplayManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class arTemplateActivity extends AppCompatActivity
    implements DisplayManager.DisplayListener{
    private static final String TAG = arTemplateActivity.class.getSimpleName();
    private static final int MSG_UPDATE_INTERVAL = 1000;//in milliseconds
    private GLSurfaceView surfaceView;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    //For touch event
    private GestureDetector gestureDetector;

    // further will be cast to a cpp class. Use address instead
    private long controllerAddr;

    //Message(show to user) about plane detection: snackbar
    private Snackbar planeDetection_msg;

    //Plane Detection task
    private Handler planeDetection_handler;
    private final Runnable planeDetection_runnable =
            new Runnable() {
                @Override
                public void run() {
                    //check if plane has been detected
                    try{
                        if(JniInterface.JNIhasDetectedPlane()){
                            if(planeDetection_msg != null){
                                planeDetection_msg.dismiss();
                                planeDetection_msg = null;
                            }
                        }else{
                            // continue detecting planes
                            planeDetection_handler.postDelayed(planeDetection_runnable, MSG_UPDATE_INTERVAL);
                        }

                    }catch (Exception e){
                        Log.e(TAG, e.getMessage());
                    }
                }
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ar_template);
        JniInterface.assetManager = getAssets();

        controllerAddr = JniInterface.createController();
        planeDetection_handler = new Handler();
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

        setupPlaneDetectionMsg();

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterface.JNIonPause();
        planeDetection_handler.removeCallbacks(planeDetection_runnable);
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);

    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        synchronized (this){
            JniInterface.JNIonDestroy();
            controllerAddr = 0;
        }
    }

    private void setupPlaneDetectionMsg(){
        planeDetection_msg = Snackbar.make(arTemplateActivity.this.findViewById(android.R.id.content),
                "Searching for surfaces...",
                Snackbar.LENGTH_INDEFINITE);
        planeDetection_msg.getView().setBackgroundColor(0xbf323232);
        planeDetection_msg.show();

        planeDetection_handler.postDelayed(planeDetection_runnable,MSG_UPDATE_INTERVAL);
    }
    private void setupSurfaceView(){
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new arTemplateActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private void setupTouchDetector(){
        gestureDetector =
                new GestureDetector(
                        this,
                        new GestureDetector.SimpleOnGestureListener(){
                            @Override
                            public boolean onSingleTapUp(final MotionEvent e){
                                surfaceView.queueEvent(
                                        new Runnable() {
                                            @Override
                                            public void run() {
                                                JniInterface.JNIonTouched(e.getX(), e.getY());
                                            }
                                        }
                                );
                                return true;
                            }
                            @Override
                            public boolean onDown(MotionEvent e){return true;}
                        });
        surfaceView.setOnTouchListener(
                new View.OnTouchListener(){
                    @Override
                    public boolean onTouch(View v, MotionEvent e){
                        return gestureDetector.onTouchEvent(e);
                    }
                }
        );
    }
    private class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES20.glClearColor(1.0f, .0f, .0f, 1.0f);
            JniInterface.JNIonGlSurfaceCreated();
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
                if (controllerAddr == 0)
                    return;
                if (viewportChanged) {
                    int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                    JniInterface.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);
                    viewportChanged = false;
                }
                JniInterface.JNIdrawFrame();
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
