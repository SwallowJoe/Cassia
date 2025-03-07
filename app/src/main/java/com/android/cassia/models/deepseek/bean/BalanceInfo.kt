package com.android.cassia.models.deepseek.bean

import kotlinx.serialization.Serializable


@Serializable
data class BalanceInfo(
    val is_available: Boolean,
    val balance_infos: List<BalanceInfo>
)