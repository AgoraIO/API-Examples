package io.agora.api.example.compose.model

import androidx.compose.runtime.Composable
import io.agora.api.example.compose.samples.JoinVideoChannel

data class Example(
    val name: String,
    val description: String,
    val sourceUrl: String,
    val content: @Composable () -> Unit
)

val BasicExampleList = listOf(
    Example(
        "Join a Channel",
        "Join a channel to start or join a video call.",
        ""
    ){
        JoinVideoChannel()
    }
)