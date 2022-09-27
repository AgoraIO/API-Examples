# API Example iOS

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC Objective-C SDK for iOS.

![image](https://user-images.githubusercontent.com/10089260/116365169-91ef9f80-a837-11eb-87df-15de8218f880.png)

## Project structure

The project uses a single app to combine a variety of functionalities. Each function is loaded as a storyboard for you to play with.

| Function                                                                        | Location                                                                                                                 |
| ------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| Live streaming                                                                  | [LiveStreaming.swift](./APIExample/Examples/Advanced/LiveStreaming/LiveStreaming.swift)                                  |
| Custom audio capture                                                            | [CustomAudioSource.swift](./APIExample/Examples/Advanced/CustomAudioSource/CustomAudioSource.swift)                      |
| Custom video renderer                                                           | [CustomVideoRender.swift](./APIExample/Examples/Advanced/CustomVideoRender/CustomVideoRender.swift)                      |
| Raw audio and video frames (Objective-C with C++, uses `AgoraMediaRawData.h` )  | [RawMediaData.swift](./APIExample/Examples/Advanced/RawMediaData/RawMediaData.swift)                                     |
| Raw audio frames (Native Objective-C interface)                                 | [RawAudioData.swift](./APIExample/Examples/Advanced/RawAudioData/RawAudioData.swift)                                     |
| Custom video capture (Push)                                                     | [CustomVideoSourcePush.swift](./APIExample/Examples/Advanced/CustomVideoSourcePush/CustomVideoSourcePush.swift)          |
| Custom video capture (mediaIO)                                                  | [CustomVideoSourceMediaIO.swift](./APIExample/Examples/Advanced/CustomVideoSourceMediaIO/CustomVideoSourceMediaIO.swift) |
| Switch a channel                                                                | [QuickSwitchChannel.swift](./APIExample/Examples/Advanced/QuickSwitchChannel/QuickSwitchChannel.swift)                   |
| Join multiple channels                                                          | [JoinMultiChannel.swift](.Examples/Advanced/JoinMultiChannel/JoinMultiChannel.swift)                                     |
| Join an audio channel                                                           | [JoinChannelAudio.swift](./APIExample/Examples/Basic/JoinChannelAudio/JoinChannelAudio.swift)                            |
| Join a video channel                                                            | [JoinChannelVideo.swift](./APIExample/Examples/Basic/JoinChannelVideo/JoinChannelVideo.swift)                            |
| Play audio files and audio mixing                                               | [AudioMixing.swift](API-Examples/iOS/APIExample/Examples/Advanced/AudioMixing/AudioMixing.swift)                         |
| Voice effects                                                                   | [VoiceChanger.swift](./APIExample/Examples/Advanced/VoiceChanger/VoiceChanger.swift)                                     |
| MediaPlayer Kit                                                                 | [MediaPlayer.swift](./APIExample/Examples/Advanced/MediaPlayer/MediaPlayer.swift)                                        |
| RTMP streaming                                                                  | [RTMPStreaming.swift](./APIExample/Examples/Advanced/RTMPStreaming/RTMPStreaming.swift)                                  |
| Audio/video stream SDK/custom encryption                                        | [StreamEncryption.swift](./APIExample/Examples/Advanced/StreamEncryption/StreamEncryption.swift)                         |
| Video metadata                                                                  | [VideoMetadata.swift](./APIExample/Examples/Advanced/VideoMetadata/VideoMetadata.swift)                                  |
| Group video chat                                                                | [VideoChat.swift](./APIExample/Examples/Advanced/VideoChat/VideoChat.swift)                                              |
| Pre-call test                                                                   | [PrecallTest.swift](./APIExample/Examples/Advanced/PrecallTest/PrecallTest.swift)                                        |
| Channel media relay                                                             | [MediaChannelRelay.swift](./APIExample/Examples/Advanced/MediaChannelRelay/MediaChannelRelay.swift)                      |
| Super resolution                                                                | [SuperResolution.swift](./APIExample/Examples/Advanced/SuperResolution/SuperResolution.swift)                            |
| Use multi-processing to send video streams from screen sharing and local camera | [ScreenShare.swift](./APIExample/Examples/Advanced/ScreenShare/ScreenShare.swift)                                        |
| Use custom video source (mediaIO) to implement AR function                      | [ARKit.swift](./APIExample/Examples/Advanced/ARKit/ARKit.swift)                                                          |
| Send data stream                                                                | [CreateDataStream.swift](./APIExample/Examples/Advanced/CreateDataStream/CreateDataStream.swift)                         |
| Geofencing                                                                      | [GlobalSettings.swift](./APIExample/Common/GlobalSettings.swift)                                                         |

## How to run the sample project

## Prerequisites

- Xcode 10.0+
- Physical iOS device (iPhone or iPad)
- iOS simulator is NOT supported

### Steps to run

1. Navigate to the **iOS** folder and run following command to install project dependencies:

    ```shell
    $ pod install
    ```

2. Open the generated `APIExample.xcworkspace` file with Xcode.
3. Edit the `KeyCenter.swift` file.
   - Replace `YOUR APP ID` with your App ID.
   - Replace `YOUR Certificate` with the Certificate.

    ```swift
    
    /**
      Agora assigns App IDs to app developers to identify projects and organizations.
     If you have multiple completely separate apps in your organization, for example built by different teams,
     you should use different App IDs.
     If applications need to communicate with each other, they should use the same App ID.
     In order to get the APP ID, you can open the agora console (https://console.agora.io/) to create a project,
     then the APP ID can be found in the project detail page.
     */
     static let AppId: String = <# YOUR APPID#>
     
     /**
      Agora provides App certificate to generate Token. You can deploy and generate a token on your server,
     or use the console to generate a temporary token.
     In order to get the APP ID, you can open the agora console (https://console.agora.io/) to create a project with the App Certificate enabled,
     then the APP Certificate can be found in the project detail page.
     PS: If the project does not have certificates enabled, leave this field blank.
     */
     static var Certificate: String? = <#YOUR Certificate#>
    ```

   > See [Get Started with Agora](https://docs.agora.io/en/Agora%20Platform/get_appid_token) to learn how to get an App ID and access token. You can get a temporary access token to quickly try out this sample project.
   >
   > The Channel name you used to generate the token must be the same as the channel name you use to join a channel.

   > To ensure communication security, Agora uses access tokens (dynamic keys) to authenticate users joining a channel.
   >
   > Temporary access tokens are for demonstration and testing purposes only and remain valid for 24 hours. In a production environment, you need to deploy your own server for generating access tokens. See [Generate a Token](https://docs.agora.io/en/Interactive%20Broadcast/token_server) for details.

4. Build and run the project in your iOS device.

You are all set! Feel free to play with this sample project and explore features of the Agora RTC SDK.

## Feedback

If you have any problems or suggestions regarding the sample projects, feel free to file an issue.

## Reference

- [RTC Objective-C SDK Product Overview](https://docs.agora.io/en/Interactive%20Broadcast/product_live?platform=iOS)
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

The sample projects are under the MIT license.
