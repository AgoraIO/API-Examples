import {
  AgoraMediaRecorder,
  ChannelMediaOptions,
  Constants,
  IMediaRecorderCallback,
  MediaRecorderConfiguration,
  RecorderInfo,
  RecorderStreamInfo,
  RtcEngine,
  RtcEngineConfig,
  VideoCanvas,
  VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { GlobalInfo } from '../Data';
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'MediaRecorder'

@Entry
@Component
struct MediaRecorder {
  private rtcEngine: RtcEngine | undefined = undefined;
  private localRecorder: AgoraMediaRecorder | undefined = undefined
  private remoteRecorder: AgoraMediaRecorder | undefined = undefined
  @State message: string = 'Hello World';
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State localRecording: boolean = false
  @State remoteRecording: boolean = false

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
      ShowToast.longToast("onError " + err + " : " + message)
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
  }

  aboutToDisappear(): void {
    if(this.localRecorder != undefined){
      this.localRecorder?.release()
      this.localRecorder = undefined
    }
    if(this.remoteRecorder != undefined){
      this.remoteRecorder?.release()
      this.remoteRecorder = undefined
    }
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

  build() {
    Column() {
      TitleBar({
        title: $r('app.string.item_mediarecorder'),
        showBack: true
      })

      Column() {
        Row() {

          if (this.localUid != 0) {
            Stack() {
              XComponent({
                id: 'preview_local',
                type: XComponentType.SURFACE,
                libraryname: Constants.AGORA_LIB_NAME,
              }).onLoad(() => {
                let localCanvas = new VideoCanvas("preview_local");
                localCanvas.uid = this.localUid;
                localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
                localCanvas.mirrorMode = 0;
                this.rtcEngine?.setupLocalVideo(localCanvas);
              })

              Button(this.localRecording ? $r('app.string.stop_recording') : $r('app.string.start_recording'))
                .onClick(() => {
                  this.localRecording = !this.localRecording
                  let virtualPath = '/data/storage/el2/base/haps/entry/files/media_recorder_local.mp4'
                  let realPath =
                    '/data/app/el2/100/base/io.agora.hmos.apiexample/haps/entry/files/media_recorder_local.mp4'
                  if (!this.localRecording) {
                    this.localRecorder?.stopRecording()
                    this.localRecorder?.release()
                    this.localRecorder = undefined
                    AlertDialog.show(
                      {
                        title: 'MediaFilePath',
                        message: realPath,
                        confirm: {
                          value: $r('app.string.confirm'),
                          backgroundColor: Color.White,
                          action: () => {

                          }
                        },
                        alignment: DialogAlignment.Center
                      }
                    )
                  } else {
                    if (this.localRecorder != undefined) {
                      this.localRecorder?.release()
                    }
                    let info = new RecorderStreamInfo()
                    info.uid = this.localUid
                    info.channelId = this.channelName
                    this.localRecorder = this.rtcEngine?.createMediaRecorder(info)
                    let config = new MediaRecorderConfiguration(
                      virtualPath,
                      AgoraMediaRecorder.CONTAINER_MP4,
                      AgoraMediaRecorder.STREAM_TYPE_BOTH,
                      120000,
                      0
                    )
                    let observer: IMediaRecorderCallback = {
                      onRecorderStateChanged: (channelId: string, uid: number, state: number, reason: number): void => {
                        Logger.info('onRecorderStateChanged channelId=' + channelId + ', uid=' + uid + ', state=' +
                          state + ', reason=' + reason)
                      },
                      onRecorderInfoUpdated: (channelId: string, uid: number, info: RecorderInfo): void => {
                        Logger.info('onRecorderInfoUpdated channelId=' + channelId + ', uid=' + uid + ', filename=' +
                        info.fileName + ', durationMs=' + info.durationMs + ', fileSize=' + info.fileSize)
                      }
                    }
                    this.localRecorder?.setMediaRecorderObserver(observer)
                    let ret = this.localRecorder?.startRecording(config)
                    ShowToast.shortToast('startRecording ret=' + ret)
                  }
                })
                .margin({ right: 12, bottom: 12 })
            }.width('50%')
            .alignContent(Alignment.BottomEnd)
          }

          if (this.remoteUid != 0) {
            Stack() {
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
              })
              Button(this.remoteRecording ? $r('app.string.stop_recording') : $r('app.string.start_recording'))
                .onClick(() => {
                  this.remoteRecording = !this.remoteRecording
                  let virtualPath = '/data/storage/el2/base/haps/entry/files/media_recorder_remote.mp4'
                  let realPath =
                    '/data/app/el2/100/base/io.agora.hmos.apiexample/haps/entry/files/media_recorder_remote.mp4'
                  if (!this.remoteRecording) {
                    this.remoteRecorder?.stopRecording()
                    this.remoteRecorder?.release()
                    this.remoteRecorder = undefined
                    AlertDialog.show(
                      {
                        title: 'MediaFilePath',
                        message: realPath,
                        confirm: {
                          value: $r('app.string.confirm'),
                          backgroundColor: Color.White,
                          action: () => {

                          }
                        },
                        alignment: DialogAlignment.Center
                      }
                    )
                  } else {
                    if (this.remoteRecorder != undefined) {
                      this.remoteRecorder?.release()
                    }
                    let info = new RecorderStreamInfo()
                    info.uid = this.remoteUid
                    info.channelId = this.channelName
                    this.remoteRecorder = this.rtcEngine?.createMediaRecorder(info)
                    let config = new MediaRecorderConfiguration(
                      virtualPath,
                      AgoraMediaRecorder.CONTAINER_MP4,
                      AgoraMediaRecorder.STREAM_TYPE_BOTH,
                      120000,
                      0
                    )
                    let observer: IMediaRecorderCallback = {
                      onRecorderStateChanged: (channelId: string, uid: number, state: number, reason: number): void => {
                        Logger.info('onRecorderStateChanged channelId=' + channelId + ', uid=' + uid + ', state=' +
                          state + ', reason=' + reason)
                      },
                      onRecorderInfoUpdated: (channelId: string, uid: number, info: RecorderInfo): void => {
                        Logger.info('onRecorderInfoUpdated channelId=' + channelId + ', uid=' + uid + ', filename=' +
                        info.fileName + ', durationMs=' + info.durationMs + ', fileSize=' + info.fileSize)
                      }
                    }
                    this.remoteRecorder?.setMediaRecorderObserver(observer)
                    let ret = this.remoteRecorder?.startRecording(config)
                    ShowToast.shortToast('startRecording ret=' + ret)
                  }
                })
                .margin({ right: 12, bottom: 12 })
            }.width('50%')
            .alignContent(Alignment.BottomEnd)
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')
      }
      .align(Alignment.TopStart)
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
            if (this.isJoin) {
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
            } else {
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
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              let token = await TokenUtils.genRtcToken(this.channelName, "")
              let ret = this.rtcEngine?.joinChannelWithOptions(token, this.channelName, 0, mediaOption)
              if (ret != Constants.ErrorCode.ERR_OK) {
                ShowToast.longToast("joinChannelWithOptions error " + ret + " : " +
                RtcEngine.getErrorDescription(ret))
                return
              }

              this.rtcEngine?.startPreview()
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