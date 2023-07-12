package io.agora.beautyapi.sensetime.utils.utils

import java.util.concurrent.Callable
import java.util.concurrent.Executors
import java.util.concurrent.Future
import kotlin.math.max
import kotlin.math.min

class QueueBlockThreadExecutor<T>(
    private val nQueues: Int,
    private val nThreads: Int
) {

    private val executor = Executors.newFixedThreadPool(min(nThreads, max(nQueues, 1)))
    private val futureArray = arrayOfNulls<Future<T>>(nQueues)

    @Volatile
    private var index = 0

    fun current() = index

    fun execute(task: Callable<T>, beforeSubmit: ((T?) -> Unit)? = null): T? {
        val ret: T?
        if (nQueues <= 0) {
            val future = executor.submit(task)
            ret = future.get()
        } else {
            val future = futureArray[index]

            if (future == null) {
                ret = null
                beforeSubmit?.invoke(ret)
                futureArray[index] = executor.submit(task)

            } else {
                ret = future.get()
                beforeSubmit?.invoke(ret)
                futureArray[index] = executor.submit(task)
            }
            index = (index + 1) % nQueues
        }
        return ret
    }

    fun cleanAllTasks() {
        futureArray.forEachIndexed { index, future ->
            future?.cancel(true)
            futureArray[index] = null
        }
        index = 0
    }

    fun shutdown() {
        futureArray.forEachIndexed { index, _ -> futureArray[index] = null }
        index = 0
        executor.shutdownNow()
    }


}