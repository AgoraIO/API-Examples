package io.agora.api.example.compose.model

import androidx.annotation.StringRes
import androidx.compose.runtime.Composable
import io.agora.api.example.compose.R
import io.agora.api.example.compose.samples.JoinChannelAudio
import io.agora.api.example.compose.samples.JoinChannelVideo
import io.agora.api.example.compose.samples.JoinChannelVideoToken
import io.agora.api.example.compose.samples.LiveStreaming

data class Example(
    @StringRes val name: Int,
    val description: String = "",
    val content: @Composable () -> Unit
)

val BasicExampleList = listOf(
    Example(R.string.example_join_channel_video_token) { JoinChannelVideoToken() },
    Example(R.string.example_join_channel_video) { JoinChannelVideo() },
    Example(R.string.example_join_channel_audio) { JoinChannelAudio() }
)

val AdvanceExampleList = listOf(
    Example(R.string.example_live_streaming) { LiveStreaming() },
)