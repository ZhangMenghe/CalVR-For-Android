package com.samsung.arcalvr_gpu;
import android.content.res.AssetManager;

public class JniInterface {
    static {
        System.loadLibrary("ARCalVR_GPU");
    }
    private static final String TAG = "JniInterfaceCalVR";
    static AssetManager assetManager;

    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIonGlSurfaceCreated(String calvr_path);

    public static native void JNIonViewChanged(int rot, int width, int height);

    public static native void JNIdrawFrame();

    public static native void JNIonResume();

    public static native void JNIonPause();

    public static native void JNIonDestroy(long controller_path);

    public static native float JNIgetFPS();

    public static native void JNIonSingleTouchDown(int type, float x, float y);

    public static native void JNIonSingleTouchUp(int type, float x, float y);

    public static native void JNIonDoubleTouch(int type, float x, float y);

    public static native void JNIonTouchMove(int type, float x, float y);
}
