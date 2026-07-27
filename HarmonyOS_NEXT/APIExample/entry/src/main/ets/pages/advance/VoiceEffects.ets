import {
  ChannelMediaOptions,
  Constants,
  RtcEngine,
  RtcEngineConfig
} from '@shengwang/rtc-full';
import { common } from '@kit.AbilityKit';
import { TitleBar } from '../../common/TitleBar';
import { AppID } from '../../common/KeyCenter';
import Logger from '../../util/Logger';
import PermissionHelper from '../../util/PermissionHelper';
import ShowToast from '../../util/ShowToast';
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'VoiceEffects'

@Entry
@Component
struct VoiceEffects {
  private rtcEngine: RtcEngine | undefined = undefined
  private voiceBeautifierPresets: Constants.VoiceBeautifierPreset[] = [
    Constants.VoiceBeautifierPreset.OFF,
    Constants.VoiceBeautifierPreset.CHAT_BEAUTIFIER_MAGNETIC,
    Constants.VoiceBeautifierPreset.CHAT_BEAUTIFIER_FRESH,
    Constants.VoiceBeautifierPreset.CHAT_BEAUTIFIER_VITALITY,
    Constants.VoiceBeautifierPreset.SINGING_BEAUTIFIER,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_VIGOROUS,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_DEEP,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_MELLOW,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_FALSETTO,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_FULL,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_CLEAR,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_RESOUNDING,
    Constants.VoiceBeautifierPreset.TIMBRE_TRANSFORMATION_RINGING
  ]
  private voiceBeautifierNames: ResourceStr[] = [
    'OFF',
    'MAGNETIC',
    'FRESH',
    'VITALITY',
    'SINGING',
    'VIGOROUS',
    'DEEP',
    'MELLOW',
    'FALSETTO',
    'FULL',
    'CLEAR',
    'RESOUNDING',
    'RINGING'
  ]
  private audioEffectPresets: Constants.AudioEffectPreset[] = [
    Constants.AudioEffectPreset.AUDIO_EFFECT_OFF,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_KTV,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_VOCAL_CONCERT,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_STUDIO,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_PHONOGRAPH,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_VIRTUAL_STEREO,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_SPACIAL,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_ETHEREAL,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_3D_VOICE,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_VIRTUAL_SURROUND_SOUND,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_CHORUS,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_BATHROOM,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_STAIRWELL,
    Constants.AudioEffectPreset.ROOM_ACOUSTICS_NATURE,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_UNCLE,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_OLDMAN,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_BOY,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_SISTER,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_GIRL,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_PIGKING,
    Constants.AudioEffectPreset.VOICE_CHANGER_EFFECT_HULK,
    Constants.AudioEffectPreset.STYLE_TRANSFORMATION_RNB,
    Constants.AudioEffectPreset.STYLE_TRANSFORMATION_POPULAR,
    Constants.AudioEffectPreset.PITCH_CORRECTION
  ]
  private audioEffectNames: ResourceStr[] = [
    'OFF',
    'KTV',
    'VOCAL_CONCERT',
    'STUDIO',
    'PHONOGRAPH',
    'VIRTUAL_STEREO',
    'SPACIAL',
    'ETHEREAL',
    '3D_VOICE',
    'VIRTUAL_SURROUND',
    'CHORUS',
    'BATHROOM',
    'STAIRWELL',
    'NATURE',
    'UNCLE',
    'OLD_MAN',
    'BOY',
    'SISTER',
    'GIRL',
    'PIG_KING',
    'HULK',
    'RNB',
    'POPULAR',
    'PITCH_CORRECTION'
  ]
  private equalizationBands: Constants.AudioEqualizationBandFrequency[] = [
    Constants.AudioEqualizationBandFrequency.BAND_31,
    Constants.AudioEqualizationBandFrequency.BAND_62,
    Constants.AudioEqualizationBandFrequency.BAND_125,
    Constants.AudioEqualizationBandFrequency.BAND_250,
    Constants.AudioEqualizationBandFrequency.BAND_500,
    Constants.AudioEqualizationBandFrequency.BAND_1K,
    Constants.AudioEqualizationBandFrequency.BAND_2K,
    Constants.AudioEqualizationBandFrequency.BAND_4K,
    Constants.AudioEqualizationBandFrequency.BAND_8K,
    Constants.AudioEqualizationBandFrequency.BAND_16K
  ]
  private equalizationBandNames: ResourceStr[] = [
    '31 Hz',
    '62 Hz',
    '125 Hz',
    '250 Hz',
    '500 Hz',
    '1 kHz',
    '2 kHz',
    '4 kHz',
    '8 kHz',
    '16 kHz'
  ]
  private equalizationGains: number[] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
  private reverbKeys: Constants.AudioReverbType[] = [
    Constants.AudioReverbType.DRY_LEVEL,
    Constants.AudioReverbType.WET_LEVEL,
    Constants.AudioReverbType.ROOM_SIZE,
    Constants.AudioReverbType.WET_DELAY,
    Constants.AudioReverbType.STRENGTH
  ]
  private reverbKeyNames: ResourceStr[] = [
    'DRY_LEVEL',
    'WET_LEVEL',
    'ROOM_SIZE',
    'WET_DELAY',
    'STRENGTH'
  ]
  private reverbValues: number[] = [0, 0, 0, 0, 0]

  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State voiceBeautifierIndex: number = 0
  @State audioEffectIndex: number = 0
  @State equalizationBandIndex: number = 0
  @State equalizationGain: number = 0
  @State reverbKeyIndex: number = 0
  @State reverbValue: number = 0

