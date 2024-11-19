import {
  ChannelMediaOptions,
  Constants,
  RtcEngine,
  RtcEngineConfig,
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

const TAG: string = 'JoinVideoChannelToken'

@Entry
@Component
struct JoinVideoChannelToken {
  private rtcEngine: RtcEngine | undefined = undefined;
  private appId: string = AppID;
  private token: string = ''
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
  }

  private initRtcEngine(appId: string) {
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = appId;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onRequestToken = () => {
      Logger.info(TAG, "onRequestToken ...");
    }
    config.mEventHandler.onTokenPrivilegeWillExpire = async (oldToken: string) => {
      Logger.info(TAG, `onTokenPrivilegeWillExpire oldToken = ${oldToken}`);
      //请求服务端重新生产token
      // this.token = getTokenFromServer()
      // this.rtcEngine?.renewToken(this.token)
    }
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.remoteUid == 0) {
        this.remoteUid = uid;
      }
    };
    config.mEventHandler.onConnectionStateChanged =
      (state: Constants.ConnectionState, reason: Constants.ConnectionChangedReason) => {
        Logger.info(TAG, `onConnectionStateChanged state=${state}    reason=${reason}`);
      }
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason);
      if (this.remoteUid == uid) {
        this.remoteUid = 0;
      }
    };
    config.mEventHandler.onJoinChannelSuccess = (cid: string, uid: number, elapsed: number) => {
      Logger.info(TAG, "mEventHandler.onJoinChannelSuccess: " + uid);
      this.isJoin = true;
      this.localUid = uid;
    };
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isJoin = false;
      this.localUid = 0;
      this.remoteUid = 0;
    };
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message);
    };
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);

    let encoderConfig = new VideoEncoderConfiguration();
    encoderConfig.dimensions = GlobalInfo.settings.dimensions;
    encoderConfig.frameRate = GlobalInfo.settings.frameRate;
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode;
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig);
    this.rtcEngine.enableVideo();
  }

  aboutToDisappear(): void {
    this.destroyRtcEngine();
  }

  private destroyRtcEngine() {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine.stopPreview();
        this.rtcEngine.leaveChannel();
      }
      Logger.info(TAG, "destroy begin");
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done");
      });
      Logger.info(TAG, "destroy end");
      this.rtcEngine = undefined;
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_joinvideochanneltoken'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
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
            }).width('50%')
          }

          if (this.remoteUid != 0) {
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
            }).width('50%')
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      TextInput({ placeholder: $r('app.string.app_id'), text: this.appId })
        .id("input_app_id")
        .enabled(!this.isJoin)
        .maxLines(1)
        .enableKeyboardOnFocus(false)
        .onChange((value: string) => {
          this.appId = value
        })
        .flexGrow(0)
        .margin({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })

      TextInput({ placeholder: $r('app.string.token'), text: this.token })
        .id("input_token")
        .enabled(!this.isJoin)
        .maxLines(1)
        .enableKeyboardOnFocus(false)
        .onChange((value: string) => {
          this.token = value
        })
        .flexGrow(0)
        .margin({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name'), text: this.channelName })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .maxLines(1)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .flexGrow(1)
          .margin({ right: 6 })

        if (this.isJoin) {
          Button($r('app.string.leave'))
            .id('button_to_leave')
            .onClick(() => {
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
              this.destroyRtcEngine();
              focusControl.requestFocus("button_to_leave")
            })
            .flexGrow(0)
        } else {
          Button($r('app.string.join'))
            .id('button_to_join')
            .onClick(async () => {
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

              this.initRtcEngine(this.appId)
              focusControl.requestFocus("button_to_join")

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let ret = this.rtcEngine?.joinChannelWithOptions(this.token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }
              this.rtcEngine?.startPreview()
            })
            .flexGrow(0)
        }
      }
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