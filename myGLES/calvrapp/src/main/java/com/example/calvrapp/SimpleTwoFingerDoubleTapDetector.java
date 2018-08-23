package com.example.calvrapp;

import android.os.Handler;
import android.view.MotionEvent;
import android.view.ViewConfiguration;



public abstract class SimpleTwoFingerDoubleTapDetector {
    private static final int TIMEOUT = ViewConfiguration.getDoubleTapTimeout() + 100;
    private long mFirstDownTime = 0;
    private boolean mSeparateTouches = false;
    private byte mTwoFingerTapCount = 0;
    Handler handler = new android.os.Handler();
    Runnable singleRunnable;

    private void reset(long time) {
        mFirstDownTime = time;
        mSeparateTouches = false;
        mTwoFingerTapCount = 0;
    }

    public boolean onTouchEvent(final MotionEvent event) {

        switch(event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                if(mFirstDownTime == 0 || event.getEventTime() - mFirstDownTime > TIMEOUT)
                    reset(event.getDownTime());
                return true;
            case MotionEvent.ACTION_POINTER_UP:
                if(event.getPointerCount() == 2){
                    mTwoFingerTapCount++;
                    singleRunnable =new Runnable() {
                        @Override
                        public void run() {
                            if(mTwoFingerTapCount == 1)
                                onTwoFingerSingleTap(event.getX(), event.getY());
                        }
                    };
                    handler.postDelayed(singleRunnable, TIMEOUT);
                }

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
                    handler.removeCallbacks(singleRunnable);
                    return true;
                }

        }

        return false;
    }

    public abstract void onTwoFingerLongPress(float ex, float ey);
    public abstract void onTwoFingerSingleTap(float ex, float ey);
    public abstract void onTwoFingerDoubleTap(float ex, float ey);
}