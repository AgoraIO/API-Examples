# API Example iOS

*[English](README.md) | 中文*

这个开源示例项目演示了Agora视频SDK的部分API使用示例，以帮助开发者更好地理解和运用Agora视频SDK的API。

## 问题描述
iOS 系统版本升级至 14.0 版本后，用户首次使用集成了声网 iOS 语音或视频 SDK 的 app 时会看到查找本地网络设备的弹窗提示。默认弹窗界面如下图所示：

![](../pictures/ios_14_privacy_zh.png)

[解决方案](https://docs.agora.io/cn/faq/local_network_privacy)

## 环境准备

- XCode 10.0 +
- iOS 真机设备
- 不支持模拟器

## 运行示例程序

这个段落主要讲解了如何编译和运行实例程序。

### 安装依赖库

切换到 **iOS** 目录，运行以下命令使用CocoaPods安装依赖，Agora视频SDK会在安装后自动完成集成。

```
pod install
```

运行后确认 `APIExample.xcworkspace` 正常生成即可。

### 创建Agora账号并获取AppId

在编译和启动实例程序前，你需要首先获取一个可用的App Id:

1. 在[agora.io](https://dashboard.agora.io/signin/)创建一个开发者账号
2. 前往后台页面，点击左部导航栏的 **项目 > 项目列表** 菜单
3. 复制后台的 **App Id** 并备注，稍后启动应用时会用到它
4. 在项目页面生成临时 **Access Token** (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。

5. 打开 `APIExample.xcworkspace` 并编辑 `KeyCenter.swift`，将你的 AppID 和 Token 分别替换到 `<#Your App Id#>` 与 `<#Temp Access Token#>`

    ```
    let AppID: String = <#Your App Id#>
    // 如果你没有打开Token功能，token可以直接给nil
    let Token: String? = <#Temp Access Token#>
    ```

然后你就可以使用 `APIExample.xcworkspace` 编译并运行项目了。

## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Basic-Video-Call/issues)

## 代码许可

The MIT License (MIT)
