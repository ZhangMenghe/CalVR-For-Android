package com.samsung.arcalvr;

import android.app.Activity;
import android.support.design.widget.FloatingActionButton;
import android.view.View;
import android.widget.TextView;

import com.samsung.arcalvr.UI.MovableFloatingActionButton;

public class UIsController {
    protected Activity activity_;
    //Label
    protected TextView FPSlabel;
    protected MovableFloatingActionButton track_bnt;
    public UIsController(Activity activity){
        activity_ = activity;
        track_bnt = activity_.findViewById(R.id.main_button);
        track_bnt.bringToFront();

        track_bnt.addSubButton(activity_.findViewById(R.id.moveButton), -300f, -200f);
        track_bnt.addSubButton(activity_.findViewById(R.id.rotateButton), 300f, -200f);

        final FloatingActionButton trans_bnt = activity_.findViewById(R.id.moveButton);
        final FloatingActionButton move_bnt = activity_.findViewById(R.id.rotateButton);
        trans_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                trans_bnt.setAlpha(1.0f);
                move_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(3,view.getX(), view.getY());
            }
        });


        move_bnt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                move_bnt.setAlpha(1.0f);
                trans_bnt.setAlpha(0.5f);
                JniInterface.JNIonSingleTouchDown(4, view.getX(), view.getY());
            }
        });

        FPSlabel = (TextView) activity_.findViewById(R.id.textViewFPS);
        JUIonUICreated();
    }
    public void updateFPS(final float fFPS){
        if( FPSlabel == null )
            return;
        activity_.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                FPSlabel.setText(String.format("%2.2f FPS", fFPS));
            }});
    }
    public void popButtons(){
        activity_.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                track_bnt = activity_.findViewById(R.id.main_button);
                track_bnt.StartLockAnimation();
            }
        });
    }
    public static native void JUIonUICreated();
}