  private showResult(api: string, value: string, result: number): void {
    Logger.info(TAG, `${api} value:${value} result:${result}`)
    if (result == Constants.ErrorCode.ERR_OK) {
      ShowToast.shortToast(`${api}: ${value}`)
    } else {
      ShowToast.longToast(`${api} failed: ${result}`)
    }
  }

  private resetPresetSelections(engine: RtcEngine): void {
    if (this.voiceBeautifierIndex != 0) {
      engine.setVoiceBeautifierPreset(Constants.VoiceBeautifierPreset.OFF)
    }
    if (this.audioEffectIndex != 0) {
      engine.setAudioEffectPreset(Constants.AudioEffectPreset.AUDIO_EFFECT_OFF)
    }
    this.voiceBeautifierIndex = 0
    this.audioEffectIndex = 0
  }

  private resetCustomEffectValues(): void {
    this.equalizationGains = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    this.reverbValues = [0, 0, 0, 0, 0]
    this.equalizationGain = 0
    this.reverbValue = 0
  }

  private getReverbMin(): number {
    return this.reverbKeyIndex <= 1 ? -20 : 0
  }

  private getReverbMax(): number {
    if (this.reverbKeyIndex <= 1) {
      return 10
    }
    if (this.reverbKeyIndex == 3) {
      return 200
    }
    return 100
  }

