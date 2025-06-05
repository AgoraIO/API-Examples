# API Example Android

*English | [中文](README.zh.md)*

This project presents you a set of API examples to help you understand how to use Agora APIs.

## Prerequisites

- Android Studio 3.0+
- Physical Android device
- Android simulator is supported

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Save the **App Certificate** from the Dashboard for later use.

5. Open `Android/APIExample` and edit the `app/src/main/res/values/string-configs.xml` file. Update `YOUR APP ID` with your App Id, update `YOUR APP CERTIFICATE` with the main app certificate from dashboard. Note you can leave the certificate variable `null` if your project has not turned on security token.

    ```
    // Agora APP ID.
    <string name="agora_app_id" translatable="false">YOUR APP ID</string>
    // Agora APP Certificate. If the project does not have certificates enabled, leave this field blank.
    // PS：It is unsafe to place the App Certificate on the client side, it is recommended to place it on the server side to ensure that the App Certificate is not leaked.
    <string name="agora_app_certificate" translatable="false">YOUR APP CERTIFICATE</string>
    ```

You are all set. Now connect your Android device and run the project.

### Beauty Configuration

> Third Part Beauty case use
> the [Agora Beauty API Library](https://github.com/AgoraIO-Community/BeautyAPI)

This project contains third-party beauty integration examples, which cannot be enabled by default
without configuring resources and certificates. The resource certificate configuration method is as
follows:

#### SenseTime

1. Contact SenseTime customer service to obtain the download link and certificate of the beauty sdk
2. Unzip the beauty sdk, and copy the following resources to the corresponding path

| SenseTime Beauty SDK Path                                            | Location                                                 |
|----------------------------------------------------------------------|----------------------------------------------------------|
| Android/models                                                       | app/src/main/assets/beauty_sensetime/models              |
| Android/smaple/SenseMeEffects/app/src/main/assets/sticker_face_shape | app/src/main/assets/beauty_sensetime/sticker_face_shape  |
| Android/smaple/SenseMeEffects/app/src/main/assets/style_lightly      | app/src/main/assets/beauty_sensetime/style_lightly       |
| Android/smaple/SenseMeEffects/app/src/main/assets/makeup_lip         | app/src/main/assets/beauty_sensetime/makeup_lip          |
| SenseME.lic                                                          | app/src/main/assets/beauty_sensetime/license/SenseME.lic |

#### FaceUnity

1. Contact FaceUnity customer service to obtain beauty resources and certificates
2. Put the beauty resource and certificate in the corresponding path

| FaceUnity Beauty Resources          | Location                                                                       |
|-------------------------------------|--------------------------------------------------------------------------------|
| makeup resource(e.g. naicha.bundle) | app/src/main/assets/beauty_faceunity/makeup                                    |
| sticker resource(e.g. fashi.bundle) | app/src/main/assets/beauty_faceunity/sticker                                   |
| authpack.java                       | app/src/main/java/io/agora/api/example/examples/advanced/beauty/authpack.java  |

#### ByteDance

1. Contact ByteDance customer service to obtain the download link and certificate of the beauty sdk
2. Unzip the ByteDance beauty resource and copy the following files/directories to the corresponding path

| ByteDance Beauty Resources      | Location                             |
|---------------------------------|--------------------------------------|
| resource/LicenseBag.bundle      | app/src/main/assets/beauty_bytedance |
| resource/ModelResource.bundle   | app/src/main/assets/beauty_bytedance |
| resource/ComposeMakeup.bundle   | app/src/main/assets/beauty_bytedance |
| resource/StickerResource.bundle | app/src/main/assets/beauty_bytedance |
| resource/StickerResource.bundle | app/src/main/assets/beauty_bytedance |

3. Modify the LICENSE_NAME in the app/src/main/java/io/agora/api/example/examples/advanced/beauty/ByteDanceBeauty.java file to the name of the applied certificate file.


### For Agora Extension Developers

Since version 4.0.0, Agora SDK provides an Extension Interface Framework. Developers could publish their own video/audio extension to Agora Extension Market. In this project includes a sample SimpleFilter example, by default it is disabled.
In order to enable it, you could do as follows:

1. Download [opencv](https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/androidLibs/opencv4.zip) library, unzip it and copy into Android/APIExample/agora-simple-filter/src/main/libs
```text
Android/APIExample/agora-simple-filter/src/main/libs
├── arm64-v8a
└── armeabi-v7a
```
2. Download [Agora SDK RESOURCE](https://docs.agora.io/en/sdks?platform=android), unzip it and copy c++ .so library (keeps arch folder) to Android/APIExample/agora-simple-filter/src/main/agoraLibs; Replace the low_level_api/include .h files in the Android/APIExample/agora-simple-filter/src/main/cpp/AgoraRtcKit directory.

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

3. Modify simpleFilter to true in Android/APIExample/gradle.properties

### Stream Encrypt

This project contains custom stream encrypt examples, which cannot be enabled by default. 
The configuration method is as follows:

1. Download [Agora SDK RESOURCE](https://docs.agora.io/en/sdks?platform=android), unzip it and copy c++ .so library (keeps arch folder) to Android/APIExample/agora-stream-encrypt/src/main/agoraLibs; Replace the hight_level_api/include .h files in the Android/APIExample/agora-stream-encrypt/src/main/cpp/include/agora directory.

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

2. Modify streamEncrypt to true in Android/APIExample/gradle.properties


## Contact Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/API-Examples/issues)

## License

The MIT License (MIT)
