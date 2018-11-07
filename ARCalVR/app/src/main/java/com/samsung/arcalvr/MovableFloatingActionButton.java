package com.samsung.arcalvr;

import android.animation.ObjectAnimator;
import android.animation.PropertyValuesHolder;
import android.content.Context;
import android.support.design.widget.FloatingActionButton;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import java.util.HashMap;
import java.util.Map;

public class MovableFloatingActionButton extends FloatingActionButton implements View.OnTouchListener {

    private final static float CLICK_DRAG_TOLERANCE = 10; // Often, there will be a slight, unintentional, drag when the user taps the FAB, so we need to account for this.
    private float _originX, _originY;
    private float downRawX, downRawY;
    private float dX, dY;
    private Map<FloatingActionButton, ObjectAnimator> sub_buttons;
    private boolean islocked = false;

    public MovableFloatingActionButton(Context context) {
        super(context);
        init();
    }

    public MovableFloatingActionButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public MovableFloatingActionButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        _originX = 540;
        _originY = 1980;
        setOnTouchListener(this);
        sub_buttons = new HashMap<>();
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {

        int action = motionEvent.getAction();
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                downRawX = motionEvent.getRawX();
                downRawY = motionEvent.getRawY();
                dX = view.getX() - downRawX;
                dY = view.getY() - downRawY;
                RayCast(motionEvent.getX(), motionEvent.getY());
                break;
            case MotionEvent.ACTION_MOVE:
                int viewWidth = view.getWidth();
                int viewHeight = view.getHeight();
//
//                View viewParent = (View) view.getParent();
//                int parentWidth = viewParent.getWidth();
//                int parentHeight = viewParent.getHeight();
//
//                float newX = motionEvent.getRawX() + dX;
//                newX = Math.max(0, newX); // Don't allow the FAB past the left hand side of the parent
//                newX = Math.min(parentWidth - viewWidth, newX); // Don't allow the FAB past the right hand side of the parent
//
//                float newY = motionEvent.getRawY() + dY;
//                newY = Math.max(0, newY); // Don't allow the FAB past the top of the parent
//                newY = Math.min(parentHeight - viewHeight, newY); // Don't allow the FAB past the bottom of the parent
                RayMove(getX() +viewWidth/2, getY()+viewHeight/2);
//                view.animate()
//                        .x(newX)
//                        .y(newY)
//                        .setDuration(0)
//                        .start();
                break;
            case MotionEvent.ACTION_UP:
                RayCastEnd(motionEvent.getX(), motionEvent.getY());
//                this.setImageResource(R.drawable.pokeball_empty);
                float upRawX = motionEvent.getRawX();
                float upRawY = motionEvent.getRawY();

                float upDX = upRawX - downRawX;
                float upDY = upRawY - downRawY;

                if (Math.abs(upDX) < CLICK_DRAG_TOLERANCE && Math.abs(upDY) < CLICK_DRAG_TOLERANCE) { // A click
                    return performClick();
                } else { // A drag
                    return true; // Consumed
                }

            default:
                super.onTouchEvent(motionEvent);
        }
        return true;
    }
    private void RayMove(float x, float y){JniInterface.JNIonTouchMove(2, x, y);}
    private void RayCast(float x, float y){
        StartUnLockAnimation();
        JniInterface.JNIonSingleTouchDown(2, x, y);
    }
    private void RayCastEnd(float x, float y ){
        JniInterface.JNIonSingleTouchUp(2, x, y);
    }
    private void StartUnLockAnimation(){
        if(!islocked) return;
        for (Map.Entry<FloatingActionButton, ObjectAnimator> entry : sub_buttons.entrySet()){
            entry.getKey().setVisibility(INVISIBLE);
        }
        setImageResource(R.drawable.pokeball_empty);
        islocked = !islocked;
    }
    public void StartLockAnimation(){
        if(islocked) return;
        for (Map.Entry<FloatingActionButton, ObjectAnimator> entry : sub_buttons.entrySet()){
            entry.getKey().setVisibility(VISIBLE);
            entry.getValue().start();
        }
        setImageResource(R.drawable.pokeball);
        islocked = !islocked;
    }

//    public void startAnimation(){
//        if(!islocked){
//            for (Map.Entry<FloatingActionButton, ObjectAnimator> entry : sub_buttons.entrySet()){
//                entry.getKey().setVisibility(VISIBLE);
//                entry.getValue().start();
//            }
//            setImageResource(R.drawable.pokeball);
//        }else{
//            for (Map.Entry<FloatingActionButton, ObjectAnimator> entry : sub_buttons.entrySet()){
//                entry.getKey().setVisibility(INVISIBLE);
//            }
//            setImageResource(R.drawable.pokeball_empty);
//        }
//        islocked = !islocked;
//    }
    public void addSubButton(View bnt, float offset_x, float offset_y){
        bnt.setVisibility(INVISIBLE);

        PropertyValuesHolder holderX = PropertyValuesHolder.ofFloat("translationX", getX(), offset_x);
        PropertyValuesHolder holderY = PropertyValuesHolder.ofFloat("translationY", getY(), offset_y);

        ObjectAnimator animation = ObjectAnimator.ofPropertyValuesHolder(bnt, holderX, holderY);
        animation.setDuration(1500);

        sub_buttons.put((FloatingActionButton) bnt, animation);
    }
}
