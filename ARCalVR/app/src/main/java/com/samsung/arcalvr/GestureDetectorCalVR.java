package com.samsung.arcalvr;

import android.content.Context;
import android.view.MotionEvent;

public class GestureDetectorCalVR {
    private MultiFingerTapDetector multiDetector;
    final static String TAG = "CalVR_Gesture";
    GestureDetectorCalVR(Context ctx){
        multiDetector = new MultiFingerTapDetector() {
            // Methods that need to be overridden
            // public abstract void testing();
            public void onOneFingerDown(MotionEvent event){
                JniInterface.JNIonSingleTouchDown(0, event.getX(), event.getY());
            }
            public void onOneFingerMove(MotionEvent event){
                JniInterface.JNIonTouchMove(0, event.getX(), event.getY());
            }

            public  void onFling(int pointerNum, float srcx, float srcy, float dstx, float dsty){}

            // LEFT(0) OR Right(1) Single Tap [UP]
            public  void onSingleTapUp(int pointerNum, MotionEvent event){
                JniInterface.JNIonSingleTouchUp(pointerNum - 1, event.getX(), event.getY());
            }

            // TWO FINGERS (right mouse click)
            // RIGHT SINGLE TOUCH
            public void onMoreFingersDown(int pointerNum, MotionEvent event){
                JniInterface.JNIonSingleTouchDown(pointerNum - 1, event.getX(), event.getY());
            }
            public void onTwoFingersMove(MotionEvent event){}
            public void onThreeFingersMove(MotionEvent event){}

            // ONE FINGER (left mouse click)
            // left double tap
            public void onOneFingerDoubleTap(float ex, float ey){
                JniInterface.JNIonDoubleTouch(0, ex, ey);
            }
            public void onOneFingerTripleTap(){}
            public void onOneFingerLongPress(){}

            // TWO FINGER DOUBLE: right double
            public void onTwoFingerDoubleTap(float ex, float ey){
                JniInterface.JNIonDoubleTouch(1, ex, ey);
            }

            public void onTwoFingerTripleTap(){}
            public void onTwoFingerLongPress(MotionEvent event){}

            // THREE FINGER TAPS
            public void onThreeFingerDoubleTap(){}
            public void onThreeFingerTripleTap(){}
            public void onThreeFingerLongPress(MotionEvent event){}
        };
    }
    public boolean onTouchEvent(MotionEvent event) {
        multiDetector.onTouchEvent(event);
        return true;
    }
}
