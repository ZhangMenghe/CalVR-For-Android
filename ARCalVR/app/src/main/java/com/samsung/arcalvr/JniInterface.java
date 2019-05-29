package com.samsung.arcalvr;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.IOException;

public class JniInterface {
    static {
        System.loadLibrary("ARCalVRd");
    }
    private static final String TAG = "JniInterfaceCalVR";
    static AssetManager assetManager;

    public static native long JNIcreateController(AssetManager asset_manager);

    public static native void JNIonGlSurfaceCreated(String calvr_path);

    public static native void JNIonViewChanged(int rot, int width, int height);

    public static native void JNIdrawFrame();

    public static native void JNIonSingleTouchDown(int type, float x, float y);

    public static native void JNIonSingleTouchUp(int type, float x, float y);

    public static native void JNIonDoubleTouch(int type, float x, float y);

    public static native void JNIonTouchMove(int type, float x, float y);

    public static native void JNIonResume(Context context, Activity activity);

    public static native void JNIonPause();

    public static native void JNIonDestroy();

    public static native float JNIgetFPS();

    // Spatial Viz messages
    public static native int JNIgetMatches();
    public static native int JNIwhoseOpen();
    public static native boolean JNIpuzzleComplete();
}
