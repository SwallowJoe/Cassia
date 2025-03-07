package com.android.cassia.models.deepseek

import com.android.cassia.ALog
import com.android.cassia.models.deepseek.bean.BalanceInfo
import kotlinx.serialization.json.Json

object ResponseDecoder {
    private const val TAG = "ResponseDecoder"

    fun decodeBalanceInfo(content: String): BalanceInfo? {
        try {
            return Json.decodeFromString<BalanceInfo>(content)
        } catch (e: Exception) {
            ALog.e(TAG, "decodeBalanceInfo error[$content]", e)
        }
        return null
    }
}