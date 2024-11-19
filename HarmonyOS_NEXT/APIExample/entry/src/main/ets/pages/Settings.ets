import { Constants, OrientationMode, RtcEngine, VideoDimensions } from '@shengwang/rtc-full'
import { TitleBar } from '../common/TitleBar'
import { CommonItemSelect } from '../common/Widgets'
import { GlobalInfo } from './Data'


@Entry
@Component
struct Settings {

  private dimensionList: VideoDimensions[] = [
    { width: 640, height: 360 },
    { width: 960, height: 540 },
    { width: 1280, height: 720 },
    { width: 1920, height: 1080 },
  ]
  private dimensionStrList: ResourceStr[] = []
  private dimensionSelectedIndex = 0
  private frameRateList: number[] = [
    15, 24, 30, 60
  ]
  private frameRateStrList: ResourceStr[] = []
  private frameRateSelectedIndex = 0
  private orientationModeList: OrientationMode[] = [
    OrientationMode.ORIENTATION_MODE_ADAPTIVE,
    OrientationMode.ORIENTATION_MODE_FIXED_LANDSCAPE,
    OrientationMode.ORIENTATION_MODE_FIXED_PORTRAIT,
  ]
  private orientationModeStrList: ResourceStr[] = [
    "ADAPTIVE", "FIXED_LANDSCAPE", "FIXED_PORTRAIT"
  ]
  private orientationModeSelectedIndex = 0
  private areaCodeList: Constants.AreaCode[] = [
    Constants.AreaCode.GLOB,
    Constants.AreaCode.CN,
    Constants.AreaCode.NA,
    Constants.AreaCode.EU,
    Constants.AreaCode.AS,
    Constants.AreaCode.JP,
    Constants.AreaCode.IN
  ]
  private areaCodeSelectedIndex = 0

  aboutToAppear(): void {
    for (let i = 0; i < this.dimensionList.length; i++) {
      this.dimensionStrList[i] = "VD_" + this.dimensionList[i].width + "_" + this.dimensionList[i].height
      if (GlobalInfo.settings.dimensions.width == this.dimensionList[i].width &&
        GlobalInfo.settings.dimensions.height == this.dimensionList[i].height) {
        this.dimensionSelectedIndex = i
      }
    }
    for (let i = 0; i < this.frameRateList.length; i++) {
      this.frameRateStrList[i] = "FPS_" + this.frameRateList[i]
      if (GlobalInfo.settings.frameRate == this.frameRateList[i]) {
        this.frameRateSelectedIndex = i
      }
    }
    for (let i = 0; i < this.orientationModeList.length; i++) {
      if (GlobalInfo.settings.orientationMode == this.orientationModeList[i]) {
        this.orientationModeSelectedIndex = i
      }
    }
    for (let i = 0; i < this.areaCodeList.length; i++) {
      if (GlobalInfo.settings.areaCode == this.areaCodeList[i]) {
        this.areaCodeSelectedIndex = i
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.settings'),
        showBack: true
      }).flexGrow(0)

      Column() {

        CommonItemSelect({
          name: $r("app.string.resolution"),
          selects: this.dimensionStrList,
          selectIndex: this.dimensionSelectedIndex,
          callback: (index: number, value?: string) => {
            GlobalInfo.settings.dimensions = this.dimensionList[index]
          }
        }).margin({ left: 26, right: 26 })

        Divider().margin({ left: 20, right: 20 })

        CommonItemSelect({
          name: $r("app.string.framerate"),
          selects: this.frameRateStrList,
          selectIndex: this.frameRateSelectedIndex,
          callback: (index: number, value?: string) => {
            GlobalInfo.settings.frameRate = this.frameRateList[index]
          }
        }).margin({ left: 26, right: 26 })

        Divider().margin({ left: 20, right: 20 })

        CommonItemSelect({
          name: $r("app.string.orientation"),
          selects: this.orientationModeStrList,
          selectIndex: this.orientationModeSelectedIndex,
          callback: (index: number, value?: string) => {
            GlobalInfo.settings.orientationMode = this.orientationModeList[index]
          }
        }).margin({ left: 26, right: 26 })

        Divider().margin({ left: 20, right: 20 })

        CommonItemSelect({
          name: $r("app.string.area"),
          selects: this.getAreaCodeKeys(),
          selectIndex: this.areaCodeSelectedIndex,
          callback: (index: number, value?: string) => {
            GlobalInfo.settings.areaCode = this.areaCodeList[index]
          }
        }).margin({ left: 26, right: 26 })

        Divider().margin({ left: 20, right: 20 })

        Text("SDK Version:" + RtcEngine.getSdkVersion())
          .width('100%')
          .textAlign(TextAlign.End)
          .padding({top: 12, right: 26})
      }
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }

  private getAreaCodeKeys(): string[] {
    const allKeys = Object.keys(Constants.AreaCode);
    const allValues = Object.values(Constants.AreaCode);

    // Filter matching keys based on the values in areaCodeList
    return this.areaCodeList.map(areaCodeValue => {
      const index = allValues.indexOf(areaCodeValue);
      return allKeys[index] || '';
    });
  }
}