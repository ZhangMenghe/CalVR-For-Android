package lapras.testapp;

import android.app.Activity;

import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.AdapterView;

import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class UIsController {
    Activity activity;
    final static String TAG = "UIsController";

    public static TextView FPSlabel, toggleValueTex;
    public static View raycastPanel, cutPanel;
    private Spinner spinner_toggle, spinner_toggle_sub, spinner_switch;
    private SeekBar seekbar_toggle, seekbar_cut;
    private Switch switch_widget;
    public static boolean b_naiverenderer = false;
    final public static float MAX_SAMPLE_STEP_REPRESENT = 800.0f, MAX_THRESHOLD_REPRESENT = 2.0f,
                              MAX_BRIGHTNESS_REPRESENT=500.0f, DENSITY_CUTTING_REPRESENT = 50.0f, MAX_OPACITY_REPRESENT = 1.0f;
    final public static int MAX_SAMPLE_STEP = 800, MAX_THRESHOLD = 50,
            MAX_BRIGHTNESS=500, DENSITY_CUTTING = 50, ORIGINAL_CUT = 0;

    private enum TOGGLE_VALUE{
        SAMPLE_STEP, THRESHOLD, BRIGHTNESS, OPACITY
    };
    private enum SWITCH_VALUE{
        TRANSFER_COLOR, USE_LIGHTING, USE_INTERPOLATION, SIMPLE_CUBE, NAIVE_RENDERER, CUTTING
    }

    private int toggle_id = 0, toggle_id_sub = 0, switch_id = 0;
    private float toggle_values[], toggle_values_sub[];

    private boolean bool_values[];
    private List<float[]> tvalues = new ArrayList<float[]>();
    public static TranslateAnimation panelHiddenAction, panelShownAction;
    static {
        panelHiddenAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF,
                0.0f, Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                -1.0f);
        panelHiddenAction.setDuration(500);

        panelShownAction = new TranslateAnimation(Animation.RELATIVE_TO_SELF, 0.0f,
                Animation.RELATIVE_TO_SELF, 0.0f, Animation.RELATIVE_TO_SELF,
                -1.0f, Animation.RELATIVE_TO_SELF, 0.0f);
        panelShownAction.setDuration(500);
    }

    private void init_values(){
        toggle_values = new float[TOGGLE_VALUE.values().length];
        toggle_values_sub = new float[3];
        bool_values = new boolean[SWITCH_VALUE.values().length];
        ///get initial_value here
        for(int i=0; i<TOGGLE_VALUE.values().length; i++){
            toggle_values[i] = JNIInterface.JNIgetOriginalValue(i);
            if(i == 1)
                toggle_values[i] *= MAX_THRESHOLD / MAX_THRESHOLD_REPRESENT;
            if(i == 3){
                toggle_values[i] = -100;
                for(int j=0; j<3; j++)
                    toggle_values_sub[j] = JNIInterface.JNIgetOriginalValue(100 + j) * MAX_THRESHOLD / MAX_OPACITY_REPRESENT;

            }

        }

        seekbar_toggle.setMax(MAX_SAMPLE_STEP);
        if(toggle_values[toggle_id] < 0)
            seekbar_toggle.setProgress((int)toggle_values[toggle_id]);
        else
            seekbar_toggle.setProgress((int)toggle_values_sub[toggle_id_sub]);

        for(int i=0; i<SWITCH_VALUE.values().length; i++)
            bool_values[i] = JNIInterface.JNIgetOriginalChecked(i);
        switch_widget.setChecked(bool_values[switch_id]);

        //naive hide panel
        if(bool_values[4])
            ToggleShowView();

        seekbar_cut.setMax(DENSITY_CUTTING);
        seekbar_cut.setProgress(ORIGINAL_CUT);
        //init cut panel
        if(bool_values[5]){// show cut
            cutPanel.setVisibility(View.VISIBLE);
        }else{
            cutPanel.setVisibility(View.GONE);
        }

    }
    public UIsController(final Activity activity_){
        activity = activity_;
        //panels
        raycastPanel = activity.findViewById(R.id.raycastPanel);
        //fps monitor
        FPSlabel = (TextView) activity.findViewById(R.id.textViewFPS);
        //toggles
        spinner_toggle = (Spinner)activity.findViewById(R.id.toggleSpinner);
        spinner_toggle.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                toggle_id = i;
                update_toggle_item_display();
                JNIInterface.JNIsetJavaUIStatus(0, toggle_id);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        spinner_toggle_sub = (Spinner)activity.findViewById(R.id.toggleSubSpinner);
        spinner_toggle_sub.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                toggle_id_sub = i;
                update_toggle_item_display();}
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {}
        });

        seekbar_toggle = (SeekBar) activity.findViewById(R.id.toggleSeekBar);
        toggleValueTex = (TextView) activity.findViewById(R.id.toggleText);
        seekbar_toggle.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                if(TOGGLE_VALUE.values()[toggle_id] == TOGGLE_VALUE.OPACITY)
                    toggle_values_sub[toggle_id_sub] = (float)i;
                else
                    toggle_values[toggle_id] = (float)i;
                update_toggle_item_display();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        //switches
        spinner_switch = (Spinner)activity.findViewById(R.id.switchSpinner);
        spinner_switch.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                switch_id = i;
                JNIInterface.JNIsetJavaUIStatus(1, switch_id);
                update_switch_item_display();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        switch_widget = (Switch)activity.findViewById(R.id.switcherSwitch);
        switch_widget.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(!compoundButton.isPressed()) return;
                bool_values[switch_id] = !bool_values[switch_id];
                update_switch_item_display();
            }
        });
        //cut
        seekbar_cut = (SeekBar)activity.findViewById(R.id.cuttingSeekBar);
        seekbar_cut.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(!b) return;
                JNIInterface.JNIsetParam(-1, 1.0f * i / DENSITY_CUTTING);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        cutPanel = (View)activity.findViewById(R.id.cutPanel);
        init_values();

    }
    private void update_toggle_item_display(){
        spinner_toggle_sub.setVisibility(View.GONE);
        switch (TOGGLE_VALUE.values()[toggle_id]){
            case SAMPLE_STEP:
                seekbar_toggle.setMax(MAX_SAMPLE_STEP);
                seekbar_toggle.setProgress((int)toggle_values[toggle_id]);
                toggleValueTex.setText(activity.getString(R.string.text_sample, toggle_values[toggle_id]));
                JNIInterface.JNIsetParam(toggle_id, toggle_values[toggle_id]);
                break;
            case THRESHOLD:
                seekbar_toggle.setMax(MAX_THRESHOLD);
                seekbar_toggle.setProgress((int)toggle_values[toggle_id]);
                float actual_value = MAX_THRESHOLD_REPRESENT/ MAX_THRESHOLD * toggle_values[toggle_id];
                toggleValueTex.setText(activity.getString(R.string.text_threshold, actual_value));
                JNIInterface.JNIsetParam(toggle_id, actual_value);
                break;
            case BRIGHTNESS:
                seekbar_toggle.setMax(MAX_BRIGHTNESS);
                seekbar_toggle.setProgress((int)toggle_values[toggle_id]);
                toggleValueTex.setText(activity.getString(R.string.text_brightness, toggle_values[toggle_id]));
                JNIInterface.JNIsetParam(toggle_id, toggle_values[toggle_id]);
                break;
            case OPACITY:
                seekbar_toggle.setMax(MAX_THRESHOLD);
                seekbar_toggle.setProgress((int)toggle_values_sub[toggle_id_sub]);
                float opacity_adj_real = MAX_OPACITY_REPRESENT/ MAX_THRESHOLD * toggle_values_sub[toggle_id_sub];
                toggleValueTex.setText(activity.getString(R.string.text_opacity_threshold, opacity_adj_real));
                JNIInterface.JNIsetParam(toggle_id_sub + 100, opacity_adj_real);
                spinner_toggle_sub.setVisibility(View.VISIBLE);
                break;
                default:
                    break;
        }
    }
    private void update_switch_item_display(){
        boolean cvalue = bool_values[switch_id];
        switch (SWITCH_VALUE.values()[switch_id]){
            case CUTTING:
                if(cvalue){
                    cutPanel.setVisibility(View.VISIBLE);
                    Toast.makeText(activity, "Use Bottom SeekBar to Cut", Toast.LENGTH_LONG).show();
                }
                else cutPanel.setVisibility(View.GONE);
                break;
            case NAIVE_RENDERER:
                if(cvalue) ToggleShowView();
                break;
                default:
                    break;
        }
        switch_widget.setChecked(cvalue);
        JNIInterface.JNIsetSwitches(switch_id, cvalue);
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
    public static void ToggleShowView(){
        //do nothing
    }
    public static void ToggleShowView_animate(){
        if(raycastPanel.getVisibility() == View.VISIBLE){
            raycastPanel.startAnimation(panelHiddenAction);
            raycastPanel.setVisibility(View.GONE);
        }else{
            raycastPanel.startAnimation(panelShownAction);
            raycastPanel.setVisibility(View.VISIBLE);
        }
    }
}
