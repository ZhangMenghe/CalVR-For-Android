package com.example.menghe.mygles;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;

public class JniInterfaceOSG {
    static {
        System.loadLibrary("osgScened");
    }
    private static final String TAG = "JniInterface";
    static AssetManager assetManager;

    public static void setupGrphicDraw(int w, int t){
        JNIsetupGrphicDraw(w, t);
    }
    public static long createController(){
        return JNIcreateController();
    }
    //pass a Java object
    public static native long JNIcreateController();

    public static native void JNIsetupGrphicDraw(int w, int t);

    public static native void JNIdrawFrame(boolean btn_status_normal);

    public static native void JNIonGlSurfaceCreated();

    public static native void JNIonViewChanged(int rotation, int width, int height);

    public static native void JNIonTouched(float x, float y);

    public static native boolean JNIhasDetectedPlane();

    public static native void JNIonResume(Context context, Activity activity);

    public static native void JNIonPause();

    public static native void JNIonDestroy();

    public static native void JNIDebugScene(String filename);
}
