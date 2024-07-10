import { OrientationMode, VideoDimensions } from 'AgoraRtcSdk'

export interface ExampleCategory{
  title: Resource,
  childNodes: ExampleItem[]
}

export interface ExampleItem{
  title: Resource,
  url: string
}

export interface SettingsInfo {
  dimensions: VideoDimensions,
  frameRate: number,
  orientationMode: OrientationMode
}