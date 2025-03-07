package com.android.cassia.network

import okhttp3.Call
import okhttp3.Callback
import okhttp3.MediaType
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import java.io.IOException
import java.util.concurrent.CompletableFuture
import java.util.concurrent.Future


class OkHttpManager {
    private val client: OkHttpClient = OkHttpClient().newBuilder().build()

    fun request(url: String, mediaType: String, headers: HashMap<String, String>): Future<String?> {
        val result = CompletableFuture<String?>()
        val body = "".toRequestBody(mediaType.toMediaTypeOrNull())
        val requestBuilder: Request.Builder = Request.Builder()
            .url(url)
            .method("GET", body)
        headers.forEach { (name, value) -> requestBuilder.addHeader(name, value) }
        client.newCall(requestBuilder.build()).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                result.complete(null)
            }

            override fun onResponse(call: Call, response: Response) {
                response.body?.takeIf { response.isSuccessful }?.let { body ->
                    try {
                        result.complete(body.string())
                    } catch (e: IOException) {
                        result.complete(null)
                    }
                } ?: result.complete(null)
            }
        })

        return result
    }

    fun request(): Future<String?> {
        val result = CompletableFuture<String?>()
        val mediaType: MediaType? = "text/plain".toMediaTypeOrNull()
        val body = "".toRequestBody(mediaType)
        val request: Request = Request.Builder()
            .url("https://api.deepseek.com/models ")
            .method("GET", body)
            .addHeader("Accept", "application/json")
            .addHeader("Authorization", "Bearer <TOKEN>")
            .build()
        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                result.complete(null)
            }

            override fun onResponse(call: Call, response: Response) {
                response.body?.takeIf { response.isSuccessful }?.let { body ->
                    try {
                        result.complete(body.string())
                    } catch (e: IOException) {
                        result.complete(null)
                    }
                } ?: result.complete(null)
            }
        })

        return result
    }
}