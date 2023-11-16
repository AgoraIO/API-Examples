package io.agora.api.example.utils;

import android.util.Log;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionHandler;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Executors
 *
 * @version 1.0
 * @since 16 /4/28 下午4:07
 */
public final class DefaultPoolExecutor extends ThreadPoolExecutor {
    private static final String TAG = DefaultPoolExecutor.class.getSimpleName();
    //    Thread args
    private static final int CPU_COUNT = Runtime.getRuntime().availableProcessors();
    private static final int INIT_THREAD_COUNT = CPU_COUNT + 1;
    private static final int MAX_THREAD_COUNT = INIT_THREAD_COUNT;
    private static final long SURPLUS_THREAD_LIFE = 30L;

    private static DefaultPoolExecutor instance;

    /**
     * Gets instance.
     *
     * @return the instance
     */
    public static DefaultPoolExecutor getInstance() {
        if (null == instance) {
            synchronized (DefaultPoolExecutor.class) {
                if (null == instance) {
                    instance = new DefaultPoolExecutor(
                            INIT_THREAD_COUNT,
                            MAX_THREAD_COUNT,
                            SURPLUS_THREAD_LIFE,
                            TimeUnit.SECONDS,
                            new ArrayBlockingQueue<Runnable>(64),
                            new DefaultThreadFactory());
                }
            }
        }
        return instance;
    }

    private DefaultPoolExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue, ThreadFactory threadFactory) {
        super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue, threadFactory, new RejectedExecutionHandler() {
            @Override
            public void rejectedExecution(Runnable r, ThreadPoolExecutor executor) {
                Log.e(TAG, "Task rejected, too many task!");
            }
        });
    }

    /* thread execution complete, handle possible exceptions.
     * @param r the runnable that has completed
     * @param t the exception that caused termination, or null if
     */
    @Override
    protected void afterExecute(Runnable r, Throwable t) {
        super.afterExecute(r, t);
        if (t == null && r instanceof Future<?>) {
            try {
                ((Future<?>) r).get();
            } catch (CancellationException ce) {
                t = ce;
            } catch (ExecutionException ee) {
                t = ee.getCause();
            } catch (InterruptedException ie) {
                Thread.currentThread().interrupt(); // ignore/reset
            }
        }
        if (t != null) {
            Log.w(TAG, "Running task appeared exception! Thread [" + Thread.currentThread().getName() + "], because [" + t.getMessage() + "]\n" + TextUtils.formatStackTrace(t.getStackTrace()));
        }
    }
}
