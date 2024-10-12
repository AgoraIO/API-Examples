# API Example Windows

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK，展示了常用场景的API示例

本开源项目使用 **C++** 语言

## 环境主备
* vs 2013(或更高版本)，默认支持vs2017。
* Windows 7(或更高版本)。

**注意** 使用其他版本需要自行修改该配置，编译release还需要安装兼容xp相关插件。


## 运行示例程序

这个段落主要讲解了如何编译和运行实例程序。

### 创建Agora账号并获取AppId

在编译和启动实例程序前，您需要首先获取一个可用的App ID:
1. 在[agora.io](https://dashboard.agora.io/signin/)创建一个开发者账号
2. 前往后台页面，点击左部导航栏的 **项目 > 项目列表** 菜单
3. 复制后台的 **App ID** 并备注，稍后启动应用时会用到它
4. 在项目页面生成临时 **Access Token** (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。

5. 将 AppID 内容替换至 APP_ID 宏定义中

    ```
    #define APP_ID _T("Your App ID")
    #define APP_TOKEN _T("Your Token")
    ```
6. (可选)你也可以在Debug/Release目录下创建一个AppId.ini文件以配置你应用程序的AppID, 修改AppId的值为刚才申请的App ID
    ```
    [AppID]
    AppID=xxxxxxxxxxxxxxxxxxx
    ```

### 编译项目
**这个开源示例项目使用了Agora RTC SDK,DirectShow SDK,MeidaPlayer SDK。**

你可以通过直接运行`APIExample/installThirdParty.bat`来自动进行依赖下载与环境配置。配置完成后使用vs2017打开项目，选择x86版本进行编译就可以运行了。 

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
* 清除SEI信息


### 共享屏幕

* 进入频道，枚举所有可见窗口
* 选择一个可见窗口
* 录制屏幕
* 停止录制

### 美颜

* 设置明暗对比等级
* 设置明亮度
* 设置红润度
* 设置平滑度

### 美声

* 设置音效或者美声

### 音频设置

* 设置音频参数
* 设置场景
* 设置频道内的音频设置

### 音频混合

* 设置音频路径
* 设置播放次数
* 设置是否仅仅本地播放
* 设置是否替换麦克风音频

### 自定义摄像头采集

* 摄像头采集使用DirectShow
* 枚举所有图像采集设备和类型
* 创建图像采集过滤器
* 开始采集摄像头数据
* SDK获取摄像头数据
* 停止采集摄像头数据

### 处理视频原始数据

* 注册视频观察者
* 实现了对原始图像进行灰度处理，和模糊处理
* 在onCaptureVideoFrame中对视频帧进行处理


### 自定义音频采集

* 音频采集使用DirectShow
* 枚举所有音频采集设备和类型
* 创建音频采集过滤器
* 开始采集麦克风数据
* SDK获取麦克风数据
* 停止采集麦克风数据


### 处理音频原始数据

* 注册音频观察者
* 在onRecordAudioFrame中对音频帧进行处理


### 自定义媒体加密

* 注册数据包观察者
* 在onSendAudioPacket中对音频流发送前进行加密
* 在onSendVideoPacket中对视频流发送前进行加密
* 在onReceiveAudioPacket中对音频流接收后进行解密
* 在onReceiveVideoPacket中对视频流接收后进行解密

### 媒体播放器组件

* 使用MeidaPlayer Kit 进行媒体的打开，播放等操作。
* 使用MeidaPlayerExtensions 向AgoraRtc Engine的频道推流。
* 使用IMediaPlayerObserver来处理MeidaPlayer的回调事件。例如（打开，播放）

### 透明背景

* 加入频道，发送视频流,接受视频流；
* 渲染的时候配置canvas属性enableAlphaMask = true；

### 极速直播URL拉流

* 初始化Rte,Player,并注册相关回调；
* 初始化成功后,可以调用媒体相关的方法,比如play,pause等；
* 在播放器回调方法里面处理异常情况；

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
