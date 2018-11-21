package com.samsung.arcalvr;

import android.graphics.Rect;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.util.SizeF;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import java.io.File;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.hardware.camera2.CameraCharacteristics.SENSOR_INFO_PIXEL_ARRAY_SIZE;

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
    TextView FPSlabel;
    MovableFloatingActionButton track_bnt;

    //Sensor pixel 2 meter
    ArrayList<Size> pixel_arr_size = new ArrayList<>();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
        try{
            CameraManager manager = (CameraManager) getSystemService(this.CAMERA_SERVICE);
            for (String camera_id : manager.getCameraIdList()) {
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(camera_id);
//                int facing = characteristics.get(CameraCharacteristics.LENS_FACING);
//                float[] f = characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS);
//                Log.e(TAG, "===onResume: FOCAL LENS:" + f[0]);
//                SizeF ps = characteristics.get(CameraCharacteristics.SENSOR_INFO_PHYSICAL_SIZE);
//                Log.e(TAG, "===onResume: SENSOR_INFO_PHYSICAL_SIZE:" + ps.toString());
                pixel_arr_size.add(characteristics.get(CameraCharacteristics.SENSOR_INFO_PIXEL_ARRAY_SIZE));
//                Log.e(TAG, "===onResume: SENSOR_INFO_PIXEL_ARRAY_SIZE:" + pas.toString());
//                Rect aas = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
//                Log.e(TAG, "===onResume:SENSOR_INFO_ACTIVE_ARRAY_SIZE:" + aas.toString());
            }
        }catch (CameraAccessException e){
            e.printStackTrace();
        }




        JniInterface.JNIonResume(getApplicationContext(), this);
        JniInterface.JNIsetPixelSize(getPixelSize());
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

        FPSlabel = (TextView) findViewById(R.id.textViewFPS);
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
                updateFPS(JniInterface.JNIgetFPS());
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
    public void popButtons(){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                track_bnt = findViewById(R.id.main_button);
                track_bnt.StartLockAnimation();
            }
        });
    }
    public float[] getPixelSize(){
        float[] arr = new float[pixel_arr_size.size() * 2];
        int i=0;
        for(Size sz:pixel_arr_size){
            arr[2*i] = sz.getWidth();
            arr[2*i+1] = sz.getHeight();
            i++;
        }
        return arr;
    }
    public native void JNIOnMainActivityCreated();
}
