# Android 示例项目

_[English](README.md) | 中文_

## 简介

该仓库包含了使用 RTC Java SDK for Android 的示例项目。

![image](https://user-images.githubusercontent.com/10089260/116193950-be85b780-a762-11eb-8cac-1eb708d0b1d4.png)

## 项目结构

此项目使用一个单独的 app 实现了多种功能。每个功能以 fragment 的形式加载，方便你进行试用。

| 功能                                             | 位置                                                                                                                    |
| ------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------- |
| 音频直播                                         | [JoinChannelAudio.java](./app/src/main/java/io/agora/api/example/examples/basic/JoinChannelAudio.java)                  |
| 视频直播                                         | [JoinChannelVideo.java](./app/src/main/java/io/agora/api/example/examples/basic/JoinChannelVideo.java)                  |
| 自定义音频采集                                   | [CustomAudioSource.java](./app/src/main/java/io/agora/api/example/examples/advanced/customaudio/CustomAudioSource.java) |
| 自定义视频渲染                                   | [CustomRemoteVideoRender.java](./app/src/main/java/io/agora/api/example/examples/advanced/CustomRemoteVideoRender.java) |
| 原始音频和视频数据 (使用 JNI 接口)               | [ProcessRawData.java](./app/src/main/java/io/agora/api/example/examples/advanced/ProcessRawData.java)                   |
| 原始音频数据 (使用 Java 方法)                    | [ProcessAudioRawData.java](./app/src/main/java/io/agora/api/example/examples/advanced/ProcessAudioRawData.java)         |
| 自定义视频采集 (Push)                            | [PushExternalVideo.java](./app/src/main/java/io/agora/api/example/examples/advanced/PushExternalVideo.java)             |
| 切换频道                                         | [VideoQuickSwitch.java](./app/src/main/java/io/agora/api/example/examples/advanced/VideoQuickSwitch.java)               |
| 加入多频道                                       | [JoinMultipleChannel.java](./app/src/main/java/io/agora/api/example/examples/advanced/JoinMultipleChannel.java)         |
| 设置音频属性                                     | [SetAudioProfile.java](./app/src/main/java/io/agora/api/example/examples/advanced/SetAudioProfile.java)                 |
| 设置视频属性                                     | [SetVideoProfile.java](./app/src/main/java/io/agora/api/example/examples/advanced/SetVideoProfile.java)                 |
| 播放音频文件与混音                               | [PlayAudioFiles.java](./app/src/main/java/io/agora/api/example/examples/advanced/PlayAudioFiles.java)                   |
| 音频效果                                         | [VoiceEffects.java](./app/src/main/java/io/agora/api/example/examples/advanced/VoiceEffects.java)                       |
| 媒体播放器组件                                   | [MediaPlayerKit.java](./app/src/main/java/io/agora/api/example/examples/advanced/MediaPlayerKit.java)                   |
| 区域访问限制                                     | [GeoFencing.java](./app/src/main/java/io/agora/api/example/examples/advanced/GeoFencing.java)                           |
| RTMP 推流                                        | [RTMPStreaming.java](./app/src/main/java/io/agora/api/example/examples/advanced/RTMPStreaming.java)                     |
| 自定义加密媒体流                                 | [StreamEncrypt.java](./app/src/main/java/io/agora/api/example/examples/advanced/StreamEncrypt.java)                     |
| 切换视频源 (自定义视频采集 (MediaIO) 与屏幕共享) | [SwitchExternalVideo.java](./app/src/main/java/io/agora/api/example/examples/advanced/SwitchExternalVideo.java)         |
| 视频元数据                                       | [VideoMetadata.java](./app/src/main/java/io/agora/api/example/examples/advanced/VideoMetadata.java)                     |
| 报告通话状态                                     | [InCallReport.java](./app/src/main/java/io/agora/api/example/examples/advanced/InCallReport.java)                       |
| 调整音量                                         | [AdjustVolume.java](./app/src/main/java/io/agora/api/example/examples/advanced/AdjustVolume.java)                       |
| 呼叫前测试                                       | [PreCallTest.java](./app/src/main/java/io/agora/api/example/examples/advanced/PreCallTest.java)                         |
| 频道媒体流转发                                   | [HostAcrossChannel.java](./app/src/main/java/io/agora/api/example/examples/advanced/HostAcrossChannel.java)             |
| 超清分辨率                                       | [SuperResolution.java](./app/src/main/java/io/agora/api/example/examples/advanced/SuperResolution.java)                 |
| 使用 SDK 提供的方法加密媒体流                    | [ChannelEncryption.java](./app/src/main/java/io/agora/api/example/examples/advanced/ChannelEncryption.java)             |
| 使用多进程同时发送摄像头采集的视频和屏幕共享视频 | [MultiProcess.java](./app/src/main/java/io/agora/api/example/examples/advanced/MultiProcess.java)                       |
| 在直播场景下切换角色                             | [LiveStreaming.java](./app/src/main/java/io/agora/api/example/examples/advanced/LiveStreaming.java)                     |
| 使用自定义视频采集 (mediaIO) 实现 AR 功能        | [ARCore.java](./app/src/main/java/io/agora/api/example/examples/advanced/ARCore.java)                                   |
| 发送数据流                                       | [SendDataStream.java](./app/src/main/java/io/agora/api/example/examples/advanced/SendDataStream.java)                   |

## 如何运行示例项目

### 前提条件

- 真实的 Android 设备或 Android 虚拟机
- Android Studio (推荐最新版)

### 运行步骤

1. 在 Android Studio 中，开启 `/Android/APIExample`。
2. 将项目与 Gradle 文件同步。
3. 编辑 `/Android/APIExample/app/src/main/res/values/string_config.xml` 文件。

   - 将 `YOUR APP ID` 替换为你的 App ID。
   - 将 `YOUR ACCESS TOKEN` 替换为你的 Access Token。

   ```xml
   <string name="agora_app_id" translatable="false">YOUR APP ID</string>
   <string name="agora_access_token" translatable="false">YOUR ACCESS TOKEN</string>
   ```

   > 参考 [开始使用 Agora 平台](https://docs.agora.io/cn/Agora%20Platform/get_appid_token) 了解如何获取 App ID 和 Token。你可以获取一个临时 token，快速运行示例项目。
   >
   > 生成 Token 使用的频道名必须和加入频道时使用的频道名一致。

   > 为提高项目的安全性，Agora 使用 Token（动态密钥）对即将加入频道的用户进行鉴权。
   >
   > 临时 Token 仅作为演示和测试用途。在生产环境中，你需要自行部署服务器签发 Token，详见[生成 Token](https://docs.agora.io/cn/Interactive%20Broadcast/token_server)。

4. 构建项目，在虚拟器或真实 Android 设备中运行项目。

一切就绪。你可以自由探索示例项目，体验 RTC Java SDK 的丰富功能。

## 反馈

如果你有任何问题或建议，可以通过 issue 的形式反馈。

## 参考文档

- [RTC Java SDK 产品概述](https://docs.agora.io/cn/Interactive%20Broadcast/product_live?platform=Android)
- [RTC Java SDK API 参考](https://docs.agora.io/cn/Interactive%20Broadcast/API%20Reference/java/index.html)

## 相关资源

- 你可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方 SDK 示例](https://github.com/AgoraIO)
- 如果你想了解声网 SDK 在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单

## 代码许可

示例项目遵守 MIT 许可证。
