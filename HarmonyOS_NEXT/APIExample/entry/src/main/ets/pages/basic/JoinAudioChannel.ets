import { ChannelMediaOptions, Constants, RtcEngine, RtcEngineEx, RtcEngineConfig, AudioVolumeInfo } from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common, WantAgent, wantAgent } from '@kit.AbilityKit';
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
  AUDIO_SCENARIO,
  CHANNEL_PROFILE
} from '../../util/AudioConfigContant';
import { JSON } from '@kit.ArkTS';
import { backgroundTaskManager } from '@kit.BackgroundTasksKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { AVCastPicker } from '@kit.AVSessionKit';
import { audio } from '@kit.AudioKit';
import { GlobalInfo } from '../Data';

const TAG: string = 'JoinAudioChannel'

/**
 * 鸿蒙系统不支持音频设备api切换 请参看网址 https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/using-switch-call-devices-V5
 * 本场景是通过AVCastPicker选择实现的
 */
@Entry
@Component
struct JoinAudioChannel {
  private context = getContext(this) as common.UIAbilityContext;
  private rtcEngine: RtcEngine | undefined = undefined;
  private audioRoutingManager = audio.getAudioManager().getRoutingManager();
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []
  @State isMicophotoOpen: boolean = false
  @State isEarMonitorChecked: boolean = false
  private channelProfileType = Constants.ChannelProfile.COMMUNICATION
  private audioScenarioType = Constants.AudioScenarioType.DEFAULT
  private audioProfileType = Constants.AudioProfileType.DEFAULT
  private routerChangeListener = (desc: audio.AudioDeviceDescriptors) => {
    //https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-audio-V5#devicetype
    Logger.info(TAG, `路由切换  设备类型 : ${desc[0].deviceType}`);
  }
  @Builder
  customPickerBuilder(): void {
    Image($r('app.media.ic_connect'))
      .width('100%')
      .height('100%')
      // .fillColor(Color.Blue)
  }
  handleMicospeakerClick = () => {
    this.isMicophotoOpen = !this.isMicophotoOpen
    this.rtcEngine?.muteLocalAudioStream(!this.isMicophotoOpen);
  }
  onInEarMonitorCheck: OnCheckListener = (isCheck: boolean) => {
    this.isEarMonitorChecked = isCheck
  }
  onInEarMonitorVolSlide: OnSliderListener = (progress: number) => {
    Logger.info(TAG, `onInEarMonitorVolSlide progress:${progress}`)
    if (progress == 0) {
      this.rtcEngine?.enableInEarMonitoring(false, 1)
    } else {
      this.rtcEngine?.enableInEarMonitoring(true, 1);
      this.rtcEngine?.setInEarMonitoringVolume(progress);
    }
  }
  onPlayoutVolSlide: OnSliderListener = (progress: number) => {
    Logger.info(TAG, `onPlayoutVolSlide progress:${progress}`)
    this.rtcEngine?.adjustPlaybackSignalVolume(progress);
  }
  onRecordingVolSlide: OnSliderListener = (progress: number) => {
    Logger.info(TAG, `onRecordingVolSlide progress:${progress}`)
    this.rtcEngine?.adjustRecordingSignalVolume(progress);
  }
  onScenarioSelect: OnSelectListener = (index: number, text: string) => {
    Logger.info(TAG, `onScenarioSelect index ${index}  text ${text}`)
    this.audioScenarioType = AudioConfigHelper.getAudioScenarioType(text)
    this.rtcEngine?.setAudioScenario(this.audioScenarioType)
  }
  onAudioProfileSelect: OnSelectListener = (index: number, text: string) => {
    Logger.info(TAG, `onAudioProfileSelect index ${index}  text ${text}}`)
    if (!this.isJoin) {
      return
    }
    this.audioProfileType = AudioConfigHelper.getAudioProfileType(text)
    this.rtcEngine?.setAudioProfile(this.audioProfileType)
  }
  onChannelProfileSelect: OnSelectListener = (index: number, text: string) => {
    Logger.info(TAG, `onChannelProfileSelect index ${index}  text ${text}}`)
    this.channelProfileType = AudioConfigHelper.getChannelProfileType(text)
  }

  startServiceTask() {
    let wantAgentInfo: wantAgent.WantAgentInfo = {
      wants: [
        {
          bundleName: this.context.abilityInfo.bundleName,
          abilityName: this.context.abilityInfo.name
        }
      ],
      actionType: wantAgent.OperationType.START_ABILITY,
      requestCode: 0,
      actionFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };
    wantAgent.getWantAgent(wantAgentInfo).then((wantAgentObj: WantAgent) => {
      backgroundTaskManager.startBackgroundRunning(this.context,
        backgroundTaskManager.BackgroundMode.AUDIO_RECORDING, wantAgentObj).then(() => {
        console.info(`Succeeded in operationing startBackgroundRunning.`);
        Logger.info(TAG, '录音服务启动成功 ...')
      }).catch((err: BusinessError) => {
        Logger.error(TAG, `录音服务启动失败. Code is ${err.code}, message is ${err.message}`);
      });
    });
  }

  stopServiceTask() {
    backgroundTaskManager.stopBackgroundRunning(this.context).then(() => {
      Logger.info(TAG, '停止服务成功。。。')
    }).catch((err: BusinessError) => {
      Logger.info(TAG, `停止服务失败. Code is ${err.code}, message is ${err.message}`);
    });
  }

  onPageShow(): void {
    this.stopServiceTask()
  }

  onPageHide(): void {
    if (this.isJoin) {
      this.startServiceTask()
    }
  }

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onAudioVolumeIndication = (speakers: Array<AudioVolumeInfo>, totalVolume: number) => {
      Logger.info(TAG,`onAudioVolumeIndication totalVolume:${totalVolume}    speakers:${JSON.stringify(speakers)}`)
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
      this.isMicophotoOpen = false
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
    const retAudio = this.rtcEngine.enableAudioVolumeIndication(1000, 3, true)
    Logger.info(TAG, `enableAudioVolumeIndication retAudio ${retAudio}`)

    //注册路由监听
    this.audioRoutingManager.on('preferOutputDeviceChangeForRendererInfo', {
      usage: audio.StreamUsage.STREAM_USAGE_VOICE_COMMUNICATION, rendererFlags: 0
    }, this.routerChangeListener);
  }

  aboutToDisappear(): void {
    this.audioRoutingManager.off('preferOutputDeviceChangeForRendererInfo',this.routerChangeListener)
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
          Text('')
          Button((this.isJoin && !this.isMicophotoOpen) ? $r('app.string.open_microphone') :
          $r('app.string.close_microphone'))
            .onClick(this.handleMicospeakerClick)
            .enabled(this.isJoin)
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)

        Row() {
          Text($r('app.string.audio_router'))
          AVCastPicker({
            normalColor: Color.Red,
            customPicker: () => this.customPickerBuilder()
          })
            .width(40)
            .height(40)
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)

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