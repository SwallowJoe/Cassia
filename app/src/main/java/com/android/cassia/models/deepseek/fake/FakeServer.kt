package com.android.cassia.models.deepseek.fake

object FakeServer {

    fun getBalanceInfo(): String {
        return """
        {
          "is_available": true,
          "balance_infos": [
            {
              "currency": "CNY",
              "total_balance": "110.00",
              "granted_balance": "10.00",
              "topped_up_balance": "100.00"
            }
          ]
        }
    """.trimIndent()
    }

    fun chatRequest(): String {
        return """
            {
              "messages": [
                {
                  "content": "You are a helpful assistant",
                  "role": "system"
                },
                {
                  "content": "Hi",
                  "role": "user"
                }
              ],
              "model": "deepseek-chat",
              "frequency_penalty": 0,
              "max_tokens": 2048,
              "presence_penalty": 0,
              "response_format": {
                "type": "text"
              },
              "stop": null,
              "stream": false,
              "stream_options": null,
              "temperature": 1,
              "top_p": 1,
              "tools": null,
              "tool_choice": "none",
              "logprobs": false,
              "top_logprobs": null
            }
        """.trimIndent()
    }

    fun chat(): String {
        return """
            {
              "id": "string",
              "choices": [
                {
                  "finish_reason": "stop",
                  "index": 0,
                  "message": {
                    "content": "string",
                    "reasoning_content": "string",
                    "tool_calls": [
                      {
                        "id": "string",
                        "type": "function",
                        "function": {
                          "name": "string",
                          "arguments": "string"
                        }
                      }
                    ],
                    "role": "assistant"
                  },
                  "logprobs": {
                    "content": [
                      {
                        "token": "string",
                        "logprob": 0,
                        "bytes": [
                          0
                        ],
                        "top_logprobs": [
                          {
                            "token": "string",
                            "logprob": 0,
                            "bytes": [
                              0
                            ]
                          }
                        ]
                      }
                    ]
                  }
                }
              ],
              "created": 0,
              "model": "string",
              "system_fingerprint": "string",
              "object": "chat.completion",
              "usage": {
                "completion_tokens": 0,
                "prompt_tokens": 0,
                "prompt_cache_hit_tokens": 0,
                "prompt_cache_miss_tokens": 0,
                "total_tokens": 0,
                "completion_tokens_details": {
                  "reasoning_tokens": 0
                }
              }
            }
        """.trimIndent()
    }
}