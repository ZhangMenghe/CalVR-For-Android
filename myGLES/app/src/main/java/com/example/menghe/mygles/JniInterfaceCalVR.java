package com.example.menghe.mygles;

import android.content.res.AssetManager;

public class JniInterfaceCalVR {
    static {
        System.loadLibrary("simpleCalVR");
    }
    private static final String TAG = "JniInterfaceCalVR";
    static AssetManager assetManager;

    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIonGlSurfaceCreated(String calvr_path);

    public static native void JNIonResourceLoaded(String path);

    public static native void JNIonViewChanged(int rot, int width, int height);

    public static native void JNIdrawFrame();

    public static native void JNIonTouched(float x, float y);

    public static native void JNIonDoubleTouch(float x, float y);

    public static native void JNIonTouchMove(float srcx, float srcy, float destx, float testy);
}
