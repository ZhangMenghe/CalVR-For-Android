package com.example.calvrapp;

import android.content.res.AssetManager;

public class JniInterfaceCalVR {
    static {
        System.loadLibrary("simpleCalVRd");
        System.loadLibrary("MenuBasicsd");
    }
    private static final String TAG = "JniInterfaceCalVR";
    static AssetManager assetManager;

    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIonGlSurfaceCreated(String calvr_path);

    public static native void JNIonResourceLoaded(String path);

    public static native void JNIonViewChanged(int rot, int width, int height);

    public static native void JNIdrawFrame();

    public static native void JNIonSingleTouchDown(int pointer_num, float x, float y);

    public static native void JNIonSingleTouchUp(int pointer_num, float x, float y);

    public static native void JNIonDoubleTouch(int pointer_num, float x, float y);

    public static native void JNIonTouchMove(int pointer_num, float destx, float testy);
}
