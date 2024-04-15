@file:OptIn(ExperimentalMaterial3Api::class)

package io.agora.api.example.compose.ui.settings

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.material3.Divider
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuBox
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.APIExampleScaffold
import io.agora.rtc2.RtcEngine
import io.agora.rtc2.video.VideoEncoderConfiguration

@Composable
fun Settings(onBackClick: () -> Unit) {
    APIExampleScaffold(
        topBarTitle = "Settings",
        showBackNavigationIcon = true,
        onBackClick = onBackClick
    ) { paddingValues ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .consumeWindowInsets(WindowInsets.safeDrawing)
                .padding(paddingValues),
            contentAlignment = Alignment.TopCenter
        ) {

            Column {
                val dimensions = listOf(
                    VideoEncoderConfiguration.VD_120x120,
                    VideoEncoderConfiguration.VD_160x120,
                    VideoEncoderConfiguration.VD_180x180,
                    VideoEncoderConfiguration.VD_240x180,
                    VideoEncoderConfiguration.VD_320x180,
                    VideoEncoderConfiguration.VD_240x240,
                    VideoEncoderConfiguration.VD_320x240,
                    VideoEncoderConfiguration.VD_424x240,
                    VideoEncoderConfiguration.VD_360x360,
                    VideoEncoderConfiguration.VD_480x360,
                    VideoEncoderConfiguration.VD_640x360,
                    VideoEncoderConfiguration.VD_640x360,
                    VideoEncoderConfiguration.VD_480x480,
                    VideoEncoderConfiguration.VD_640x480,
                    VideoEncoderConfiguration.VD_840x480,
                    VideoEncoderConfiguration.VD_960x540,
                    VideoEncoderConfiguration.VD_960x720,
                    VideoEncoderConfiguration.VD_1280x720,
                    VideoEncoderConfiguration.VD_1920x1080,
                    VideoEncoderConfiguration.VD_2540x1440,
                    VideoEncoderConfiguration.VD_3840x2160,
                )
                SettingItem(
                    title = "Dimension",
                    options = dimensions.map { it.toText() },
                    default = SettingPreferences.getVideoDimensions().toText()
                ) { option ->
                    SettingPreferences.setVideoDimensions(dimensions[option])
                }
                Divider(modifier = Modifier.padding(horizontal = 16.dp))

                val frameRates = listOf(
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_1,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_7,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_10,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_24,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_30,
                    VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_60,
                )
                SettingItem(
                    title = "FrameRate",
                    options = frameRates.map { it.toText() },
                    default = SettingPreferences.getVideoFrameRate().toText()
                ) { option ->
                    SettingPreferences.setVideoFrameRate(frameRates[option])
                }
                Divider(modifier = Modifier.padding(horizontal = 16.dp))

                val orientationMode = listOf(
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_LANDSCAPE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT,
                )
                SettingItem(
                    title = "Orientation",
                    options = orientationMode.map { it.toText() },
                    default = SettingPreferences.getOrientationMode().toText()
                ) { option ->
                    SettingPreferences.setOrientationMode(orientationMode[option])
                }

                Divider(modifier = Modifier.padding(horizontal = 16.dp))

                Spacer(Modifier.height(16.dp))
                Text(
                    text = "SDK Version: ${RtcEngine.getSdkVersion()}",
                    style = MaterialTheme.typography.bodyLarge,
                    modifier = Modifier
                        .padding(16.dp)
                        .fillMaxWidth(),
                    textAlign = TextAlign.End
                )
            }

        }
    }

}

@Composable
fun SettingItem(
    title: String,
    options: List<String>,
    default: String? = null,
    onSelected: (Int) -> Unit
) {
    var expanded by remember { mutableStateOf(false) }
    var text by remember { mutableStateOf(options[options.indexOf(default ?: options[0])]) }

    Row(verticalAlignment = Alignment.CenterVertically) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(16.dp)
        )
        Spacer(Modifier.weight(1f))
        ExposedDropdownMenuBox(
            expanded = expanded,
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
                trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded) },
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
                expanded = expanded,
                onDismissRequest = { expanded = false },
            ) {
                options.forEach { option ->
                    DropdownMenuItem(
                        text = { Text(option, style = MaterialTheme.typography.bodyLarge) },
                        onClick = {
                            text = option
                            expanded = false
                            onSelected(options.indexOf(option))
                        },
                        contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding,
                    )
                }
            }
        }
    }
}

@Preview
@Composable
fun SettingItemPreview(){
    SettingItem(
        title = "Dimension",
        options = listOf("VD_960x540", "VD_1280x720", "VD_1920x1080"),
        default = "VD_960x540"
    ){}
}

fun VideoEncoderConfiguration.VideoDimensions.toText(): String{
    return "VD_${this.width}x${this.height}"
}

fun VideoEncoderConfiguration.FRAME_RATE.toText() : String{
    return this.name.substring(11)
}

fun VideoEncoderConfiguration.ORIENTATION_MODE.toText() : String{
    return this.name.substring(17)
}