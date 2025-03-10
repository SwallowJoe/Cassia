package com.android.cassia.models.deepseek.bean

import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable


@Serializable
data class ChatCompletion(
    val id: String,
    val choices: List<Choice>,
    val created: Long,
    val model: String,
    val system_fingerprint: String,
    @SerialName("object")
    val obj: String,
    val usage: Usage
)

@Serializable
data class Choice(
    val finish_reason: String,
    val index: Int,
    val message: Message,
    val logprobs: LogProbs
)

@Serializable
data class Message(
    val content: String,
    val reasoning_content: String,
    val tool_calls: List<ToolCall>,
    val role: String
)

@Serializable
data class ToolCall(
    val id: String,
    val type: String,
    val function: Function
)

@Serializable
data class Function(
    val name: String,
    val arguments: String
)

@Serializable
data class LogProbs(
    val content: List<LogProbContent>
)

@Serializable
data class LogProbContent(
    val token: String,
    val logprob: Double,
    val bytes: List<Int>,
    val top_logprobs: List<TopLogProb>
)

@Serializable
data class TopLogProb(
    val token: String,
    val logprob: Double,
    val bytes: List<Int>,
)

@Serializable
data class Usage(
    val completion_tokens: Int,
    val prompt_tokens: Int,
    val prompt_cache_hit_tokens: Int,
    val prompt_cache_miss_tokens: Int,
    val total_tokens: Int,
    val completion_tokens_details: CompletionTokensDetails
)

@Serializable
data class CompletionTokensDetails(
    val reasoning_tokens: Int
)