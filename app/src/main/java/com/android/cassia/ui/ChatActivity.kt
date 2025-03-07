package com.android.cassia.ui

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.android.cassia.ui.ui.theme.CassiaTheme


class ChatActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            CassiaTheme {
                MyActivity()
            }
        }
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}


@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MyActivity() {
    val message = remember { mutableStateOf("") }

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("新对话") },
                actions = {
                    IconButton(onClick = { /*ToDo*/ }) {
                        Icon(Icons.Filled.Add, "添加")
                    }
                }
            )
        },
        bottomBar = {
            BottomAppBar() {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(8.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    OutlinedTextField(
                        value = message.value,
                        onValueChange = { newValue ->
                            message.value = newValue // 更新消息内容
                        },
                        modifier = Modifier.fillMaxWidth(),
                        label = { Text("输入消息") }
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Button(onClick = {
                        // 发送消息
                        message.value = ""
                    }) {
                        Text("发送")
                    }
                }
            }
        }
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding),
        ) {
            Spacer(modifier = Modifier.weight(1f))
            LazyColumn(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(8f),
                reverseLayout = true
            ) {
                items(10) {
                    MessageBubble(
                        message = "这是一个测试消息。",
                        isSentByUser = it % 2 == 0
                    )
                }
            }
            Spacer(modifier = Modifier.weight(1f))
        }
    }
}

@Composable
fun MessageBubble(
    message: String,
    isSentByUser: Boolean
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(8.dp),
        horizontalArrangement = if (isSentByUser) Arrangement.End else Arrangement.Start
    ) {
        Box(
            modifier = Modifier
                .background(
                    color = if (isSentByUser) MaterialTheme.colorScheme.primary else Color.LightGray,
                    shape = MaterialTheme.shapes.medium
                )
                .padding(8.dp)
        ) {
            Column {
                Text("已深度思考（用时 5 秒）")
                Text(
                    text = message,
                    modifier = Modifier.padding(4.dp)
                )
                // 添加 LaTeX 公式显示
                Text(text = "LaTeX 公式: \\(E=mc^2\\)", style = MaterialTheme.typography.bodyMedium)
            }
        }
    }
}

@Composable
fun MarkdownViewer() {
    val markdownContent = """
        # Demo

        Emphasis, aka italics, with *asterisks* or _underscores_. Strong emphasis, aka bold, with **asterisks** or __underscores__. Combined emphasis with **asterisks and _underscores_**. [Links with two blocks, text in square-brackets, destination is in parentheses.](https://www.example.com). Inline `code` has `back-ticks around` it.

        1. First ordered list item
        2. Another item
            * Unordered sub-list.
        3. And another item.
            You can have properly indented paragraphs within list items. Notice the blank line above, and the leading spaces (at least one, but we'll use three here to also align the raw Markdown).

        * Unordered list can use asterisks
        - Or minuses
        + Or pluses
        ---

        ```javascript
        var s = "code blocks use monospace font";
        alert(s);
        ```

        Markdown | Table | Extension
        --- | --- | ---
        *renders* | `beautiful images` | ![random image](https://picsum.photos/seed/picsum/400/400 "Text 1")
        1 | 2 | 3

        > Blockquotes are very handy in email to emulate reply text.
        > This line is part of the same quote.
    """.trimIndent()

    Markdown(markdownContent)
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    CassiaTheme {
        MyActivity()
    }
}