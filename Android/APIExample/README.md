# API Example Android

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC Java SDK for Android.

## Project structure

The project uses a single app to combine a variety of functionalities. Each function is loaded as a fragment for you to play with.

| Function                                                                        | Location                                                                                                                                 |
| ------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| Audio live streaming                                                            | [JoinChannelAudio.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\basic\JoinChannelAudio.java)                  |
| Video live streaming                                                            | [JoinChannelVideo.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\basic\JoinChannelVideo.java)                  |
| Custom audio source                                                             | [CustomAudioSource.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\customaudio\CustomAudioSource.java) |
| Custom video renderer                                                           | [CustomRemoteVideoRender.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\CustomRemoteVideoRender.java) |
| Raw audio and video frames (JNI)                                                | [ProcessRawData.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\ProcessRawData.java)                   |
| Raw audio frames (Java)                                                         | [ProcessAudioRawData.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\ProcessAudioRawData.java)         |
| Custom video capture (Push)                                                     | [PushExternalVideo.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\PushExternalVideo.java)             |
| Switch a channel                                                                | [VideoQuickSwitch.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\VideoQuickSwitch.java)               |
| Join multiple channels                                                          | [JoinMultipleChannel.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\JoinMultipleChannel.java)         |
| Set Audio profile                                                               | [SetAudioProfile.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SetAudioProfile.java)                 |
| Set Video profile                                                               | [SetVideoProfile.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SetVideoProfile.java)                 |
| Play audio files                                                                | [PlayAudioFiles.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\PlayAudioFiles.java)                   |
| Voice effects                                                                   | [VoiceEffects.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\VoiceEffects.java)                       |
| MediaPlayer Kit                                                                 | [MediaPlayerKit.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\MediaPlayerKit.java)                   |
| Geofencing                                                                      | [GeoFencing.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\GeoFencing.java)                           |
| RTMP streaming                                                                  | [RTMPStreaming.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\RTMPStreaming.java)                     |
| Audio/video stream custom encryption                                            | [StreamEncrypt.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\StreamEncrypt.java)                     |
| Switch between custom video capture (MediaIO) and screen sharing                | [SwitchExternalVideo.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SwitchExternalVideo.java)         |
| Video metadata                                                                  | [VideoMetadata.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\VideoMetadata.java)                     |
| In call report                                                                  | [InCallReport.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\InCallReport.java)                       |
| Adjust volume                                                                   | [AdjustVolume.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\AdjustVolume.java)                       |
| Pre-call test                                                                   | [PreCallTest.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\PreCallTest.java)                         |
| Channel media relay                                                             | [HostAcrossChannel.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\HostAcrossChannel.java)             |
| Super resolution                                                                | [SuperResolution.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SuperResolution.java)                 |
| Set video profile                                                               | [SetVideoProfile.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SetVideoProfile.java)                 |
| Audio/video stream encryption with methods provided by the SDK                  | [ChannelEncryption.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\ChannelEncryption.java)             |
| Use multi-processing to send video streams from screen sharing and local camera | [MultiProcess.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\MultiProcess.java)                       |
| Switch role in live streaming                                                   | [LiveStreaming.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\LiveStreaming.java)                     |
| Use custom video source (mediaIO) to implement AR function                      | [ARCore.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\ARCore.java)                                   |
| Send data stream                                                                | [SendDataStream.java](Android\APIExample\app\src\main\java\io\agora\api\example\examples\advanced\SendDataStream.java)                   |

## Steps to run

### Prerequisites

- Android Studio 3.0+
- Physical Android device
- Android simulator is supported

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the sign-up process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.
5. Open `Android/APIExample` and edit the `app/src/main/res/values/string_config.xml` file. Update `<#Your App Id#>` with your App Id, and change `<#Temp Access Token#>` with the temp Access Token generated from dashboard. Note you can leave the token variable `null` if your project has not turned on security token.

   ```xml
   <string name="agora_app_id" translatable="false">YOUR APP ID</string>
   <!- You can set your token to null if you have not enabled App Certificate in your project ->
   <string name="agora_access_token" translatable="false">YOUR ACCESS TOKEN</string>
   ```

You are all set. Now connect your Android device and run the project.

> To ensure communication security, Agora uses tokens (dynamic keys) to authenticate users joining a channel.
>
> Temporary tokens are for demonstration and testing purposes only and remain valid for 24 hours. In a production environment, you need to deploy your own server for generating tokens. See [Generate a Token](https://docs.agora.io/en/Interactive Broadcast/token_server)for details.

## Contact Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Basic-Video-Call/issues)

## License

The MIT License (MIT)
