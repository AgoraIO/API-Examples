package io.agora.api.example.examples.advanced.beauty.faceunity.utils.device;

/**
 * DESC：
 * Created on 2021/3/12
 */
public class MathUtils {
    public static double getScore(double num, double baseNum, double intervalScore, double intervalNum) {
        return (baseNum - num) * intervalScore / intervalNum;
    }
}
