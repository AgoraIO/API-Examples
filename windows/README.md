# API Example Windows

_English | [中文](README.zh.md)_

## Overview

This repository contains sample projects using the Agora RTC C++ SDK for Windows.

## Project structure

The project uses a single program to combine a variety of functionalities. Each function is loaded as a window for you to play with.

| Function                                                    | Location                                                                                       |
| ----------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| Live streaming                                              | [LiveBroadcasting.h/cpp](./APIExample/APIExample/Basic/LiveBroadcasting)                         |
| Audio effect                                                | [CAgoraEffectDlg.h/cpp](./APIExample/APIExample/Advanced/AudioEffect)                            |
| Audio mixing                                                | [CAgoraAudioMixingDlg.h/cpp](./APIExample/APIExample/Advanced/AudioMixing)                       |
| Set audio profile                                           | [CAgoraAudioProfile.h/cpp](./APIExample/APIExample/Advanced/AudioProfile)                        |
| Set audio volume                                            | [CAgoraAudioVolumeDlg.h/cpp](./APIExample/APIExample/Advanced/AudioVolume)                       |
| Video enhancement                                           | [CAgoraBeautyDlg.h/cpp](./APIExample/APIExample/Advanced/Beauty)                                 |
| Audio enhancement                                           | [CAgoraBeautyAudio.h/cpp](./APIExample/APIExample/Advanced/BeautyAudio)                          |
| Channel media relay                                         | [CAgoraCrossChannelDlg.h/cpp](./APIExample/APIExample/Advanced/CrossChannel)                     |
| Custom audio capture                                        | [CAgoraCaptureAudioDlg.h/cpp](./APIExample/APIExample/Advanced/CustomAudioCapture)               |
| Custom media encryption                                     | [CAgoraCustomEncryptDlg.h/cpp](./APIExample/APIExample/Advanced/CustomEncrypt)                   |
| Custom Video capture (push)                                       | [CAgoraCaptureVideoDlg.h/cpp](./APIExample/APIExample/Advanced/CustomVideoCapture)               |
| SDK media encryption                                        | [CAgoraMediaEncryptDlg.h/cpp](./APIExample/APIExample/Advanced/MediaEncrypt)                     |
| Custom Video capture (mediaIO)                                       | [CAgoraMediaIOVideoCaptureDlg.h/cpp](./APIExample/APIExample/Advanced/MediaIOCustomVideoCaptrue) |
| Media player (Agora Media Player Kit)                             | [CAgoraMediaPlayer.h/cpp](./APIExample/APIExample/Advanced/MediaPlayer)                          |
| Join multiple channels                                      | [CAgoraMultiChannelDlg.h/cpp](./APIExample/APIExample/Advanced/MultiChannel)                     |
| Publish camera and screen capture video via multiprocessing | [CAgoraMultiVideoSourceDlg.h/cpp](./APIExample/APIExample/Advanced/MultiVideoSource)             |
| Raw audio source                                            | [CAgoraOriginalAudioDlg.h/cpp](./APIExample/APIExample/Advanced/OriginalAudio)                   |
| Raw video source                                            | [CAgoraOriginalVideoDlg.h/cpp](./APIExample/APIExample/Advanced/OriginalVideo)                   |
| Pre-call test                                               | [CAgoraPreCallTestDlg.h/cpp](./APIExample/APIExample/Advanced/PreCallTest)                       |
| Regional connection                                         | [CAgoraRegionConnDlg.h/cpp](./APIExample/APIExample/Advanced/RegionConn)                         |
| In call report                                              | [CAgoraReportInCallDlg.h/cpp](./APIExample/APIExample/Advanced/ReportInCall)                     |
| RTMP streaming                                              | [AgoraRtmpStreaming.h/cpp](./APIExample/APIExample/Advanced/RTMPStream)                          |
| Screen capture                                              | [AgoraScreenCapture.h/cpp](./APIExample/APIExample/Advanced/ScreenShare)                         |
| Video metatdata                                             | [CAgoraMetaDataDlg.h/cpp](./APIExample/APIExample/Advanced/VideoMetadata)                        |
| Video profile                                               | [CAgoraVideoProfileDlg.h/cpp](./APIExample/APIExample/Advanced/VideoProfile)                     |

## How to run the sample project

## Prerequisites

- The default Visual Studio version for this project is 2017. If you are using other versions of Visual Studio, you may need additional adjustments.
- Windows 7 or higher. If you use Windows XP, you also need to install plugins for Windows XP compatibility when compiling the Release version.

### Steps to run

1. Navigate to the **windows** folder and run following command to install project dependencies:

   ```shell
   $ installThirdParty.bat
   ```

   **Note:**
   If you encounter ps1 script errors, you may need to update your powershell.


2. Open the `APIExample.sln` file with Visual Studio.
3. Edit the `stdafx.h` file. Enter your App ID and token.

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

4. Select Win32 as the platform version. Build and run the solution in your Windows device.

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

## License

The sample projects are under the MIT license. See the [LICENSE](/LICENSE) file for details.
