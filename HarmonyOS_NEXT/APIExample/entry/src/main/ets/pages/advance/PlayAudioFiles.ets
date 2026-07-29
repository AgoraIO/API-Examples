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
const PRIMARY_AUDIO_EFFECT_ID: number = 0
const SECONDARY_AUDIO_EFFECT_ID: number = 1
const PRIMARY_AUDIO_EFFECT_FILE_NAME: string = 'effectA.wav'
const SECONDARY_AUDIO_EFFECT_FILE_NAME: string = 'music_1.m4a'

@Entry
@Component
struct PlayAudioFiles {
  private rtcEngine: RtcEngine | undefined = undefined;
  private primaryEffectFilePath: string = ''
  private secondaryEffectFilePath: string = ''
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUidList: number[] = []
  @State isPrimaryEffectReady: boolean = false
  @State isSecondaryEffectReady: boolean = false
  @State isPrimaryEffectPreloaded: boolean = false
  @State isSecondaryEffectPreloaded: boolean = false
  @State effectVolume: number = 100

  private getResultText(result: number): string {
    return result == Constants.ErrorCode.ERR_OK ? 'success' : `error(${result})`
  }

  private updateEffectFileState(soundId: number, filePath: string, isReady: boolean): void {
    if (soundId == PRIMARY_AUDIO_EFFECT_ID) {
      this.primaryEffectFilePath = filePath
      this.isPrimaryEffectReady = isReady
    } else {
      this.secondaryEffectFilePath = filePath
      this.isSecondaryEffectReady = isReady
    }
  }

