package com.android.cassia

import java.util.concurrent.Callable
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.Future

object ExecutorPool {
    private val threadPool: ExecutorService by lazy { Executors.newCachedThreadPool() }

    /**
     * 提交一个任务到线程池，并返回一个 Future 对象。
     * @param task 任务函数，无参数无返回值
     * @return Future 对象，可用于跟踪任务执行状态
     */
    fun submit(task: () -> Unit): Future<*> { // 也可以泛型
        return threadPool.submit(task)
    }

    fun <T> submit(callable: Callable<T>): Future<T> {
        return threadPool.submit(callable)
    }

    fun <T> submit(runnable: Runnable, result: T): Future<T> {
        return threadPool.submit(runnable, result)
    }

    /**
     * 执行一个任务（无返回值）
     * @param task 任务函数，无参数无返回值
     */
    fun execute(task: () -> Unit) {
        threadPool.execute(task)
    }

    /**
     * 关闭线程池
     */
    fun shutdown() {
        threadPool.shutdown()
    }
}