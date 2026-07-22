import { ChannelMediaOptions, Constants, RtcEngine, RtcEngineConfig } from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { TokenUtils } from '../../util/TokenUtils';
import { fileIo } from '@kit.CoreFileKit';
import { BusinessError } from '@kit.BasicServicesKit';

const TAG: string = 'PlayAudioFiles'
const AUDIO_EFFECT_ID: number = 0
const AUDIO_EFFECT_FILE_NAME: string = 'effectA.wav'

@Entry
@Component
struct PlayAudioFiles {
  private rtcEngine: RtcEngine | undefined = undefined;
  private effectFilePath: string = ''
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []
  @State isEffectReady: boolean = false
  @State isEffectPreloaded: boolean = false
  @State effectVolume: number = 100

  private getResultText(result: number): string {
    return result == Constants.ErrorCode.ERR_OK ? 'success' : `error(${result})`
  }

  private prepareEffectFile(context: common.UIAbilityContext): void {
    const targetPath: string = `${context.filesDir}/${AUDIO_EFFECT_FILE_NAME}`
    context.resourceManager.getRawFileContent(AUDIO_EFFECT_FILE_NAME)
      .then((buffer: Uint8Array) => {
        const file = fileIo.openSync(targetPath,
          fileIo.OpenMode.CREATE | fileIo.OpenMode.WRITE_ONLY | fileIo.OpenMode.TRUNC)
        let writtenLength: number = 0
        try {
          writtenLength = fileIo.writeSync(file.fd, buffer.buffer as ArrayBuffer)
        } finally {
          fileIo.closeSync(file)
        }
        if (writtenLength != buffer.byteLength) {
          Logger.error(TAG, `prepareEffectFile incomplete:${writtenLength}/${buffer.byteLength}`)
          ShowToast.longToast('prepareEffectFile: incomplete write')
          return
        }
        this.effectFilePath = targetPath
        this.isEffectReady = true
        Logger.info(TAG, `prepareEffectFile success:${targetPath}`)
      })
      .catch((error: BusinessError) => {
        this.effectFilePath = ''
        this.isEffectReady = false
        Logger.error(TAG, `prepareEffectFile failed. Code:${error.code}, message:${error.message}`)
        ShowToast.longToast(`prepareEffectFile: error(${error.code})`)
      })
  }

  private async startEffect(): Promise<void> {
    const engine = this.rtcEngine
    const effectFilePath: string = this.effectFilePath
    if (engine == undefined || !this.isEffectReady || effectFilePath.length == 0) {
      ShowToast.shortToast('Audio effect is not ready')
      return
    }

    let didPreload: boolean = false
    if (!this.isEffectPreloaded) {
      const preloadResult: number = await engine.preloadEffect(AUDIO_EFFECT_ID, effectFilePath, 0)
      Logger.info(TAG, `preloadEffect result:${preloadResult}`)
      if (preloadResult != Constants.ErrorCode.ERR_OK) {
        ShowToast.longToast(`preloadEffect: ${this.getResultText(preloadResult)}`)
        return
      }
      this.isEffectPreloaded = true
      didPreload = true
    }

    const playResult: number = await engine.playEffect(
      AUDIO_EFFECT_ID,
      effectFilePath,
      -1,
      1,
      0,
      this.effectVolume,
      true,
      0
    )
    Logger.info(TAG, `playEffect result:${playResult}`)
    if (playResult == Constants.ErrorCode.ERR_OK) {
      ShowToast.shortToast(didPreload ? 'preloadEffect/playEffect: success' : 'playEffect: success')
    } else {
      ShowToast.longToast(`playEffect: ${this.getResultText(playResult)}`)
    }
  }

  private pauseEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.pauseEffect(AUDIO_EFFECT_ID)
    Logger.info(TAG, `pauseEffect result:${result}`)
    ShowToast.shortToast(`pauseEffect: ${this.getResultText(result)}`)
  }

  private resumeEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.resumeEffect(AUDIO_EFFECT_ID)
    Logger.info(TAG, `resumeEffect result:${result}`)
    ShowToast.shortToast(`resumeEffect: ${this.getResultText(result)}`)
  }

  private stopEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const stopResult: number = this.rtcEngine.stopEffect(AUDIO_EFFECT_ID)
    const unloadResult: number = this.rtcEngine.unloadEffect(AUDIO_EFFECT_ID)
    this.isEffectPreloaded = false
    Logger.info(TAG, `stopEffect result:${stopResult}, unloadEffect result:${unloadResult}`)
    ShowToast.shortToast(
      `stopEffect: ${this.getResultText(stopResult)}, unloadEffect: ${this.getResultText(unloadResult)}`)
  }

  private setEffectVolume(volume: number, showResult: boolean): void {
    if (this.rtcEngine == undefined) {
      return
    }
    this.effectVolume = Math.round(volume)
    const setEffectsResult: number = this.rtcEngine.setEffectsVolume(this.effectVolume)
    const setEffectResult: number = this.rtcEngine.setVolumeOfEffect(AUDIO_EFFECT_ID, this.effectVolume)
    const effectsVolume: number = this.rtcEngine.getEffectsVolume()
    const effectVolume: number = this.rtcEngine.getVolumeOfEffect(AUDIO_EFFECT_ID)
    Logger.info(TAG,
      `setEffectsVolume result:${setEffectsResult}, setVolumeOfEffect result:${setEffectResult}, ` +
        `getEffectsVolume:${effectsVolume}, getVolumeOfEffect:${effectVolume}`)
    if (showResult) {
      ShowToast.shortToast(`effects volume: ${effectsVolume}, effect volume: ${effectVolume}`)
    }
  }

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
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
    config.mEventHandler.onAudioMixingPositionChanged = (position: bigint) => {
      Logger.info(TAG, "mEventHandler.onAudioMixingPositionChanged: " + position.toString())
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config)

    this.rtcEngine.enableAudio()
    this.prepareEffectFile(context)
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isEffectPreloaded) {
        this.rtcEngine.stopEffect(AUDIO_EFFECT_ID)
        this.rtcEngine.unloadEffect(AUDIO_EFFECT_ID)
        this.isEffectPreloaded = false
      }
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

        Text($r('app.string.audio_effect')).padding({ top: 12, bottom: 6 })
        Divider()
        Row() {
          Button($r('app.string.start'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .enabled(this.isEffectReady)
            .onClick(async () => {
              await this.startEffect()
            })
          Button($r('app.string.resume'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .enabled(this.isEffectPreloaded)
            .onClick(() => {
              this.resumeEffect()
            })
          Button($r('app.string.pause'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .enabled(this.isEffectPreloaded)
            .onClick(() => {
              this.pauseEffect()
            })
          Button($r('app.string.stop'))
            .layoutWeight(1)
            .fontColor(Color.Blue)
            .buttonStyle(ButtonStyleMode.NORMAL)
            .enabled(this.isEffectPreloaded)
            .onClick(() => {
              this.stopEffect()
            })
        }.alignItems(VerticalAlign.Center).padding({ top: 12, bottom: 6 })
        Row() {
          Text($r('app.string.volume')).layoutWeight(1)
          Slider({ value: this.effectVolume, min: 0, max: 100, step: 1 })
            .layoutWeight(1.5)
            .enabled(this.isEffectPreloaded)
            .onChange((value: number, mode: SliderChangeMode) => {
              this.setEffectVolume(value, mode == SliderChangeMode.End || mode == SliderChangeMode.Click)
            })
        }.alignItems(VerticalAlign.Center)
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
