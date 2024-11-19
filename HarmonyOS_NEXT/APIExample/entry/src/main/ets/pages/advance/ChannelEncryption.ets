import {
    ChannelMediaOptions, Constants,
    EncryptionConfig,
    EncryptionMode, RtcEngine,
    RtcEngineConfig, VideoCanvas,
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
import { CommonItemSelect } from '../../common/Widgets';
import { util } from '@kit.ArkTS';

const TAG: string = 'ChannelEncryption'

@Entry
@Component
struct ChannelEncryption {
  private rtcEngine: RtcEngine | undefined = undefined;
  private encryptionModeOptions = [
    EncryptionMode.AES_128_XTS,
    EncryptionMode.AES_128_ECB,
    EncryptionMode.AES_256_XTS,
    EncryptionMode.SM4_128_ECB,
    EncryptionMode.AES_128_GCM,
    EncryptionMode.AES_256_GCM,
    EncryptionMode.AES_128_GCM2,
    EncryptionMode.AES_256_GCM2
  ]
  private encryptionModeSelects: ResourceStr[] = [
    'AES_128_XTS',
    'AES_128_ECB',
    'AES_256_XTS',
    'SM4_128_ECB',
    'AES_128_GCM',
    'AES_256_GCM',
    'AES_128_GCM2',
    'AES_256_GCM2',
  ]
  @State encryptionModeSelectIndex: number = 0
  @State channelName: string = ''
  @State encryptionKey: string = ''
  @State isJoin: boolean = false
  @State isPushing: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1

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
    config.mEventHandler.onEncryptionError = (errorType: Constants.EncryptionError) => {
      Logger.info(TAG, "mEventHandler.onEncryptionError: " + errorType);
      ShowToast.longToast("onError " + errorType)
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
        title: $r('app.string.item_channelencryption'),
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


      CommonItemSelect({
        name: $r("app.string.encryption_mode"),
        selects: this.encryptionModeSelects,
        selectIndex: this.encryptionModeSelectIndex,
        callback: (index: number, value?: string) => {
          this.encryptionModeSelectIndex = index
        }
      }).enabled(!this.isJoin)
        .padding({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })

      Row() {
        Text($r('app.string.encryption_key'))
        TextInput({ text: this.encryptionKey })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.encryptionKey = value
          })
          .layoutWeight(1)
          .margin({ right: 6, left: 6 })
      }.alignItems(VerticalAlign.Center)
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

              let config = new EncryptionConfig()
              config.encryptionMode = this.encryptionModeOptions[this.encryptionModeSelectIndex]
              config.encryptionKey = this.encryptionKey
              let base64 = new util.Base64Helper();
              config.encryptionKdfSalt = base64.decodeSync("NiIeJ08AbtcQVjvV+oOEvF/4Dz5dy1CIwa805C8J2w0=", util.Type.BASIC).buffer as ArrayBuffer
              this.rtcEngine?.enableEncryption(true, config)

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

    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}