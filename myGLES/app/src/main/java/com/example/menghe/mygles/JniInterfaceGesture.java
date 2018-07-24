package com.example.menghe.mygles;

public class JniInterfaceGesture {
    public static native void JNIDrawSphere(long osgAppAddr);

    public static native void JNIonTouched(boolean down, float x, float y);

    public static native void JNIonMove(float x, float y);
}
