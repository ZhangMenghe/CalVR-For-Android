package com.example.calvrapp;

import android.view.MotionEvent;
import android.view.ViewConfiguration;

public abstract class SimpleTwoFingerDoubleTapDetector {
    private static final int TIMEOUT = ViewConfiguration.getDoubleTapTimeout() + 100;
    private long mFirstDownTime = 0;
    private boolean mSeparateTouches = false;
    private byte mTwoFingerTapCount = 0;

    private void reset(long time) {
        mFirstDownTime = time;
        mSeparateTouches = false;
        mTwoFingerTapCount = 0;
    }

    public boolean onTouchEvent(MotionEvent event) {
        switch(event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                if(mFirstDownTime == 0 || event.getEventTime() - mFirstDownTime > TIMEOUT)
                    reset(event.getDownTime());
                return true;
            case MotionEvent.ACTION_POINTER_UP:
                if(event.getPointerCount() == 2)
                    mTwoFingerTapCount++;
                else
                    mFirstDownTime = 0;
                break;
            case MotionEvent.ACTION_UP:
                if(event.getEventTime() - mFirstDownTime > TIMEOUT){
                    onTwoFingerLongPress(event.getX(), event.getY());
                    reset(event.getEventTime());
                    return true;
                }
                if(!mSeparateTouches){
                    mSeparateTouches = true;
                }
                else if(mTwoFingerTapCount == 2 && event.getEventTime() - mFirstDownTime < TIMEOUT) {
                    onTwoFingerDoubleTap(event.getX(), event.getY());
                    mFirstDownTime = 0;
                    return true;
                }
//                else if(mTwoFingerTapCount == 2 && event.getEventTime() - mFirstDownTime >TIMEOUT){                 // if(mTwoFingerTapCount == 1){
//                    onTwoFingerSingleTap(event.getX(), event.getY());
//                  //  reset(event.getDownTime());
//                    return true;
//                }

        }

        return false;
    }

    public abstract void onTwoFingerLongPress(float ex, float ey);
    public abstract void onTwoFingerSingleTap(float ex, float ey);
    public abstract void onTwoFingerDoubleTap(float ex, float ey);
}