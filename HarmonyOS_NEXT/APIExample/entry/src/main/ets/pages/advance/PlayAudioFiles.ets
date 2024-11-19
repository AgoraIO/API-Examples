import { ChannelMediaOptions, Constants, RtcEngine, RtcEngineConfig } from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { TokenUtils } from '../../util/TokenUtils';
import { GlobalInfo } from '../Data';

const TAG: string = 'PlayAudioFiles'

@Entry
@Component
struct PlayAudioFiles {
  private rtcEngine: RtcEngine | undefined = undefined;
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
    Column() {
      TitleBar({
        title: $r('app.string.item_playaudiofiles'),
        showBack: true
      })

      Column() {
        Row() {
          if (this.localUid != 0) {
            Column({ space: 12 }) {
              Text("Audio Only")
              Text("Local")
              Text(this.localUid + '')
            }
            .alignItems(HorizontalAlign.Center)
            .justifyContent(FlexAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('34%')
            .height('100%')
          }

          if (this.remoteUidList.length > 0) {
            Column({ space: 12 }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[0] + '')
            }
            .alignItems(HorizontalAlign.Center)
            .justifyContent(FlexAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }

          if (this.remoteUidList.length > 1) {
            Column({ space: 12 }) {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[1] + '')
            }
            .alignItems(HorizontalAlign.Center)
            .justifyContent(FlexAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("100%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .layoutWeight(1)

      Column() {
        Text($r('app.string.audio_mixing')).padding({ top: 6, bottom: 6 })
        Divider()
        Row() {
          Button($r('app.string.start'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .onClick(() => {
              this.rtcEngine?.startAudioMixing("https://webdemo.agora.io/audiomixing.mp3", false, -1, 0)
            })
          Button($r('app.string.resume'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .onClick(() => {
              this.rtcEngine?.resumeAudioMixing()
            })
          Button($r('app.string.pause'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .onClick(() => {
              this.rtcEngine?.pauseAudioMixing()
            })
          Button($r('app.string.stop'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .onClick(() => {
              this.rtcEngine?.stopAudioMixing()
            })
        }.alignItems(VerticalAlign.Center).padding({ top: 12, bottom: 6 })

        Row() {
          Text($r('app.string.volume')).layoutWeight(1)
          Slider({ value: 100 }).layoutWeight(1.5).onChange((value) => {
            this.rtcEngine?.adjustAudioMixingVolume(value)
          })
        }.alignItems(VerticalAlign.Center)

        Row() {
          Text($r('app.string.play_volume')).layoutWeight(1)
          Slider({ value: 100 }).layoutWeight(1.5).onChange((value) => {
            this.rtcEngine?.adjustAudioMixingPlayoutVolume(value)
          })
        }.alignItems(VerticalAlign.Center)

        Row() {
          Text($r('app.string.publish_volume')).layoutWeight(1)
          Slider({ value: 100 }).layoutWeight(1.5).onChange((value) => {
            this.rtcEngine?.adjustAudioMixingPublishVolume(value)
          })
        }.alignItems(VerticalAlign.Center)

        //todo
        if (false) {
          Text($r('app.string.audio_effect')).padding({ top: 12, bottom: 6 })
          Divider()
          Row() {
            Button($r('app.string.start'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .onClick(() => {
                this.rtcEngine?.playEffect(0, "https://webdemo.agora.io/ding.mp3", -1, 1, 0.0, 100, true, 0)
              })
            Button($r('app.string.resume'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .onClick(() => {

              })
            Button($r('app.string.pause'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .onClick(() => {

              })
            Button($r('app.string.stop'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .onClick(() => {

              })
          }.alignItems(VerticalAlign.Center).padding({ top: 12, bottom: 6 })
          Row() {
            Text($r('app.string.volume')).layoutWeight(1)
            Slider({ value: 100 }).layoutWeight(1.5).onChange((value) => {

            })
          }.alignItems(VerticalAlign.Center)
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(3)
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
            if (this.isJoin) {
              this.rtcEngine?.leaveChannel()
            } else {
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

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = false;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;
              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }
            }

          })
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