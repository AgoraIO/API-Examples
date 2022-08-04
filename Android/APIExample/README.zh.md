# API Example Android

*[English](README.md) | 中文*

这个开源示例项目演示了Agora视频SDK的部分API使用示例，以帮助开发者更好地理解和运用Agora视频SDK的API。

## 环境准备

- Android Studio 3.0+
- Android 真机设备
- 支持模拟器

## 运行示例程序

这个段落主要讲解了如何编译和运行实例程序。

### 创建Agora账号并获取AppId

在编译和启动实例程序前，你需要首先获取一个可用的App Id:

1. 在[agora.io](https://dashboard.agora.io/signin/)创建一个开发者账号
2. 前往后台页面，点击左部导航栏的 **项目 > 项目列表** 菜单
3. 复制后台的 **App Id** 并备注，稍后启动应用时会用到它
4. 在项目页面生成临时 **Access Token** (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。

5. 打开 `Android/APIExample` 并编辑 `app/src/main/res/values/string-config.xml`，将你的 AppID 和 Token 分别替换到 `<#Your App Id#>` 与 `<#Temp Access Token#>`

    ```
    <string name="agora_app_id" translatable="false">YOUR APP ID</string>
    // 如果你没有打开Token功能，token可以直接给null或者不填
    <string name="agora_access_token" translatable="false">YOUR ACCESS TOKEN</string>
    ```

然后你就可以编译并运行项目了。

### 对于Agora Extension开发者

从4.0.0SDK开始，Agora SDK支持插件系统和开放的云市场帮助开发者发布自己的音视频插件，本项目包含了一个SimpleFilter示例，默认是禁用的状态，如果需要开启编译和使用需要完成以下步骤：

1. 下载 [opencv](https://github.com/AgoraIO/API-Examples/releases/download/3.7.201/opencv4.zip) 解压后复制到 Android/APIExample/agora-simple-filter/src/main/jniLibs
2. 手动下载[Agora SDK包](https://download.agora.io/sdk/release/Agora_Native_SDK_for_Android_v4.0.0-rc.1_FULL.zip), 解压后将c++动态库（包括架构文件夹）copy到Android/APIExample/agora-simple-filter/src/main/agoraLibs
3. 修改Android/APIExample/gradle.properties配置文件中simpleFilter值为true

## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/API-Examples/issues)

## 代码许可

The MIT License (MIT)
