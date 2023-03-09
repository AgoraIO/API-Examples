# Configuration Guide
*English | [中文](README.zh.md)*

This example provides the best beauty access method. 
> The plug-in cloud market provides a more convenient integration method. You can log in to the sound network [console](https://console.agora.io/) to view the [integration tutorial](https://console.agora.io/ marketplace/license/introduce?serviceName=sensetime-ar)

## 1 SenseTime Beauty SDK

- Contact SenseTime customer service to get the latest sdk download address, download and unzip
- Copy **libst_mobile.so** to **"beauty/sense-time/src/main/jniLibs"** folder
```
    jniLibs
      |_ arm64-v8a         
      |_ armeabi-v7a
```
- Copy **STMobileJNI-release.aar** and **SenseArSourceManager-release.aar** to **"beauty/sense-time/libs"** folder
```
    libs
      |_ SenseArSourceManager-release.aar         
      |_ STMobileJNI-release.aar
```

## 2 Beauty resources

Copy the resource files in the SDK to the **"beauty/sense-time/src/main/assets"** directory. The resource files used in this project are listed below:

- Model resources (required): Loaded in [STRenderer](src/main/java/com/sensetime/effects/STRenderer.java)
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

- MakeUp resources (test resources): Loaded when beauty is enabled in [BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)
  makeup_lip/12自然.zip

- Stickers (test resource): Loaded when stickers are enabled in [BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)
  sticker_face_shape/lianxingface.zip

- Filter (test resource): Loaded when the filter is enabled in [BeautySenseTimeImpl](src/main/java/io/agora/beauty/sensetime/BeautySenseTimeImpl.java)
  filter_portrait/filter_style_babypink.model

Note: Filter resources are time-limited, and you need to get valid filter resources from SenseTime customer service.

## 3 License

SenseTime provides two kinds of License configurations, one is the offline method using the local license file, and the other is the online method obtained through the SenseTime interface according to the applicationId. The following describes these two configuration methods in the project.

### 3.1 Offline mode
- Contact SenseTime customer service to get the license file, the license file is also divided **Online/Offline** need to pay attention
- If it is an offline license, rename the certificate to **"SenseME.lic"** , if it is an online license, rename it to **"SenseME_Online.lic"** , then copy **"beauty/sense-time/src/main/assets/license"** folder
- Set the **USING_SERVER_LICENSE** variable in [STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java) to false
- According to whether the obtained license is offline, if it is offline, set the **USING_ASSETS_ONLINE_LICENSE** variable in [STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java) to false, otherwise set it to true


### 3.2 Online mode
- Contact SenseTime customer service to configure the certificate corresponding to the appId of the application
- Set the **USING_SERVER_LICENSE** variable in [STLicenseUtils](src/main/java/com/sensetime/effects/utils/STLicenseUtils.java) to true

## 4 Configure compilation

- Set the **beauty_sensetime** variable in **gradle.properties** in the project directory to true
- compile and run


