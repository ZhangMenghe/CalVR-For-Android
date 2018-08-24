package com.example.calvrapp;

import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class gestureListener extends GestureDetector.SimpleOnGestureListener
                            implements GestureDetector.OnDoubleTapListener{
    final static String TAG = "gestureListener";

    @Override
    public boolean onSingleTapUp(final MotionEvent e){
        JniInterfaceCalVR.JNIonSingleTouch(1, e.getX(), e.getY());
        return true;
    }
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                           float velocityY) {
        JniInterfaceCalVR.JNIonTouchMove(1, e1.getX(),e1.getY(),e2.getX(),e2.getY());
        return true;
    }
    @Override
    public boolean onDown(MotionEvent e){return true;}

    @Override
    public boolean onDoubleTap(MotionEvent e) {
        JniInterfaceCalVR.JNIonDoubleTouch(1, e.getX(), e.getY());
        return true;
    }
}
