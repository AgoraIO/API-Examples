# 配置指南
*[English](README.md) | 中文*

此示例提供了两种相芯美颜接入方式，单输入（仅NV21）和双输入（NV21和纹理）。双输入比单输入少了 纹理转NV21 的这步操作，转换性能比较高，但是在低端机上可能出现贴纸不跟脸等情况，建议根据面向的客户来选择合适的接入方式。

> 声网插件云市场上提供了更便捷的集成方式，可以登录声网[控制台](https://console.agora.io/)查看[集成教程](https://console.agora.io/marketplace/license/introduce?serviceName=faceunity-ar)

## 1 证书激活

请联系 sales@agora.io 获取证书文件替换本项目中的 **src/main/java/io/agora/rtcwithfu/authpack.java**。


## 2 配置编译

- 将项目目录下**gradle.properties**里的**beauty_faceunity**设置true
- 编译运行

