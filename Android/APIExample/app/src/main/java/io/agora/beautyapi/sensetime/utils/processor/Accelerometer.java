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

/**
 * The type Accelerometer.
 */
public class Accelerometer {
    /**
     * The enum Clockwise angle.
     *
     * @author MatrixCV
     *
     * CLOCKWISE_ANGLE为手机旋转角度
     * 其Deg0定义如下图所示
     *  ___________________
     * | +--------------+  |
     * | |              |  |
     * | |              |  |
     * | |              | O|
     * | |              |  |
     * | |______________|  |
     * ---------------------
     * 顺时针旋转后得到Deg90，即手机竖屏向上，如下图所示
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
    public enum ClockwiseAngle {
        /**
         * Deg 0 clockwise angle.
         */
        Deg0(0),
        /**
         * Deg 90 clockwise angle.
         */
        Deg90(1),
        /**
         * Deg 180 clockwise angle.
         */
        Deg180(2),
        /**
         * Deg 270 clockwise angle.
         */
        Deg270(3);
        private int value;

        ClockwiseAngle(int value) {
            this.value = value;
        }

        /**
         * Gets value.
         *
         * @return the value
         */
        public int getValue() {
            return value;
        }
    }

    private SensorManager sensorManager = null;

    private boolean hasStarted = false;

    private ClockwiseAngle rotation;

    private SensorEvent sensorEvent;

    /**
     * Instantiates a new Accelerometer.
     *
     * @param ctx 用Activity初始化获得传感器
     */
    public Accelerometer(Context ctx) {
        sensorManager = (SensorManager) ctx
                .getSystemService(Context.SENSOR_SERVICE);
        rotation = ClockwiseAngle.Deg90;
    }

    /**
     * 开始对传感器的监听
     */
    public void start() {
        if (hasStarted) {
            return;
        }
        hasStarted = true;
        rotation = ClockwiseAngle.Deg90;
        sensorManager.registerListener(accListener,
                sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_NORMAL);
    }

    /**
     * 结束对传感器的监听
     */
    public void stop() {
        if (!hasStarted) {
            return;
        }
        hasStarted = false;
        sensorManager.unregisterListener(accListener);
    }

    /**
     * Gets direction.
     *
     * @return 返回当前手机转向 direction
     */
    public int getDirection() {
        return rotation.getValue();
    }

    /**
     * Gets sensor event.
     *
     * @return the sensor event
     */
    public SensorEvent getSensorEvent() {
        return sensorEvent;
    }

    /**
     * 传感器与手机转向之间的逻辑
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
                if (Math.abs(x) > 3 || Math.abs(y) > 3) {
                    if (Math.abs(x) > Math.abs(y)) {
                        if (x > 0) {
                            rotation = ClockwiseAngle.Deg0;
                        } else {
                            rotation = ClockwiseAngle.Deg180;
                        }
                    } else {
                        if (y > 0) {
                            rotation = ClockwiseAngle.Deg90;
                        } else {
                            rotation = ClockwiseAngle.Deg270;
                        }
                    }
                }
            }
        }
    };
}
