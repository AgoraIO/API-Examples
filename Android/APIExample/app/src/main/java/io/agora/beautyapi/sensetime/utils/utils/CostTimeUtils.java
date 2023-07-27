package io.agora.beautyapi.sensetime.utils.utils;

import android.util.Log;

import java.util.ArrayList;
import java.util.LongSummaryStatistics;

/**
 * @Description
 * @Author Lu Guoqiang
 * @Time 6/7/21 5:50 PM
 */
public class CostTimeUtils {

    private static ArrayList<Long> runTimesList = new ArrayList<>();

    // 50次的平均耗时
    private static final int MAX_RUN_COUNT = 100;

    public static void printAverage(final String TAG, long runTime) {
        if (runTimesList == null) runTimesList = new ArrayList<>();
        if (runTimesList.size() < MAX_RUN_COUNT) runTimesList.add(runTime);
        if (runTimesList.size() == MAX_RUN_COUNT) {
            double averageTime;
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
                LongSummaryStatistics stats = runTimesList.stream().mapToLong((x) -> x).summaryStatistics();
                averageTime = stats.getAverage();
            } else {
                long sumTime = 0;
                for (Long time : runTimesList) {
                    sumTime = sumTime + time;
                }
                averageTime = sumTime / MAX_RUN_COUNT;
            }
            Log.i(TAG, "human action cost time average:" + averageTime);
            runTimesList.clear();
        }
    }
}
