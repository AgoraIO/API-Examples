# API Example Windows

*其他语言版本： [简体中文](README.zh.md)*

The API Example Windows Sample App is an open-source demo that show common API usage.

This demo is written in **C++**

## Developer Environment Requirements
* VS 2013(or higher), default is vs2017
* Windows 7(or higher)

### Obtain an App ID

To build and run the sample application, get an App ID:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App ID** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.
5. Define the APP_ID with your App ID.

        #define APP_ID _T("Your App ID")

6. (Optional)Alternate approach to setup your APPID is to create an AppId.ini file under Debug/Release. Modify the appId value to the App ID you just applied.
    
        [AppID]

        AppID=xxxxxxxxxxxxxxxxxxx
    
### Build the application


## Basic Scene


## Advanced Scene 

### Multiple Cameras

* Set user role, broadcaster or Audience.
* Joinchanel and LeaveChannel
* If broadcaster joinChannel, capture multiple cameas, render cameras video and publish cameras video.
* Broadcaster only publish one audio track.
* Broadcaster doesn't subscribe audio and video.
* Audience doesn't publish audio and video, just subscribe audio and video.
* Audience render remote videos.

**Note**

If you want broadcaster to subscribe or want audience to publish, you need to modify source code.The audience and can reference to each other. Broadcaster publish audio and video, audience subscribe.

### Screen Capture

* Set user role, broadcaster or Audience.
* Enumerate windows and screens. Insert window titles and screens into a commbox.Select what to share.
* Broadcaster capture, share and publish screen or window. 
* Audience subscribe video frome broadcaster.
* Joinchannel and LeaveChannel.

## Connect Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/cn/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Basic-Video-Broadcasting/issues)

## License

The MIT License (MIT).
