package com.example.menghe.mygles;

import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CalVRActivity extends AppCompatActivity {
    final static String TAG = "CalVRActivity";
    private long controllerAddr;
    private GLSurfaceView surfaceView;
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cal_vr);
        JniInterfaceCalVR.assetManager = getAssets();
        copyFromAssets();
        controllerAddr = JniInterfaceCalVR.JNIcreateController(JniInterfaceCalVR.assetManager);
        setupSurfaceView();
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
    private void setupResources()
    {
        String filename = "/config.xml";
        File dir = getFilesDir();
        String path = dir.getAbsolutePath();
        String modelPath = path + filename;
        File model = new File(modelPath);
        try {
            if (!model.exists()) {
                // Note: this only works for small files,
                // because we read the whole file into memory.
                InputStream is = getResources().openRawResource(R.raw.config);
                byte[] buffer = new byte[is.available()];
                is.read(buffer);
                FileOutputStream os = new FileOutputStream(model);
                os.write(buffer);
            }
            JniInterfaceCalVR.JNIonResourceLoaded(modelPath);
        }
        catch (Exception e) {
            Log.e(TAG, "setupResources: can not copy resource files");
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
