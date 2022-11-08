# 配置指南
*English | [中文](README.zh.md)*

This example demonstrates how to integrate byte beauty in Rtc call.

> The plug-in cloud market provides a more convenient integration method. You can log in to the sound network [console](https://console.agora.io/) to view the [integration tutorial](https://console.agora.io/ marketplace/license/introduce?serviceName=bytedance-volcengine-ar)


## 1 Beauty SDK

- Contact ByteDance customer service to get the latest sdk download address, download and unzip.
- Copy **effectAAR-release.aar** to **"beauty/bytedance/libs"** folder
```
    libs
      |_ effectAAR-release.aar
```

## 2 License

- Contact ByteDance customer service to get the license with application Id
- Modify **applicationId** in **app/build.gradle** to the special application Id
- Copy the license to **assets/resource/LicenseBag.bundle**
- Change **LICENSE_NAME** in **beauty/bytedance/src/main/java/com/byteddance/effect/ResourceHelper.java** to the license name.


## 3 美颜资源

1. Contact ByteDance customer service to get the resources
2. Copy the resource files in the SDK to the **"beauty/bytedance/src/main/assets"** directory：
   resource/LicenseBag.bundle
   resource/ModelResource.bundle
   resource/StickerResource.bundle
   resource/ComposeMakeup.bundle
   resource/FilterResource.bundle

Note: There is a binding relationship between resources and certificates, and you need to get the corresponding resource files from byte customer service.

## 4 Configure compilation

- Set the **beauty_bytedance** variable in **gradle.properties** in the project directory to true
- compile and run
