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
4. 复制后台的 **App 证书** 并备注，稍后启动应用时会用到它

5. 打开 `Android/APIExample` 并编辑 `app/src/main/res/values/string-configs.xml`，将你的 AppID 、App主证书 分别替换到 `Your App Id` 和 `YOUR APP CERTIFICATE`

    ```
    // 声网APP ID。
    <string name="agora_app_id" translatable="false">YOUR APP ID</string>
    // 声网APP证书。如果项目没有开启证书鉴权，这个字段留空。
    // 注意：App证书放在客户端不安全，推荐放在服务端以确保 App 证书不会泄露。
    <string name="agora_app_certificate" translatable="false">YOUR APP CERTIFICATE</string>
    ```

然后你就可以编译并运行项目了。

### 美颜配置

> 第三方美颜集成使用了[美颜场景化API库](https://github.com/AgoraIO-Community/BeautyAPI)

本项目包含第三方美颜集成示例，在没有配置资源和证书的情况下，默认是无法启用的。资源证书配置方法如下：

#### 商汤美颜

1. 联系商汤客服获取美颜sdk下载链接以及证书
2. 解压美颜sdk，并将以下资源复制到对应路径

| 商汤SDK文件/目录                                                           | 项目路径                                                     |
|----------------------------------------------------------------------|----------------------------------------------------------|
| Android/models                                                       | app/src/main/assets/beauty_sensetime/models              |
| Android/smaple/SenseMeEffects/app/src/main/assets/sticker_face_shape | app/src/main/assets/beauty_sensetime/sticker_face_shape  |
| Android/smaple/SenseMeEffects/app/src/main/assets/style_lightly      | app/src/main/assets/beauty_sensetime/style_lightly       |
| Android/smaple/SenseMeEffects/app/src/main/assets/makeup_lip         | app/src/main/assets/beauty_sensetime/makeup_lip          |
| SenseME.lic                                                          | app/src/main/assets/beauty_sensetime/license/SenseME.lic |

#### 相芯美颜

1. 联系相芯客服获取美颜资源以及证书
2. 将美颜资源及证书放到对应路径下

| 美颜资源                 | 项目路径                                                                           |
|----------------------|--------------------------------------------------------------------------------|
| 美妆资源(如naicha.bundle) | app/src/main/assets/beauty_faceunity/makeup                                    |
| 贴纸资源(如fashi.bundle)  | app/src/main/assets/beauty_faceunity/sticker                                   |
| 证书authpack.java      | app/src/main/java/io/agora/api/example/examples/advanced/beauty/authpack.java  |

#### 字节美颜

1. 联系字节客服获取美颜sdk下载链接以及证书
2. 解压字节/火山美颜资源并复制以下文件/目录到对应路径下

| 字节SDK文件/目录                                       | 项目路径                                                  |
|--------------------------------------------------|-------------------------------------------------------|
| resource/LicenseBag.bundle                       | app/src/main/assets/beauty_bytedance           |
| resource/ModelResource.bundle                    | app/src/main/assets/beauty_bytedance           |
| resource/ComposeMakeup.bundle                    | app/src/main/assets/beauty_bytedance           |
| resource/StickerResource.bundle                  | app/src/main/assets/beauty_bytedance           |
| resource/StickerResource.bundle                  | app/src/main/assets/beauty_bytedance           |

3.
修改app/src/main/java/io/agora/api/example/examples/advanced/beauty/ByteDanceBeauty.java文件里LICENSE_NAME为申请到的证书文件名

### 对于Agora Extension开发者

从4.0.0SDK开始，Agora SDK支持插件系统和开放的云市场帮助开发者发布自己的音视频插件，本项目包含了一个SimpleFilter示例，默认是禁用的状态，如果需要开启编译和使用需要完成以下步骤：

1. 下载 [opencv](https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/androidLibs/opencv4.zip) 解压后复制到 Android/APIExample/agora-simple-filter/src/main/libs
```text
Android/APIExample/agora-simple-filter/src/main/libs
├── arm64-v8a
└── armeabi-v7a
```
2. 手动下载[Agora SDK包](https://doc.shengwang.cn/doc/rtc/android/resources), 解压后将c++动态库（包括架构文件夹）copy到Android/APIExample/agora-simple-filter/src/main/agoraLibs, 将 low_level_api/include 头文件替换到 Android/APIExample/agora-simple-filter/src/main/cpp/AgoraRtcKit 目录中

```text
Android/APIExample/agora-simple-filter/src/main/agoraLibs
├── arm64-v8a
├── armeabi-v7a
├── x86
└── x86_64
```
```text
Android/APIExample/agora-simple-filter/src/main/cpp
└── AgoraRtcKit
```

3. 修改Android/APIExample/gradle.properties配置文件中simpleFilter值为true

### 自定义加密

本项目包含自定义加密示例，默认是不启用的。配置方法如下：

1. 手动下载[Agora SDK包](https://doc.shengwang.cn/doc/rtc/android/resources), 解压后将c++动态库（包括架构文件夹）copy到Android/APIExample/agora-stream-encrypt/src/main/agoraLibs，将 hight_level_api/include 头文件替换到 Android/APIExample/agora-stream-encrypt/src/main/cpp/include/agora 目录中

```text
Android/APIExample/agora-stream-encrypt/src/main/agoraLibs
├── arm64-v8a
├── armeabi-v7a
├── x86
└── x86_64
```
```text
Android/APIExample/agora-stream-encrypt/src/main/cpp
└── include
    └── agora
```

2. 修改Android/APIExample/gradle.properties配置文件中streamEncrypt值为true

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
