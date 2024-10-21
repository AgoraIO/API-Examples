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

    ```
    #define APP_ID _T("Your App ID")
    #define APP_TOKEN _T("Your Token")
    ```
6. (Optional)Alternate approach to setup your APPID is to create an AppId.ini file under Debug/Release. Modify the appId value to the App ID you just applied.

    ```
    #[AppID]

    #AppID=xxxxxxxxxxxxxxxxxxx
    ```

### Build the application
**This open source sample project uses the Agora RTC SDK,DirectShow SDK, and MeidaPlayer SDK.**

You can directly run `APIExample/installThirdParty.bat` to automatically environment configuration.Once the configuration is complete, open the project with VS2017, select the x86 version to compile and run.

## Basic Scene


### LiveBroadcasting


* change client role
* support 1v1，1v3, 1v8, 1v15
* join/leave channel
* render local and remote video


## Advanced Scene 

### RTMP Streaming

* Add publish stream url after join channel success
* remove publish stream url before leave channel
* show information returned by rtmp streaming callback

### Inject Stream Url

* inject stream url after join channel success
* show information returned by inject status callback
* Receive 666 jonied callback after inject stream url succeed.You can mute video and audio of 666. Also,you can render it.
* remove inject stream url before leave channel

### Video Metadata(Video SEI)

* You need enable video and joinchannel.
* Send video SEI information. The maximum is 1024 byte.
* Receive SEI information and show it.
* Clear SEI information

### Share the screen

* Enter the channel and enumerate all visible Windows
* Select a visible window
* Recording screen
* Stop recording


### Audio Profile

* Set profile
* Set scenario
* Set audio property to channel audio

### Audio Mixing

* Set the audio path
* Set the number of playback times
* Sets whether to play locally only
* Sets whether to replace the microphone audio

### Camera Capture

* Camera capture using DirectShow
* Enumerates all image acquisition devices and types
* Create image acquisition filters
* Start collecting camera data
* SDK acquires camera data
* Stop collecting camera data

### Process Raw Video Data

* Sign up as a video observer
* Process video frames in onCaptureVideoFrame

### Audio Capture

* Audio acquisition using DirectShow
* Enumerates all audio acquisition devices and types
* Create audio capture filters
* Start collecting microphone data
* SDK gets microphone data
* Stop collecting microphone data

### Process Raw Audio Data

* Register Audio Observer
* Process Audio Frames in onRecordAudioFrame

### Custom Encrypt

* Register Packet Observer
* Encrypt the audio stream before sending it in onSendAudioPacket
* Encrypt the video stream before it is sent in the onSendVideoPacket
* Decrypt the audio stream after receiving it in onReceiveAudioPacket
* Decrypt a video stream after receiving it from an onReceiveVideoPacket


### Transparent Background

* Join the channel, send video streams, and receive video streams.
* Configure the canvas property `enableAlphaMask = true` during rendering.

### Ultra-Low Latency Live Streaming URL Pull

* Initialize Rte, Player, and register relevant callbacks.
* After successful initialization, you can call media-related methods, such as play, pause, etc.
* Handle exceptions in the player callback methods.

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