  aboutToAppear(): void {
    const config = new RtcEngineConfig()
    config.mAppId = AppID
    config.mContext = getContext(this) as common.UIAbilityContext
    config.mEventHandler = {}
    config.mEventHandler.onJoinChannelSuccess = (cid: string, uid: number, elapsed: number) => {
      Logger.info(TAG, `onJoinChannelSuccess uid:${uid}`)
      this.isJoin = true
      this.localUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, 'onLeaveChannel')
      this.isJoin = false
      this.localUid = 0
      this.remoteUid = 0
    }
    config.mEventHandler.onUserJoined = (uid: number, elapsed: number) => {
      Logger.info(TAG, `onUserJoined uid:${uid} elapsed:${elapsed}`)
      if (this.remoteUid == 0) {
        this.remoteUid = uid
      }
    }
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, `onUserOffline uid:${uid} reason:${reason}`)
      if (this.remoteUid == uid) {
        this.remoteUid = 0
      }
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.error(TAG, `onError err:${err} message:${message}`)
      ShowToast.longToast(`onError ${err}: ${message}`)
    }
    this.rtcEngine = RtcEngine.create(config)
    this.rtcEngine.enableAudio()
  }

  aboutToDisappear(): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    if (this.isJoin) {
      engine.leaveChannel()
    }
    RtcEngine.destroy().then(() => {
      Logger.info(TAG, 'destroy done')
    })
    this.rtcEngine = undefined
  }

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_voiceeffects'),
        showBack: true
      })

      if (this.isJoin) {
        Row({ space: 8 }) {
          Column() {
            Text('Local')
            Text(this.localUid.toString())
          }
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .backgroundColor($r('app.color.background_audio_cell'))
          .layoutWeight(1)
          .height(72)

          if (this.remoteUid != 0) {
            Column() {
              Text('Remote')
              Text(this.remoteUid.toString())
            }
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .backgroundColor($r('app.color.background_audio_cell'))
            .layoutWeight(1)
            .height(72)
          }
        }
        .width('100%')
        .padding({ left: 12, right: 12, top: 8 })
      }

      Scroll() {
        Column({ space: 10 }) {
          Row() {
            Text($r('app.string.voice_beautifier_preset')).layoutWeight(1)
            Select(this.voiceBeautifierNames.map((name: ResourceStr) => {
              const option: SelectOption = { value: name }
              return option
            }))
              .selected(this.voiceBeautifierIndex)
              .value(this.voiceBeautifierNames[this.voiceBeautifierIndex])
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('58%')
              .optionWidth(280)
              .optionHeight(320)
              .onSelect((index: number, text: string) => {
                const engine = this.rtcEngine
                if (engine == undefined) {
                  return
                }
                this.voiceBeautifierIndex = index
                this.audioEffectIndex = 0
                this.resetCustomEffectValues()
                engine.setAudioEffectPreset(Constants.AudioEffectPreset.AUDIO_EFFECT_OFF)
                const result = engine.setVoiceBeautifierPreset(this.voiceBeautifierPresets[index])
                this.showResult('setVoiceBeautifierPreset', text, result)
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.audio_effect_preset')).layoutWeight(1)
            Select(this.audioEffectNames.map((name: ResourceStr) => {
              const option: SelectOption = { value: name }
              return option
            }))
              .selected(this.audioEffectIndex)
              .value(this.audioEffectNames[this.audioEffectIndex])
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('58%')
              .optionWidth(280)
              .optionHeight(320)
              .onSelect((index: number, text: string) => {
                const engine = this.rtcEngine
                if (engine == undefined) {
                  return
                }
                this.audioEffectIndex = index
                this.voiceBeautifierIndex = 0
                this.resetCustomEffectValues()
                engine.setVoiceBeautifierPreset(Constants.VoiceBeautifierPreset.OFF)
                const result = engine.setAudioEffectPreset(this.audioEffectPresets[index])
                this.showResult('setAudioEffectPreset', text, result)
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Divider()

          Row() {
            Text($r('app.string.equalization_band')).layoutWeight(1)
            Select(this.equalizationBandNames.map((name: ResourceStr) => {
              const option: SelectOption = { value: name }
              return option
            }))
              .selected(this.equalizationBandIndex)
              .value(this.equalizationBandNames[this.equalizationBandIndex])
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('50%')
              .onSelect((index: number) => {
                this.equalizationBandIndex = index
                this.equalizationGain = this.equalizationGains[index]
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.equalization_gain')).layoutWeight(1)
            Text(`${this.equalizationGain} dB`).width(52)
            Slider({ value: this.equalizationGain, min: -15, max: 15, step: 1 })
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('48%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.equalizationGain = Math.round(value)
                if (mode != SliderChangeMode.End && mode != SliderChangeMode.Click) {
                  return
                }
                const engine = this.rtcEngine
                if (engine == undefined) {
                  return
                }
                this.equalizationGains[this.equalizationBandIndex] = this.equalizationGain
                this.resetPresetSelections(engine)
                const result = engine.setLocalVoiceEqualization(
                  this.equalizationBands[this.equalizationBandIndex], this.equalizationGain)
                this.showResult('setLocalVoiceEqualization',
                  `${this.equalizationBandNames[this.equalizationBandIndex]} / ${this.equalizationGain} dB`, result)
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Divider()

          Row() {
            Text($r('app.string.reverb_key')).layoutWeight(1)
            Select(this.reverbKeyNames.map((name: ResourceStr) => {
              const option: SelectOption = { value: name }
              return option
            }))
              .selected(this.reverbKeyIndex)
              .value(this.reverbKeyNames[this.reverbKeyIndex])
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('50%')
              .onSelect((index: number) => {
                this.reverbKeyIndex = index
                this.reverbValue = this.reverbValues[index]
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)

          Row() {
            Text($r('app.string.reverb_value')).layoutWeight(1)
            Text(this.reverbValue.toString()).width(44)
            Slider({
              value: this.reverbValue,
              min: this.getReverbMin(),
              max: this.getReverbMax(),
              step: 1
            })
              .enabled(this.isJoin)
              .opacity(this.isJoin ? 1 : 0.4)
              .width('48%')
              .onChange((value: number, mode: SliderChangeMode) => {
                this.reverbValue = Math.round(value)
                if (mode != SliderChangeMode.End && mode != SliderChangeMode.Click) {
                  return
                }
                const engine = this.rtcEngine
                if (engine == undefined) {
                  return
                }
                this.reverbValues[this.reverbKeyIndex] = this.reverbValue
                this.resetPresetSelections(engine)
                const result = engine.setLocalVoiceReverb(
                  this.reverbKeys[this.reverbKeyIndex], this.reverbValue)
                this.showResult('setLocalVoiceReverb',
                  `${this.reverbKeyNames[this.reverbKeyIndex]} / ${this.reverbValue}`, result)
              })
          }
          .width('100%')
          .alignItems(VerticalAlign.Center)
        }
        .width('100%')
        .padding({ left: 12, right: 12, top: 12, bottom: 12 })
      }
      .layoutWeight(1)
      .width('100%')
      .scrollBar(BarState.Auto)

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id('input_channel_name')
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
            focusControl.requestFocus('button_to_join')
            const engine = this.rtcEngine
            if (engine == undefined) {
              return
            }
            if (this.isJoin) {
              engine.leaveChannel()
              return
            }
            if (this.channelName == '') {
              ShowToast.shortToast('The channel name is empty!')
              return
            }
            if (!(await PermissionHelper.checkPermissions(
              getContext(this) as common.UIAbilityContext,
              ['ohos.permission.MICROPHONE']
            ))) {
              ShowToast.shortToast('Permission leak!')
              return
            }

            engine.setAudioProfile(Constants.AudioProfileType.MUSIC_HIGH_QUALITY_STEREO)
            engine.setAudioScenario(Constants.AudioScenarioType.GAME_STREAMING)
            const mediaOptions = new ChannelMediaOptions()
            mediaOptions.publishCameraTrack = false
            mediaOptions.publishMicrophoneTrack = true
            mediaOptions.autoSubscribeVideo = false
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING
            mediaOptions.clientRoleType = Constants.ClientRole.BROADCASTER

            const token = await TokenUtils.genRtcToken(this.channelName, '')
            const result = engine.joinChannelWithOptions(token, this.channelName, 0, mediaOptions)
            if (result != Constants.ErrorCode.ERR_OK) {
              ShowToast.longToast(`joinChannelWithOptions failed: ${result} : ${RtcEngine.getErrorDescription(result)}`)
            }
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
