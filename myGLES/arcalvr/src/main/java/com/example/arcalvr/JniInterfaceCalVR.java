package com.example.arcalvr;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;

public class JniInterfaceCalVR {
    static {
        System.loadLibrary("ARCalVR");
//        System.loadLibrary("MenuBasics");
    }
    private static final String TAG = "JniInterfaceCalVR";
    static AssetManager assetManager;

    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIonGlSurfaceCreated(String calvr_path);

    public static native void JNIonResourceLoaded(String path);

    public static native void JNIonViewChanged(int rot, int width, int height);

    public static native void JNIdrawFrame(boolean moveCam);

    public static native void JNIonSingleTouchDown(int pointer_num, float x, float y);

    public static native void JNIonSingleTouchUp(int pointer_num, float x, float y);

    public static native void JNIonDoubleTouch(int pointer_num, float x, float y);

    public static native void JNIonTouchMove(int pointer_num, float destx, float testy);

    public static native void JNIonResume(Context context, Activity activity);

    public static native void JNIonPause();

    public static native void JNIonDestroy();

}
