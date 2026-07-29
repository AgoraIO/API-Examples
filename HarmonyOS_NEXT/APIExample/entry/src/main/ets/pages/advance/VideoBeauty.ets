import {
  BeautyOptions,
  ChannelMediaOptions,
  Constants,
  RtcEngine,
  RtcEngineConfig,
  VideoCanvas,
  VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { common } from '@kit.AbilityKit';
import { TitleBar } from '../../common/TitleBar';
import { AppID } from '../../common/KeyCenter';
import { GlobalInfo } from '../Data';
import Logger from '../../util/Logger';
import PermissionHelper from '../../util/PermissionHelper';
import ShowToast from '../../util/ShowToast';
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'VideoBeauty'
const VIDEO_PREVIEW_ASPECT_RATIO: number = 1.2676

@Entry
@Component
struct VideoBeauty {
  private rtcEngine: RtcEngine | undefined = undefined
  private localXComponentController: XComponentController = new XComponentController()
  private remoteXComponentController: XComponentController = new XComponentController()
  private contrastLevels: number[] = [
    BeautyOptions.LIGHTENING_CONTRAST_LOW,
    BeautyOptions.LIGHTENING_CONTRAST_NORMAL,
    BeautyOptions.LIGHTENING_CONTRAST_HIGH
  ]
  private contrastNames: ResourceStr[] = [
    $r('app.string.beauty_contrast_low'),
    $r('app.string.beauty_contrast_normal'),
    $r('app.string.beauty_contrast_high')
  ]

  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State beautyEnabled: boolean = false
  @State contrastIndex: number = 1
  @State lighteningLevel: number = 0.6
  @State smoothnessLevel: number = 0.5
  @State rednessLevel: number = 0.1
  @State sharpnessLevel: number = 0.3

  private applyBeautyEffect(detail: string, showToast: boolean): boolean {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return false
    }
    const options = new BeautyOptions(
      this.contrastLevels[this.contrastIndex],
      this.lighteningLevel,
      this.smoothnessLevel,
      this.rednessLevel,
      this.sharpnessLevel
    )
    const result = engine.setBeautyEffectOptions(
      this.beautyEnabled,
      options,
      Constants.MediaSourceType.PRIMARY_CAMERA
    )
    Logger.info(TAG, `setBeautyEffectOptions ${detail} result:${result}`)
    if (showToast) {
      if (result == Constants.ErrorCode.ERR_OK) {
        ShowToast.shortToast(`setBeautyEffectOptions: ${detail}`)
      } else {
        ShowToast.longToast(`setBeautyEffectOptions failed: ${result} : ${RtcEngine.getErrorDescription(result)}`)
      }
    }
    return result == Constants.ErrorCode.ERR_OK
  }

  private resetBeautyEffect(): void {
    if (this.beautyEnabled) {
      this.beautyEnabled = false
      this.applyBeautyEffect('enabled=false', false)
    }
  }

  private leaveChannel(): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    this.resetBeautyEffect()
    engine.stopPreview()
    const result = engine.leaveChannel()
    if (result != Constants.ErrorCode.ERR_OK) {
      ShowToast.longToast(`leaveChannel failed: ${result} : ${RtcEngine.getErrorDescription(result)}`)
    }
  }

  aboutToAppear(): void {
    const config = new RtcEngineConfig()
    config.mAppId = AppID
    config.mContext = getContext(this) as common.UIAbilityContext
    config.mEventHandler = {}
    config.mEventHandler.onUserJoined = (uid: number, elapsed: number) => {
      Logger.info(TAG, `onUserJoined uid:${uid}, elapsed:${elapsed}`)
      if (this.remoteUid == 0) {
        this.remoteUid = uid
      }
    }
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, `onUserOffline uid:${uid}, reason:${reason}`)
      if (this.remoteUid == uid) {
        this.remoteUid = 0
      }
    }
    config.mEventHandler.onJoinChannelSuccess = (channel: string, uid: number, elapsed: number) => {
      Logger.info(TAG, `onJoinChannelSuccess channel:${channel}, uid:${uid}, elapsed:${elapsed}`)
      this.isJoin = true
      this.localUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, 'onLeaveChannel')
      this.isJoin = false
      this.localUid = 0
      this.remoteUid = 0
      this.beautyEnabled = false
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.error(TAG, `onError err:${err}, message:${message}`)
      ShowToast.longToast(`onError ${err}: ${message}`)
    }

    this.rtcEngine = RtcEngine.create(config)
    const encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    this.resetBeautyEffect()
    if (this.isJoin) {
      engine.stopPreview()
      engine.leaveChannel()
    }
    RtcEngine.destroy().then(() => {
      Logger.info(TAG, 'destroy done')
    })
    this.rtcEngine = undefined
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_videobeauty'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
            XComponent({
              id: 'video_beauty_local',
              type: XComponentType.SURFACE,
              controller: this.localXComponentController
            })
              .onLoad(() => {
                const surfaceId = this.localXComponentController.getXComponentSurfaceId()
                const canvas = new VideoCanvas(surfaceId, true)
                canvas.uid = this.localUid
                canvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN
                canvas.mirrorMode = 0
                this.rtcEngine?.setupLocalVideo(canvas)
              })
              .width('50%')
          }

          if (this.remoteUid != 0) {
            XComponent({
              id: 'video_beauty_remote',
              type: XComponentType.SURFACE,
              controller: this.remoteXComponentController
            })
              .onLoad(() => {
                const surfaceId = this.remoteXComponentController.getXComponentSurfaceId()
                const canvas = new VideoCanvas(surfaceId, true)
                canvas.uid = this.remoteUid
                canvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN
                canvas.mirrorMode = 0
                this.rtcEngine?.setupRemoteVideo(canvas)
              })
              .width('50%')
          }
        }
        .align(Alignment.TopStart)
        .width('100%')
        .aspectRatio(VIDEO_PREVIEW_ASPECT_RATIO)
      }
      .align(Alignment.TopStart)
      .flexGrow(0)

      Scroll() {
        Column({ space: 8 }) {
          Row() {
            Text($r('app.string.beauty_effect')).layoutWeight(1)
            Toggle({ type: ToggleType.Switch, isOn: this.beautyEnabled })
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .onChange((isOn: boolean) => {
                const previousValue = this.beautyEnabled
                this.beautyEnabled = isOn
                if (!this.applyBeautyEffect(`enabled=${isOn}`, true)) {
                  this.beautyEnabled = previousValue
                }
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.beauty_contrast')).layoutWeight(1)
            Select(this.contrastNames.map((name: ResourceStr) => {
              const option: SelectOption = { value: name }
              return option
            }))
              .selected(this.contrastIndex)
              .value(this.contrastNames[this.contrastIndex])
              .enabled(this.isJoin && this.beautyEnabled)
              .opacity(this.isJoin && this.beautyEnabled ? 1 : 0.4)
              .width('54%')
              .onSelect((index: number, text: string) => {
                const previousIndex = this.contrastIndex
                this.contrastIndex = index
                if (!this.applyBeautyEffect(`lighteningContrastLevel=${text}`, true)) {
                  this.contrastIndex = previousIndex
                }
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.beauty_lightening')).layoutWeight(1)
            Text(this.lighteningLevel.toFixed(1)).width(32)
            Slider({ value: this.lighteningLevel, min: 0, max: 1, step: 0.1 })
              .enabled(this.isJoin && this.beautyEnabled)
              .opacity(this.isJoin && this.beautyEnabled ? 1 : 0.4)
              .width('54%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.lighteningLevel = Math.round(value * 10) / 10
                this.applyBeautyEffect(
                  `lighteningLevel=${this.lighteningLevel.toFixed(1)}`,
                  mode == SliderChangeMode.End || mode == SliderChangeMode.Click
                )
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.beauty_smoothness')).layoutWeight(1)
            Text(this.smoothnessLevel.toFixed(1)).width(32)
            Slider({ value: this.smoothnessLevel, min: 0, max: 1, step: 0.1 })
              .enabled(this.isJoin && this.beautyEnabled)
              .opacity(this.isJoin && this.beautyEnabled ? 1 : 0.4)
              .width('54%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.smoothnessLevel = Math.round(value * 10) / 10
                this.applyBeautyEffect(
                  `smoothnessLevel=${this.smoothnessLevel.toFixed(1)}`,
                  mode == SliderChangeMode.End || mode == SliderChangeMode.Click
                )
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.beauty_redness')).layoutWeight(1)
            Text(this.rednessLevel.toFixed(1)).width(32)
            Slider({ value: this.rednessLevel, min: 0, max: 1, step: 0.1 })
              .enabled(this.isJoin && this.beautyEnabled)
              .opacity(this.isJoin && this.beautyEnabled ? 1 : 0.4)
              .width('54%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.rednessLevel = Math.round(value * 10) / 10
                this.applyBeautyEffect(
                  `rednessLevel=${this.rednessLevel.toFixed(1)}`,
                  mode == SliderChangeMode.End || mode == SliderChangeMode.Click
                )
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.beauty_sharpness')).layoutWeight(1)
            Text(this.sharpnessLevel.toFixed(1)).width(32)
            Slider({ value: this.sharpnessLevel, min: 0, max: 1, step: 0.1 })
              .enabled(this.isJoin && this.beautyEnabled)
              .opacity(this.isJoin && this.beautyEnabled ? 1 : 0.4)
              .width('54%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.sharpnessLevel = Math.round(value * 10) / 10
                this.applyBeautyEffect(
                  `sharpnessLevel=${this.sharpnessLevel.toFixed(1)}`,
                  mode == SliderChangeMode.End || mode == SliderChangeMode.Click
                )
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)
        }
        .width('100%')
        .padding({ left: 12, right: 12, top: 8, bottom: 8 })
      }
      .layoutWeight(1)
      .width('100%')
      .scrollBar(BarState.Auto)

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id('input_video_beauty_channel')
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .layoutWeight(1)
          .margin({ right: 6 })

        Button(this.isJoin ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_video_beauty_join')
          .onClick(async () => {
            focusControl.requestFocus('button_video_beauty_join')
            const engine = this.rtcEngine
            if (engine == undefined) {
              return
            }
            if (this.isJoin) {
              this.leaveChannel()
              return
            }
            if (this.channelName.length == 0) {
              ShowToast.shortToast($r('app.string.channel_name_empty'))
              return
            }
            if (!(await PermissionHelper.checkPermissions(
              getContext(this) as common.UIAbilityContext,
              ['ohos.permission.MICROPHONE', 'ohos.permission.CAMERA']
            ))) {
              ShowToast.shortToast('Permission leak!')
              return
            }

            const options = new ChannelMediaOptions()
            options.publishCameraTrack = true
            options.publishMicrophoneTrack = true
            options.autoSubscribeVideo = true
            options.autoSubscribeAudio = true
            options.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING
            options.clientRoleType = Constants.ClientRole.BROADCASTER

            const token = await TokenUtils.genRtcToken(this.channelName, '')
            const result = engine.joinChannelWithOptions(token, this.channelName, 0, options)
            if (result != Constants.ErrorCode.ERR_OK) {
              ShowToast.longToast(`joinChannelWithOptions failed: ${result} : ${RtcEngine.getErrorDescription(result)}`)
              return
            }
            engine.startPreview()
          })
      }
      .width('100%')
      .alignItems(VerticalAlign.Center)
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}
