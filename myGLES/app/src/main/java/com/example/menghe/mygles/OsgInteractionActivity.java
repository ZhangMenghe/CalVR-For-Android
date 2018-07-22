package com.example.menghe.mygles;

import android.hardware.display.DisplayManager;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.PopupMenu;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OsgInteractionActivity extends AppCompatActivity
        implements DisplayManager.DisplayListener{
    private GLSurfaceView surfaceView;
    // further will be cast to a cpp class. Use address instead
    private long controllerAddr;
    public boolean btn_status_normal = true;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    private GestureDetector touchDetector;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_osg_interaction);
        JniInterfaceOSG.assetManager = getAssets();
        controllerAddr = JniInterfaceOSG.createController();

        setupSurfaceView();
        setupTouchDetection();
        setupGestureMenu();
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
        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterfaceOSG.JNIonPause();
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
        surfaceView.setRenderer(new OsgInteractionActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    private void setupTouchDetection(){
        touchDetector = new GestureDetector(this, new gestureListener());
    }

    private void setupGestureMenu(){
        final Button menu_btn = (Button)findViewById(R.id.button);
        menu_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PopupMenu pop_menu = new PopupMenu(OsgInteractionActivity.this, menu_btn);
                pop_menu.getMenuInflater().inflate(R.menu.pop_menu, pop_menu.getMenu());
                pop_menu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        switch (menuItem.getItemId()){
                            case R.id.touchId:
                                Toast.makeText(OsgInteractionActivity.this, "Do touch detection",
                                        Toast.LENGTH_SHORT).show();
                                JniInterfaceGesture.JNIDrawSphere();
                                surfaceView.setOnTouchListener(
                                        new View.OnTouchListener(){
                                            @Override
                                            public boolean onTouch(View v, MotionEvent e){
                                                return touchDetector.onTouchEvent(e);
                                            }
                                        }
                                );
                                break;
                            case R.id.mtouchId:
                                Toast.makeText(OsgInteractionActivity.this, "call from leaves",
                                        Toast.LENGTH_SHORT).show();
                                break;
                        }
                        return true;
                    }
                });
                pop_menu.show();
            }
        });

    }
   private class gestureListener extends GestureDetector.SimpleOnGestureListener{
       @Override
        public boolean onSingleTapUp(final MotionEvent e){
            surfaceView.queueEvent(
                    new Runnable() {
                        @Override
                        public void run() {
                            JniInterfaceGesture.JNIonTouched(e.getX(), e.getY());
                        }
                    }
            );
            return true;
        }
        @Override
        public boolean onDown(MotionEvent e){return true;}
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
            JniInterfaceOSG.JNIonGlSurfaceCreated();
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
