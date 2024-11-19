import {
  ChannelMediaOptions,
  ClientRoleOptions,
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
import { LengthMetrics } from '@kit.ArkUI';

const TAG: string = 'LiveStreaming'

@Entry
@Component
struct LiveStreaming {
  private rtcEngine: RtcEngine | undefined = undefined;
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1
  @State isLinking: boolean = false
  @State showSetting: boolean = false
  @State lowLegacyLiving: boolean = false
  @State pullLowStream: boolean = false

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
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.remoteUid == uid) {
        this.remoteUid = -1
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
      this.localUid = -1
      this.remoteUid = -1
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
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
    this.rtcEngine.setDualStreamMode(Constants.SimulcastStreamMode.ENABLE)
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

  @Builder
  buildSettingSheet() {
    Column() {
      Text($r('app.string.settings'))
        .align(Alignment.Center)
        .padding({
          left: 12,
          right: 12,
          top: 20,
          bottom: 12
        })

      Divider()

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
        Text($r('app.string.low_legacy_living'))
          .flexGrow(1)

        Toggle({ type: ToggleType.Switch, isOn: this.lowLegacyLiving })
          .onChange((isOn) => {
            this.lowLegacyLiving = isOn
            if(!this.isLinking){
              let option = new ClientRoleOptions()
              option.audienceLatencyLevel = this.lowLegacyLiving ? Constants.AudienceLatencyLevel.LOW_LATENCY : Constants.AudienceLatencyLevel.ULTRA_LOW_LATENCY
              this.rtcEngine?.setClientRole(Constants.ClientRole.AUDIENCE, option)
            }
          })
          .flexGrow(0)
      }
      .padding({
        top: 8,
        bottom: 8,
        left: 12,
        right: 12
      })

      Divider().margin({ left: 12, right: 12 })

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
        Text($r('app.string.low_stream'))
          .flexGrow(1)

        Toggle({ type: ToggleType.Switch, isOn: this.pullLowStream })
          .onChange((isOn) => {
            this.pullLowStream = isOn
            this.rtcEngine?.setRemoteDefaultVideoStreamType(this.pullLowStream ? Constants.VideoStreamType.LOW : Constants.VideoStreamType.HIGH)
          })
          .flexGrow(0)
      }
      .padding({
        top: 8,
        bottom: 8,
        left: 12,
        right: 12
      })

      Divider().margin({ left: 12, right: 12 })

    }.width('100%').height('100%')
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_livestreaming'),
        showBack: true
      }).flexGrow(0)

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

        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.End, justifyContent: FlexAlign.Start }) {
          Button(this.isLinking ? $r('app.string.stop_linking') : $r('app.string.start_linking'))
            .id('button_to_linking')
            .enabled(this.isJoin)
            .onClick(() => {
              this.isLinking = !this.isLinking
              this.rtcEngine?.updateChannelMediaOptions({
                clientRoleType: this.isLinking ? Constants.ClientRole.BROADCASTER : Constants.ClientRole.AUDIENCE,
                publishCameraTrack: this.isLinking,
                publishMicrophoneTrack: this.isLinking
              })
              focusControl.requestFocus("button_to_linking")
            })
            .flexGrow(0)

          Button($r('app.string.switch_camera'))
            .id('button_to_switch_camera')
            .enabled(this.isJoin)
            .onClick(() => {
              this.rtcEngine?.switchCamera()
            })
            .margin({ left: 12 })
        }
        .width('100%')
        .height('100%')
        .padding({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })
      }
      .flexGrow(1)

      Flex({
        direction: FlexDirection.Row,
        alignItems: ItemAlign.Center,
        justifyContent: FlexAlign.Center,
        space: { main: LengthMetrics.px(12) }
      }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .width('60%')
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
              mediaOption.publishCameraTrack = this.isLinking;
              mediaOption.publishMicrophoneTrack = this.isLinking;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType =
                this.isLinking ? Constants.ClientRole.BROADCASTER : Constants.ClientRole.AUDIENCE;

              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " +
                RtcEngine.getErrorDescription(ret))
                return
              }
              this.rtcEngine?.startPreview()
            } else {
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            }
          })
          .width('20%')
          .margin({ right: 6 })

        Button($r('app.string.settings'))
          .id('button_to_settings')
          .onClick(() => {
            this.showSetting = !this.showSetting
          })
          .width('20%')
      }
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
    .bindSheet(this.showSetting, this.buildSettingSheet(), {
      detents: [SheetSize.MEDIUM, SheetSize.LARGE, 200],
      backgroundColor: Color.White,
      blurStyle: BlurStyle.Thick,
      showClose: false,
      enableOutsideInteractive: false, //不允许交互，显示蒙层
      preferType: SheetType.CENTER,
      shouldDismiss: ((sheetDismiss: SheetDismiss) => {
        Logger.debug("bind sheet shouldDismiss")
        sheetDismiss.dismiss()
      }),
      onWillDismiss: ((sheetDismiss: SheetDismiss) => {
        Logger.debug("bind sheet onWillDismiss")
        this.showSetting = false
      }),
    })
  }
}