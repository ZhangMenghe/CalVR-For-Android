package com.example.menghe.mygles;

import android.gesture.Gesture;
import android.gesture.GestureLibraries;
import android.gesture.GestureLibrary;
import android.gesture.GestureOverlayView;
import android.gesture.Prediction;
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
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.PopupMenu;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OsgInteractionActivity extends AppCompatActivity
        implements DisplayManager.DisplayListener{
    private static final String TAG = "OSG-Interaction";
    private GLSurfaceView surfaceView;
    // further will be cast to a cpp class. Use address instead
    private long controllerAddr;
    public boolean btn_status_normal = true;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    private GestureDetector touchDetector;
    private GestureDetector multiTouchDetector;
    private gestureMultiTouchListener multiTouchListener;
    private GestureOverlayView touchOverlayView;

    private ViewFlipper flipper;
    private int[] imgIds = {R.drawable.pre_arrow, R.drawable.next_arrow, R.drawable.up_arrow, R.drawable.down_arrow};
    private GestureLibrary gestureLibrary;

    String resourceDest = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_osg_interaction);
        JniInterfaceOSG.assetManager = getAssets();
        controllerAddr = JniInterfaceOSG.createController();
        setupSurfaceView();
        setupViewFlipper("action");
        setupTouchDetection();
        setupGestureMenu();

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

    private void setupViewFlipper(String choose){
        flipper = (ViewFlipper) findViewById(R.id.flipper);
        if(choose.equalsIgnoreCase("action")){
            ImageView imageView = new ImageView(this);
            imageView.setImageResource(R.drawable.leaf);
            flipper.addView(imageView);
            imageView.setVisibility(View.GONE);
            return;
        }
        for(int i:imgIds){
            ImageView imageView = new ImageView(this);
            imageView.setImageResource(i);
            flipper.addView(imageView);
        }
        flipper.setFlipInterval(3000);
        flipper.startFlipping();
    }

    private void setupTouchDetection(){
        //Simple touch detector
        touchDetector = new GestureDetector(this, new gestureListener());

        //Multi touch detector
        TextView textView = (TextView) findViewById(R.id.gestureNameView);
        textView.setVisibility(View.GONE);
        multiTouchListener = new gestureMultiTouchListener(
                this.getApplicationContext(),textView);
        multiTouchDetector = new GestureDetector(this, multiTouchListener);

        //customize gesture
        gestureLibrary = GestureLibraries.fromRawResource(OsgInteractionActivity.this, R.raw.gesture);
        gestureLibrary.load();
        touchOverlayView = (GestureOverlayView)findViewById(R.id.gestureOverlayView1);
        touchOverlayView.addOnGesturePerformedListener(new gestureOverlayListener());
        touchOverlayView.setEnabled(false);
        touchOverlayView.getChildAt(0).setVisibility(View.GONE);
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
                        TextView textView = (TextView) findViewById(R.id.gestureNameView);
                        touchOverlayView.setEnabled(false);
                        surfaceView.setOnTouchListener(null);
                        textView.setVisibility(View.GONE);
                        touchOverlayView.getChildAt(0).setVisibility(View.GONE);
                        flipper.getCurrentView().setVisibility(View.GONE);
                        switch (menuItem.getItemId()){
                            case R.id.touchId:
                                Toast.makeText(OsgInteractionActivity.this, "Do touch detection",
                                        Toast.LENGTH_SHORT).show();
                                JniInterfaceGesture.JNICreateNode(controllerAddr, getAssets());
                                surfaceView.setOnTouchListener(
                                        new View.OnTouchListener(){
                                            @Override
                                            public boolean onTouch(View v, MotionEvent e){
                                                int action = e.getAction() & e.ACTION_MASK;
                                                if(action == MotionEvent.ACTION_MOVE){
                                                    JniInterfaceGesture.JNIonMove(e.getX(), e.getY());
                                                }else if(action == MotionEvent.ACTION_DOWN){
                                                    JniInterfaceGesture.JNIonTouched(true,e.getX(), e.getY());
                                                }else if(action==MotionEvent.ACTION_CANCEL){
                                                    JniInterfaceGesture.JNIonTouched(false,e.getX(), e.getY());
                                                }
                                                return touchDetector.onTouchEvent(e);
                                            }
                                        }
                                );
//                                flipper.getCurrentView().setVisibility(View.VISIBLE);
                                break;
                            case R.id.mtouchId:
                                Toast.makeText(OsgInteractionActivity.this, "call from multi touch",
                                        Toast.LENGTH_SHORT).show();
                                surfaceView.setOnTouchListener(
                                        new View.OnTouchListener(){
                                            @Override
                                            public boolean onTouch(View v, MotionEvent e){
                                                return multiTouchListener.onTouchEvent(e)||multiTouchDetector.onTouchEvent(e);
                                            }
                                        }
                                );
                                textView.setVisibility(View.VISIBLE);
                                break;
                            case R.id.customize:
                                Toast.makeText(OsgInteractionActivity.this, "call from customize gestures",
                                        Toast.LENGTH_SHORT).show();

                                touchOverlayView.getChildAt(0).setVisibility(View.VISIBLE);
                                touchOverlayView.setEnabled(true);

                                break;
                        }
                        return true;
                    }
                });
                pop_menu.show();
            }
        });

    }

