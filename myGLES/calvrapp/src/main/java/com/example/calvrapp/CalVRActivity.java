package com.example.calvrapp;

import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CalVRActivity extends AppCompatActivity {
    final static String TAG = "CalVRActivity";
    private long controllerAddr;
    private GLSurfaceView surfaceView;
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    //For touch event
    private GestureDetector gestureDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_cal_vr);
        JniInterfaceCalVR.assetManager = getAssets();
        copyFromAssets();
        controllerAddr = JniInterfaceCalVR.JNIcreateController(JniInterfaceCalVR.assetManager);
        setupSurfaceView();
        setupTouchDetector();
    }
    private void setupSurfaceView(){
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new CalVRActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private void setupTouchDetector(){
        gestureDetector = new GestureDetector(this,new gestureListener());

        surfaceView.setOnTouchListener(
                new View.OnTouchListener(){
                    @Override
                    public boolean onTouch(View v, MotionEvent e){
                        return gestureDetector.onTouchEvent(e);
                    }
                }
        );
    }
    private void copyFromAssets(){
        calvr_dest = getFilesDir().getAbsolutePath() + "/" + calvr_folder;
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
        private boolean viewportChanged = false;
        private int viewportWidth;
        private int viewportHeight;

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES30.glClearColor(1.0f, .0f, .0f, 1.0f);
            JniInterfaceCalVR.JNIonGlSurfaceCreated(calvr_dest);
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
                    viewportChanged = false;
                }
            JniInterfaceCalVR.JNIdrawFrame();
            }
        }
    }
}
