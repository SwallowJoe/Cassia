package com.android.cassia.models.deepseek

import com.android.cassia.ExecutorPool
import com.android.cassia.base.IResponse
import com.android.cassia.models.deepseek.bean.BalanceInfo
import java.util.concurrent.TimeUnit

class DeepSeekManager {
    private val mCloudServer = CloudServer()

    fun listAllModels(): List<String>? {
        return null
    }

    fun getUserBalance(callback: IResponse<BalanceInfo?>) {
        ExecutorPool.submit {
            // 1. fetch balance info from server
            val content = mCloudServer.getUserBalance().get(5, TimeUnit.SECONDS)
            if (content.isNullOrEmpty()) {
                callback.onResponse(null)
                return@submit
            }
            // 2. decode content
            val balanceInfo = ResponseDecoder.decodeBalanceInfo(content)
            callback.onResponse(balanceInfo)
        }
    }
}