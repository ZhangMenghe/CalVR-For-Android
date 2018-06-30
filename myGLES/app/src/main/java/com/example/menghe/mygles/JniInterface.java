package com.example.menghe.mygles;

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
    public static void drawFrame(){
        JNIdrawFrame();
    }
    //pass a Java object
    public static native void JNIsetupGrphicDraw(int w, int t, AssetManager asset_manager);

    public static native void JNIdrawFrame();
}
