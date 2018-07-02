package com.example.menghe.mygles;

        import android.app.Activity;
        import android.content.Context;
        import android.content.res.AssetManager;

public class JniInterface {
    static {
        System.loadLibrary("arTemplate");
    }
    private static final String TAG = "JniInterface";
    static AssetManager assetManager;

    public static void setupGrphicDraw(int w, int t){
        JNIsetupGrphicDraw(w, t, assetManager);
    }
    public static long createController(){
        return JNIcreateController(assetManager);
    }
    //pass a Java object
    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIsetupGrphicDraw(int w, int t, AssetManager asset_manager);

    public static native void JNIdrawFrame(long controller_addr);

    public static native void JNIonGlSurfaceCreated();

    public static native void JNIonViewChanged(int rotation, int width, int height);

    public static native void JNIonTouched(float x, float y);

    public static native boolean JNIhasDetectedPlane();

    public static native void JNIonResume(Context context, Activity activity);

    public static native void JNIonPause();

    public static native void JNIonDestroy();
}
