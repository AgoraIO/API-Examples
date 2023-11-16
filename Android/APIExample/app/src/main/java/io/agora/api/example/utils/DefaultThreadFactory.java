package io.agora.api.example.utils;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * DefaultThreadFactory
 *
 * @author zhilong <a href="mailto:zhilong.liu@aliyun.com">Contact me.</a>
 * @version 1.0
 * @since 15 /12/25 上午10:51
 */
public class DefaultThreadFactory implements ThreadFactory {
    private static final String TAG = DefaultThreadFactory.class.getSimpleName();

    private static final AtomicInteger POOL_NUMBER = new AtomicInteger(1);

    private final AtomicInteger threadNumber = new AtomicInteger(1);
    private final ThreadGroup group;
    private final String namePrefix;

    /**
     * Instantiates a new Default thread factory.
     */
    public DefaultThreadFactory() {
        SecurityManager s = System.getSecurityManager();
        group = (s != null) ? s.getThreadGroup()
                : Thread.currentThread().getThreadGroup();
        namePrefix = "ARouter task pool No." + POOL_NUMBER.getAndIncrement() + ", thread No.";
    }

    /**
     * New thread.
     * @param runnable a runnable to be executed by new thread instance
     * @return Thread
     */
    public Thread newThread(@NonNull Runnable runnable) {
        String threadName = namePrefix + threadNumber.getAndIncrement();
        Log.i(TAG, "Thread production, name is [" + threadName + "]");
        Thread thread = new Thread(group, runnable, threadName, 0);
        if (thread.isDaemon()) {   //Make non-background thread
            thread.setDaemon(false);
        }
        if (thread.getPriority() != Thread.NORM_PRIORITY) {
            thread.setPriority(Thread.NORM_PRIORITY);
        }

        // Catching exceptions in multi-threaded processing
        thread.setUncaughtExceptionHandler((thread1, ex) -> Log.i(TAG, "Running task appeared exception! Thread [" + thread1.getName() + "], because [" + ex.getMessage() + "]"));
        return thread;
    }
}