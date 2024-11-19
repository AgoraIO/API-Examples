import {
  ChannelMediaOptions,
  ChannelMediaRelayConfiguration,
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
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'JoinVideoChannel'

@Entry
@Component
struct HostCrossChannel {
  private rtcEngine: RtcEngine | undefined = undefined;
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State relayChannelName: string = ''
  @State isRelayStart: boolean = false
  @State isRelayPause: boolean = false

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.remoteUid == 0) {
        this.remoteUid = uid
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.remoteUid == uid) {
        this.remoteUid = 0
      }
    };
    config.mEventHandler.onJoinChannelSuccess = (cid: string, uid: number, elapsed: number) => {
      Logger.info(TAG, "mEventHandler.onJoinChannelSuccess: " + uid);
      this.isJoin = true
      this.localUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isJoin = false
      this.localUid = 0
      this.remoteUid = 0
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mEventHandler.onChannelMediaRelayStateChanged = (state: number, code: number) => {
      Logger.info(TAG, "mEventHandler.onChannelMediaRelayStateChanged: state=" + state + " code=" + code);
      if (code == 0) {
        if (state == 2) {
          // RELAY_STATE_RUNNING
          this.isRelayStart = true
        } else {
          this.isRelayStart = false
        }
      } else {
        ShowToast.longToast("onChannelMediaRelayStateChanged error >> " + code + " : " + code)
      }
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
        title: $r('app.string.item_hostcrosschannel'),
        showBack: true
      })

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
      .layoutWeight(1.0)


      Row() {
        TextInput({ placeholder: $r('app.string.text_input_target_channel_name') })
          .id("input_target_channel_name")
          .enabled(!this.isRelayStart)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.relayChannelName = value
          })
          .layoutWeight(1.0)
          .margin({ right: 6 })

        // Button(this.isRelayPause ? $r('app.string.resume') : $r('app.string.pause'))
        //   .id('button_to_pause_relay')
        //   .enabled(this.isRelayJoin)
        //   .onClick(async () => {
        //     focusControl.requestFocus("button_to_pause_relay")
        //     if (this.isRelayPause) {
        //       this.rtcEngine?.resumeAllChannelMediaRelay()
        //     } else {
        //       this.rtcEngine?.pauseAllChannelMediaRelay()
        //     }
        //   })

        Button(this.isRelayStart ? $r('app.string.stop') : $r('app.string.start'))
          .id('button_to_join_relay')
          .enabled(this.isJoin)
          .onClick(async () => {
            focusControl.requestFocus("button_to_join_relay")
            if (this.isRelayStart) {
              this.rtcEngine?.stopChannelMediaRelay()
            } else {
              if (this.relayChannelName == "") {
                ShowToast.shortToast("The target channel name is empty!")
                return
              }
              let config = new ChannelMediaRelayConfiguration()
              config.srcInfo.channelName = this.channelName
              config.srcInfo.uid = this.localUid
              config.srcInfo.token = ""
              config.destInfos.set(this.relayChannelName, {
                channelName: this.relayChannelName,
                uid: 999,
                token: ""
              })
              this.rtcEngine?.startOrUpdateChannelMediaRelay(config)
            }
          })
      }
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .layoutWeight(1.0)
          .margin({ right: 6 })

        Button(this.isJoin ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_to_join')
          .onClick(async () => {

            focusControl.requestFocus("button_to_join")

            if (this.isJoin) {
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            } else {
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
            }
          })
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