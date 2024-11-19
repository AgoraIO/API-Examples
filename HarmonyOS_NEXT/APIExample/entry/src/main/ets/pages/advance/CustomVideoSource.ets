import {
  ChannelMediaOptions,
  Constants,
  RtcEngine,
  RtcEngineConfig,
  RtcEngineEx,
  VideoBufferType,
  VideoCanvas,
  VideoEncoderConfiguration,
  VideoFrame,
  VideoPixelFormat
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { GlobalInfo } from '../Data';

const TAG: string = 'CustomVideoSource'

@Entry
@Component
struct CustomVideoSource {
  private rtcEngine: RtcEngineEx | undefined = undefined;
  private pushingVideoFrame = false
  private intervalVideoNum = -1;
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
    config.mEventHandler = {};
    config.mEventHandler.onUserJoined = (uid: number, collapse: number) => {
      Logger.info(TAG, "mEventHandler.onUserJoined: " + uid + " , " + collapse);
      if (this.remoteUid == 0) {
        this.remoteUid = uid
      }
    };
    config.mEventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, "mEventHandler.onUserOffline: " + uid + " , " + reason)
      if (this.remoteUid == uid) {
        this.remoteUid = 0
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
      this.remoteUid = 0
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
    }
    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config) as RtcEngineEx;
    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)

    this.rtcEngine.enableVideo()

    this.rtcEngine.setExternalVideoSource(true,false,Constants.ExternalVideoSourceType.VIDEO_FRAME);
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine.stopPreview();
        this.rtcEngine.leaveChannel();
      }
      this.stopPushVideo()
      Logger.info(TAG, "destroy begin")
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done")
      });
      Logger.info(TAG, "destroy end")
      this.rtcEngine = undefined;
    }
  }

  async startPushVideo() {
    let file_name = 'sample.yuv'
    let width = 320
    let height = 180
    let frame_size = width * height / 2 * 3
    let frame_rate = 15
    let frame_interval_ms = 1000 / frame_rate

    getContext(this)
      .resourceManager
      .getRawFileContent(file_name)
      .then((buffer) => {
        this.pushingVideoFrame = true
        let readStart = 0
        this.intervalVideoNum = setInterval(() => {
          if (!this.pushingVideoFrame) {
            return
          }
          if (readStart == buffer.byteLength) {
            readStart = 0
          }
          let frameBuffer: Uint8Array = buffer.slice(readStart, readStart + frame_size)
          readStart += frame_size
          let frame: VideoFrame = new VideoFrame(
            VideoBufferType.RAW_DATA,
            VideoPixelFormat.VIDEO_PIXEL_I420);
          frame.buffer = frameBuffer.buffer as ArrayBuffer;
          frame.rotation = 0;
          frame.yStride = width;
          frame.height = height;
          frame.timestamp = 0;
          let ret = this.rtcEngine?.pushExternalVideoFrame(frame)
          if (!ret) {
            Logger.error(TAG, "pushExternalVideoFrameEx failed: " + ret)
          }
        }, frame_interval_ms)
      })
      .catch((reason: BusinessError) => {
        Logger.error(TAG, "push video called failed : " + reason)
      })
  }

  async stopPushVideo() {
    this.pushingVideoFrame = false
    if (this.intervalVideoNum != -1) {
      clearInterval(this.intervalVideoNum)
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_customvideosource'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
            XComponent({
              id: 'preview_local',
              type: XComponentType.SURFACE,
              libraryname: Constants.AGORA_LIB_NAME,
            }).onLoad(() => {
              let localCanvas = new VideoCanvas("preview_local");
              localCanvas.uid = this.localUid;
              localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
              localCanvas.mirrorMode = VideoCanvas.VIDEO_MIRROR_MODE_DISABLED;
              // TODO set source type
              localCanvas.sourceType = Constants.VideoSourceType.CUSTOM
              this.rtcEngine?.setupLocalVideo(localCanvas);
            }).width('50%')
          }

          if (this.remoteUid != 0) {
            XComponent({
              id: 'preview_remote',
              type: XComponentType.SURFACE,
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

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .type(InputType.Number)
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .flexGrow(1)
          .margin({ right: 6 })

        if (this.isJoin) {
          Button($r('app.string.leave'))
            .id('button_to_leave')
            .onClick(() => {
              this.stopPushVideo()
              this.rtcEngine?.stopPreview(Constants.VideoSourceType.CUSTOM)
              this.rtcEngine?.leaveChannel()
              focusControl.requestFocus("button_to_leave")
            })
            .flexGrow(0)
        } else {
          Button($r('app.string.join'))
            .id('button_to_join')
            .onClick(async () => {
              if (this.channelName == "") {
                ShowToast.shortToast("The channel name is empty!")
                return
              }
              if (!(await PermissionHelper.checkPermissions(
                getContext(this) as common.UIAbilityContext,
                ['ohos.permission.MICROPHONE', 'ohos.permission.CAMERA']
              ))) {
                ShowToast.shortToast("Permission leak!")
                return
              }
              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCustomVideoTrack = true
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption);

              this.startPushVideo()
              this.rtcEngine?.startPreview(Constants.VideoSourceType.CUSTOM)
              focusControl.requestFocus("button_to_join")
            })
            .flexGrow(0)
        }
      }
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}