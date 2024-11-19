import { Constants, OrientationMode, VideoDimensions } from '@shengwang/rtc-full'
import { ExampleCategory, ExampleItem, SettingsInfo } from '../model/DataType'

export const BASIC_ITEMS: ExampleItem[] =
  [
    {
      title: $r('app.string.item_joinvideochanneltoken'),
      url: 'pages/basic/JoinVideoChannelToken'
    },
    {
      title: $r('app.string.item_joinvideochannel'),
      url: 'pages/basic/JoinVideoChannel'
    },
    {
      title: $r('app.string.item_joinaudiochannel'),
      url: 'pages/basic/JoinAudioChannel'
    }
  ]

export const ADVANCE_ITEMS: ExampleItem[] =
  [
    {
      title: $r('app.string.item_livestreaming'),
      url: 'pages/advance/LiveStreaming'
    },
    {
      title: $r('app.string.item_rtmpstreaming'),
      url: 'pages/advance/RTMPStreaming'
    },
    {
      title: $r('app.string.item_joinmultichannel'),
      url: 'pages/advance/JoinMultiChannel'
    },
    {
      title: $r('app.string.item_mediametadata'),
      url: 'pages/advance/MediaMetadataWrap'
    },
    {
      title: $r('app.string.item_customaudiosource'),
      url: 'pages/advance/CustomAudioSource'
    },
    {
      title: $r('app.string.item_customaudiorender'),
      url: 'pages/advance/CustomAudioRenderWrap'
    },
    {
      title: $r('app.string.item_originaudiosource'),
      url: 'pages/advance/OriginAudioDataWrap'
    },
    {
      title: $r('app.string.item_customvideosource'),
      url: 'pages/advance/CustomVideoSource'
    },
    {
      title: $r('app.string.item_originvideodata'),
      url: 'pages/advance/OriginVideoDataWrap'
    },
    {
      title: $r('app.string.item_channelencryption'),
      url: 'pages/advance/ChannelEncryption'
    },
    {
      title: $r('app.string.item_screensharing'),
      url: 'pages/advance/ScreenSharing'
    },
    {
      title: $r('app.string.item_playaudiofiles'),
      url: 'pages/advance/PlayAudioFiles'
    },
    {
      title: $r('app.string.item_precalltest'),
      url: 'pages/advance/PreCallTest'
    },
    {
      title: $r('app.string.item_mediarecorder'),
      url: 'pages/advance/MediaRecorder'
    },
    {
      title: $r('app.string.item_localvideotranscoding'),
      url: 'pages/advance/LocalVideoTranscoding'
    },
    {
      title: $r('app.string.item_hostcrosschannel'),
      url: 'pages/advance/HostCrossChannel'
    },
  ]

export const COLLECTION_CATEGORIES: ExampleCategory[] =
  [
    {
      childNodes: BASIC_ITEMS,
      title: $r('app.string.category_basic')
    },
    {
      childNodes: ADVANCE_ITEMS,
      title: $r("app.string.category_advanced")
    },
  ]

export class GlobalInfo {
  static settings: SettingsInfo = {
    dimensions: new VideoDimensions(960, 540),
    frameRate: 15,
    orientationMode: OrientationMode.ORIENTATION_MODE_ADAPTIVE,
    areaCode: Constants.AreaCode.GLOB
  }
}