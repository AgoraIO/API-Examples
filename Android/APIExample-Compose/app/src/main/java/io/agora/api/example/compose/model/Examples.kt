package io.agora.api.example.compose.model

import androidx.annotation.StringRes
import androidx.compose.runtime.Composable
import io.agora.api.example.compose.R
import io.agora.api.example.compose.samples.ChannelEncryption
import io.agora.api.example.compose.samples.CustomAudioRender
import io.agora.api.example.compose.samples.CustomAudioSource
import io.agora.api.example.compose.samples.CustomVideoRender
import io.agora.api.example.compose.samples.CustomVideoSource
import io.agora.api.example.compose.samples.HostAcrossChannel
import io.agora.api.example.compose.samples.JoinChannelAudio
import io.agora.api.example.compose.samples.JoinChannelVideo
import io.agora.api.example.compose.samples.JoinChannelVideoToken
import io.agora.api.example.compose.samples.JoinMultiChannel
import io.agora.api.example.compose.samples.LiveStreaming
import io.agora.api.example.compose.samples.LocalVideoTranscoding
import io.agora.api.example.compose.samples.MediaMetadata
import io.agora.api.example.compose.samples.MediaPlayer
import io.agora.api.example.compose.samples.MediaRecorder
import io.agora.api.example.compose.samples.OriginAudioData
import io.agora.api.example.compose.samples.OriginVideoData
import io.agora.api.example.compose.samples.PlayAudioFiles
import io.agora.api.example.compose.samples.PreCallTest
import io.agora.api.example.compose.samples.RTMPStreaming
import io.agora.api.example.compose.samples.RhythmPlayer
import io.agora.api.example.compose.samples.ScreenSharing
import io.agora.api.example.compose.samples.SendDataStream
import io.agora.api.example.compose.samples.VideoProcessExtension
import io.agora.api.example.compose.samples.VoiceEffects

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
    Example(R.string.example_rtmp_streaming) { RTMPStreaming() },
    Example(R.string.example_media_metadata) { MediaMetadata() },
    Example(R.string.example_voice_effects) { VoiceEffects() },
    Example(R.string.example_originaudiodata) { OriginAudioData() },
    Example(R.string.example_customaudiosource) { CustomAudioSource() },
    Example(R.string.example_customaudiorender) { CustomAudioRender() },
    Example(R.string.example_originvideodata) { OriginVideoData() },
    Example(R.string.example_customvideosource) { CustomVideoSource() },
    Example(R.string.example_customvideorender) { CustomVideoRender() },
    Example(R.string.example_joinmultichannel) { JoinMultiChannel() },
    Example(R.string.example_channelencryption) { ChannelEncryption() },
    Example(R.string.example_playaudiofiles) { PlayAudioFiles() },
    Example(R.string.example_precalltest) { PreCallTest() },
    Example(R.string.example_mediarecorder) { MediaRecorder() },
    Example(R.string.example_mediaplayer) { MediaPlayer() },
    Example(R.string.example_screensharing) { ScreenSharing() },
    Example(R.string.example_videoprocessextension) { VideoProcessExtension() },
    Example(R.string.example_rhythmplayer) { RhythmPlayer() },
    Example(R.string.example_localvideotranscoding) { LocalVideoTranscoding() },
    Example(R.string.example_senddatastream) { SendDataStream() },
    Example(R.string.example_hostacrosschannel) { HostAcrossChannel() },
)