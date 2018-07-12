package com.example.menghe.mygles;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class simpleOsgActivity extends AppCompatActivity {
    static{
        System.loadLibrary("osgScened");
    }
    private GLSurfaceView surfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_simple_osg);
        setupSurfaceView();
        setupResources();
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
    private void setupResources()
    {
        // Get absolute path to internal storage.
        File dir = getFilesDir();
        String path = dir.getAbsolutePath();
        // Check if 'box.osgt' already exists.
        String modelPath = path + "/box.osgt";
        File model = new File(modelPath);
        // Copy 'box.ogst' from 'res/raw', if it does not exist.
        try {
            if (!model.exists()) {
                // Note: this only works for small files,
                // because we read the whole file into memory.
                InputStream is = getResources().openRawResource(R.raw.box);
                byte[] buffer = new byte[is.available()];
                is.read(buffer);
                FileOutputStream os = new FileOutputStream(model);
                os.write(buffer);
            }
            loadModel(modelPath);
        }
        catch (Exception e) {

        }
    }
    private class Renderer implements GLSurfaceView.Renderer{
        public void onDrawFrame(GL10 gl) {
            step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            init(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing
            gl.glEnable(GL10.GL_DEPTH_TEST);
        }
    }
    public static native void init(int w, int t);

    public static native void step();

    public static native void loadModel(String modelPath);
}