//    单击：onDown(MotionEvent e)
//
//    抬起：onSingleTapUp(MotionEvent e)
//
//    短按：onShowPress(MotionEvent e)
//
//    长按：onLongPress(MotionEvent e)
//
//    滚动；onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
//
//    滑动：onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    private class gestureListener extends GestureDetector.SimpleOnGestureListener
                                  implements GestureDetector.OnDoubleTapListener{
       @Override
        public boolean onSingleTapUp(final MotionEvent e){
//            surfaceView.queueEvent(
//                    new Runnable() {
//                        @Override
//                        public void run() {
//                            JniInterfaceGesture.JNIonTouched(true,e.getX(), e.getY());
//                        }
//                    }
//            );
            return true;
        }
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                               float velocityY) {
            Log.i(TAG, "============onFling=============: ");
            drawTouch(e1.getX(),e1.getY(),e2.getX(),e2.getY());

            return true;
        }

       @Override
        public boolean onDown(MotionEvent e){
//           JniInterfaceGesture.JNIonMove(e.getX(), e.getY());
           return true;
       }

        private void drawTouch(float x, float y, float upx, float upy){
           ImageView view = (ImageView) flipper.getCurrentView();
           if(upx - x > 100)
               view.setImageResource(R.drawable.next_arrow);
           else if(upx - x  < -100)
               view.setImageResource(R.drawable.pre_arrow);
           else if(upy - y > 100)
               view.setImageResource(R.drawable.down_arrow);
           else if(upy - y < -100)
               view.setImageResource(R.drawable.up_arrow);

        }

        @Override
        public boolean onDoubleTap(MotionEvent e) {
            Log.e(TAG, "onDoubleTap() ptrs:" + e.getPointerCount() );

            return true;
        }

        @Override
        public boolean onDoubleTapEvent(MotionEvent e) {
            Log.e(TAG, "onDoubleTapEvent() ptrs:" + e.getPointerCount() );

            return true;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            Log.e(TAG, "onSingleTapConfirmed() ptrs:" + e.getPointerCount() );

            return true;
        }
    }

    private class gestureOverlayListener implements GestureOverlayView.OnGesturePerformedListener{
        @Override
        public void onGesturePerformed(GestureOverlayView overlay, Gesture gesture){
            // Read gesture file
            Prediction prediction = gestureLibrary.recognize(gesture).get(0);
            TextView tview = (TextView) touchOverlayView.getChildAt(0);
            if(prediction.score >= 5.0){
                if(prediction.name.equals("tick")){
                    ImageView view = (ImageView) flipper.getCurrentView();
                    view.setImageResource(R.drawable.checked);
                    view.setVisibility(View.VISIBLE);
                    tview.setText("This is true");
                }
            }else{
                tview.setText("Draw another tick");
            }

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
