package com.example.menghe.mygles;

import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class gestureListener extends GestureDetector.SimpleOnGestureListener
                            implements GestureDetector.OnDoubleTapListener{
    final static String TAG = "gestureListener";

    @Override
    public boolean onSingleTapUp(final MotionEvent e){
        JniInterfaceCalVR.JNIonTouched(e.getX(), e.getY());
        return true;
    }
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                           float velocityY) {
        Log.i(TAG, "============onFling=============: ");
        JniInterfaceCalVR.JNIonTouchMove(e1.getX(),e1.getY(),e2.getX(),e2.getY());

        return true;
    }
    @Override
    public boolean onDown(MotionEvent e){return true;}

    @Override
    public boolean onDoubleTap(MotionEvent e) {
        Log.e(TAG, "onDoubleTap() ptrs:" + e.getPointerCount() );
        JniInterfaceCalVR.JNIonDoubleTouch(e.getX(), e.getY());
        return true;
    }
}
