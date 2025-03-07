package com.android.cassia.base

interface IResponse<T> {
    fun onResponse(result: T?)
}