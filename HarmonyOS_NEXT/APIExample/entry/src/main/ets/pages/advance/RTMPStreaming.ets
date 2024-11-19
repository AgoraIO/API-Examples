import {
    ChannelMediaOptions, Constants,
    LiveTranscoding,
    RtcEngine,
    RtcEngineConfig,
    TranscodingUser,
    VideoCanvas,
    VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { GlobalInfo } from '../Data';
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'RTMPStreaming'

@Entry
@Component
struct RTMPStreaming {
  private rtcEngine: RtcEngine | undefined = undefined;
  private transcodingParameters = new LiveTranscoding()
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State isPushing: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1
  @State transcoding: boolean = false
  @State pushUrl: string = 'rtmp://push.webdemo.agoraio.cn/lbhd/test'

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.remoteUid == -1) {
        this.remoteUid = uid
        let user = new TranscodingUser();
        user.uid = uid;
        user.x = 0;
        user.y = this.transcodingParameters.height / 2;
        user.width =  this.transcodingParameters.width;
        user.height =  this.transcodingParameters.height / 2;
        user.zOrder = 1;
        user.alpha = 1.0
        user.audioChannel = 2;
        this.transcodingParameters.transcodingUsers.set(uid,user)
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.remoteUid == uid) {
        this.remoteUid = -1
        this.transcodingParameters.transcodingUsers.delete(uid)
      }
    };
    config.mEventHandler.onJoinChannelSuccess = (cid: string, uid: number, elapsed: number) => {
      Logger.info(TAG, "mEventHandler.onJoinChannelSuccess: " + uid);
      this.transcodingParameters.transcodingUsers = new Map<number, TranscodingUser>();
      let localUser:TranscodingUser = {
        uid: uid,
        x: 0,
        y: 0,
        width: this.transcodingParameters.width,
        height: this.transcodingParameters.height / 2,
        zOrder: 1,
        alpha:1.0,
        audioChannel:0,
      }
      this.transcodingParameters.transcodingUsers.set(uid,localUser)
      this.isJoin = true
      this.localUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isJoin = false
      this.localUid = -1
      this.remoteUid = -1
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mEventHandler.onRtmpStreamingStateChanged =
      (url: string, state: Constants.RtmpStreamPublishState, errCode: Constants.RtmpStreamPublishReason) => {
        Logger.info(TAG,
          "mEventHandler.onRtmpStreamingStateChanged state=" + state + ", errorCode=" + errCode + ", url=" + url);
      }
    config.mEventHandler.onRtmpStreamingEvent = (url: string, event: Constants.RtmpStreamingEvent) => {
      Logger.info(TAG,
        "mEventHandler.onRtmpStreamingEvent event=" + event + ", url=" + url);
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);

    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine.stopPreview();
        this.rtcEngine.leaveChannel();
      }
      Logger.info(TAG, "destroy begin")
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done")
      });
      Logger.info(TAG, "destroy end")
      this.rtcEngine = undefined;
    }
  }

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_rtmpstreaming'),
        showBack: true
      })

      Stack({ alignContent: Alignment.TopEnd }) {
        if (this.localUid != -1) {
          XComponent({
            id: 'preview_local',
            type: XComponentType.SURFACE,
            libraryname: Constants.AGORA_LIB_NAME,
          }).onLoad(() => {
            let localCanvas = new VideoCanvas("preview_local");
            localCanvas.uid = this.localUid;
            localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
            localCanvas.mirrorMode = 0;
            this.rtcEngine?.setupLocalVideo(localCanvas);
          }).width('100%').height('100%')
        }

        if (this.remoteUid != -1) {
          XComponent({
            id: 'preview_remote',
            type: XComponentType.SURFACE,
            libraryname: Constants.AGORA_LIB_NAME,
          }).onLoad(() => {
            let localCanvas = new VideoCanvas("preview_remote");
            localCanvas.uid = this.remoteUid;
            localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
            localCanvas.mirrorMode = 0;
            this.rtcEngine?.setupRemoteVideo(localCanvas);
          }).width('30%').height('30%').margin({ top: 12, right: 12 })
        }
      }
      .layoutWeight(1)

      Row() {
        Text($r('app.string.transcoding'))
          .layoutWeight(1)

        Toggle({ type: ToggleType.Switch, isOn: this.transcoding })
          .onChange((isOn) => {
            this.transcoding = isOn
            if (this.isPushing) {
              this.rtcEngine?.stopRtmpStream(this.pushUrl)
              if (this.transcoding) {
                this.rtcEngine?.startRtmpStreamWithTranscoding(this.pushUrl, this.transcodingParameters)
              } else {
                this.rtcEngine?.startRtmpStreamWithoutTranscoding(this.pushUrl)
              }
            }
          })
          .margin({ right: 12 })
      }
      .alignItems(VerticalAlign.Center)
      .padding({
        top: 8,
        bottom: 8,
        left: 12,
        right: 12
      })

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .layoutWeight(1)
          .margin({ right: 6 })

        Button(this.isJoin ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_to_join')
          .onClick(async () => {
            focusControl.requestFocus("button_to_join")
            if (!this.isJoin) {
              if (this.channelName == "") {
                ShowToast.shortToast("The channel name is empty!")
                return
              }
              if (!(await PermissionHelper.checkPermissions(
                getContext(this) as common.UIAbilityContext,
                ['ohos.permission.MICROPHONE', 'ohos.permission.CAMERA']
              ))) {
                ShowToast.shortToast("Permission leak!")
                return
              }
              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " +
                RtcEngine.getErrorDescription(ret))
                return
              }
              this.rtcEngine?.startPreview()
            } else {
              if (this.isPushing) {
                this.isPushing = false
                this.rtcEngine?.stopRtmpStream(this.pushUrl)
              }
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            }
          })
          .margin({ right: 6 })
      }
      .alignItems(VerticalAlign.Center)
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_url'), text: this.pushUrl })
          .id("input_push_url")
          .enabled(!this.isPushing)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.pushUrl = value
          })
          .layoutWeight(1)
          .margin({ right: 6 })

        Button(this.isPushing ? $r('app.string.stop_push') : $r('app.string.push'))
          .id('button_to_push')
          .enabled(this.isJoin)
          .onClick(async () => {
            focusControl.requestFocus("button_to_push")
            if (!this.isPushing) {
              this.isPushing = true
              if (this.transcoding) {
                this.rtcEngine?.startRtmpStreamWithTranscoding(this.pushUrl, this.transcodingParameters)
              } else {
                this.rtcEngine?.startRtmpStreamWithoutTranscoding(this.pushUrl)
              }
            } else {
              this.isPushing = false
              this.rtcEngine?.stopRtmpStream(this.pushUrl)
            }
          })
          .margin({ right: 6 })
      }
      .alignItems(VerticalAlign.Center)
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}