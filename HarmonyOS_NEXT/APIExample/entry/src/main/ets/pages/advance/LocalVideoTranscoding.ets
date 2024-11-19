import {
  CameraCapturerConfiguration,
  ChannelMediaOptions,
  Constants,
  LocalTranscoderConfiguration,
  RtcEngine,
  RtcEngineConfig,
  ScreenCaptureParameters,
  TranscodingVideoStream,
  VideoCanvas,
  VideoDimensions,
  VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { GlobalInfo } from '../Data';
import { JSON } from '@kit.ArkTS';

const TAG: string = 'LocalVideoTranscoding'

@Entry
@Component
struct LocalVideoTranscoding {
  private rtcEngine: RtcEngine | undefined = undefined;
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
    config.areaCode = GlobalInfo.settings.areaCode;
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
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      ShowToast.longToast("onError " + err + " : " + message)
    }
    config.mEventHandler.onLocalVideoTranscoderError = (stream:TranscodingVideoStream,  error:Constants.VideoTranscoderError)  => {
      Logger.info(TAG, "mEventHandler.onLocalVideoTranscoderError stream "+JSON.stringify(stream) + " error: "+error);
      ShowToast.longToast("onLocalVideoTranscoderError " + error )
    }

    config.mEventHandler.onTranscodingUpdated = () => {
      Logger.info(TAG, "mEventHandler.onTranscodingUpdated");
    }

    config.mContext = context;
    Logger.info(TAG, "in thread create engine begin: ");
    this.rtcEngine = RtcEngine.create(config);

    this.rtcEngine.setParameters("{\"che.video.hmos_capture.copy_enable\": true}"); //设置true采集出rgb texture，暂时不可用
    this.rtcEngine.setParameters("{\"rtc.enable_camera_capture_yuv\": true}"); //设置true采集出yuv

    this.rtcEngine.setParameters("{\"engine.video.enable_hw_decoder\": true}"); //设置true走硬解，需要强制设置
    this.rtcEngine.setParameters("{\"rtc.video.decoder_out_byte_frame\": true}"); //设置true走硬解yuv
    this.rtcEngine.setParameters("{\"che.video.hmos_decoder.copy_enable\": false}"); //设置true走硬出rgb texture，暂时不可用

    this.rtcEngine.setParameters("{\"che.video.hmos_encoder.surface_enable\": true}"); //设置编码是否走texture模式
    this.rtcEngine.setParameters("{\"engine.video.enable_hw_encoder\": true}"); //设置true走硬编，需要强制设置

    this.rtcEngine.setParameters("{\"che.video.hmos_screen.surface_enable\": true}"); //设置屏幕共享采集纹理
    this.rtcEngine.setParameters("{\"he.video.hmos_screen.copy_enable\": true}"); //设置屏幕共享采集纹理

    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      if (this.isJoin) {
        this.rtcEngine?.stopLocalVideoTranscoder()
        this.rtcEngine?.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
        this.rtcEngine?.stopScreenCapture()
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

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_localvideotranscoding'),
        showBack: true
      })

      Stack({ alignContent: Alignment.TopEnd }) {
        if (this.localUid != -1) {
          XComponent({
            id: 'preview_local',
            type: XComponentType.SURFACE,
            libraryname: Constants.AGORA_LIB_NAME,
          }).onLoad(() => {
            let localCanvas = new VideoCanvas("preview_local");
            localCanvas.uid = this.localUid;
            localCanvas.renderMode = VideoCanvas.RENDER_MODE_FIT;
            localCanvas.mirrorMode = VideoCanvas.VIDEO_MIRROR_MODE_DISABLED;
            localCanvas.sourceType = Constants.VideoSourceType.TRANSCODED
            this.rtcEngine?.setupLocalVideo(localCanvas);
          }).width('100%').height('100%')
        }

        if (this.remoteUid != -1) {
          XComponent({
            id: 'preview_remote',
            type: XComponentType.SURFACE,
            libraryname: Constants.AGORA_LIB_NAME,
          }).onLoad(() => {
            let localCanvas = new VideoCanvas("preview_remote");
            localCanvas.uid = this.remoteUid;
            localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
            localCanvas.mirrorMode = 0;
            localCanvas.sourceType = Constants.VideoSourceType.TRANSCODED;
            this.rtcEngine?.setupRemoteVideo(localCanvas);
          }).width('30%').height('30%').margin({ top: 12, right: 12 })
        }
      }
      .layoutWeight(1)

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
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
            focusControl.requestFocus("button_to_join")
            if (!this.isJoin) {
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


              let width = GlobalInfo.settings.dimensions.height
              let height = GlobalInfo.settings.dimensions.width

              let screenConfig = new ScreenCaptureParameters()
              screenConfig.videoParams.dimensions = new VideoDimensions(width,height)
              this.rtcEngine?.startScreenCapture(screenConfig)

              let cameraConfig = new CameraCapturerConfiguration()
              this.rtcEngine?.startCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY, cameraConfig)

              let config = new LocalTranscoderConfiguration()
              config.transcodingVideoStreams = new Array(2)
              config.videoOutputConfiguration.dimensions = new VideoDimensions(width,height)
              config.transcodingVideoStreams[0]  = new TranscodingVideoStream();
              config.transcodingVideoStreams[0].sourceType = Constants.VideoSourceType.SCREEN_PRIMARY
              config.transcodingVideoStreams[0].width = width
              config.transcodingVideoStreams[0].height = height
              config.transcodingVideoStreams[0].x = 0
              config.transcodingVideoStreams[0].y = 0
              config.transcodingVideoStreams[0].zOrder = 1

              config.transcodingVideoStreams[1]  = new TranscodingVideoStream()
              config.transcodingVideoStreams[1].sourceType = Constants.VideoSourceType.CAMERA_PRIMARY
              config.transcodingVideoStreams[1].width = width/2
              config.transcodingVideoStreams[1].height = height/2
              config.transcodingVideoStreams[1].x = 0
              config.transcodingVideoStreams[1].y = height / 2
              config.transcodingVideoStreams[1].zOrder = 2
              config.transcodingVideoStreams[1].mirror = true

              this.rtcEngine?.startLocalVideoTranscoder(config)

              this.rtcEngine?.startPreview(Constants.VideoSourceType.TRANSCODED)

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.publishTranscodedVideoTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let token = ""
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " +
                RtcEngine.getErrorDescription(ret))
                return
              }
            } else {
              this.rtcEngine?.stopLocalVideoTranscoder()
              this.rtcEngine?.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
              this.rtcEngine?.stopScreenCapture()
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            }
          })
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