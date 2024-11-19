import {
    ChannelMediaOptions,
    Constants,
    IRtcEngineEventHandler,
    RtcConnection,
    RtcEngine,
    RtcEngineConfig,
    RtcEngineEx,
    VideoCanvas,
    VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import { common } from '@kit.AbilityKit';
import { AppID } from '../../common/KeyCenter';
import ShowToast from '../../util/ShowToast';
import PermissionHelper from '../../util/PermissionHelper';
import { GlobalInfo } from '../Data';
import { TokenUtils } from '../../util/TokenUtils';

const TAG = 'JoinMultiChannel'

@Entry
@Component
struct JoinMultiChannel {
  private rtcEngine: RtcEngineEx | undefined = undefined;
  private exRtcConnection: RtcConnection = new RtcConnection("", 0)
  @State mainChannelName: string = ''
  @State exChannelName: string = ''
  @State isMainJoined: boolean = false
  @State isExJoined: boolean = false
  @State exRemoteUid: number = 0
  @State mainRemoteUid: number = 0
  @State mainLocalUid: number = 0
  @State exLocalUid: number = 0

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.mainRemoteUid == 0) {
        this.mainRemoteUid = uid
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.mainRemoteUid == uid) {
        this.mainRemoteUid = 0
      }
    };
    config.mEventHandler.onJoinChannelSuccess = (cid: string, uid: number, elapsed: number) => {
      Logger.info(TAG, "mEventHandler.onJoinChannelSuccess: " + uid);
      this.isMainJoined = true
      this.mainLocalUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isMainJoined = false
      this.mainLocalUid = 0
      this.mainRemoteUid = 0
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config) as RtcEngineEx;
    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isMainJoined) {
        if (this.isExJoined) {
          this.leaveExChannel()
        }
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
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_joinmultichannel'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.mainLocalUid != 0) {
            XComponent({
              id: 'preview_local',
              type: XComponentType.SURFACE,
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {
              let localCanvas = new VideoCanvas("preview_local");
              localCanvas.uid = this.mainLocalUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = 0;
              this.rtcEngine?.setupLocalVideo(localCanvas);
            }).width('50%')
          }

          if (this.mainRemoteUid != 0) {
            XComponent({
              id: 'preview_remote',
              type: XComponentType.SURFACE,
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {
              let localCanvas = new VideoCanvas("preview_remote");
              localCanvas.uid = this.mainRemoteUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = 0;
              this.rtcEngine?.setupRemoteVideo(localCanvas);
            }).width('50%')
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')

        Row() {
          if (this.exRemoteUid != 0) {
            XComponent({
              id: 'preview_remote_ex',
              type: XComponentType.SURFACE,
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {

              let localCanvas = new VideoCanvas("preview_remote_ex");
              localCanvas.uid = this.exRemoteUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = 0;
              this.rtcEngine?.setupRemoteVideoEx(localCanvas, this.exRtcConnection);
            }).width('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        Text($r('app.string.ex_channel_name')).flexGrow(0)
        Text(": ").flexGrow(0)
        Text(this.exChannelName).flexGrow(1).margin({ right: 6 })
        if (this.isExJoined) {
          Button($r('app.string.leave'))
            .enabled(this.isMainJoined)
            .onClick(() => {
              this.leaveExChannel();
            })
            .flexGrow(0)
        } else {
          Button($r('app.string.join'))
            .enabled(this.isMainJoined)
            .onClick(() => {
              this.joinExChannel()
            })
            .flexGrow(0)
        }
      }
      .flexGrow(0)
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .enabled(!this.isMainJoined)
          .onChange((value) => {
            this.mainChannelName = value
            this.exChannelName = value + '-2'
          })
          .enableKeyboardOnFocus(false)
          .flexGrow(1)
          .margin({ right: 6 })

        if (this.isMainJoined) {
          Button($r('app.string.leave'))
            .onClick(() => {
              if (this.isExJoined) {
                this.leaveExChannel()
              }
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            })
            .flexGrow(0)
        } else {
          Button($r('app.string.join'))
            .onClick(async () => {
              if (this.mainChannelName == "") {
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
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = false;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;
              let token = await TokenUtils.genRtcToken(this.mainChannelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.mainChannelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }

              this.rtcEngine?.startPreview()
              this.joinExChannel()
              focusControl.requestFocus("button_to_join")
            })
            .flexGrow(0)
        }
      }
      .height(60)
      .flexGrow(0)
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }

  private leaveExChannel() {
    let ret = this.rtcEngine?.leaveChannelEx(this.exRtcConnection);
    if (ret == 0) {
      this.isExJoined = false;
      this.exLocalUid = 0;
      this.exRemoteUid = 0;
    } else {
      ShowToast.shortToast("LeaveChannelEx error: " + ret);
    }
  }

  private async joinExChannel() {
    this.exLocalUid = Math.round(Math.random() * 1000) + 10000;
    this.exRtcConnection.localUid = this.exLocalUid
    this.exRtcConnection.channelId = this.exChannelName
    let exEventHandler: IRtcEngineEventHandler = {};
    exEventHandler.onJoinChannelSuccess = () => {
      this.isExJoined = true;
    };
    exEventHandler.onLeaveChannel = () => {
      this.isExJoined = false;
      this.exLocalUid = 0;
      this.exRemoteUid = 0;
    };
    exEventHandler.onUserJoined = (uid: number) => {
      if (this.exRemoteUid == 0) {
        this.exRemoteUid = uid;
      }
    };
    exEventHandler.onUserOffline = (uid: number) => {
      if (this.exRemoteUid == uid) {
        this.exRemoteUid = 0;
      }
    };
    let exMediaOption: ChannelMediaOptions = new ChannelMediaOptions();
    exMediaOption.publishCameraTrack = true;
    exMediaOption.publishMicrophoneTrack = true;
    exMediaOption.autoSubscribeVideo = true;
    exMediaOption.autoSubscribeAudio = true;
    exMediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
    exMediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;
    let token = await TokenUtils.genRtcToken(this.exRtcConnection.channelId, this.exRtcConnection.localUid.toString())
    let ret = this.rtcEngine?.joinChannelEx(token, this.exRtcConnection, exMediaOption, exEventHandler);
    if (ret != Constants.ErrorCode.ERR_OK) {
      ShowToast.longToast("joinChannelEx error " + ret + " : " + RtcEngine.getErrorDescription(ret))
      return
    }
  }
}