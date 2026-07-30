import {
  CameraCapturerConfiguration,
  ChannelMediaOptions,
  Constants,
  IMediaPlayer,
  IMediaPlayerObserver,
  LocalTranscoderConfiguration,
  MediaPlayerSource,
  RtcEngine,
  RtcEngineConfig,
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
const MEDIA_URL: string = 'https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4'

@Entry
@Component
struct LocalVideoTranscoding {
  private rtcEngine: RtcEngine | undefined = undefined;
  private mediaPlayer: IMediaPlayer | undefined = undefined;
  private mediaPlayerObserver: IMediaPlayerObserver = {
    onPlayerStateChanged: (state: Constants.MediaPlayerState, reason: Constants.MediaPlayerReason) => {
      Logger.info(TAG, "onPlayerStateChanged state: " + state + " reason: " + reason)
      if (state == Constants.MediaPlayerState.FAILED) {
        ShowToast.longToast("Media player failed: " + reason)
      }
    }
  }
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State isOperating: boolean = false
  @State localUid: number = -1
  @State remoteUid: number = -1
  private isTranscodingStarted: boolean = false

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
      this.isOperating = false
      this.localUid = uid
    }
    config.mEventHandler.onLeaveChannel = () => {
      Logger.info(TAG, "mEventHandler.onLeaveChannel");
      this.isJoin = false
      this.isOperating = false
      this.localUid = -1
      this.remoteUid = -1
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.info(TAG, "mEventHandler.onError: " + err + " message " + message);
      this.isOperating = false
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

    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()

    this.mediaPlayer = this.rtcEngine.createMediaPlayer()
    if (this.mediaPlayer == undefined) {
      ShowToast.longToast("createMediaPlayer failed")
      return
    }
    this.mediaPlayer.registerPlayerObserver(this.mediaPlayerObserver)
    this.mediaPlayer.setLoopCount(-1)
    this.mediaPlayer.adjustPlayoutVolume(0)

    let mediaSource = new MediaPlayerSource()
    mediaSource.url = MEDIA_URL
    mediaSource.autoPlay = false
    let ret = this.mediaPlayer.openWithMediaSource(mediaSource)
    if (ret != Constants.ErrorCode.ERR_OK) {
      ShowToast.longToast("openWithMediaSource error " + ret)
    }
  }

  aboutToDisappear(): void {
    if (this.rtcEngine != undefined) {
      this.stopTranscoding()
      if (this.isJoin) {
        this.rtcEngine.leaveChannel();
      }
      this.mediaPlayer?.stop()
      this.mediaPlayer?.unRegisterPlayerObserver()
      this.mediaPlayer?.destroy()
      this.mediaPlayer = undefined
      Logger.info(TAG, "destroy begin")
      RtcEngine.destroy().then(() => {
        Logger.info(TAG, "destroy done")
      });
      Logger.info(TAG, "destroy end")
      this.rtcEngine = undefined;
    }
  }

  private stopTranscoding(): void {
    this.rtcEngine?.stopPreview(Constants.VideoSourceType.TRANSCODED)
    if (this.isTranscodingStarted) {
      this.rtcEngine?.stopLocalVideoTranscoder()
      this.rtcEngine?.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
      this.mediaPlayer?.pause()
      this.isTranscodingStarted = false
    }
  }

  private startTranscoding(): boolean {
    if (this.rtcEngine == undefined || this.mediaPlayer == undefined) {
      ShowToast.longToast("RTC engine or media player is not ready")
      return false
    }

    if (!this.isTranscodingStarted) {
      let width = GlobalInfo.settings.dimensions.width
      let height = GlobalInfo.settings.dimensions.height

      let cameraConfig = new CameraCapturerConfiguration()
      let cameraRet = this.rtcEngine.startCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY, cameraConfig)
      if (cameraRet != Constants.ErrorCode.ERR_OK) {
        ShowToast.longToast("startCameraCapture error " + cameraRet)
        return false
      }

      let config = new LocalTranscoderConfiguration()
      config.transcodingVideoStreams = new Array(2)
      config.videoOutputConfiguration.dimensions = new VideoDimensions(width,height)
      config.transcodingVideoStreams[0] = new TranscodingVideoStream()
      config.transcodingVideoStreams[0].sourceType = Constants.VideoSourceType.MEDIA_PLAYER
      config.transcodingVideoStreams[0].mediaPlayerId = this.mediaPlayer.getMediaPlayerId()
      config.transcodingVideoStreams[0].width = width
      config.transcodingVideoStreams[0].height = height
      config.transcodingVideoStreams[0].x = 0
      config.transcodingVideoStreams[0].y = 0
      config.transcodingVideoStreams[0].zOrder = 1

      config.transcodingVideoStreams[1] = new TranscodingVideoStream()
      config.transcodingVideoStreams[1].sourceType = Constants.VideoSourceType.CAMERA_PRIMARY
      config.transcodingVideoStreams[1].width = width/2
      config.transcodingVideoStreams[1].height = height/2
      config.transcodingVideoStreams[1].x = 0
      config.transcodingVideoStreams[1].y = height / 2
      config.transcodingVideoStreams[1].zOrder = 2
      config.transcodingVideoStreams[1].mirror = true

      let playerRet = this.mediaPlayer.play()
      if (playerRet != Constants.ErrorCode.ERR_OK) {
        this.rtcEngine.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
        ShowToast.longToast("mediaPlayer.play error " + playerRet)
        return false
      }

      let transcoderRet = this.rtcEngine.startLocalVideoTranscoder(config)
      if (transcoderRet != Constants.ErrorCode.ERR_OK) {
        this.mediaPlayer.pause()
        this.rtcEngine.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
        ShowToast.longToast("startLocalVideoTranscoder error " + transcoderRet)
        return false
      }

      let previewRet = this.rtcEngine.startPreview(Constants.VideoSourceType.TRANSCODED)
      if (previewRet != Constants.ErrorCode.ERR_OK) {
        this.rtcEngine.stopLocalVideoTranscoder()
        this.rtcEngine.stopCameraCapture(Constants.VideoSourceType.CAMERA_PRIMARY)
        this.mediaPlayer.pause()
        ShowToast.longToast("startPreview error " + previewRet)
        return false
      }
      this.isTranscodingStarted = true
    }
    return true
  }

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_localvideotranscoding'),
        showBack: true
      })

      Stack({ alignContent: Alignment.TopEnd }) {
        XComponent({
          id: 'preview_local',
          type: XComponentType.SURFACE,
          libraryname: Constants.AGORA_LIB_NAME,
        }).onLoad(() => {
          let localCanvas = new VideoCanvas("preview_local");
          localCanvas.uid = 0;
          localCanvas.renderMode = VideoCanvas.RENDER_MODE_FIT;
          localCanvas.mirrorMode = VideoCanvas.VIDEO_MIRROR_MODE_DISABLED;
          localCanvas.sourceType = Constants.VideoSourceType.TRANSCODED
          this.rtcEngine?.setupLocalVideo(localCanvas);
        }).width('100%').height('100%')

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

        if (!this.isJoin) {
          Column() {
          }
          .width('100%')
          .height('100%')
          .backgroundColor($r('app.color.background_shallow_grey'))
        }
      }
      .layoutWeight(1)

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .enabled(!this.isJoin && !this.isOperating)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .layoutWeight(1)
          .margin({ right: 6 })

        Button(this.isJoin ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_to_join')
          .enabled(!this.isOperating)
          .onClick(async () => {
            focusControl.requestFocus("button_to_join")
            if (this.isOperating) {
              return
            }
            if (!this.isJoin) {
              if (this.channelName == "") {
                ShowToast.shortToast("The channel name is empty!")
                return
              }
              this.isOperating = true
              if (!(await PermissionHelper.checkPermissions(
                getContext(this) as common.UIAbilityContext,
                ['ohos.permission.MICROPHONE', 'ohos.permission.CAMERA']
              ))) {
                this.isOperating = false
                ShowToast.shortToast("Permission leak!")
                return
              }

              let rtcEngine = this.rtcEngine
              if (rtcEngine == undefined) {
                this.isOperating = false
                ShowToast.longToast("RTC engine is not ready")
                return
              }
              if (!this.startTranscoding()) {
                this.isOperating = false
                return
              }

              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = false;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.publishTranscodedVideoTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let token = ""
              let ret = rtcEngine.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                this.isOperating = false
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " +
                RtcEngine.getErrorDescription(ret))
                return
              }
            } else {
              this.isOperating = true
              let rtcEngine = this.rtcEngine
              if (rtcEngine == undefined) {
                this.isOperating = false
                ShowToast.longToast("RTC engine is not ready")
                return
              }
              let ret = rtcEngine.leaveChannel()
              if (ret != Constants.ErrorCode.ERR_OK) {
                this.isOperating = false
                ShowToast.longToast("leaveChannel error " + ret)
              }
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
