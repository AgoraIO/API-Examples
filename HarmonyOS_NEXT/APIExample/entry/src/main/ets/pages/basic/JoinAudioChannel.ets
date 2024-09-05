import { ChannelMediaOptions, Constants, RtcEngine,RtcEngineEx, RtcEngineConfig, AudioVolumeInfo } from 'AgoraRtcSdk';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { TokenUtils } from '../../util/TokenUtils';
import {
  AudioChannelSlider,
  AudioChannelSpinner,
  AudioChannelToggle,
  OnCheckListener,
  OnSelectListener,
  OnSliderListener
} from './AudioConfigComponent';
import {
  AudioConfigHelper,
  AUDIO_PROFILE,
  AUDIO_ROUTE,
  AUDIO_SCENARIO,
  CHANNEL_PROFILE
} from '../../util/AudioConfigContant';
import { faceDetector } from '@kit.CoreVisionKit';
import { JSON } from '@kit.ArkTS';

const TAG: string = 'JoinAudioChannel'

@Entry
@Component
struct JoinAudioChannel {
  private rtcEngine: RtcEngine | undefined = undefined;
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []
  @State isMicophotoOpen: boolean = true
  @State isEarMonitorChecked: boolean = false
  @State routeSelectIndex: number = 0
  private channelProfileType = Constants.ChannelProfile.COMMUNICATION
  private audioScenarioType = Constants.AudioScenarioType.DEFAULT
  private audioProfileType = Constants.AudioProfileType.DEFAULT
  handleMicospeakerClick = () => {
    if (this.isMicophotoOpen != null) {
      this.isMicophotoOpen = !this.isMicophotoOpen
    }
  }
  onAudioRouteSelect: OnSelectListener = (index: number, text: string) => {
    console.info(`onAudioRouteSelect index ${index}  text ${text}`)
  }
  onInEarMonitorCheck: OnCheckListener = (isCheck: boolean) => {
    this.isEarMonitorChecked = isCheck
  }
  onInEarMonitorVolSlide: OnSliderListener = (progress: number) => {
    console.info(`onInEarMonitorVolSlide progress:${progress}`)
    if (progress == 0) {
      this.rtcEngine?.enableInEarMonitoring(false, 1)
    } else {
      this.rtcEngine?.enableInEarMonitoring(true, 1);
      this.rtcEngine?.setInEarMonitoringVolume(progress);
    }
  }
  onPlayoutVolSlide: OnSliderListener = (progress: number) => {
    console.info(`onPlayoutVolSlide progress:${progress}`)
    this.rtcEngine?.adjustPlaybackSignalVolume(progress);
  }
  onRecordingVolSlide: OnSliderListener = (progress: number) => {
    console.info(`onRecordingVolSlide progress:${progress}`)
    this.rtcEngine?.adjustRecordingSignalVolume(progress);
  }
  onScenarioSelect: OnSelectListener = (index: number, text: string) => {
    console.info(`onScenarioSelect index ${index}  text ${text}`)
    this.audioScenarioType = AudioConfigHelper.getAudioScenarioType(text)
    this.rtcEngine?.setAudioScenario(this.audioScenarioType)
  }
  onAudioProfileSelect: OnSelectListener = (index: number, text: string) => {
    console.info(`onAudioProfileSelect index ${index}  text ${text}}`)
    if (!this.isJoin) {
      return
    }
    this.audioProfileType = AudioConfigHelper.getAudioProfileType(text)
    this.rtcEngine?.setAudioProfile(this.audioProfileType)
    const isCommunication = AudioConfigHelper.CHANNEL_PROFILE_COMMUNICATION == text;
    //todo
    // this.rtcEngine?.setEnableSpeakerphone()
    // this.rtcEngine?.setRouteInCommunicationMode()
  }
  onChannelProfileSelect: OnSelectListener = (index: number, text: string) => {
    console.info(`onChannelProfileSelect index ${index}  text ${text}}`)
    this.channelProfileType = AudioConfigHelper.getChannelProfileType(text)
  }

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.mEventHandler = {};
    config.mEventHandler.onAudioVolumeIndication = (speakers: Array<AudioVolumeInfo>, totalVolume: number) => {
      Logger.info(`totalVolume:${totalVolume}    speakers:${JSON.stringify(speakers)}`)
    }
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
      this.isMicophotoOpen = true
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isJoin = false
      this.localUid = 0
      this.isMicophotoOpen = true
      this.remoteUidList = []
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config) as RtcEngineEx

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
        title: $r('app.string.item_joinaudiochannel'),
        showBack: true
      })

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
            Column() {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[0] + '')
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }

          if (this.remoteUidList.length > 1) {
            Column() {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[1] + '')
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("15%")
        .width('100%')

        Row() {
          if (this.remoteUidList.length > 2) {
            Column() {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[2] + '')
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('34%')
            .height('100%')
          }

          if (this.remoteUidList.length > 3) {
            Column() {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[3] + '')
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }

          if (this.remoteUidList.length > 4) {
            Column() {
              Text("Audio Only")
              Text("Remote")
              Text(this.remoteUidList[4] + '')
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .width('33%')
            .height('100%')
          }
        }
        .align(Alignment.TopStart)
        .height("15%")
        .width('100%')
      }
      .align(Alignment.TopStart)

      Column({ space: 5 }) {
        Row() {
          Button('更改状态').onClick(() => {
            if (this.localUid <= 0) {
              this.localUid = 1
              this.isJoin = true
            } else {
              this.localUid = 0
              this.isJoin = false
            }
          })

          Button((this.isJoin && !this.isMicophotoOpen) ? $r('app.string.open_microphone') :
          $r('app.string.close_microphone'))
            .onClick(this.handleMicospeakerClick)
            .enabled(this.isJoin)

        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)

        AudioChannelSpinner({
          name: $r('app.string.audio_router'),
          items: AUDIO_ROUTE,
          selectIndex: this.routeSelectIndex,
          onSelect: this.onAudioRouteSelect
        })
        AudioChannelToggle({
          isEnable: this.isJoin,
          name: $r('app.string.inear_monitor'),
          isCheck: false,
          onCheck: this.onInEarMonitorCheck
        })
        AudioChannelSlider({
          isEnable: this.isJoin && this.isEarMonitorChecked,
          name: $r('app.string.inear_monitor_vol'),
          onSlide: this.onInEarMonitorVolSlide
        })
        AudioChannelSlider({
          isEnable: this.isJoin,
          name: $r('app.string.playout_vol'),
          onSlide: this.onPlayoutVolSlide
        })
        AudioChannelSlider({
          isEnable: this.isJoin,
          name: $r('app.string.recording_vol'),
          onSlide: this.onRecordingVolSlide
        })
        AudioChannelSpinner({
          name: $r('app.string.scenario'),
          items: AUDIO_SCENARIO,
          onSelect: this.onScenarioSelect
        })
        AudioChannelSpinner({
          // isEnable: !this.isJoin,
          name: $r('app.string.audio_profile'),
          items: AUDIO_PROFILE,
          onSelect: this.onAudioProfileSelect
        })
        AudioChannelSpinner({
          isEnable: !this.isJoin,
          name: $r('app.string.channel_profile'),
          items: CHANNEL_PROFILE,
          onSelect: this.onChannelProfileSelect
        })
      }
      .layoutWeight(1)
      .margin({ top: 10, bottom: 10 })
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.End)
      .width('100%')

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .margin({ right: 6 })
          .layoutWeight(1)

        if (this.isJoin) {
          Button($r('app.string.leave'))
            .id('button_to_leave')
            .onClick(() => {
              this.routeSelectIndex = 0
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
              focusControl.requestFocus("button_to_leave")
            })
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


              this.rtcEngine?.setAudioProfile(this.audioProfileType)
              this.rtcEngine?.setAudioScenario(this.audioScenarioType)

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = false;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = this.channelProfileType;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;
              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }
            })
        }
      }
    }
    .alignItems(HorizontalAlign.Start)
    .width('100%')
    .height('100%')
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}