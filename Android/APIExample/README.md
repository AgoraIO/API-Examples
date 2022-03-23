# API Example Android

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC Java SDK for Android.

![image](https://user-images.githubusercontent.com/10089260/116193554-1ff95680-a762-11eb-9f51-479aef5f458e.png)

## Project structure

The project uses a single app to combine a variety of functionalities. Each function is loaded as a fragment for you to play with.

| Function                                                                        | Location                                                                                                                                 |
| ------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| Audio live streaming                                                            | [JoinChannelAudio.java](./app/src/main/java/io/agora/api/example/examples/basic/JoinChannelAudio.java)                  |
| Video live streaming                                                            | [JoinChannelVideo.java](./app/src/main/java/io/agora/api/example/examples/basic/JoinChannelVideo.java)                  |
| Custom audio capture                                                             | [CustomAudioSource.java](./app/src/main/java/io/agora/api/example/examples/advanced/customaudio/CustomAudioSource.java) |
| Custom video renderer                                                           | [CustomRemoteVideoRender.java](./app/src/main/java/io/agora/api/example/examples/advanced/CustomRemoteVideoRender.java) |
| Raw audio and video frames (JNI interface)                                                | [ProcessRawData.java](./app/src/main/java/io/agora/api/example/examples/advanced/ProcessRawData.java)                   |
| Raw audio frames (Native Java interface)                                                         | [ProcessAudioRawData.java](./app/src/main/java/io/agora/api/example/examples/advanced/ProcessAudioRawData.java)         |
| Custom video capture (Push)                                                     | [PushExternalVideo.java](./app/src/main/java/io/agora/api/example/examples/advanced/PushExternalVideo.java)             |
| Switch a channel                                                                | [VideoQuickSwitch.java](./app/src/main/java/io/agora/api/example/examples/advanced/VideoQuickSwitch.java)               |
| Join multiple channels                                                          | [JoinMultipleChannel.java](./app/src/main/java/io/agora/api/example/examples/advanced/JoinMultipleChannel.java)         |
| Set Audio profile                                                               | [SetAudioProfile.java](./app/src/main/java/io/agora/api/example/examples/advanced/SetAudioProfile.java)                 |
| Set Video profile                                                               | [SetVideoProfile.java](./app/src/main/java/io/agora/api/example/examples/advanced/SetVideoProfile.java)                 |
| Play audio files and audio mixing                                                                | [PlayAudioFiles.java](./app/src/main/java/io/agora/api/example/examples/advanced/PlayAudioFiles.java)                   |
| Voice effects                                                                   | [VoiceEffects.java](./app/src/main/java/io/agora/api/example/examples/advanced/VoiceEffects.java)                       |
| MediaPlayer Kit                                                                 | [MediaPlayerKit.java](./app/src/main/java/io/agora/api/example/examples/advanced/MediaPlayerKit.java)                   |
| Geofencing                                                                      | [GeoFencing.java](./app/src/main/java/io/agora/api/example/examples/advanced/GeoFencing.java)                           |
| RTMP streaming                                                                  | [RTMPStreaming.java](./app/src/main/java/io/agora/api/example/examples/advanced/RTMPStreaming.java)                     |
| Audio/video stream custom encryption                                            | [StreamEncrypt.java](./app/src/main/java/io/agora/api/example/examples/advanced/StreamEncrypt.java)                     |
| Switch between custom video capture (MediaIO) and screen sharing                | [SwitchExternalVideo.java](./app/src/main/java/io/agora/api/example/examples/advanced/SwitchExternalVideo.java)         |
| Video metadata                                                                  | [VideoMetadata.java](./app/src/main/java/io/agora/api/example/examples/advanced/VideoMetadata.java)                     |
| Report call status                                                   | [InCallReport.java](./app/src/main/java/io/agora/api/example/examples/advanced/InCallReport.java)                       |
| Adjust volume                                                                   | [AdjustVolume.java](./app/src/main/java/io/agora/api/example/examples/advanced/AdjustVolume.java)                       |
| Pre-call test                                                                   | [PreCallTest.java](./app/src/main/java/io/agora/api/example/examples/advanced/PreCallTest.java)                         |
| Channel media relay                                                             | [HostAcrossChannel.java](./app/src/main/java/io/agora/api/example/examples/advanced/HostAcrossChannel.java)             |
| Super resolution                                                                | [SuperResolution.java](./app/src/main/java/io/agora/api/example/examples/advanced/SuperResolution.java)                 |
| Audio/video stream encryption with methods provided by the SDK                  | [ChannelEncryption.java](./app/src/main/java/io/agora/api/example/examples/advanced/ChannelEncryption.java)             |
| Use multi-processing to send video streams from screen sharing and local camera | [MultiProcess.java](./app/src/main/java/io/agora/api/example/examples/advanced/MultiProcess.java)                       |
| Switch role in live streaming                                                   | [LiveStreaming.java](./app/src/main/java/io/agora/api/example/examples/advanced/LiveStreaming.java)                     |
| Use custom video source (mediaIO) to implement AR function                      | [ARCore.java](./app/src/main/java/io/agora/api/example/examples/advanced/ARCore.java)                                   |
| Send data stream                                                                | [SendDataStream.java](./app/src/main/java/io/agora/api/example/examples/advanced/SendDataStream.java)                   |

## How to run the sample project

### Prerequisites

- Physical Android device or Android simulator with Android 4.1+
- Android Studio (latest version recommended)

### Steps to run

1. In Android Studio, open `/Android/APIExample`.
2. Sync the project with Gradle files.
3. Edit the `/Android/APIExample/app/src/main/res/values/string_configs.xml` file.

   - Replace `YOUR APP ID` with your App ID.
   - Replace `YOUR ACCESS TOKEN` with the Access Token.

   ```xml
   <string name="agora_app_id" translatable="false">YOUR APP ID</string>
   <string name="agora_access_token" translatable="false">YOUR ACCESS TOKEN</string>
   ```

   > See [Get Started with Agora](https://docs.agora.io/en/Agora%20Platform/get_appid_token) to learn how to get an App ID and access token. You can get a temporary access token to quickly try out this sample project.
   >
   > The Channel name you used to generate the token must be the same as the channel name you use to join a channel.

   > To ensure communication security, Agora uses access tokens (dynamic keys) to authenticate users joining a channel.
   >
   > Temporary access tokens are for demonstration and testing purposes only and remain valid for 24 hours. In a production environment, you need to deploy your own server for generating access tokens. See [Generate a Token](https://docs.agora.io/en/Interactive%20Broadcast/token_server) for details.

4. Make the project and run the app in the simulator or connected physical Android device.

You are all set! Feel free to play with this sample project and explore features of the Agora RTC SDK.

## Feedback

If you have any problems or suggestions regarding the sample projects, feel free to file an issue.

## Reference

- [RTC Java SDK Product Overview](https://docs.agora.io/en/Interactive%20Broadcast/product_live?platform=Android)
- [RTC Java SDK API Reference](https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/java/index.html)

## Related resources

- Check our [FAQ](https://docs.agora.io/en/faq) to see if your issue has been recorded.
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- If you encounter problems during integration, feel free to ask questions in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)

## License

The sample projects are under the MIT license.
