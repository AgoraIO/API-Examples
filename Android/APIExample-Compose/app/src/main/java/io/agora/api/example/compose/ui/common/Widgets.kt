package io.agora.api.example.compose.ui.common

import android.content.Context
import android.util.Log
import android.view.TextureView
import android.view.View
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxScope
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.selection.selectable
import androidx.compose.foundation.selection.selectableGroup
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuBox
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.RadioButton
import androidx.compose.material3.Slider
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.RectangleShape
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.semantics.Role
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.TextUnitType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import io.agora.api.example.compose.R
import io.agora.rtc2.IRtcEngineEventHandler

@Composable
fun ChannelNameInput(
    modifier: Modifier = Modifier,
    channelName: String,
    isJoined: Boolean,
    onJoinClick: (String) -> Unit = {_ -> },
    onLeaveClick: () -> Unit = {},
) {
    var text by rememberSaveable { mutableStateOf(channelName) }
    InputRaw(
        modifier = modifier,
        text = text,
        btnText = if (isJoined) stringResource(id = R.string.leave) else stringResource(id = R.string.join),
        label = stringResource(id = R.string.channel_name),
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
    setupVideo: (View, Int, Boolean) -> Unit,
    statsMap: Map<Int, VideoStatsInfo> = emptyMap(),
    overlay: @Composable BoxScope.(index: Int, id: Int) -> Unit? = { _, _ -> }
) {
    Column(modifier) {
        for (rawIndex in 0..<raw) {
            Row(Modifier.weight(1.0f)) {
                for (columnIndex in 0..<column) {
                    Box(modifier = Modifier.weight(1.0f)) {
                        val index = rawIndex * column + columnIndex
                        videoIdList.getOrNull(index)?.let { id ->
                            VideoCell(
                                id = id,
                                isLocal = index == 0,
                                setupVideo = setupVideo,
                                statsInfo = statsMap[id],
                                overlay = { overlay(index, id) }
                            )
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun VideoCell(
    modifier: Modifier = Modifier,
    id: Int,
    isLocal: Boolean,
    createView: ((context: Context) -> View)? = null,
    setupVideo: (renderView: View, id: Int, isFirstSetup: Boolean) -> Unit,
    statsInfo: VideoStatsInfo? = null,
    overlay: @Composable BoxScope.() -> Unit? = { }
) {
    Box(modifier) {
        if (id != 0) {
            AndroidView(
                factory = { context ->
                    Log.d("VideoCell", "VideoCell: create render view.")
                    createView?.invoke(context) ?: TextureView(context).apply {
                        tag = id
                        setupVideo(this, id, true)
                    }
                },
                update = { view ->
                    if (view.tag != id) {
                        Log.d("VideoCell", "VideoCell: update render view.")
                        view.tag = id
                        setupVideo(view, id, false)
                    }
                })
        }

        if (statsInfo != null) {
            var text = ""
            if (isLocal) {
                statsInfo.localVideoStats?.let { text += "${it.encodedFrameWidth}x${it.encodedFrameHeight},${it.encoderOutputFrameRate}fps" }

                statsInfo.rtcStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "LM Delay: ${it.lastmileDelay}ms"
                }

                statsInfo.localVideoStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "VSend: ${it.sentBitrate}kbps"
                }

                statsInfo.localAudioStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "ASend: ${it.sentBitrate}kbps"
                }

                statsInfo.rtcStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "CPU: ${it.cpuAppUsage}%/${it.cpuTotalUsage}%"
                }

                statsInfo.rtcStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "VSend Loss: ${it.txPacketLossRate}%"
                }
            } else {
                statsInfo.remoteVideoStats?.let { text += "${it.width}x${it.height},${it.decoderOutputFrameRate}fps" }

                statsInfo.remoteVideoStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "VRecv: ${it.receivedBitrate}kbps"
                }

                statsInfo.remoteAudioStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "ARecv: ${it.receivedBitrate}kbps"
                }

                statsInfo.remoteVideoStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "VRecv Loss: ${it.packetLossRate}%"
                }

                statsInfo.remoteAudioStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "ARecv Loss: ${it.audioLossRate}%"
                }

                statsInfo.remoteAudioStats?.let {
                    if (text.isNotEmpty()) {
                        text += "\n"
                    }
                    text += "AQuality: ${it.quality}"
                }
            }
            Text(text = text, color = Color.White)
        }

        overlay()
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
    selectedValue: T = options.getOrNull(selected)?.second ?: options.first().second,
    enable: Boolean = true,
    onSelected: (Int, Pair<String, T>) -> Unit = { _, _ -> }
) {
    var expanded by remember { mutableStateOf(false) }
    var text by remember {
        mutableStateOf(
            options.find { it.second == selectedValue }?.first ?: options.first().first
        )
    }

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
                            onSelected(options.indexOf(option), option)
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
    var swChecked by rememberSaveable { mutableStateOf(checked) }

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
    isDraggingWhileUpdating : Boolean = false,
    onValueChanged: (Float) -> Unit = {},
) {
    var slValue by remember { mutableFloatStateOf(value) }
    var updatable by rememberSaveable {
        mutableStateOf(true)
    }

    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(16.dp, 8.dp)
        )
        Spacer(Modifier.weight(1f))
        Slider(
            modifier = Modifier.padding(16.dp, 0.dp),
            value = if (updatable && isDraggingWhileUpdating) value else slValue,
            onValueChange = {
                updatable = false
                slValue = it
            },
            onValueChangeFinished = {
                updatable = true
                onValueChanged(slValue)
            },
            enabled = enable
        )
    }
}


