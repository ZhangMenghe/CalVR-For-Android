package lapras.testapp;

import android.content.res.AssetManager;

public class JNIInterface {
    public static native long JNIonCreate(AssetManager asset_manager);

    public static native void JNIsendDCMImgs(dcmImage img_arr[], int size);

    public static native void JNIonGlSurfaceCreated();
    public static native void JNIonSurfaceChanged(int width, int height);
    public static native void JNIdrawFrame();

    public static native void JNIonSingleTouchDown(float x, float y);
    public static native void JNIonTouchMove(float x, float y);
    public static native void JNIonDoubleTouch(int indicate, float x, float y);

    public static native boolean JNIchangeRender();
    public static native void JNIsetSwitches(int idx, boolean isSet);
    public static native void JNIsetJavaUIStatus(int item, int id);
    public static native void JNIsetParam(int idx, float value);

    public static native boolean JNIgetOriginalChecked(int idx);
    public static native float JNIgetOriginalValue(int idx);
    public static native float JNIgetFPS();

}
