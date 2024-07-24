import { OrientationMode, VideoDimensions } from 'AgoraRtcSdk'
import { ExampleCategory, ExampleItem, SettingsInfo } from '../model/DataType'

export const BASIC_ITEMS: ExampleItem[] =
  [
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
      title: $r('app.string.item_joinmultichannel'),
      url: 'pages/advance/JoinMultiChannel'
    },
    {
      title: $r('app.string.item_mediametadata'),
      url: 'pages/advance/MediaMetadata'
    },
    {
      title: $r('app.string.item_customaudiosource'),
      url: 'pages/advance/CustomAudioSource'
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
    orientationMode: OrientationMode.ORIENTATION_MODE_ADAPTIVE
  }
}