  private prepareEffectFile(context: common.UIAbilityContext, soundId: number, fileName: string): void {
    const targetPath: string = `${context.filesDir}/${fileName}`
    context.resourceManager.getRawFileContent(fileName)
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
          Logger.error(TAG, `prepareEffectFile(${soundId}) incomplete:${writtenLength}/${buffer.byteLength}`)
          ShowToast.longToast(`prepareEffectFile(${soundId}): incomplete write`)
          return
        }
        this.updateEffectFileState(soundId, targetPath, true)
        Logger.info(TAG, `prepareEffectFile(${soundId}) success:${targetPath}`)
      })
      .catch((error: BusinessError) => {
        this.updateEffectFileState(soundId, '', false)
        Logger.error(TAG, `prepareEffectFile(${soundId}) failed. Code:${error.code}, message:${error.message}`)
        ShowToast.longToast(`prepareEffectFile(${soundId}): error(${error.code})`)
      })
  }

  private async startEffect(): Promise<void> {
    const engine = this.rtcEngine
    const effectFilePath: string = this.primaryEffectFilePath
    if (engine == undefined || !this.isPrimaryEffectReady || effectFilePath.length == 0) {
      ShowToast.shortToast('Audio effect is not ready')
      return
    }

    let didPreload: boolean = false
    if (!this.isPrimaryEffectPreloaded) {
      const preloadResult: number = await engine.preloadEffect(PRIMARY_AUDIO_EFFECT_ID, effectFilePath, 0)
      Logger.info(TAG, `preloadEffect result:${preloadResult}`)
      if (preloadResult != Constants.ErrorCode.ERR_OK) {
        ShowToast.longToast(`preloadEffect: ${this.getResultText(preloadResult)}`)
        return
      }
      this.isPrimaryEffectPreloaded = true
      didPreload = true
    }

    const playResult: number = await engine.playEffect(
      PRIMARY_AUDIO_EFFECT_ID,
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

  private async startAllEffects(): Promise<void> {
    const engine = this.rtcEngine
    if (engine == undefined || !this.isPrimaryEffectReady || !this.isSecondaryEffectReady ||
      this.primaryEffectFilePath.length == 0 || this.secondaryEffectFilePath.length == 0) {
      ShowToast.shortToast('Audio effects are not ready')
      return
    }

    if (!this.isPrimaryEffectPreloaded) {
      const primaryResult: number = await engine.preloadEffect(
        PRIMARY_AUDIO_EFFECT_ID, this.primaryEffectFilePath, 0)
      Logger.info(TAG, `preloadEffect(${PRIMARY_AUDIO_EFFECT_ID}) result:${primaryResult}`)
      if (primaryResult != Constants.ErrorCode.ERR_OK) {
        ShowToast.longToast(`preloadEffect(${PRIMARY_AUDIO_EFFECT_ID}): ${this.getResultText(primaryResult)}`)
        return
      }
      this.isPrimaryEffectPreloaded = true
    }

    if (!this.isSecondaryEffectPreloaded) {
      const secondaryResult: number = await engine.preloadEffect(
        SECONDARY_AUDIO_EFFECT_ID, this.secondaryEffectFilePath, 0)
      Logger.info(TAG, `preloadEffect(${SECONDARY_AUDIO_EFFECT_ID}) result:${secondaryResult}`)
      if (secondaryResult != Constants.ErrorCode.ERR_OK) {
        ShowToast.longToast(`preloadEffect(${SECONDARY_AUDIO_EFFECT_ID}): ${this.getResultText(secondaryResult)}`)
        return
      }
      this.isSecondaryEffectPreloaded = true
    }

    const result: number = engine.playAllEffects(-1, 1, 0, this.effectVolume, true)
    Logger.info(TAG, `playAllEffects result:${result}`)
    ShowToast.shortToast(`playAllEffects: ${this.getResultText(result)}`)
  }

  private pauseEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.pauseEffect(PRIMARY_AUDIO_EFFECT_ID)
    Logger.info(TAG, `pauseEffect result:${result}`)
    ShowToast.shortToast(`pauseEffect: ${this.getResultText(result)}`)
  }

  private resumeEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.resumeEffect(PRIMARY_AUDIO_EFFECT_ID)
    Logger.info(TAG, `resumeEffect result:${result}`)
    ShowToast.shortToast(`resumeEffect: ${this.getResultText(result)}`)
  }

  private stopEffect(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const stopResult: number = this.rtcEngine.stopEffect(PRIMARY_AUDIO_EFFECT_ID)
    const unloadResult: number = this.rtcEngine.unloadEffect(PRIMARY_AUDIO_EFFECT_ID)
    this.isPrimaryEffectPreloaded = false
    Logger.info(TAG, `stopEffect result:${stopResult}, unloadEffect result:${unloadResult}`)
    ShowToast.shortToast(
      `stopEffect: ${this.getResultText(stopResult)}, unloadEffect: ${this.getResultText(unloadResult)}`)
  }

  private pauseAllEffects(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.pauseAllEffects()
    Logger.info(TAG, `pauseAllEffects result:${result}`)
    ShowToast.shortToast(`pauseAllEffects: ${this.getResultText(result)}`)
  }

  private resumeAllEffects(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.resumeAllEffects()
    Logger.info(TAG, `resumeAllEffects result:${result}`)
    ShowToast.shortToast(`resumeAllEffects: ${this.getResultText(result)}`)
  }

  private stopAllEffects(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.stopAllEffects()
    Logger.info(TAG, `stopAllEffects result:${result}`)
    ShowToast.shortToast(`stopAllEffects: ${this.getResultText(result)}`)
  }

  private unloadAllEffects(): void {
    if (this.rtcEngine == undefined) {
      return
    }
    const result: number = this.rtcEngine.unloadAllEffects()
    Logger.info(TAG, `unloadAllEffects result:${result}`)
    if (result == Constants.ErrorCode.ERR_OK) {
      this.isPrimaryEffectPreloaded = false
      this.isSecondaryEffectPreloaded = false
    }
    ShowToast.shortToast(`unloadAllEffects: ${this.getResultText(result)}`)
  }

  private setEffectVolume(volume: number, showResult: boolean): void {
    if (this.rtcEngine == undefined) {
      return
    }
    this.effectVolume = Math.round(volume)
    const setEffectsResult: number = this.rtcEngine.setEffectsVolume(this.effectVolume)
    const setEffectResult: number = this.rtcEngine.setVolumeOfEffect(PRIMARY_AUDIO_EFFECT_ID, this.effectVolume)
    const effectsVolume: number = this.rtcEngine.getEffectsVolume()
    const effectVolume: number = this.rtcEngine.getVolumeOfEffect(PRIMARY_AUDIO_EFFECT_ID)
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
    this.prepareEffectFile(context, PRIMARY_AUDIO_EFFECT_ID, PRIMARY_AUDIO_EFFECT_FILE_NAME)
    this.prepareEffectFile(context, SECONDARY_AUDIO_EFFECT_ID, SECONDARY_AUDIO_EFFECT_FILE_NAME)
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isPrimaryEffectPreloaded || this.isSecondaryEffectPreloaded) {
        this.rtcEngine.stopAllEffects()
        this.rtcEngine.unloadAllEffects()
        this.isPrimaryEffectPreloaded = false
        this.isSecondaryEffectPreloaded = false
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

      Scroll() {
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
              .enabled(this.isPrimaryEffectReady)
              .onClick(async () => {
                await this.startEffect()
              })
            Button($r('app.string.resume'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded)
              .onClick(() => {
                this.resumeEffect()
              })
            Button($r('app.string.pause'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded)
              .onClick(() => {
                this.pauseEffect()
              })
            Button($r('app.string.stop'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded)
              .onClick(() => {
                this.stopEffect()
              })
          }.alignItems(VerticalAlign.Center).padding({ top: 12, bottom: 6 })
          Row() {
            Text($r('app.string.volume')).layoutWeight(1)
            Slider({ value: this.effectVolume, min: 0, max: 100, step: 1 })
              .layoutWeight(1.5)
              .enabled(this.isPrimaryEffectPreloaded)
              .onChange((value: number, mode: SliderChangeMode) => {
                this.setEffectVolume(value, mode == SliderChangeMode.End || mode == SliderChangeMode.Click)
              })
          }.alignItems(VerticalAlign.Center)

          Text($r('app.string.all_audio_effects')).padding({ top: 12, bottom: 6 })
          Divider()
          Row() {
            Button($r('app.string.play_all'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectReady && this.isSecondaryEffectReady)
              .onClick(async () => {
                await this.startAllEffects()
              })
            Button($r('app.string.resume_all'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded || this.isSecondaryEffectPreloaded)
              .onClick(() => {
                this.resumeAllEffects()
              })
            Button($r('app.string.pause_all'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded || this.isSecondaryEffectPreloaded)
              .onClick(() => {
                this.pauseAllEffects()
              })
          }.alignItems(VerticalAlign.Center).padding({ top: 12, bottom: 6 })
          Row() {
            Button($r('app.string.stop_all'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded || this.isSecondaryEffectPreloaded)
              .onClick(() => {
                this.stopAllEffects()
              })
            Button($r('app.string.unload_all'))
              .layoutWeight(1)
              .fontColor(Color.Blue)
              .buttonStyle(ButtonStyleMode.NORMAL)
              .enabled(this.isPrimaryEffectPreloaded || this.isSecondaryEffectPreloaded)
              .onClick(() => {
                this.unloadAllEffects()
              })
          }.alignItems(VerticalAlign.Center).padding({ bottom: 6 })
        }
        .alignItems(HorizontalAlign.Start)
        .width('100%')
        .padding({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })
      }
      .layoutWeight(3)
      .width('100%')
      .scrollBar(BarState.Auto)

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
