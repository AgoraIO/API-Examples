package io.agora.api.example.examples.advanced.beauty.faceunity.utils.device;

/**
 * DESC：
 * Created on 2021/3/12
 */
interface DeviceScoreProvider {
    /**
     * get score for cpu
     *
     * @param cpuName String
     * @return double
     */
    double getCpuScore(String cpuName);

    /**
     * get score for gpu
     *
     * @param glRenderer String GPU名称
     * @return double
     */
    double getGpuScore(String glRenderer);
}
