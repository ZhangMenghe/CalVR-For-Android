package com.samsung.arcalvr;

import android.content.Context;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class GestureDetectorCalVR {
    private MultiFingerTapDetector multiDetector;
    final static String TAG = "CalVR_Gesture";
    GestureDetectorCalVR(Context ctx){
        multiDetector = new MultiFingerTapDetector() {
            // Methods that need to be overridden
            //public abstract void testing();
            public void onOneFingerDown(MotionEvent event){}
            public void onOneFingerMove(MotionEvent event){}

            public  void onFling(int pointerNum, float srcx, float srcy, float dstx, float dsty){}

            public  void onSingleTapUp(int pointerNum, MotionEvent event){
                if(pointerNum != 1)
                    JniInterface.JNIonSingleTouchUp(pointerNum, event.getX(), event.getY());
            }

            public void onMoreFingersDown(MotionEvent event){
                JniInterface.JNIonSingleTouchDown(2,event.getX(), event.getY());
            }

            public void onTwoFingersMove(MotionEvent event){}
            public void onThreeFingersMove(MotionEvent event){}

            public void onOneFingerDoubleTap(float ex, float ey){
                JniInterface.JNIonDoubleTouch(1, ex, ey);
            }

            public void onOneFingerTripleTap(){}
            public void onOneFingerLongPress(){}

            // TWO FINGER TAPS
            public void onTwoFingerDoubleTap(float avgX, float avgY){
                JniInterface.JNIonDoubleTouch(2, avgX, avgY);
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
