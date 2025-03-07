package com.android.cassia.models.deepseek

import com.android.cassia.models.deepseek.fake.FakeServer
import java.util.concurrent.CompletableFuture
import java.util.concurrent.Future

class CloudServer {
    fun getUserBalance(): Future<String?> {
        val future = CompletableFuture<String?>()
        future.complete(FakeServer.getBalanceInfo())
        return future
    }
}