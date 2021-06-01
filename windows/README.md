# API Example Windows

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC C++ SDK for Windows.

## Project structure

The project uses a single program to combine a variety of functionalities. Each function is loaded as a window for you to play with.

| Function                                                    | Location                                                                                       |
| ----------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| Live streaming                                              | [LiveBroadcasting.h/cpp](./APIExample/Examples/Basic/LiveBroadcasting)                         |
| Audio effect                                                | [CAgoraEffectDlg.h/cpp](./APIExample/Examples/Advanced/AudioEffect)                            |
| Audio mixing                                                | [CAgoraAudioMixingDlg.h/cpp](./APIExample/Examples/Advanced/AudioMixing)                       |
| Set audio profile                                           | [CAgoraAudioProfile.h/cpp](./APIExample/Examples/Advanced/AudioProfile)                        |
| Set audio volume                                            | [CAgoraAudioVolumeDlg.h/cpp](./APIExample/Examples/Advanced/AudioVolume)                       |
| Video enhancement                                           | [CAgoraBeautyDlg.h/cpp](./APIExample/Examples/Advanced/Beauty)                                 |
| Audio enhancement                                           | [CAgoraBeautyAudio.h/cpp](./APIExample/Examples/Advanced/BeautyAudio)                          |
| Channel media relay                                         | [CAgoraCrossChannelDlg.h/cpp](./APIExample/Examples/Advanced/CrossChannel)                     |
| Custom audio capture                                        | [CAgoraCaptureAudioDlg.h/cpp](./APIExample/Examples/Advanced/CustomAudioCapture)               |
| Custom media encryption                                     | [CAgoraCustomEncryptDlg.h/cpp](./APIExample/Examples/Advanced/CustomEncrypt)                   |
| Custom Video capture (push)                                       | [CAgoraCaptureVideoDlg.h/cpp](./APIExample/Examples/Advanced/CustomVideoCapture)               |
| SDK media encryption                                        | [CAgoraMediaEncryptDlg.h/cpp](./APIExample/Examples/Advanced/MediaEncrypt)                     |
| Custom Video capture (mediaIO)                                       | [CAgoraMediaIOVideoCaptureDlg.h/cpp](./APIExample/Examples/Advanced/MediaIOCustomVideoCapture) |
| Media player (Agora Media Player Kit)                             | [CAgoraMediaPlayer.h/cpp](./APIExample/Examples/Advanced/MediaPlayer)                          |
| Join multiple channels                                      | [CAgoraMultiChannelDlg.h/cpp](./APIExample/Examples/Advanced/MultiChannel)                     |
| Publish camera and screen capture video via multiprocessing | [CAgoraMultiVideoSourceDlg.h/cpp](./APIExample/Examples/Advanced/MultiVideoSource)             |
| Raw audio source                                            | [CAgoraOriginalAudioDlg.h/cpp](./APIExample/Examples/Advanced/OriginalAudio)                   |
| Raw video source                                            | [CAgoraOriginalVideoDlg.h/cpp](./APIExample/Examples/Advanced/OriginalVideo)                   |
| Pre-call test                                               | [CAgoraPreCallTestDlg.h/cpp](./APIExample/Examples/Advanced/PreCallTest)                       |
| Regional connection                                         | [CAgoraRegionConnDlg.h/cpp](./APIExample/Examples/Advanced/RegionConn)                         |
| In call report                                              | [CAgoraReportInCallDlg.h/cpp](./APIExample/Examples/Advanced/ReportInCall)                     |
| RTMP streaming                                              | [AgoraRtmpStreaming.h/cpp](./APIExample/Examples/Advanced/RTMPStream)                          |
| Screen capture                                              | [AgoraScreenCapture.h/cpp](./APIExample/Examples/Advanced/ScreenShare)                         |
| Video metatdata                                             | [CAgoraMetaDataDlg.h/cpp](./APIExample/Examples/Advanced/VideoMetadata)                        |
| Video profile                                               | [CAgoraVideoProfileDlg.h/cpp](./APIExample/Examples/Advanced/VideoProfile)                     |

## How to run the sample project

## Prerequisites

- The default Visual Studio version for this project is 2017. If you are using other versions of Visual Studio, you may need additional adjustments.
- Windows 7 or higher. If you use Windows XP, you also need to install plugins for Windows XP compatibility when compiling the Release version.

### Steps to run

1. Navigate to the **Windows** folder and run following command to install project dependencies:

   ```shell
   $ installThirdParty.bat
   ```

   **Note:**
   If you encounter ps1 script errors, you may need to update your powershell.

2. Open the `APIExample.sln` file with Visual Studio.
3. Edit the `stdafx.h` file. Enter your App ID and access token.

   ```c++
   #define APP_ID     "<enter your agora app id>"


   #define APP_TOKEN  ""
   ```

   > See [Set up Authentication](https://docs.agora.io/en/Agora%20Platform/token) to learn how to get an App ID and access token. You can get a temporary access token to quickly try out this sample project.
   >
   > The Channel name you used to generate the token must be the same as the channel name you use to join a channel.

   > To ensure communication security, Agora uses access tokens (dynamic keys) to authenticate users joining a channel.
   >
   > Temporary access tokens are for demonstration and testing purposes only and remain valid for 24 hours. In a production environment, you need to deploy your own server for generating access tokens. See [Generate a Token](https://docs.agora.io/en/Interactive%20Broadcast/token_server) for details.

4. Select x86 as the platform version. Build and run the solution in your Windows device.

You are all set! Feel free to play with this sample project and explore features of the Agora RTC SDK.

## Feedback

If you have any problems or suggestions regarding the sample projects, feel free to file an issue.

## Reference

- [RTC C++ SDK Product Overview](https://docs.agora.io/en/Interactive%20Broadcast/product_live?platform=Windows)
- [RTC C++ SDK API Reference](https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/cpp/index.html)

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

The sample projects are under the MIT license. See the [LICENSE](/LICENSE) file for details.
