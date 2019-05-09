package lapras.testapp;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

public class UIsController {
    TextView FPSlabel;
    TextView text_param1, text_param2, text_param3;
    View raycastPanel;

    Activity activity;

    public UIsController(final Activity activity_){
        activity = activity_;
        //panels
        raycastPanel = activity.findViewById(R.id.raycastPanel);
        raycastPanel.setVisibility(View.INVISIBLE);
        //fps monitor
        FPSlabel = (TextView) activity.findViewById(R.id.textViewFPS);
        //next button
        Button render_bnt = (Button)activity.findViewById(R.id.changeRenderButton);
        render_bnt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(JNIInterface.JNIchangeRender())
                    raycastPanel.setVisibility(View.VISIBLE);
                else
                    raycastPanel.setVisibility(View.INVISIBLE);
            }
        });

        //switches
        Switch sw_trans = (Switch)activity.findViewById(R.id.switch_color);
        sw_trans.setChecked(JNIInterface.JNIgetOriginalChecked(0));
        sw_trans.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                JNIInterface.JNIsetSwitches(0, b);
            }
        });
        Switch sw_lighting = (Switch)activity.findViewById(R.id.switch_lighting);
        sw_lighting.setChecked(JNIInterface.JNIgetOriginalChecked(1));
        sw_lighting.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                JNIInterface.JNIsetSwitches(1, b);
            }
        });


        text_param1 = activity.findViewById(R.id.text_param1);
        float initialValue_p1 = JNIInterface.JNIgetOriginalValue(0);
        text_param1.setText(activity.getString(R.string.text_param1, initialValue_p1));
        SeekBar sb1 = (SeekBar)activity.findViewById(R.id.seekbar_param1);
        sb1.setProgress((int)initialValue_p1);
        sb1.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                text_param1.setText(activity.getString(R.string.text_param1, (float)i));
                JNIInterface.JNIsetParam(0, (float)i);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        text_param2 = activity.findViewById(R.id.text_param2);
        float initialValue_p2 = JNIInterface.JNIgetOriginalValue(1);
        text_param2.setText(activity.getString(R.string.text_param2, initialValue_p2));
        SeekBar sb2 = (SeekBar)activity.findViewById(R.id.seekbar_param2);
        sb2.setProgress((int)(initialValue_p2/ 2.0f * 20));

        sb2.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                float actual_value = i * 0.1f;
                text_param2.setText(activity.getString(R.string.text_param2, actual_value));
                JNIInterface.JNIsetParam(1, actual_value);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });


        text_param3 = activity.findViewById(R.id.text_param3);
        float initialValue_p3 = JNIInterface.JNIgetOriginalValue(2);
        text_param3.setText(activity.getString(R.string.text_param3, initialValue_p3));
        SeekBar sb3 = (SeekBar)activity.findViewById(R.id.seekbar_param3);
        sb3.setProgress((int)initialValue_p3);
        sb3.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                text_param3.setText(activity.getString(R.string.text_param3, (float)i));
                JNIInterface.JNIsetParam(2, (float)i);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    public void updateFPS(final float fFPS)
    {
        if( FPSlabel == null )
            return;
        activity.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                FPSlabel.setText(String.format("%2.2f FPS", fFPS));
            }});
    }
}
