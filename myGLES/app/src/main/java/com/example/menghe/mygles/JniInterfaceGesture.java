package com.example.menghe.mygles;

public class JniInterfaceGesture {
    public static native void JNIDrawSphere(long osgAppAddr);

    public static native void JNIonTouched(float x, float y);
}
