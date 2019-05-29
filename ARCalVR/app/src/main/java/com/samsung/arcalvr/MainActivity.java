package com.samsung.arcalvr;

import android.animation.ObjectAnimator;
import android.animation.PropertyValuesHolder;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Path;
import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.security.acl.Group;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity
    implements DisplayManager.DisplayListener{
    final static String TAG = "ARCalVR_Activity";
    final static boolean skipLoadingResource = false;

    private long controllerAddr;

    //Surface view
    private GLSurfaceView surfaceView;
    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    // Resource
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    String resourceDest = null;

    //For touch event
    private GestureDetectorCalVR gestureDetector;

    //Label
    TextView puzzleMessage;
    MovableFloatingActionButton track_bnt;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);

        JniInterface.assetManager = getAssets();
        controllerAddr = JniInterface.JNIcreateController(JniInterface.assetManager);

        setupLabelandButton();
        setupResource();
        setupSurfaceView();
        setupTouchDetector();

        JNIOnMainActivityCreated();
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
        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
        JniInterface.JNIonPause();
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }
    @Override
    protected void onDestroy(){
        super.onDestroy();
        //synchronized to avoid racing
        if(isFinishing()) {
            synchronized (this) {
                JniInterface.JNIonDestroy();
                controllerAddr = 0;
            }
        }
    }
    private void setupLabelandButton(){
// add button actions
//        final Button restart_bnt = (Button)findViewById(R.id.restart_button);
//        restart_bnt.setOnClickListener(new View.OnClickListener(){
//            public void onClick(View v){
//                Intent restartIntent = getBaseContext().getPackageManager()
//                        .getLaunchIntentForPackage(getBaseContext().getPackageName());
//                restartIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
//                finish();
//                JniInterface.JNIonDestroy();
//                startActivity(restartIntent);
//            }
//        });
        track_bnt = findViewById(R.id.main_button);
        track_bnt.bringToFront();

        track_bnt.addSubButton(findViewById(R.id.moveButton), -300f, -200f);
        track_bnt.addSubButton(findViewById(R.id.rotateButton), 300f, -200f);

        final FloatingActionButton trans_bnt = findViewById(R.id.moveButton);
        final FloatingActionButton move_bnt = findViewById(R.id.rotateButton);
        trans_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                trans_bnt.setAlpha(1.0f);
                move_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(3,view.getX(), view.getY());
            }
        });


        move_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                move_bnt.setAlpha(1.0f);
                trans_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(4, view.getX(), view.getY());
            }
        });

        puzzleMessage = (TextView) findViewById(R.id.textViewPuzzleMessage);
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
        gestureDetector = new GestureDetectorCalVR(this);

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
        if(skipLoadingResource){
            File destDir = new File(calvr_dest);
            if(destDir.exists())
                return;
        }
        try{
            fileUtils.copyFromAsset(getAssets(), calvr_folder, calvr_dest);
        }catch (Exception e){
            Log.e(TAG, "copyFromAssets: Failed to copy from asset folder");
        }
    }
    private class Renderer implements GLSurfaceView.Renderer {

        int matches = -1;
        int NONE_OPEN = 0;
        int MAZE = 1;
        int FIVE = 2;
        int LAB = 3;
        int TETRIS = 4;
        int TETRIS_2 = 5;
        int openPuzzle = 0;
        boolean puzzleFinished = false;

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES30.glClearColor(1.0f, .0f, .0f, 1.0f);
            JniInterface.JNIonGlSurfaceCreated(calvr_dest);
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
                    JniInterface.JNIonViewChanged(displayRotation, viewportWidth, viewportHeight);

                    viewportChanged = false;
                }
                JniInterface.JNIdrawFrame();
