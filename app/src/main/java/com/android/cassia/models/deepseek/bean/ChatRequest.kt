package com.android.cassia.models.deepseek.bean

import kotlinx.serialization.Serializable

@Serializable
data class ChatRequest(
    val messages: List<RequestMessage>,
    val model: String,
    val frequency_penalty: Double? = null,
    val max_tokens: Int? = null,
    val presence_penalty: Double? = null,
    val response_format: ResponseFormat? = null,
    val stop: List<String>? = null,
    val stream: Boolean? = null,
    val stream_options: StreamOptions? = null,
    val temperature: Double = 1.0,
    val top_p: Double = 1.0,
    val tools: List<Tool>? = null
)

@Serializable
data class RequestMessage(
    val content: String,
    val role: String,
    val name: String? = null
)

@Serializable
data class ResponseFormat(
    val type: String
)

@Serializable
data class StreamOptions(
    val include_usage: Boolean
)

@Serializable
data class Tool(
    val type: String,
    val function: ToolFunction
)

@Serializable
data class ToolFunction(
    val description: String,
    val name: String,
    // TODO: parameters...
)