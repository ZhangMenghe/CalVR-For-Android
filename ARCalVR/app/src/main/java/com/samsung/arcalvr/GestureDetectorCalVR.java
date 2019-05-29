package com.samsung.arcalvr;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

public class GestureDetectorCalVR {
    private MultiFingerTapDetector multiDetector;
    final static String TAG = "CalVR_Gesture";

    GestureDetectorCalVR(final Context ctx){
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
            public void onThreeFingerDoubleTap(){
                LayoutInflater inflater = LayoutInflater.from(ctx);
                final Activity activity = (Activity) ctx;
                View layout = inflater.inflate(R.layout.toast_layout, (ViewGroup) activity.findViewById(R.id.toast_root));

//                Toast toast = Toast.makeText(ctx, "testing", Toast.LENGTH_SHORT);
//                View view = toast.getView();

                TextView toastText = layout.findViewById(R.id.toast_text);
                ImageView toastImate = layout.findViewById(R.id.toast_image);
//                toastText.setText("DIFFERENT");

                Toast toast = new Toast(ctx);
                toast.setGravity(Gravity.CENTER_HORIZONTAL, 0,0);
                toast.setDuration(Toast.LENGTH_LONG);
                toast.setView(layout);
                toast.show();
            }
            public void onThreeFingerTripleTap(){}
            public void onThreeFingerLongPress(MotionEvent event){}
        };
    }
    public boolean onTouchEvent(MotionEvent event) {
        multiDetector.onTouchEvent(event);
        return true;
    }
}
