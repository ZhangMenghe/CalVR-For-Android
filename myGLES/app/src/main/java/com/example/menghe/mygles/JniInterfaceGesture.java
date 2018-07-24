package com.example.menghe.mygles;

import android.content.res.AssetManager;

public class JniInterfaceGesture {
    public static native void JNICreateNode(long osgAppAddr, AssetManager manager);

    public static native void JNIonTouched(boolean down, float x, float y);

    public static native void JNIonMove(float x, float y);
}
