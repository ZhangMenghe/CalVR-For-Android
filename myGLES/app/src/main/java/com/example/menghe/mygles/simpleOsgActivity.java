package com.example.menghe.mygles;

import android.hardware.display.DisplayManager;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class simpleOsgActivity extends AppCompatActivity
        implements DisplayManager.DisplayListener{
    private GLSurfaceView surfaceView;
    // further will be cast to a cpp class. Use address instead
    private long controllerAddr;
    public boolean btn_status_normal = true;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;
    String resourceDest = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_simple_osg);
        JniInterfaceOSG.assetManager = getAssets();
        controllerAddr = JniInterfaceOSG.createController();

        setupSurfaceView();
        //setupResources("/box.osgt", R.raw.box);
        resourceDest = getFilesDir().getAbsolutePath() + "/";
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
        JniInterfaceOSG.JNIonResume(getApplicationContext(), this);

//        setupPlaneDetectionMsg();

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterfaceOSG.JNIonPause();
//        planeDetection_handler.removeCallbacks(planeDetection_runnable);
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);

    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        if(isFinishing()) {
            synchronized (this) {
                JniInterfaceOSG.JNIonDestroy();
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
        surfaceView.setRenderer(new simpleOsgActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private void setupResources(String filename, int id)
    {
        // Get absolute path to internal storage.
        File dir = getFilesDir();
        String path = dir.getAbsolutePath();
        // Check if 'box.osgt' already exists.
        String modelPath = path + filename;
        File model = new File(modelPath);
        // Copy 'box.ogst' from 'res/raw', if it does not exist.
        try {
            if (!model.exists()) {
                // Note: this only works for small files,
                // because we read the whole file into memory.
                InputStream is = getResources().openRawResource(id);
                byte[] buffer = new byte[is.available()];
                is.read(buffer);
                FileOutputStream os = new FileOutputStream(model);
                os.write(buffer);
            }
            JniInterfaceOSG.JNIDebugScene(modelPath);
        }
        catch (Exception e) {

        }
    }
    private class Renderer implements GLSurfaceView.Renderer{
        public void onDrawFrame(GL10 gl) {
            // Synchronized to avoid racing onDestroy.
            synchronized (this) {
                if (controllerAddr == 0)
                    return;
                if (viewportChanged) {
                    int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                    JniInterfaceOSG.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);
                    viewportChanged = false;
                }
                JniInterfaceOSG.JNIdrawFrame(btn_status_normal);
            }
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            viewportWidth = width;
            viewportHeight = height;
            viewportChanged = true;
            //JniInterfaceOSG.JNIonViewChanged(0, width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing
            gl.glEnable(GL10.GL_DEPTH_TEST);
            JniInterfaceOSG.JNIonGlSurfaceCreated(resourceDest);
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
