package com.example.menghe.mygles;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("glTriangle");
    }
    private GLSurfaceView surfaceView;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;
//    private long nativeApplication;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupSurfaceView();
//        JniInterface.assetManager = getAssets();
//        nativeApplication = JniInterface.createNativeApplication(getAssets());
    }
    private void setupSurfaceView(){
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private class Renderer implements GLSurfaceView.Renderer {
        private boolean viewportChanged = false;
        private int viewportWidth;
        private int viewportHeight;

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES20.glClearColor(1.0f, .0f, .0f, 1.0f);
//        JniInterface.onGlSurfaceCreated(nativeApplication);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            viewportWidth = width;
            viewportHeight = height;
            viewportChanged = true;
            init(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            // Synchronized to avoid racing onDestroy.
            synchronized (this) {
//            if (nativeApplication == 0) {
//                return;
//            }
                if (viewportChanged) {
                    //int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
//                JniInterface.onDisplayGeometryChanged(
//                        nativeApplication, displayRotation, viewportWidth, viewportHeight);
                    viewportChanged = false;
                }
//            JniInterface.onGlSurfaceDrawFrame(nativeApplication);
            }
            step();
        }


    }
    public static native void init(int w, int t);

    public static native void step();
}
