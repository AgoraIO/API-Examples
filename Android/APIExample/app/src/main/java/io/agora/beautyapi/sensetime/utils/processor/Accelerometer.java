/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.sensetime.utils.processor;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class Accelerometer {
    /**
     *
     * @author MatrixCV
     *
     * The CLOCKWISE_ANGLE represents the rotation angle of the phone, with Deg0 defined as shown in the diagram below.
     *  ___________________
     * | +--------------+  |
     * | |              |  |
     * | |              |  |
     * | |              | O|
     * | |              |  |
     * | |______________|  |
     * ---------------------
     * After rotating clockwise, the angle becomes Deg90, which represents the phone’s portrait orientation with the top facing up, as shown in the diagram below.
     *  ___________
     * |           |
     * |+---------+|
     * ||         ||
     * ||         ||
     * ||         ||
     * ||         ||
     * ||         ||
     * |+---------+|
     * |_____O_____|
     */
    public enum CLOCKWISE_ANGLE {
        Deg0(0), Deg90(1), Deg180(2), Deg270(3);
        private int value;
        private CLOCKWISE_ANGLE(int value){
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

    private SensorManager sensorManager = null;

    private boolean hasStarted = false;

    private CLOCKWISE_ANGLE rotation;

    private SensorEvent sensorEvent;

    /**
     * @param ctx
     * Initialize the sensor using the Activity
     */
    public Accelerometer(Context ctx) {
        sensorManager = (SensorManager) ctx
                .getSystemService(Context.SENSOR_SERVICE);
        rotation = CLOCKWISE_ANGLE.Deg90;
    }

    /**
     * Start listening to the sensor
     */
    public void start() {
        if (hasStarted) return;
        hasStarted = true;
        rotation = CLOCKWISE_ANGLE.Deg90;
        sensorManager.registerListener(accListener,
                sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_NORMAL);
    }

    /**
     * Stop listening to the sensor
     */
    public void stop() {
        if (!hasStarted) return;
        hasStarted = false;
        sensorManager.unregisterListener(accListener);
    }

    /**
     * @return
     * Returns the current orientation of the phone
     */
    public int getDirection() {
        return rotation.getValue();
    }

    public SensorEvent getSensorEvent() {
        return sensorEvent;
    }

    /**
     * The logic between the sensor and the phone's orientation
     */
    private SensorEventListener accListener = new SensorEventListener() {

        @Override
        public void onAccuracyChanged(Sensor arg0, int arg1) {
        }

        @Override
        public void onSensorChanged(SensorEvent arg0) {
            if (arg0.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                float x = arg0.values[0];
                float y = arg0.values[1];
                float z = arg0.values[2];
                if (Math.abs(x)>3 || Math.abs(y)>3) {
                    if (Math.abs(x)> Math.abs(y)) {
                        if (x > 0) {
                            rotation = CLOCKWISE_ANGLE.Deg0;
                        } else {
                            rotation = CLOCKWISE_ANGLE.Deg180;
                        }
                    } else {
                        if (y > 0) {
                            rotation = CLOCKWISE_ANGLE.Deg90;
                        } else {
                            rotation = CLOCKWISE_ANGLE.Deg270;
                        }
                    }
                }
            }
        }
    };
}
