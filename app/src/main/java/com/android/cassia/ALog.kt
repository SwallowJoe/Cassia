package com.android.cassia

import android.util.Log

object ALog {
    private val TAG_PREFIX = "cassia_"

    fun i(tag: String, content: String, e: Exception? = null) {
        Log.i("${TAG_PREFIX}${tag}", content, e)
    }

    fun d(tag: String, content: String, e: Exception? = null) {
        Log.d("${TAG_PREFIX}${tag}", content, e)
    }

    fun w(tag: String, content: String, e: Exception? = null) {
        Log.w("${TAG_PREFIX}${tag}", content, e)
    }

    fun e(tag: String, content: String, e: Exception? = null) {
        Log.e("${TAG_PREFIX}${tag}", content, e)
    }
}