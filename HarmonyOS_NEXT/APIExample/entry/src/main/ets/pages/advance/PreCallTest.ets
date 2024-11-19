import {
  Constants,
  LastmileProbeConfig,
  LastmileProbeResult,
  RtcEngine,
  RtcEngineConfig,
  VideoCanvas
} from '@shengwang/rtc-full';
import { AppID } from '../../common/KeyCenter';
import Logger from '../../util/Logger';
import { common } from '@kit.AbilityKit';
import ShowToast from '../../util/ShowToast';
import { TitleBar } from '../../common/TitleBar';
import { GlobalInfo } from '../Data';

const TAG: string = 'PlayAudioFiles'

@Entry
@Component
struct PlayAudioFiles {
  private rtcEngine: RtcEngine | undefined = undefined
  private lastmileResult: Map<String, String> = new Map
  private audioEchoCountDown: number | undefined = undefined
  @State isLastmileTesting: boolean = false
  @State lastmileResultText: string = ''
  @State isAudioEchoTesting: boolean = false
  @State isVideoEchoTesting: boolean = false
  @State audioEchoTestingText: string = ''

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mEventHandler.onLastmileProbeResult = (result: LastmileProbeResult) => {
      let map = new Map<String, String>()
      map.set("Rtt", result.rtt + 'ms')
      map.set("DownlinkAvailableBandwidth", result.downlinkReport.availableBandwidth + 'Kbps')
      map.set("DownlinkJitter", result.downlinkReport.jitter + 'ms')
      map.set("DownlinkLoss", result.downlinkReport.packetLossRate + '%')
      map.set("UplinkAvailableBandwidth", result.uplinkReport.availableBandwidth + 'Kbps')
      map.set("UplinkJitter", result.uplinkReport.jitter + 'ms')
      map.set("UplinkLoss", result.uplinkReport.packetLossRate + '%')
      this.updateLastmileResult(map)
      this.isLastmileTesting = false
    }
    config.mEventHandler.onLastmileQuality = (quality: Constants.QualityType) => {
      let map = new Map<String, String>()
      let qualityStr = ''
      switch (quality) {
        case Constants.QualityType.EXCELLENT:
          qualityStr = 'EXCELLENT';
          break;
        case Constants.QualityType.GOOD:
          qualityStr = 'GOOD';
          break;
        case Constants.QualityType.POOR:
          qualityStr = 'POOR';
          break;
        case Constants.QualityType.BAD:
          qualityStr = 'BAD';
          break;
        case Constants.QualityType.VBAD:
          qualityStr = 'VBAD';
          break;
        case Constants.QualityType.DOWN:
          qualityStr = 'DOWN';
          break;
        case Constants.QualityType.DETECTING:
          qualityStr = 'DETECTING';
          break;
        default:
          qualityStr = 'UNKNOWN';
      }
      map.set("Quality", qualityStr)
      this.updateLastmileResult(map)
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);

    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    if (this.audioEchoCountDown != undefined) {
      clearInterval(this.audioEchoCountDown)
      this.audioEchoCountDown = undefined
    }
    if (this.rtcEngine != undefined) {
      Logger.info(TAG, "destroy begin")
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done")
      });
      Logger.info(TAG, "destroy end")
      this.rtcEngine = undefined;
    }
  }

  updateLastmileResult(result: Map<String, String>) {
    result.forEach((value, key) => {
      this.lastmileResult.set(key, value)
    })
    let text = ''
    this.lastmileResult.forEach((value, key) => {
      text += key + " : " + value + "\n"
    })
    this.lastmileResultText = text
  }

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_precalltest'),
        showBack: true
      })

      Column() {
        Text('Lastmile Network Pretest').padding({ top: 6, bottom: 6 })
        Divider()
        Button(this.isLastmileTesting ? $r('app.string.testing') : $r('app.string.start'))
          .margin({ top: 6, bottom: 6 })
          .enabled(!this.isLastmileTesting)
          .onClick(() => {
            // Configure a LastmileProbeConfig instance.
            let config = new LastmileProbeConfig()
            // Probe the uplink network quality.
            config.probeUplink = true
            // Probe the downlink network quality.
            config.probeDownlink = true
            // The expected uplink bitrate (bps). The value range is [100000, 5000000].
            config.expectedUplinkBitrate = 100000
            // The expected downlink bitrate (bps). The value range is [100000, 5000000].
            config.expectedDownlinkBitrate = 100000
            // Start the last-mile network test before joining the channel.
            this.rtcEngine?.startLastmileProbeTest(config)
            this.isLastmileTesting = true
          })
        Text(this.lastmileResultText).padding({ top: 6, bottom: 6 })
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })

      //todo
      if (false) {
        Column() {
          Text('Audio Echo Pretest').padding({ top: 6, bottom: 6 })
          Divider()
          Button(this.isAudioEchoTesting ? this.audioEchoTestingText : $r('app.string.start'))
            .margin({ top: 6, bottom: 6 })
            .enabled(!this.isAudioEchoTesting && !this.isVideoEchoTesting)
            .onClick(() => {

              // TODO
              // this.rtcEngine?.startEchoTest()
              let countDownCount = 10
              let revert = false
              let countDownIndex = countDownCount
              this.audioEchoTestingText = 'Recording(' + countDownIndex + 's)'
              this.isAudioEchoTesting = true
              this.audioEchoCountDown = setInterval(() => {
                if (!revert) {
                  if (countDownIndex > 0) {
                    this.audioEchoTestingText = 'Recording(' + countDownIndex + 's)'
                    countDownIndex--
                  } else {
                    revert = true
                  }
                }
                if (revert) {
                  if (countDownIndex < countDownCount) {
                    countDownIndex++
                    this.audioEchoTestingText = 'Playing(' + countDownIndex + 's)'
                  } else {
                    clearInterval(this.audioEchoCountDown)
                    this.audioEchoCountDown = undefined
                    this.isAudioEchoTesting = false
                  }
                }
              }, 1000)
            })
          Text('Video Echo Pretest').padding({ top: 6, bottom: 6 })
          Divider()
          Button($r(this.isVideoEchoTesting ? 'app.string.stop' : 'app.string.start'))
            .margin({ top: 6, bottom: 12 })
            .enabled(!this.isAudioEchoTesting)
            .onClick(() => {
              this.isVideoEchoTesting = !this.isVideoEchoTesting
              // TODO
              // this.rtcEngine?.startEchoTest()
              if (this.isVideoEchoTesting) {
                this.rtcEngine?.startPreview()
              } else {
                this.rtcEngine?.stopPreview()
              }
            })
          XComponent({
            id: 'preview_local',
            type: XComponentType.SURFACE,
            libraryname: Constants.AGORA_LIB_NAME,
          }).onLoad(() => {
            let localCanvas = new VideoCanvas("preview_local");
            localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
            this.rtcEngine?.setupLocalVideo(localCanvas);
          }).width(150).height(150)
        }.alignItems(HorizontalAlign.Start)
        .layoutWeight(1.3)
        .padding({
          left: 12,
          right: 12,
          top: 6,
          bottom: 6
        })
      }
    }


    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}