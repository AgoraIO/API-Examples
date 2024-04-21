package io.agora.api.example.compose.ui.common

import android.util.Log
import android.view.TextureView
import android.view.View
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuBox
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.TextUnitType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import io.agora.api.example.compose.samples.TAG
import io.agora.rtc2.IRtcEngineEventHandler

@Composable
fun ChannelNameInput(
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit,
    onLeaveClick: () -> Unit,
) {
    var text by rememberSaveable { mutableStateOf(channelName) }
    InputRaw(
        text = text,
        btnText = if (isJoined) "Leave" else "Join",
        label = "Channel Name",
    ) {
        text = it
        if (isJoined) onLeaveClick() else onJoinClick(it)
    }
}


@Composable
fun VideoGrid(
    modifier: Modifier = Modifier,
    raw: Int = 2,
    column: Int = 2,
    videoIdList: List<Int>,
    setupVideo: (View, Int) -> Unit,
    statsMap: Map<Int, VideoStatsInfo> = emptyMap()
) {
    Column(modifier) {
        for (rawIndex in 0..<raw) {
            Row(Modifier.weight(1.0f)) {
                for (columnIndex in 0..<column) {
                    Box(modifier = Modifier.weight(1.0f)) {
                        val index = rawIndex * column + columnIndex
                        videoIdList.getOrNull(index)?.let { id ->
                            VideoCell(id, index == 0, setupVideo, statsMap[id])
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
    isLocal: Boolean,
    setupVideo: (View, Int) -> Unit,
    statsInfo: VideoStatsInfo? = null
) {
    Box {
        AndroidView(
            factory = { context ->
                Log.d(TAG, "VideoCell: create render view.")
                TextureView(context).apply {
                    setupVideo(this, id)
                }
            })

        if (statsInfo != null) {
            var text = ""
            if (isLocal) {
                statsInfo.localVideoStats?.let { text += "${it.encodedFrameWidth}x${it.encodedFrameHeight},${it.encoderOutputFrameRate}fps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.rtcStats?.let { text += "LM Delay: ${it.lastmileDelay}ms" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.localVideoStats?.let { text += "VSend: ${it.sentBitrate}kbps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.localAudioStats?.let { text += "ASend: ${it.sentBitrate}kbps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.rtcStats?.let { text += "CPU: ${it.cpuAppUsage}%/${it.cpuTotalUsage}%" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.rtcStats?.let { text += "VSend Loss: ${it.txPacketLossRate}%" }
            } else {
                statsInfo.remoteVideoStats?.let { text += "${it.width}x${it.height},${it.decoderOutputFrameRate}fps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.remoteVideoStats?.let { text += "VRecv: ${it.receivedBitrate}kbps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.remoteAudioStats?.let { text += "ARecv: ${it.receivedBitrate}kbps" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.remoteVideoStats?.let { text += "VRecv Loss: ${it.packetLossRate}%" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.remoteAudioStats?.let { text += "ARecv Loss: ${it.audioLossRate}%" }
                if (text.isNotEmpty()) {
                    text += "\n"
                }
                statsInfo.remoteAudioStats?.let { text += "AQuality: ${it.quality}" }
            }
            Text(text = text, color = Color.White)
        }

    }
}

data class VideoStatsInfo(
    var rtcStats: IRtcEngineEventHandler.RtcStats? = null,
    var localVideoStats: IRtcEngineEventHandler.LocalVideoStats? = null,
    var localAudioStats: IRtcEngineEventHandler.LocalAudioStats? = null,
    var remoteVideoStats: IRtcEngineEventHandler.RemoteVideoStats? = null,
    var remoteAudioStats: IRtcEngineEventHandler.RemoteAudioStats? = null,
)


@Composable
fun AudioGrid(
    modifier: Modifier = Modifier,
    raw: Int = 2,
    column: Int = 3,
    videoIdList: List<Int>,
    statsMap: Map<Int, AudioStatsInfo> = emptyMap()
) {
    Column(modifier) {
        for (rawIndex in 0..<raw) {
            Row(Modifier.weight(1.0f)) {
                for (columnIndex in 0..<column) {
                    Box(modifier = Modifier.weight(1.0f)) {
                        val index = rawIndex * column + columnIndex
                        videoIdList.getOrNull(index)?.let { id ->
                            AudioCell(uid = id, isLocal = index == 0, stats = statsMap[id])
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun AudioCell(
    uid: Int,
    isLocal: Boolean,
    stats: AudioStatsInfo? = null
) {
    Box(
        modifier = Modifier
            .background(Color.LightGray)
            .fillMaxSize()
    ) {
        Column(
            modifier = Modifier.align(Alignment.Center),
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            Text(text = "Audio Only", color = Color.White)
            Text(text = if (isLocal) "Local" else "Remote", color = Color.White)
            Text(text = uid.toString(), color = Color.White)
        }
        var statsStr = ""
        if (isLocal) {
            stats?.localAudioStats?.let {
                statsStr += "sentSampleRate: ${it.sentSampleRate}"
                statsStr += "\n"
                statsStr += "sentBitrate: ${it.sentBitrate} kbps"
                statsStr += "\n"
                statsStr += "numChannels: ${it.numChannels}"
                statsStr += "\n"
                statsStr += "internalCodec: ${it.internalCodec}"
                statsStr += "\n"
                statsStr += "audioDeviceDelay: ${it.audioDeviceDelay} ms"
            }
        } else {
            stats?.remoteAudioStats?.let {
                statsStr += "receivedSampleRate: ${it.receivedSampleRate}"
                statsStr += "\n"
                statsStr += "receivedBitrate: ${it.receivedBitrate} kbps"
                statsStr += "\n"
                statsStr += "numChannels: ${it.numChannels}"
                statsStr += "\n"
                statsStr += "audioLossRate: ${it.audioLossRate}"
                statsStr += "\n"
                statsStr += "jitterBufferDelay: ${it.jitterBufferDelay} ms"
            }
        }
        Text(
            modifier = Modifier.align(Alignment.TopStart),
            text = statsStr,
            color = Color.White,
            fontSize = 10.sp,
            lineHeight = TextUnit(1f, TextUnitType.Em)
        )
    }
}

data class AudioStatsInfo(
    var localAudioStats: IRtcEngineEventHandler.LocalAudioStats? = null,
    var remoteAudioStats: IRtcEngineEventHandler.RemoteAudioStats? = null,
)


@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun <T> DropdownMenuRaw(
    title: String,
    options: List<Pair<String, T>>,
    selected: Int = 0,
    enable: Boolean = true,
    onSelected: (Pair<String, T>) -> Unit = {}
) {
    var expanded by remember { mutableStateOf(false) }
    var text by remember { mutableStateOf(options.getOrNull(selected)?.first ?: "") }

    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(16.dp, 8.dp)
        )
        Spacer(Modifier.weight(1f))
        ExposedDropdownMenuBox(
            expanded = expanded && enable,
            onExpandedChange = { expanded = it },
        ) {
            TextField(
                // The `menuAnchor` modifier must be passed to the text field for correctness.
                modifier = Modifier.menuAnchor(),
                shape = TextFieldDefaults.shape,
                value = text,
                onValueChange = {},
                readOnly = true,
                singleLine = true,
                trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded && enable) },
                colors = TextFieldDefaults.colors(
                    focusedIndicatorColor = Color.Transparent,
                    unfocusedIndicatorColor = Color.Transparent,
                    disabledIndicatorColor = Color.Transparent,
                    errorIndicatorColor = Color.Transparent,
                    focusedContainerColor = Color.Transparent,
                    unfocusedContainerColor = Color.Transparent,
                    disabledContainerColor = Color.Transparent,
                    errorContainerColor = Color.Transparent
                ),
            )
            ExposedDropdownMenu(
                expanded = expanded && enable,
                onDismissRequest = { expanded = false },
            ) {
                options.forEach { option ->
                    DropdownMenuItem(
                        text = { Text(option.first, style = MaterialTheme.typography.bodyLarge) },
                        onClick = {
                            text = option.first
                            expanded = false
                            onSelected(option)
                        },
                        contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding,
                    )
                }
            }
        }
    }
}


@Composable
fun SwitchRaw(
    title: String,
    checked: Boolean = false,
    enable: Boolean = true,
    onCheckedChange: (Boolean) -> Unit = {},
) {
    var swChecked by remember { mutableStateOf(checked) }

    Row(
        modifier = Modifier.clickable(enabled = enable) {
            swChecked = !swChecked
            onCheckedChange(swChecked)
        },
        verticalAlignment = Alignment.CenterVertically,
    ) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(16.dp, 8.dp)
        )
        Spacer(Modifier.weight(1f))
        Switch(
            modifier = Modifier.padding(16.dp, 0.dp),
            checked = swChecked,
            onCheckedChange = {
                swChecked = it
                onCheckedChange(it)
            },
            enabled = enable
        )
    }
}


@Composable
fun SliderRaw(
    title: String,
    value: Float = 0f,
    enable: Boolean = true,
    onValueChanged: (Float) -> Unit = {},
) {
    var slValue by remember { mutableFloatStateOf(value) }

    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(16.dp, 8.dp)
        )
        Spacer(Modifier.weight(1f))
        Slider(
            value = slValue,
            onValueChange = {
                slValue = it
            },
            onValueChangeFinished = {
                onValueChanged(slValue)
            },
            enabled = enable
        )
    }
}


@Composable
fun InputRaw(
    text: String,
    btnText: String,
    label: String = "",
    enable: Boolean = true,
    onBtnClick: (String) -> Unit = {}
) {
    var rText by rememberSaveable { mutableStateOf(text) }
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        TextField(
            modifier = Modifier.weight(1.0f),
            value = rText,
            enabled = enable,
            onValueChange = { rText = it },
            label = { Text(label) },
            singleLine = true,
            colors = TextFieldDefaults.colors(
                focusedContainerColor = Color.Transparent,
                unfocusedContainerColor = Color.Transparent,
                errorContainerColor = Color.Transparent
            )
        )
        Button(
            modifier = Modifier.size(90.dp, Dp.Unspecified),
            enabled = enable,
            onClick = {
                onBtnClick(rText)
            }
        ) {
            Text(btnText)
        }
    }
}


@Preview
@Composable
fun WidgetsPreview() {
    Column {
        SwitchRaw(
            "Enable Video",
            true,
        )
        // SliderRaw("Bitrate", 0.5f)
    }
}