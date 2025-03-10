package com.android.cassia.ui.ui.components

import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import com.mikepenz.markdown.m3.Markdown

@Composable
fun MarkdownViewer(content: MutableState<String>) {
    Markdown(content.value)
}