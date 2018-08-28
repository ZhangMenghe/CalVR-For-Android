package com.example.arcalvr;

import android.content.Context;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

public class calvrGestureDetector{
        private MultiFingerTapDetector multiDetector;
        final static String TAG = "CalVR_Gesture";
    calvrGestureDetector(Context ctx){
        multiDetector = new MultiFingerTapDetector() {
            // Methods that need to be overridden
            //public abstract void testing();
            public void onOneFingerDown(MotionEvent event){
                JniInterfaceCalVR.JNIonSingleTouchDown(1,event.getX(), event.getY());
            }
            public void onOneFingerMove(MotionEvent event){
                Log.e(TAG, "=============MOVINGGGGGGGGGGGG: ================");
                JniInterfaceCalVR.JNIonTouchMove(1, event.getX(), event.getY());
            }

            public  void onFling(int pointerNum, float srcx, float srcy, float dstx, float dsty){
                Log.e(TAG, "=============onFling: ================"+pointerNum );

            }

            public  void onSingleTapUp(int pointerNum, MotionEvent event){
                JniInterfaceCalVR.JNIonSingleTouchUp(pointerNum, event.getX(), event.getY());
            }

            public void onMoreFingersDown(MotionEvent event){
                JniInterfaceCalVR.JNIonSingleTouchDown(2,event.getX(), event.getY());
            }

            public void onTwoFingersMove(MotionEvent event){}
            public void onThreeFingersMove(MotionEvent event){}

            public void onOneFingerDoubleTap(float ex, float ey){
                JniInterfaceCalVR.JNIonDoubleTouch(1, ex, ey);
            }

            public void onOneFingerTripleTap(){}
            public void onOneFingerLongPress(){}

            // TWO FINGER TAPS
            public void onTwoFingerDoubleTap(float avgX, float avgY){
                JniInterfaceCalVR.JNIonDoubleTouch(2, avgX, avgY);
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
