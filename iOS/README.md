# API Example iOS

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC Objective-C SDK for iOS.

## Project structure

The project uses a single app to combine a variety of functionalities. Each function is loaded as a storyboard for you to play with.

| Function                                                                        | Location                                                                                                      |
| ------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| Live streaming                                                                  | [LiveStreaming.swift](./Examples/Advanced/LiveStreaming/LiveStreaming.swift)                                  |
| Custom audio capture                                                            | [CustomAudioSource.swift](./Examples/Advanced/CustomAudioSource/CustomAudioSource.swift)                      |
| Custom video renderer                                                           | [CustomVideoRender.swift](./Examples/Advanced/CustomVideoRender/CustomVideoRender.swift)                      |
| Raw audio and video frames (Objective-C with C++, uses `AgoraMediaRawData.h` )  | [RawMediaData.swift](./Examples/Advanced/RawMediaData/RawMediaData.swift)                                     |
| Raw audio frames (Native Objective-C interface)                                 | [RawAudioData.swift](./Examples/Advanced/RawAudioData/RawAudioData.swift)                                     |
| Custom video capture (Push)                                                     | [CustomVideoSourcePush.swift](./Examples/Advanced/CustomVideoSourcePush/CustomVideoSourcePush.swift)          |
| Custom video capture (mediaIO)                                                  | [CustomVideoSourceMediaIO.swift](./Examples/Advanced/CustomVideoSourceMediaIO/CustomVideoSourceMediaIO.swift) |
| Switch a channel                                                                | [QuickSwitchChannel.swift](./Examples/Advanced/QuickSwitchChannel/QuickSwitchChannel.swift)                   |
| Join multiple channels                                                          | [JoinMultiChannel.swift](.Examples/Advanced/JoinMultiChannel/JoinMultiChannel.swift)                          |
| Join an audio channel                                                           | [JoinChannelAudio.swift](./Examples/Basic/JoinChannelAudio/JoinChannelAudio.swift)                            |
| Join a video channel                                                            | [JoinChannelVideo.swift](./Examples/Basic/JoinChannelAudio/JoinChannelVideo.swift)                            |
| Play audio files and audio mixing                                               | [AudioMixing.swift](API-Examples/iOS/APIExample/Examples/Advanced/AudioMixing/AudioMixing.swift)              |
| Voice effects                                                                   | [VoiceChanger.swift](./Examples/Advanced/VoiceChanger/VoiceChanger.swift)                                     |
| MediaPlayer Kit                                                                 | [MediaPlayer.swift](./Examples/Advanced/MediaPlayer/MediaPlayer.swift)                                        |
| RTMP streaming                                                                  | [RTMPStreaming.swift](./Examples/Advanced/RTMPStreaming/RTMPStreaming.swift)                                  |
| Audio/video stream SDK/custom encryption                                        | [StreamEncryption.swift](./Examples/Advanced/StreamEncryption/StreamEncryption.swift)                         |
| Video metadata                                                                  | [VideoMetadata.swift](./Examples/Advanced/VideoMetadata/VideoMetadata.swift)                                  |
| Group video chat                                                                | [VideoChat.swift](./Examples/Advanced/VideoChat/VideoChat.swift)                                              |
| Pre-call test                                                                   | [PrecallTest.swift](./Examples/Advanced/PrecallTest/PrecallTest.swift)                                        |
| Channel media relay                                                             | [MediaChannelRelay.swift](./Examples/Advanced/MediaChannelRelay/MediaChannelRelay.swift)                      |
| Super resolution                                                                | [SuperResolution.swift](./Examples/Advanced/SuperResolution/SuperResolution.swift)                            |
| Use multi-processing to send video streams from screen sharing and local camera | [ScreenShare.swift](./Examples/Advanced/ScreenShare/ScreenShare.swift)                                        |
| Use custom video source (mediaIO) to implement AR function                      | [ARKit.swift](./Examples/Advanced/ARKit/ARKit.swift)                                                          |
| Send data stream                                                                | [CreateDataStream.swift](./Examples/Advanced/CreateDataStream/CreateDataStream.swift)                         |
| Geofencing                                                                      | [GlobalSettings.swift](./APIExample/Common/GlobalSettings.swift)                                              |

## How to run the sample project

## Prerequisites

- Xcode 10.0+
- Physical iOS device (iPhone or iPad)
- iOS simulator is NOT supported

### Steps to run

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Prepare Dependencies

Change directory into **iOS** folder, run following command to install project dependencies,

```shell
pod install
```

Verify `APIExample.xcworkspace` has been properly generated.

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.
5. Open `APIExample.xcworkspace` and edit the `KeyCenter.swift` file. In the `KeyCenter` struct, update `<#Your App Id#>` with your App Id, and change `<#Temp Access Token#>` with the temp Access Token generated from dashboard. Note you can leave the token variable `nil` if your project has not turned on security token.

   ```Swift
   struct KeyCenter {
       static let AppId: String = <#Your App Id#>

       // assign token to nil if you have not enabled app certificate
       static var Token: String? = <#Temp Access Token#>
   }
   ```

You are all set. Now connect your iPhone or iPad device and run the project.

> To ensure communication security, Agora uses tokens (dynamic keys) to authenticate users joining a channel.
>
> Temporary tokens are for demonstration and testing purposes only and remain valid for 24 hours. In a production environment, you need to deploy your own server for generating tokens. See [Generate a Token](https://docs.agora.io/en/Interactive%20Broadcast/token_server)for details.

## Feedback

If you have any problems or suggestions regarding the sample projects, feel free to file an issue.

## Reference

- [RTC Objetive-C SDK Product Overview](https://docs.agora.io/en/Interactive%20Broadcast/product_live?platform=iOS)
- [RTC Objective-C SDK API Reference](https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/oc/docs/headers/Agora-Objective-C-API-Overview.html)

## Related resources

- Check our [FAQ](https://docs.agora.io/en/faq) to see if your issue has been recorded.
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- If you encounter problems during integration, feel free to ask questions in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)

## Known issues

After users upgrade their iOS devices to iOS 14.0, and use an app that integrates the Agora RTC SDK for iOS with the version **earlier than v3.1.2** for the first time, users see a prompt for finding local network devices. The following picture shows the pop-up prompt:

![](./pictures/ios_14_privacy.png)

Refer to the [this FAQ](https://docs.agora.io/en/faq/local_network_privacy) to learn how to fix this issue.

## License

The MIT License (MIT)
