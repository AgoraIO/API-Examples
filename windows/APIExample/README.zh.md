# Open Live Windows

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK，展示了常用场景的API示例

本开源项目使用 **C++** 语言

## 环境主备
* vs 2013(或更高版本)，默认支持vs2017。
* Windows 7(或更高版本)。

**注意** 使用其他版本需要自行修改该配置，编译release还需要安装兼容xp相关插件。


## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 App ID。将 APP_ID宏定义内容改为刚才申请的 App ID


* #define APP_ID _T("Your App ID")

然后在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **libs** 复制到本项目目录下（APIExample.slin所在的目录）。最后使用 Vs2017 打开 APIExample.sln，编译整个解决方案即可运行


## 基础场景

### 直播互动

* 切换角色
* 支持1v1，1v3, 1v8, 1v15
* 进出频道
* 显示本地和远端视频

## 进阶场景

### 旁路推流

* 加入频道后添加rtmp推流地址
* 移除推流地址
* 推流回调处理状态信息显示

### 插入媒体流

* 加入频道后inject 媒体流
* 显示插入状态回调信息
* 插入成功，收到一个666加入回调。本地mute 666的视频和音频（也可以不mute）
* 移除插入流

### 视频MetaData

* 加入频道，发送视频流
* 发送视频SEI信息，最大1024B
* 接收视频SEI信息
* 清楚SEI信息

## 联系我们

- 如果你遇到了困难，可以先参阅[常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考[官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考[官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看[社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Basic-Video-Broadcasting/issues)

## 代码许可

The MIT License (MIT).
