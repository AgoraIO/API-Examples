import { AudioTrackConfig, ChannelMediaOptions, Constants, RtcEngine, RtcEngineConfig,
  VideoEncoderConfiguration } from '@shengwang/rtc-full';
import { common } from '@kit.AbilityKit';
import { AppID } from '../../common/KeyCenter';
import Logger from '../../util/Logger';
import { ArrayList } from '@kit.ArkTS';
import { TitleBar } from '../../common/TitleBar';
import ShowToast from '../../util/ShowToast';
import PermissionHelper from '../../util/PermissionHelper';
import { BusinessError } from '@kit.BasicServicesKit';
import { GlobalInfo } from '../Data';


const TAG: string = 'CustomAudioSource'

@Entry
@Component
struct CustomAudioSource {
  private rtcEngine: RtcEngine | undefined = undefined;
  private intervalAudioNum = -1
  private customAudioTrack = -1
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []
  @State pushingLocalAudio: boolean = false
  @State pushingMicrophone: boolean = false

  aboutToAppear(): void {
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
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);
    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)

    let audioTrackConfig = new AudioTrackConfig()
    this.customAudioTrack = this.rtcEngine.createCustomAudioTrack(Constants.AudioTrackType.MIXABLE, audioTrackConfig)
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine.stopPreview();
        this.rtcEngine.leaveChannel();
      }
      this.stopPushAudio()
      this.rtcEngine.destroyCustomAudioTrack(this.customAudioTrack)
      this.customAudioTrack = -1
      Logger.info(TAG, "destroy begin")
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done")
      });
      Logger.info(TAG, "destroy end")
      this.rtcEngine = undefined;
    }
  }

  async startPushAudio() {
    let file_name = 'output.raw'
    let sample_rate = 44100
    let channel_num = 2
    let bits_per_sample = 16
    let byte_per_sample = 1.0 * bits_per_sample / 8
    let duration_per_sample = 1000.0 / sample_rate
    let sample_cout_per_ms = sample_rate / 1000.0 // ms
    let buffer_sample_cout = sample_cout_per_ms * 10
    let buffer_byte_size = buffer_sample_cout * byte_per_sample * channel_num
    let buffer_duration = buffer_sample_cout * duration_per_sample

    getContext(this).resourceManager
      .getRawFileContent(file_name)
      .then((buffer: Uint8Array) => {
        let readStart = 0
        this.intervalAudioNum = setInterval(async () => {
          if (!this.pushingLocalAudio) {
            return;
          }
          if (readStart == buffer.byteLength) {
            readStart = 0;
          }
          let currentTime = Date.now()
          let frameBuffer: Uint8Array = buffer.slice(readStart, readStart + buffer_byte_size)
          readStart += buffer_byte_size
          let ret = this.rtcEngine?.pushExternalAudioFrame(frameBuffer.buffer,
            currentTime,
            sample_rate,
            channel_num,
            byte_per_sample,
            this.customAudioTrack)
          if (ret != 0) {
            Logger.error(TAG, "pushExternalAudioFrame failed: " + ret)
          }
        }, buffer_duration)
      })
      .catch((reason: BusinessError) => {
        Logger.error(TAG, "push audio called failed : " + reason)
      })
  }

  async stopPushAudio() {
    this.pushingLocalAudio = false
    if (this.intervalAudioNum !== -1) {
      clearInterval(this.intervalAudioNum);
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_customaudiosource'),
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

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
        Text($r('app.string.publish_local_audio'))
          .flexGrow(1)

        Toggle({ type: ToggleType.Switch, isOn: this.pushingLocalAudio })
          .onChange((isOn) => {
            this.pushingLocalAudio = isOn

            let option = new ChannelMediaOptions()
            option.publishCustomAudioTrack = isOn
            option.publishCustomAudioTrackId = this.customAudioTrack
            this.rtcEngine?.updateChannelMediaOptions(option)
            if (isOn) {
              this.startPushAudio()
            } else {
              this.stopPushAudio()
            }
          })
          .flexGrow(0)
      }
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
      .flexGrow(0)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
        Text($r('app.string.publish_microphone'))
          .flexGrow(1)

        Toggle({ type: ToggleType.Switch, isOn: this.pushingMicrophone })
          .onChange((isOn) => {
            this.pushingMicrophone = isOn
            let option = new ChannelMediaOptions()
            option.publishMicrophoneTrack = isOn
            this.rtcEngine?.updateChannelMediaOptions(option)
          })
          .flexGrow(0)
      }
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
      .flexGrow(0)


      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .type(InputType.Number)
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
              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = this.pushingMicrophone
              mediaOption.publishCustomAudioTrack = this.pushingLocalAudio
              mediaOption.publishCustomAudioTrackId = this.customAudioTrack
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption);
              focusControl.requestFocus("button_to_join")
            })
            .flexGrow(0)
        }
      }
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}