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

        #define APP_ID _T("Your App ID")

6. (可选)你也可以在Debug/Release目录下创建一个AppId.ini文件以配置你应用程序的AppID, 修改AppId的值为刚才申请的App ID
        
        [AppID]
        AppID=xxxxxxxxxxxxxxxxxxx

### 编译项目

## 基础场景



## 进阶场景

### 多个摄像头

* 切换用户角色主播和观众
* 加入离开频道
* 如果主播加入频道，采集，渲染和发送所有可用摄像头视频。
* 主播只发送一路音频
* 主播不订阅音频和视频
* 观众不发送音频和视频，只订阅音视频
* 观众渲染远端接收的视频。


### 屏幕采集

* 切换用户角色主播和观众
* 枚举屏幕和窗口。 把屏幕和窗口的名字插入到一个commbox，选择要共享什么。
* 主播会采集，显示和发送桌面或窗口
* 观众订阅主播音视频
* 加入离开频道

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
