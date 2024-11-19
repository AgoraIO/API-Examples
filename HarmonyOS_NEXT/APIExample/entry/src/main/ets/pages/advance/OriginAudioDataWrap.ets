import {
    ChannelMediaOptions,
    Constants,
    RtcEngine,
    RtcEngineConfig
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { OriginAudioData } from 'librawdataprocessing.so';
import { GlobalInfo } from '../Data';

const TAG: string = 'OriginAudioData'

@Entry
@Component
struct OriginAudioDataWrap {
  private rtcEngine: RtcEngine | undefined = undefined;
  private native: OriginAudioData | undefined = undefined;
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      this.remoteUidList.push(uid)
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      let index = this.remoteUidList.indexOf(uid)
      if (index >= 0) {
        this.remoteUidList = this.remoteUidList.filter((value) => {
          value != uid
        })
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
      this.remoteUidList = []
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config)

    this.rtcEngine.enableAudio()
    this.native = new OriginAudioData(this.rtcEngine.getNativeHandle());
    this.native.enable(true);
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
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
        title: $r('app.string.item_originaudiosource'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Local")
              Text(this.localUid + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('34%')
            .height('100%')
          }

          if (this.remoteUidList.length > 0) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[0] + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }

          if (this.remoteUidList.length > 1) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[1] + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("30%")
        .width('100%')

        Row() {
          if (this.remoteUidList.length > 2) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[2] + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('34%')
            .height('100%')
          }

          if (this.remoteUidList.length > 3) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[3] + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }

          if (this.remoteUidList.length > 4) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[4] + '')
            }
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("30%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
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
                ['ohos.permission.MICROPHONE']
              ))) {
                ShowToast.shortToast("Permission leak!")
                return
              }

              focusControl.requestFocus("button_to_join")

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = false;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let ret = this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }
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