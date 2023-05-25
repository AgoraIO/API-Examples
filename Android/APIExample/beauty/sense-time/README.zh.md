# 配置指南
*[English](README.md) | 中文*

此示例提供了商汤美颜的最佳接入方式。

> 声网插件云市场上提供了更便捷的集成方式，可以登录声网[控制台](https://console.agora.io/)查看[集成教程](https://console.agora.io/marketplace/license/introduce?serviceName=sensetime-ar)


## 1 商汤美颜SDK

- 联系商汤客服获取最新的sdk下载地址，下载并解压
- 复制**libst_mobile.so**到**"beauty/sense-time/src/main/jniLibs"** 文件夹下
```
    jniLibs
      |_ arm64-v8a         
      |_ armeabi-v7a
```
- 复制**STMobileJNI-release.aar**和**SenseArSourceManager-release.aar**到**"beauty/sense-time/libs"** 文件夹下
```
    libs
      |_ SenseArSourceManager-release.aar         
      |_ STMobileJNI-release.aar
```

## 2 美颜资源

将SDK里的资源文件复制到**"beauty/sense-time/src/main/assets"** 目录下。这个项目用到的资源文件列举如下：

- 模型资源（必须）：在[STRenderer](src/main/java/com/sensetime/effects/STRenderer.java)中加载
models/M_Segment_DBLV2_Pant_1.1.6.model
models/M_SenseME_3DMesh_Face_2.0.2.model
models/M_SenseME_Attribute_1.0.1.model
models/M_SenseME_Avatar_Help_2.2.0.model
models/M_SenseME_CatFace_3.0.0.model
models/M_SenseME_DogFace_2.0.0.model
models/M_SenseME_Face_Extra_Advanced_6.0.13.model
models/M_SenseME_Face_Video_7.0.0.model
models/M_SenseME_Foot_1.0.2.model
models/M_SenseME_Hand_5.4.0.model
models/M_SenseME_Segment_4.14.1.model
models/M_SenseME_Segment_DBL_Face_1.0.7.model
models/M_SenseME_Segment_Hair_1.3.4.model
models/M_SenseME_Segment_Head_1.0.3.model
models/M_SenseME_Segment_MouthOcclusion_FastV1_1.1.3.model
models/M_SenseME_Segment_Skin_1.1.1.model
models/M_SenseME_Segment_Sky_1.0.3.model

- 美妆资源（测试资源）：在[BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)中开启美妆时加载
makeup_lip/12自然.zip

- 贴纸（测试资源）：在[BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)中开启贴纸时加载
sticker_face_shape/lianxingface.zip

- 滤镜（测试资源）：在[BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)中开启滤镜时加载
filter_portrait/filter_style_babypink.model

注意：滤镜资源有时效限制，需要和商汤客服拿到有效的滤镜资源。

## 3 证书

商汤提供了两种证书配置，一种是使用本地lic文件的离线方式，一种是根据applicationId通过商汤接口获取的在线方式。下面说明这两种在项目里的配置方法。

### 3.1 离线模式
- 联系商汤客服获取证书文件，证书文件也有分 **在线/离线** 需要注意
- 如果是离线证书，将证书重命名为**"SenseME.lic"** ，如果是在线证书则重命名为**"SenseME_Online.lic"** ，然后复制到**"beauty/sense-time/src/main/assets/license"** 文件夹下
- 将[STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java)里的**USING_SERVER_LICENSE**变量设成false
- 如果获取到的证书是否是离线，是离线则将[STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java)里的**USING_ASSETS_ONLINE_LICENSE**变量设成false，否则设成true


### 3.2 在线模式
- 联系商汤客服配置应用appId对应的证书
- 将[STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java)里的**USING_SERVER_LICENSE**变量设成true即可

## 4 配置编译

- 将项目目录下**gradle.properties**里的**beauty_sensetime**设置true
- 编译运行

