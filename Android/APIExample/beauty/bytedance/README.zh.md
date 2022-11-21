# 配置指南
*[English](README.md) | 中文*

此示例演示在Rtc通话中如何接入字节美颜。

> 声网插件云市场上提供了更便捷的集成方式，可以登录声网[控制台](https://console.agora.io/)查看[集成教程](https://console.agora.io/marketplace/license/introduce?serviceName=bytedance-volcengine-ar)


## 1 字节美颜SDK

请联系 labcv_business@bytedance.com 获取 ByteEffect SDK，下载并解压。
- 复制**effectAAR-release.aar**到**"beauty/bytedance/libs"** 文件夹下
```
    libs
      |_ effectAAR-release.aar
```

## 2 证书

1. 联系字节客服拿到对应appId的证书
2. 修改**app/build.gradle**中**applicationId**为对应的字节授权证书中的id
3. 复制证书到**assets/resource/LicenseBag.bundle**目录下
4. 将**beauty/bytedance/src/main/java/com/byteddance/effect/ResourceHelper.java**中**LICENSE_NAME**改为对应字节授权证书名。



## 3 美颜资源

1. 联系字节客服拿到证书对应的所需资源包，下载解压
2. 将SDK里的资源文件复制到**"beauty/bytedance/src/main/assets"** 目录下。这个项目用到的资源文件列举如下：
  resource/LicenseBag.bundle
  resource/ModelResource.bundle
  resource/StickerResource.bundle
  resource/ComposeMakeup.bundle
  resource/FilterResource.bundle

注意：资源与证书是有绑定关系的，需要和字节客服拿到对应的资源文件。

## 4 配置编译

- 将项目目录下**gradle.properties**里的**beauty_bytedance**设置true
- 编译运行