@Composable
fun InputRaw(
    modifier: Modifier = Modifier,
    text: String,
    btnText: String,
    label: String = "",
    enable: Boolean = true,
    editable: Boolean = true,
    secondVisible: Boolean = false,
    secondBtnText: String = "",
    secondEnable: Boolean = false,
    onSecondBtnClick: () -> Unit = {},
    onBtnClick: (String) -> Unit = {},
) {
    var rText by rememberSaveable { mutableStateOf(text) }
    Row(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp),
        verticalAlignment = Alignment.CenterVertically,
    ) {
        TextField(
            modifier = Modifier
                .weight(1.0f)
                .padding(0.dp),
            value = rText,
            enabled = enable && editable,
            onValueChange = { rText = it },
            label = { Text(label) },
            singleLine = true,
            shape = RectangleShape,
            colors = TextFieldDefaults.colors(
                focusedContainerColor = Color.Transparent,
                unfocusedContainerColor = Color.Transparent,
                errorContainerColor = Color.Transparent,
                disabledContainerColor = Color.Transparent
            )
        )
        if(secondVisible){
            Button(
                modifier = Modifier.padding(end = 8.dp),
                enabled = secondEnable,
                onClick = {
                    onSecondBtnClick()
                }
            ) {
                Text(secondBtnText)
            }
        }

        Button(
            enabled = enable,
            onClick = {
                onBtnClick(rText)
            }
        ) {
            Text(btnText)
        }
    }
}

enum class TwoVideoViewType {
    LargeSmall,
    Row,
    Column
}

@Composable
fun TwoVideoView(
    modifier: Modifier = Modifier,
    localUid: Int,
    remoteUid: Int,
    localStats: VideoStatsInfo = VideoStatsInfo(),
    remoteStats: VideoStatsInfo = VideoStatsInfo(),
    type: TwoVideoViewType = TwoVideoViewType.LargeSmall,
    localPrimary: Boolean = true,
    secondClickable: Boolean = false,
    onSecondClick: () -> Unit = {},
    localCreate: ((context: Context) -> View)? = null,
    remoteCreate: ((context: Context) -> View)? = null,
    localRender: (View, Int, Boolean) -> Unit,
    remoteRender: (View, Int, Boolean) -> Unit,
) {
    val primary: @Composable (Modifier) -> Unit = {
        VideoCell(
            modifier = it,
            id = if (localPrimary) localUid else remoteUid,
            isLocal = localPrimary,
            createView = if (localPrimary) localCreate else remoteCreate,
            setupVideo = if (localPrimary) localRender else remoteRender,
            statsInfo = if (localPrimary) localStats else remoteStats
        )
    }
    val second: @Composable (Modifier) -> Unit = {
        VideoCell(
            modifier = if (secondClickable) it.clickable {
                onSecondClick()
            } else it,
            id = if (!localPrimary) localUid else remoteUid,
            isLocal = !localPrimary,
            createView = if (localPrimary) remoteCreate else localCreate,
            setupVideo = if (!localPrimary) localRender else remoteRender,
            statsInfo = if (!localPrimary) localStats else remoteStats
        )
    }
    when (type) {
        TwoVideoViewType.LargeSmall -> {
            Box(modifier = modifier) {
                primary(Modifier.fillMaxSize())
                second(
                    Modifier
                        .fillMaxSize(0.5f)
                        .align(Alignment.TopEnd)
                        .padding(16.dp)
                )
            }
        }

        TwoVideoViewType.Row -> {
            Row(modifier = modifier) {
                primary(Modifier.weight(1.0f))
                second(Modifier.weight(1.0f))
            }
        }

        TwoVideoViewType.Column -> {
            Column(modifier = modifier) {
                primary(Modifier.weight(1.0f))
                second(Modifier.weight(1.0f))
            }
        }
    }
}


@Composable
fun <T> RadioGroup(
    modifier: Modifier = Modifier,
    options: List<Pair<String, T>>,
    selectedValue: T = options.first().second,
    selected: Int = options.indexOfFirst { it.second == selectedValue },
    onSelected: (index: Int, option: Pair<String, T>) -> Unit = { _, _ -> }
) {
    var selectedIndex by rememberSaveable { mutableIntStateOf(selected) }

    Column(modifier.selectableGroup()) {
        options.forEachIndexed { index, option ->
            Row(
                Modifier
                    .fillMaxWidth()
                    .height(35.dp)
                    .selectable(
                        selected = (selectedIndex == index),
                        onClick = {
                            selectedIndex = index
                            onSelected(index, option)
                        },
                        role = Role.RadioButton
                    )
                    .padding(horizontal = 16.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                RadioButton(
                    selected = (selectedIndex == index),
                    onClick = null // null recommended for accessibility with screenreaders
                )
                Text(
                    text = option.first,
                    style = MaterialTheme.typography.bodyLarge,
                    modifier = Modifier.padding(start = 16.dp)
                )
            }
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

