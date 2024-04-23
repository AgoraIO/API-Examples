@file:OptIn(ExperimentalMaterial3Api::class)

package io.agora.api.example.compose.ui.settings

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.material3.Divider
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.data.SettingPreferences
import io.agora.api.example.compose.ui.common.APIExampleScaffold
import io.agora.api.example.compose.ui.common.DropdownMenuRaw
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
                DropdownMenuRaw(
                    title = "Dimension",
                    options = dimensions.map {
                        it.toText() to it
                    },
                    selected = dimensions.indexOf(SettingPreferences.getVideoDimensions()),
                ) { _, option ->
                    SettingPreferences.setVideoDimensions(option.second)
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
                DropdownMenuRaw(
                    title = "FrameRate",
                    options = frameRates.map { it.toText()  to it},
                    selected = frameRates.indexOf(SettingPreferences.getVideoFrameRate()),
                ) { _, option ->
                    SettingPreferences.setVideoFrameRate(option.second)
                }
                Divider(modifier = Modifier.padding(horizontal = 16.dp))

                val orientationMode = listOf(
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_LANDSCAPE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT,
                )
                DropdownMenuRaw(
                    title = "Orientation",
                    options = orientationMode.map { it.toText() to it},
                    selected = orientationMode.indexOf(SettingPreferences.getOrientationMode()),
                ) { _, option ->
                    SettingPreferences.setOrientationMode(option.second)
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



@Preview
@Composable
fun SettingsPreview(){
    Settings {

    }
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