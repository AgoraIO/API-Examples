import {
  ChannelMediaOptions,
  Constants,
  LocalVideoStats,
  RtcEngine,
  RtcEngineConfig,
  ScreenCaptureParameters,
  VideoCanvas,
  VideoEncoderConfiguration
} from 'AgoraRtcSdk';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import { common, WantAgent, wantAgent } from '@kit.AbilityKit';
import { GlobalInfo } from '../Data';
import { TokenUtils } from '../../util/TokenUtils';
import PermissionHelper from '../../util/PermissionHelper';
import { backgroundTaskManager } from '@kit.BackgroundTasksKit';
import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

const TAG: string = 'ScreenSharing'

@Entry
@Component
struct ScreenSharing {
  private rtcEngine: RtcEngine | undefined = undefined;
  private captureConfig: ScreenCaptureParameters = new ScreenCaptureParameters();
  // 通过getContext方法，来获取page所在的UIAbility上下文。
  private context: Context = getContext(this);
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1
  @State pushingScreenAudio: boolean = true
  @State pushingScreenAudioVolume: number = 100

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.remoteUid == -1) {
        this.remoteUid = uid
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.remoteUid == uid) {
        this.remoteUid = -1
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
      this.localUid = -1
      this.remoteUid = -1
    }
    config.mEventHandler.onLocalVideoStats = (source:Constants.VideoSourceType , stats:LocalVideoStats)=>{
      Logger.info(TAG, "mEventHandler.onLocalVideoStats: source=" + source
        + " stats.captureFrameRate=" + stats.captureFrameRate
        + ", stats.encoderOutputFrameRate=" + stats.encoderOutputFrameRate
        + ", stats.rendererOutputFrameRate=" + stats.rendererOutputFrameRate);
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mEventHandler.onLocalVideoStateChanged = (source: Constants.VideoSourceType, state:Constants.LocalVideoStreamState,error:Constants.LocalVideoStreamReason)=>{
      Logger.info(TAG, "mEventHandler.onLocalVideoStateChanged: source=" + source + ", state " + state + ", error=" + error);
    }
    config.mEventHandler.onPermissionError = (permission:Constants.PermissionType)=>{
      Logger.info(TAG, "mEventHandler.onPermissionError: permission=" + permission);
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);

    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()


    this.rtcEngine.setParameters("{\"che.video.hmos_capture.copy_enable\": true}"); //设置true采集出rgb texture，暂时不可用
    this.rtcEngine.setParameters("{\"rtc.enable_camera_capture_yuv\": true}"); //设置true采集出yuv

    this.rtcEngine.setParameters("{\"engine.video.enable_hw_decoder\": false}"); //设置true走硬解，需要强制设置
    this.rtcEngine.setParameters("{\"rtc.video.decoder_out_byte_frame\": false}"); //设置true走硬解yuv
    this.rtcEngine.setParameters("{\"che.video.hmos_decoder.copy_enable\": false}"); //设置true走硬出rgb texture，暂时不可用

    this.rtcEngine.setParameters("{\"che.video.hmos_encoder.surface_enable\": true}"); //设置编码是否走texture模式
    this.rtcEngine.setParameters("{\"engine.video.enable_hw_encoder\": true}"); //设置true走硬编，需要强制设置

    this.rtcEngine.setParameters("{\"che.video.hmos_screen.surface_enable\": true}"); //设置屏幕共享采集纹理
    this.rtcEngine.setParameters("{\"he.video.hmos_screen.copy_enable\": true}"); //设置屏幕共享采集纹理

  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine.stopPreview();
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

  startContinuousTask() {
    let wantAgentInfo: wantAgent.WantAgentInfo = {
      // 点击通知后，将要执行的动作列表
      // 添加需要被拉起应用的bundleName和abilityName
      wants: [
        {
          bundleName: "io.agora.hmos.apiexample",
          abilityName: "io.agora.hmos.apiexample.EntryAbility"
        }
      ],
      // 指定点击通知栏消息后的动作是拉起ability
      actionType: wantAgent.OperationType.START_ABILITY,
      // 使用者自定义的一个私有值
      requestCode: 0,
      // 点击通知后，动作执行属性
      wantAgentFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };

    // 通过wantAgent模块下getWantAgent方法获取WantAgent对象
    wantAgent.getWantAgent(wantAgentInfo).then((wantAgentObj: WantAgent) => {
      backgroundTaskManager.startBackgroundRunning(this.context,
        backgroundTaskManager.BackgroundMode.AUDIO_RECORDING, wantAgentObj).then(() => {
        Logger.debug(`Succeeded in operationing startBackgroundRunning.`);
      }).catch((err: BusinessError) => {
        Logger.error(`Failed to operation startBackgroundRunning. Code is ${err.code}, message is ${err.message}`);
      });
    });
  }

  stopContinuousTask() {
    backgroundTaskManager.stopBackgroundRunning(this.context).then(() => {
      Logger.info(`Succeeded in operationing stopBackgroundRunning.`);
    }).catch((err: BusinessError) => {
      Logger.error(`Failed to operation stopBackgroundRunning. Code is ${err.code}, message is ${err.message}`);
    });
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_screensharing'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != -1) {
            XComponent({
              id: 'preview_local',
              type: 'surface',
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {
              let localCanvas = new VideoCanvas("preview_local");
              localCanvas.uid = this.localUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = 0;
              localCanvas.sourceType = Constants.VideoSourceType.SCREEN_PRIMARY
              this.rtcEngine?.setupLocalVideo(localCanvas);
            }).width('50%')
          }

          if (this.remoteUid != -1) {
            XComponent({
              id: 'preview_remote',
              type: 'surface',
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {
              let localCanvas = new VideoCanvas("preview_remote");
              localCanvas.uid = this.remoteUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = 0;
              this.rtcEngine?.setupRemoteVideo(localCanvas);
            }).width('50%')
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
        Text($r('app.string.publish_screen_audio'))
          .flexGrow(1)

        Toggle({ type: ToggleType.Switch, isOn: this.pushingScreenAudio })
          .onChange((isOn) => {
            this.pushingScreenAudio = isOn

            let option = this.captureConfig
            option.captureAudio = isOn
            option.audioParams.sampleRate = 16000;
            option.audioParams.channels = 2;
            this.rtcEngine?.updateScreenCaptureParameters(option)
          })
          .flexGrow(0)
      }
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })
      .flexGrow(0)

      Row() {
        Text($r('app.string.publish_audio_volume'))
        Slider({
          value: this.pushingScreenAudioVolume,
          min: 0,
          max: 100,
          style: SliderStyle.OutSet
        }).enabled(this.pushingScreenAudio)
          .onChange((value: number) => {
            this.pushingScreenAudioVolume = value
            let option = this.captureConfig
            option.audioParams.captureSignalVolume = value
            this.rtcEngine?.updateScreenCaptureParameters(option)
          }).layoutWeight(1)
      }
      .alignItems(VerticalAlign.Center)
      .padding({
        left: 12,
        right: 12,
        top: 6,
        bottom: 6
      })
      .flexGrow(0)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .flexGrow(1)
          .margin({ right: 6 })

        Button(this.isJoin ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_to_join')
          .onClick(async () => {
            focusControl.requestFocus("button_to_join")
            if (this.isJoin) {
              this.rtcEngine?.stopScreenCapture()
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
              this.stopContinuousTask();
            } else {
              if (this.channelName == "") {
                ShowToast.shortToast("The channel name is empty!")
                return
              }

              let captureConfig: ScreenCaptureParameters = this.captureConfig;
              captureConfig.videoParams.dimensions.width = 1280
              captureConfig.videoParams.dimensions.height = 720
              captureConfig.captureAudio = this.pushingScreenAudio
              captureConfig.audioParams.sampleRate = 44100
              captureConfig.audioParams.channels = 2
              captureConfig.audioParams.captureSignalVolume = this.pushingScreenAudioVolume
              this.rtcEngine?.startScreenCapture(captureConfig)
              this.rtcEngine?.startPreview(Constants.VideoSourceType.SCREEN_PRIMARY)
              this.localUid = 0

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishScreenCaptureVideo = true;
              mediaOption.publishScreenCaptureAudio = true;
              mediaOption.publishMicrophoneTrack = false;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " + RtcEngine.getErrorDescription(ret))
                return
              }

              this.startContinuousTask()
            }
          })
          .flexGrow(0)
      }
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