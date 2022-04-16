# API Example iOS

_[English](README.md) | 中文_

## 简介

该仓库包含了使用 RTC C++ SDK for Windows 的示例项目。

![api-example-windows-cn](https://user-images.githubusercontent.com/10089260/120452366-bacdfc00-c3c4-11eb-8264-21cb715275c8.PNG)


## 项目结构

此项目使用一个单独的 Windows 程序实现了多种功能。每个功能以 window 的形式加载，方便你进行试用。

| 功能                              | 位置                                                                                           |
| --------------------------------- | ---------------------------------------------------------------------------------------------- |
| 视频直播                          | [LiveBroadcasting.h/cpp](./APIExample/APIExample/Basic/LiveBroadcasting)                         |
| 变声与音效                        | [CAgoraEffectDlg.h/cpp](./APIExample/APIExample/Advanced/AudioEffect)                            |
| 混音                              | [CAgoraAudioMixingDlg.h/cpp](./APIExample/APIExample/Advanced/AudioMixing)                       |
| 设置音频属性                      | [CAgoraAudioProfile.h/cpp](./APIExample/APIExample/Advanced/AudioProfile)                        |
| 设置音量                          | [CAgoraAudioVolumeDlg.h/cpp](./APIExample/APIExample/Advanced/AudioVolume)                       |
| 美颜                              | [CAgoraBeautyDlg.h/cpp](./APIExample/APIExample/Advanced/Beauty)                                 |
| 美声                              | [CAgoraBeautyAudio.h/cpp](./APIExample/APIExample/Advanced/BeautyAudio)                          |
| 频道媒体转发                      | [CAgoraCrossChannelDlg.h/cpp](./APIExample/APIExample/Advanced/CrossChannel)                     |
| 自定义音频采集                    | [CAgoraCaptureAudioDlg.h/cpp](./APIExample/APIExample/Advanced/CustomAudioCapture)               |
| 自定义媒体加密                    | [CAgoraCustomEncryptDlg.h/cpp](./APIExample/APIExample/Advanced/CustomEncrypt)                   |
| 自定义视频采集 (Push 方式)        | [CAgoraCaptureVideoDlg.h/cpp](./APIExample/APIExample/Advanced/CustomVideoCapture)               |
| SDK 媒体加密                      | [CAgoraMediaEncryptDlg.h/cpp](./APIExample/APIExample/Advanced/MediaEncrypt)                     |
| 自定义视频采集 (mediaIO 方式)     | [CAgoraMediaIOVideoCaptureDlg.h/cpp](./APIExample/APIExample/Advanced/MediaIOCustomVideoCaptrue) |
| 媒体播放器 (Agora 媒体播放器组件) | [CAgoraMediaPlayer.h/cpp](./APIExample/APIExample/Advanced/MediaPlayer)                          |
| 加入多频道                        | [CAgoraMultiChannelDlg.h/cpp](./APIExample/APIExample/Advanced/MultiChannel)                     |
| 使用多进程发布摄像头和屏幕采集流  | [CAgoraMultiVideoSourceDlg.h/cpp](./APIExample/APIExample/Advanced/MultiVideoSource)             |
| 原始音频数据                      | [CAgoraOriginalAudioDlg.h/cpp](./APIExample/APIExample/Advanced/OriginalAudio)                   |
| 原始视频数据                      | [CAgoraOriginalVideoDlg.h/cpp](./APIExample/APIExample/Advanced/OriginalVideo)                   |
| 呼叫前测试                        | [CAgoraPreCallTestDlg.h/cpp](./APIExample/APIExample/Advanced/PreCallTest)                       |
| 区域访问限制                      | [CAgoraRegionConnDlg.h/cpp](./APIExample/APIExample/Advanced/RegionConn)                         |
| 通话中质量监测                    | [CAgoraReportInCallDlg.h/cpp](./APIExample/APIExample/Advanced/ReportInCall)                     |
| RTMP 推流                         | [AgoraRtmpStreaming.h/cpp](./APIExample/APIExample/Advanced/RTMPStream)                          |
| 屏幕共享                          | [AgoraScreenCapture.h/cpp](./APIExample/APIExample/Advanced/ScreenShare)                         |
| 视频元数据                        | [CAgoraMetaDataDlg.h/cpp](./APIExample/APIExample/Advanced/VideoMetadata)                        |
| 视频属性                          | [CAgoraVideoProfileDlg.h/cpp](./APIExample/APIExample/Advanced/VideoProfile)                     |

## 如何运行示例项目

### 前提条件

- 默认 Visual Studio 版本为 2017。如果你使用其他版本的 Visual Studio，可能需要额外配置。
- Windows 7 或更高版本。如果你使用 Windows XP,编译 release 还需要安装兼容 Windows XP 的插件。

### 运行步骤

1. 在 **windows** 目录下运行 `installThirdParty.bat` 文件安装依赖项:

   ```shell
   $ installThirdParty_x86.bat 或者installThirdParty_x64.bat。也可以直接到下一步，当build solution的时候会直接下载依赖库。
   ```

   **注意:**

   如果你遇到 ps1 脚本错误，你可以尝试升级 powershell。

2. 使用 Visual Studio 打开 `APIExample.sln` 文件。
3. 编辑 `stdafx.h` 文件。键入你的 App ID 和 access token。

   ```c++
   #define APP_ID     "<enter your agora app id>"



   #define APP_TOKEN  ""
   ```

   > 参考 [校验用户权限](https://docs.agora.io/cn/Agora%20Platform/token) 了解如何获取 App ID 和 Token。你可以获取一个临时 token，快速运行示例项目。
   >
   > 生成 Token 使用的频道名必须和加入频道时使用的频道名一致。

   > 为提高项目的安全性，Agora 使用 Token（动态密钥）对即将加入频道的用户进行鉴权。
   >
   > 临时 Token 仅作为演示和测试用途。在生产环境中，你需要自行部署服务器签发 Token，详见[生成 Token](https://docs.agora.io/cn/Interactive%20Broadcast/token_server)。

4. 选择 Win32 为运行平台版本。在 Windows 设备中构建并运行解决方案。

一切就绪。你可以自由探索示例项目，体验 SDK 的丰富功能。

## 反馈

如果你有任何问题或建议，可以通过 issue 的形式反馈。

## 参考文档

- [RTC C++ SDK 产品概述](https://docs.agora.io/cn/Interactive%20Broadcast/product_live?platform=Windows)
- [RTC C++ SDK API 参考](https://docs.agora.io/cn/Interactive%20Broadcast/API%20Reference/cpp/index.html)

## 相关资源

- 你可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方 SDK 示例](https://github.com/AgoraIO)
- 如果你想了解声网 SDK 在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单

## 代码许可

示例项目遵守 MIT 许可证。
