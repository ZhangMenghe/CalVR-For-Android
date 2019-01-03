package com.samsung.arcalvr_gpu;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;
import com.samsung.arcalvr_gpu.helpers.CameraPermissionHelper;

import com.samsung.arcalvr_gpu.helpers.FullScreenHelper;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity implements GLSurfaceView.Renderer  {
    private static final String TAG = MainActivity.class.getSimpleName();
    private GLSurfaceView surfaceView;
    private ARCoreManager arCoreManager = new ARCoreManager();

    //ndk controller
    private long controllerAddr;

    // Resource
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = "asdfads";
    String resourceDest = null;

    //Label
    TextView FPSlabel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        JniInterface.assetManager = getAssets();
        controllerAddr = JniInterface.JNIcreateController(JniInterface.assetManager);
        arCoreManager.onCreate(this);
        setupSurfaceView();
        setupLabelandButton();
    }
    @Override
    protected void onResume() {
        super.onResume();
        arCoreManager.onResume(this);
        surfaceView.onResume();
        JniInterface.JNIonResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        arCoreManager.onPause();
        surfaceView.onPause();
        JniInterface.JNIonPause();
    }
    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        if(isFinishing()) {
            synchronized (this) {
                JniInterface.JNIonDestroy(controllerAddr);
                controllerAddr = 0;
            }
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            Toast.makeText(this, "Camera permission is needed to run this application", Toast.LENGTH_LONG)
                    .show();
            if (!CameraPermissionHelper.shouldShowRequestPermissionRationale(this)) {
                // Permission denied with checking "Do not ask again".
                CameraPermissionHelper.launchPermissionSettings(this);
            }
            finish();
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        FullScreenHelper.setFullScreenOnWindowFocusChanged(this, hasFocus);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        arCoreManager.onSurfaceCreated();
        JniInterface.JNIonGlSurfaceCreated(calvr_dest);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        arCoreManager.onSurfaceChanged(width, height);
        GLES20.glViewport(0, 0, width, height);

        int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
        JniInterface.JNIonViewChanged(displayRotation, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Clear screen to notify driver it should not load any pixels from previous frame.
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        arCoreManager.onDrawFrame();
        updateFPS(JniInterface.JNIgetFPS());
    }
    private void setupSurfaceView(){
        surfaceView = findViewById(R.id.surfaceview);
        surfaceView.setOnTouchListener(arCoreManager.getTapListener());
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(this);
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    private void setupLabelandButton(){
        FPSlabel = (TextView) findViewById(R.id.textViewFPS);
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
    public native void JNIOnMainActivityCreated();
}
