package lapras.testapp;

import android.graphics.Bitmap;
import android.hardware.display.DisplayManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import com.imebra.*;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {
    final static String TAG = "ARCalVR_Activity";
    static {
        //System.loadLibrary("imebra_lib");
        System.loadLibrary("laprasNative");
    }
    // Resource
    final static private String calvr_folder = "calvrAssets";
    String calvr_dest = null;
    String resourceDest = null;
    final static boolean skipLoadingResource = true;

    //dcm
    List<String> file_path_lst;
    int file_nums;

    ImageView imageView_dcm;
    private int current_file_idx = 0;
    //ArrayList<Bitmap> dcm_bitmaps = new ArrayList<>();
    //ArrayList<Float> dcm_locs = new ArrayList<>();
    ArrayList<dcmImage> dcm_images = new ArrayList<>();
//    dcmImage dcm_images[];

    //glsurface
    //Surface view
    private GLSurfaceView surfaceView;

    private long nativeAddr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupSurfaceView();
        nativeAddr = JNIInterface.JNIonCreate(getAssets());
    }
    @Override
    protected void onResume() {
        super.onResume();
        surfaceView.onResume();
    }
    @Override
    protected void onPause(){
        super.onPause();
        surfaceView.onPause();
    }
    private void setupSurfaceView(){
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(3);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(new MainActivity.Renderer());
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
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

    private void setupDCMI() {
        file_path_lst = fileUtils.getListFilesFromDir(new File(calvr_dest +"/resources/dcm/"));
        file_nums = file_path_lst.size();

        for(String file_path : file_path_lst)
            dcm_images.add(new dcmImage(file_path));

        //pass down to native
//        JNIInterface.JNIsendDCMImgs(dcm_images.toArray(new dcmImage[0]), file_nums);

//        imageView_dcm = (ImageView) findViewById(R.id.dcmimageView);
//        imageView_dcm.setVisibility(View.INVISIBLE);
//        Button nextButton = (Button)findViewById(R.id.changedcmButton);
//        nextButton.setVisibility(View.INVISIBLE);
//        nextButton.setOnClickListener(new View.OnClickListener() {
//            public void onClick(View view) {
//                current_file_idx = (current_file_idx+1)%file_nums;
//                //imageView_dcm.setImageBitmap(dcm_bitmaps.get(current_file_idx));
//                //String file_path = file_path_lst.get(current_file_idx);
//                //getDCMIImage(file_path);
//            }
//        });
    }
    private void setupResource(){
        resourceDest = getFilesDir().getAbsolutePath() + "/";
        copyFromAssets();
        setupDCMI();
    }








    private class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            setupResource();
            JNIInterface.JNIonGlSurfaceCreated();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            JNIInterface.JNIonSurfaceChanged(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            JNIInterface.JNIdrawFrame();
        }

    }


}
