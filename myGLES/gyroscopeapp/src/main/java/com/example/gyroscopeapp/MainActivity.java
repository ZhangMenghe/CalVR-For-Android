package com.example.gyroscopeapp;

import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.Matrix;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
public class MainActivity extends AppCompatActivity {
    final static String TAG = "GyroscopeAppMain";
    SensorManager sensor_manager;
    mySensorEventListener sensor_litener;
    private float timestamp;
    private static final float NS2S = 1.0f / 1000000000.0f;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupButtons();
        setupSensors();
    }
    protected void onStop(){
        super.onStop();
        sensor_manager.unregisterListener(sensor_litener);
    }
    private void setupButtons(){
        // add button actions
        final Button restart_bnt = (Button)findViewById(R.id.restart_button);
        restart_bnt.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                Intent restartIntent = getBaseContext().getPackageManager()
                        .getLaunchIntentForPackage(getBaseContext().getPackageName());
                restartIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(restartIntent);
            }
        });

        final Button debug_bnt = (Button)findViewById(R.id.debug_button);
        debug_bnt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                sensor_litener.getTrueAccelerate();
            }
        });
    }


    private class mySensorEventListener implements SensorEventListener{
        private float[] accDir = new float[4];
        private float[] R = new float[16];
        private float[] I = new float[16];
        private float[] RINV = new float[16];
        private float[] gravity_values = new float[3];
        private float[] magnetic_values = new float[3];
        private float[] linearAcc_values = new float[3];
        private float[] angle = new float[3];
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if(sensorEvent.sensor.getType() == Sensor.TYPE_GRAVITY)
                gravity_values = sensorEvent.values;
            else if(sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
                magnetic_values = sensorEvent.values;
            else if(sensorEvent.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION)
                linearAcc_values = sensorEvent.values;
            else if(sensorEvent.sensor.getType() == Sensor.TYPE_GYROSCOPE){
                if(timestamp!=0){
                    //旋转时间差
                    final float dT = (sensorEvent.timestamp -timestamp) * NS2S;
                    // 将手机在各个轴上的旋转角度相加，即可得到当前位置相对于初始位置的旋转弧度

                    angle[0] += sensorEvent.values[0] * dT;
                    angle[1] += sensorEvent.values[1] * dT;
                    angle[2] += sensorEvent.values[2] * dT;
// 将弧度转化为角度

                    float anglex = (float) Math.toDegrees(angle[0]);

                    float angley = (float) Math.toDegrees(angle[1]);

                    float anglez = (float) Math.toDegrees(angle[2]);
                    Log.e(TAG, "====" + anglex + "====" + angley+"=====" +anglez);
//                    gyroscopeSensor.getMinDelay();
                }
                timestamp = sensorEvent.timestamp;
            }
        }
        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
        public void getTrueAccelerate(){
            float [] gravity_dir = new float [4];
            SensorManager.getRotationMatrix(R, I, gravity_values, magnetic_values);
            for(int i=0;i<3;i++)
                gravity_dir[i] = gravity_values[i];
            gravity_dir[3] = 1.0f;
            Matrix.multiplyMV(gravity_dir, 0, R, 0, gravity_dir, 0);
//            Matrix.invertM(RINV, 0, R, 0);
//
            Log.e(TAG, "====" + gravity_dir[0] + "====" + gravity_dir[1]+"=====" +gravity_dir[2]);
        }
    }
    private void setupSensors(){
        // Get sensor manager
        sensor_manager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        Sensor gravity_sensor = sensor_manager.getDefaultSensor(Sensor.TYPE_GRAVITY);
        Sensor magnetic_sensor = sensor_manager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        Sensor linearAcc_sensor = sensor_manager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        Sensor gyro_sensor = sensor_manager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        sensor_litener = new mySensorEventListener();

        sensor_manager.registerListener(sensor_litener, gravity_sensor, SensorManager.SENSOR_DELAY_GAME);
        sensor_manager.registerListener(sensor_litener, magnetic_sensor, SensorManager.SENSOR_DELAY_GAME);
        sensor_manager.registerListener(sensor_litener, linearAcc_sensor, SensorManager.SENSOR_DELAY_GAME);
        sensor_manager.registerListener(sensor_litener, gyro_sensor, SensorManager.SENSOR_DELAY_GAME);
    }

}
