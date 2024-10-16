import { Constants } from '@shengwang/rtc-full';

export class AudioConfigHelper {
  public static readonly AUDIO_SCENARIO_DEFAULT = 'DEFAULT'
  public static readonly AUDIO_SCENARIO_GAME_STREAMING = 'GAME_STREAMING'
  public static readonly AUDIO_SCENARIO_CHATROOM = 'CHATROOM'
  public static readonly AUDIO_PROFILE_DEFAULT = 'DEFAULT'
  public static readonly AUDIO_PROFILE_SPEECH_STANDARD = 'SPEECH_STANDARD'
  public static readonly AUDIO_PROFILE_MUSIC_STANDARD = 'MUSIC_STANDARD'
  public static readonly AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 'MUSIC_STANDARD_STEREO'
  public static readonly AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 'MUSIC_HIGH_QUALITY'
  public static readonly AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 'MUSIC_HIGH_QUALITY_STEREO'
  public static readonly CHANNEL_PROFILE_COMMUNICATION = 'COMMUNICATION'
  public static readonly CHANNEL_PROFILE_LIVE_BROADCASTING = 'LIVE_BROADCASTING'

  // public static readonly CHANNEL_PROFILE_GAME = 'GAME'
  // public static readonly CHANNEL_PROFILE_CLOUD_GAMING = 'CLOUD_GAMING'
  // public static readonly CHANNEL_PROFILE_COMMUNICATION_1v1 = 'COMMUNICATION_1v1'

  public static getAudioRouteType(position: number) {
    let type = Constants.AudioRouteTypes.AUDIO_ROUTE_EARPIECE
    switch (position) {
      case 0:
        type = Constants.AudioRouteTypes.AUDIO_ROUTE_SPEAKERPHONE
        break
      case 1:
        type = Constants.AudioRouteTypes.AUDIO_ROUTE_EARPIECE
        break
      case 2:
        type = Constants.AudioRouteTypes.AUDIO_ROUTE_HEADSET
        break
      case 3:
        type = Constants.AudioRouteTypes.AUDIO_ROUTE_BLUETOOTH_DEVICE_HFP
        break
    }
    return type

  }

  public static getChannelProfileType(channelProfile: string) {
    let type = Constants.ChannelProfile.COMMUNICATION
    switch (channelProfile) {
      case AudioConfigHelper.CHANNEL_PROFILE_LIVE_BROADCASTING:
        type = Constants.ChannelProfile.LIVE_BROADCASTING
        break
    }
    return type
  }

  public static getAudioScenarioType(audioScenario: string) {
    let type = Constants.AudioScenarioType.DEFAULT
    switch (audioScenario) {
      case AudioConfigHelper.AUDIO_SCENARIO_GAME_STREAMING:
        type = Constants.AudioScenarioType.GAME_STREAMING
        break
      case AudioConfigHelper.AUDIO_SCENARIO_CHATROOM:
        type = Constants.AudioScenarioType.CHATROOM
        break
    }
    return type
  }

  public static getAudioProfileType(audioProfile: string) {
    let type = Constants.AudioProfileType.DEFAULT
    switch (audioProfile) {
      case AudioConfigHelper.AUDIO_PROFILE_SPEECH_STANDARD:
        type = Constants.AudioProfileType.SPEECH_STANDARD
        break
      case AudioConfigHelper.AUDIO_PROFILE_MUSIC_STANDARD:
        type = Constants.AudioProfileType.MUSIC_STANDARD
        break
      case AudioConfigHelper.AUDIO_PROFILE_MUSIC_STANDARD_STEREO:
        type = Constants.AudioProfileType.MUSIC_STANDARD_STEREO
        break
      case AudioConfigHelper.AUDIO_PROFILE_MUSIC_HIGH_QUALITY:
        type = Constants.AudioProfileType.MUSIC_HIGH_QUALITY
        break
      case AudioConfigHelper.AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO:
        type = Constants.AudioProfileType.MUSIC_HIGH_QUALITY_STEREO
        break
    }
    return type
  }
}

export const AUDIO_ROUTE = [
  $r('app.string.audio_route_speakerphone'),
  $r('app.string.audio_route_earpiece'),
  $r('app.string.audio_route_headset'),
  $r('app.string.audio_route_headset_bluetooth')
]

export const AUDIO_SCENARIO =
  [AudioConfigHelper.AUDIO_SCENARIO_DEFAULT, AudioConfigHelper.AUDIO_SCENARIO_GAME_STREAMING,
    AudioConfigHelper.AUDIO_SCENARIO_CHATROOM]

export const AUDIO_PROFILE = [
  AudioConfigHelper.AUDIO_PROFILE_DEFAULT,
  AudioConfigHelper.AUDIO_PROFILE_SPEECH_STANDARD,
  AudioConfigHelper.AUDIO_PROFILE_MUSIC_STANDARD,
  AudioConfigHelper.AUDIO_PROFILE_MUSIC_STANDARD_STEREO,
  AudioConfigHelper.AUDIO_PROFILE_MUSIC_HIGH_QUALITY,
  AudioConfigHelper.AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO

]

export const CHANNEL_PROFILE = [
  AudioConfigHelper.CHANNEL_PROFILE_COMMUNICATION,
  AudioConfigHelper.CHANNEL_PROFILE_LIVE_BROADCASTING
]