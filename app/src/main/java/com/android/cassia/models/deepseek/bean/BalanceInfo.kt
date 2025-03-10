package com.android.cassia.models.deepseek.bean

import kotlinx.serialization.Serializable


@Serializable
data class BalanceInfo(
    val is_available: Boolean,
    val balance_infos: List<BalanceInfo>
)

@Serializable
data class BalanceDetail(
    val currency: String,
    val total_balance: String,
    val granted_balance: String,
    val topped_up_balance: String
)