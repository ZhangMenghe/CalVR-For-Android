package com.example.menghe.mygles;

import android.content.res.AssetManager;

public class JniInterface {
    static {
        System.loadLibrary("arTemplate");
    }
    private static final String TAG = "JniInterface";
    static AssetManager assetManager;

    public static native void setupGrphicDraw(int w, int t);

    public static native void drawFrame();
}