//                updateFPS(JniInterface.JNIgetFPS());

                // check to see which Puzzle is open
                int open = JniInterface.JNIwhoseOpen();
                // if a different puzzle is open:
                if (openPuzzle != open){
                    openPuzzle = open;
                    updatePuzzleToast(open);
                }

                // if Tetris is open:
                if (open == TETRIS || open == TETRIS_2) {
                    int currMatches = JniInterface.JNIgetMatches();
                    // call update Toast if the matches variable changes
                    if (matches != currMatches) {
                        // update the Toast message and global variable
                        updateTetrisToast(currMatches);
                        matches = currMatches;
                    }
                }

                // if 5x5 is open:
                if (open == FIVE || open == LAB || open == MAZE){
                    boolean finished = JniInterface.JNIpuzzleComplete();
                    // if there was a change
                    if (puzzleFinished != finished) {
                        puzzleFinished = finished;      // store that change
                        // make a toast message - if just finished
                        if (finished) {
                            puzzleCompleteToast();
                        }
                    }
                }
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

    /*public void updateFPS(final float fFPS)
    {
        if( FPSlabel == null )
            return;
        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                FPSlabel.setText(String.format("%2.2f FPS", fFPS));

            }});
    }*/
    public void popButtons(){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                track_bnt = findViewById(R.id.main_button);
                track_bnt.StartLockAnimation();
            }
        });
    }
    public native void JNIOnMainActivityCreated();

    // SpatialViz Puzzle Messages
    public void updateTetrisToast(final int matches){
        if (matches == -1 || matches == 0)
            return;

        this.runOnUiThread(new Runnable(){
            @Override
            public void run() {
                LayoutInflater inflater = getLayoutInflater();
                View layout = inflater.inflate(R.layout.toast_layout, (ViewGroup) findViewById(R.id.toast_root));

                TextView toastText = layout.findViewById(R.id.toast_text);
                ImageView toastImage = layout.findViewById(R.id.toast_image);

                if (matches == 1){
                    toastText.setText(R.string.position_match);
                }
                if (matches == 2){
                   toastText.setText(R.string.orientation_match);
                }
                if (matches == 3){
                    toastText.setText(R.string.complete_match);
                    toastImage.setImageResource(R.drawable.ic_toast_double_check);
                }

                Toast toast = new Toast(MainActivity.this);
                toast.setGravity(Gravity.CENTER_HORIZONTAL|Gravity.BOTTOM, 0,200);
                toast.setDuration(Toast.LENGTH_SHORT);
                toast.setView(layout);
                toast.show();
            }
        });

    }
    public void updatePuzzleToast(final int openPuzzle){

        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch(openPuzzle){
                    case 0:
                        puzzleMessage.setText(R.string.no_puzzles);
                        break;
                    case 1:
                        puzzleMessage.setText(R.string.maze_puzzle);
                        break;
                    case 2:
                        puzzleMessage.setText(R.string.five_puzzle);
                        break;
                    case 3:
                        puzzleMessage.setText(R.string.labyrinth_puzzle);
                        break;
                    case 4:
                        puzzleMessage.setText(R.string.tetris_puzzle);
                        break;
                    case 5:
                        puzzleMessage.setText(R.string.tetris_v2_puzzle);
                        break;
                }
            }
        });
    }
    public void puzzleCompleteToast(){
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                LayoutInflater inflater = getLayoutInflater();
                View layout = inflater.inflate(R.layout.toast_layout, (ViewGroup) findViewById(R.id.toast_root));

                TextView toastText = layout.findViewById(R.id.toast_text);
                ImageView toastImage = layout.findViewById(R.id.toast_image);

                toastText.setText(R.string.puzzle_complete);
                toastImage.setImageResource(R.drawable.ic_toast_double_check);

                Toast toast = new Toast(MainActivity.this);
                toast.setGravity(Gravity.CENTER_HORIZONTAL | Gravity.BOTTOM, 0, 200);
                toast.setDuration(Toast.LENGTH_SHORT);
                toast.setView(layout);
                toast.show();
            }
        });
    }
}
