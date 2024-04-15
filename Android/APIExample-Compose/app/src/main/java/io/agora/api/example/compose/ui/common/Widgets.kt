package io.agora.api.example.compose.ui.common

import android.util.Log
import android.view.TextureView
import android.view.View
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import io.agora.api.example.compose.ui.samples.TAG
import io.agora.rtc2.IRtcEngineEventHandler

@Composable
fun ChannelNameInput(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
) {
    var text by rememberSaveable { mutableStateOf(channelName) }
    Row(
        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        TextField(
            modifier = Modifier.weight(1.0f),
            value = text,
            onValueChange = { text = it },
            label = { Text("Channel Name") },
            singleLine = true,
            colors = TextFieldDefaults.colors(
                focusedContainerColor = Color.Transparent,
                unfocusedContainerColor = Color.Transparent,
                errorContainerColor = Color.Transparent
            )
        )
        Button(
            modifier = Modifier.size(90.dp, Dp.Unspecified),
            onClick = {
                if (isJoined) onLeaveClick() else onJoinClick(text)
            }
        ) {
            Text(if (isJoined) "Leave" else "Join")
        }
    }
}


@Composable
fun VideoGrid(
    modifier: Modifier = Modifier,
    raw: Int = 2,
    column: Int = 2,
    videoIdList: List<Int>,
    setupVideo: (View, Int) -> Unit,
    statsMap: Map<Int, VideoStatsConfig> = emptyMap()
) {
    Column(modifier) {
        for (rawIndex in 0..<raw) {
            Row(Modifier.weight(1.0f)) {
                for (columnIndex in 0..<column) {
                    Box(modifier = Modifier.weight(1.0f)) {
                        videoIdList.getOrNull(rawIndex * column + columnIndex)?.let { id ->
                            VideoCell(id, setupVideo, statsMap[id])
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun VideoCell(
    id: Int,
    setupVideo: (View, Int) -> Unit,
    statsConfig: VideoStatsConfig? = null
) {
    Box {
        AndroidView(
            factory = { context ->
                Log.d(TAG, "VideoCell: create render view.")
                TextureView(context).apply {
                    setupVideo(this, id)
                }
            })

        if (statsConfig != null) {
            var text = ""
            if (statsConfig.statsLocal) {
                statsConfig.localVideoStats?.let { text += "${it.encodedFrameWidth}x${it.encodedFrameHeight},${it.encoderOutputFrameRate}fps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.rtcStats?.let { text += "LM Delay: ${it.lastmileDelay}ms" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.localVideoStats?.let { text += "VSend: ${it.sentBitrate}kbps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.localAudioStats?.let { text += "ASend: ${it.sentBitrate}kbps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.rtcStats?.let { text += "CPU: ${it.cpuAppUsage}%/${it.cpuTotalUsage}%" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.rtcStats?.let { text += "VSend Loss: ${it.txPacketLossRate}%" }
            } else {
                statsConfig.remoteVideoStats?.let { text += "${it.width}x${it.height},${it.decoderOutputFrameRate}fps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.remoteVideoStats?.let { text += "VRecv: ${it.receivedBitrate}kbps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.remoteAudioStats?.let { text += "ARecv: ${it.receivedBitrate}kbps" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.remoteVideoStats?.let { text += "VRecv Loss: ${it.packetLossRate}%" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.remoteAudioStats?.let { text += "ARecv Loss: ${it.audioLossRate}%" }
                if(text.isNotEmpty()) { text += "\n" }
                statsConfig.remoteAudioStats?.let { text += "AQuality: ${it.quality}" }
            }
            Text(text = text, color = Color.White)
        }

    }
}

data class VideoStatsConfig(
    val id: Int,
    val statsLocal: Boolean = false,
    var rtcStats: IRtcEngineEventHandler.RtcStats? = null,
    var localVideoStats: IRtcEngineEventHandler.LocalVideoStats? = null,
    var localAudioStats: IRtcEngineEventHandler.LocalAudioStats? = null,
    var remoteVideoStats: IRtcEngineEventHandler.RemoteVideoStats? = null,
    var remoteAudioStats: IRtcEngineEventHandler.RemoteAudioStats? = null,
)