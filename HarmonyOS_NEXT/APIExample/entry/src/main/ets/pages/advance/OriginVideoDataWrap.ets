import {
    ChannelMediaOptions,
    Constants, RtcEngine,
    RtcEngineConfig,
    VideoCanvas,
    VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { image } from '@kit.ImageKit';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import ShowToast from '../../util/ShowToast';
import { AppID } from '../../common/KeyCenter';
import PermissionHelper from '../../util/PermissionHelper';
import { common } from '@kit.AbilityKit';
import { GlobalInfo } from '../Data';
import { OriginVideoData, SnapshotFrame } from 'librawdataprocessing.so';

const TAG: string = 'OriginVideoData'


@Entry
@Component
struct OriginVideoDataWrap {
  private rtcEngine: RtcEngine | undefined = undefined;
  private native: OriginVideoData | undefined = undefined;
  @State channelName: string = ''
  @State isJoin: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State isSavingSnapshot: boolean = false

  aboutToAppear(): void {
    Logger.info(TAG, 'aboutToAppear')
    let config: RtcEngineConfig = new RtcEngineConfig();
    let context = getContext(this) as common.UIAbilityContext;
    config.mAppId = AppID;
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
    this.rtcEngine = RtcEngine.create(config);
    let encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()

    this.native = new OriginVideoData(this.rtcEngine.getNativeHandle());
    this.native.enable(true)
  }

  aboutToDisappear(): void {
    this.native?.enable(false)
    this.native = undefined
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

  private async takeSnapshot(): Promise<void> {
    if (this.native == undefined || this.isSavingSnapshot) {
      return
    }
    this.isSavingSnapshot = true
    let result = this.native.takeSnapshot((frame: SnapshotFrame) => {
      this.saveSnapshot(frame)
    })
    if (result != 0) {
      this.isSavingSnapshot = false
      Logger.error(TAG, `takeSnapshot failed: ${result}`)
      ShowToast.longToast($r('app.string.screenshot_save_failed'))
    }
  }

  private async saveSnapshot(frame: SnapshotFrame): Promise<void> {
    let pixelMap: image.PixelMap | undefined = undefined
    let imagePacker: image.ImagePacker | undefined = undefined
    let photoHelper: photoAccessHelper.PhotoAccessHelper | undefined = undefined
    try {
      if (frame.errorCode != 0 || frame.width <= 0 || frame.height <= 0 || frame.data.byteLength == 0) {
        throw new Error(`Invalid snapshot frame: ${frame.errorCode}`)
      }

      pixelMap = await image.createPixelMap(frame.data, {
        size: { width: frame.width, height: frame.height },
        srcPixelFormat: image.PixelMapFormat.NV21,
        pixelFormat: image.PixelMapFormat.RGBA_8888,
        editable: true
      })
      if (frame.rotation != 0) {
        await pixelMap.rotate(frame.rotation)
      }

      imagePacker = image.createImagePacker()
      let jpegData = await imagePacker.packToData(pixelMap, { format: 'image/jpeg', quality: 90 })
      let context = getContext(this) as common.UIAbilityContext;
      photoHelper = photoAccessHelper.getPhotoAccessHelper(context)
      let request = photoAccessHelper.MediaAssetChangeRequest.createAssetRequest(
        context,
        photoAccessHelper.PhotoType.IMAGE,
        'jpg',
        { title: `IMG_AGORA_${Date.now()}` }
      )
      request.addResource(photoAccessHelper.ResourceType.IMAGE_RESOURCE, jpegData)
      await photoHelper.applyChanges(request)
      ShowToast.longToast($r('app.string.screenshot_saved'))
    } catch (error) {
      let err = error as BusinessError
      Logger.error(TAG, `saveSnapshot failed. Code: ${err.code}, message: ${err.message}`)
      ShowToast.longToast($r('app.string.screenshot_save_failed'))
    } finally {
      this.isSavingSnapshot = false
      if (imagePacker != undefined) {
        try {
          await imagePacker.release()
        } catch (error) {
          Logger.error(TAG, `release ImagePacker failed: ${(error as BusinessError).message}`)
        }
      }
      if (pixelMap != undefined) {
        try {
          await pixelMap.release()
        } catch (error) {
          Logger.error(TAG, `release PixelMap failed: ${(error as BusinessError).message}`)
        }
      }
      if (photoHelper != undefined) {
        try {
          await photoHelper.release()
        } catch (error) {
          Logger.error(TAG, `release PhotoAccessHelper failed: ${(error as BusinessError).message}`)
        }
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.End, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_originvideodata'),
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
              localCanvas.mirrorMode = 0;
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
      Row() {
        SaveButton({
          icon: SaveIconStyle.LINES,
          text: SaveDescription.SAVE_TO_GALLERY,
          buttonType: ButtonType.Capsule
        })
          .id('button_screenshot')
          .enabled(this.isJoin && !this.isSavingSnapshot)
          .onClick((event: ClickEvent, result: SaveButtonOnClickResult) => {
            if (result == SaveButtonOnClickResult.SUCCESS) {
              this.takeSnapshot()
            } else {
              Logger.error(TAG, `SaveButton authorization failed: ${result}`)
              ShowToast.longToast($r('app.string.screenshot_permission_denied'))
            }
          })
      }
        .margin(12)
        .flexGrow(0)
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
              this.rtcEngine?.stopPreview()
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
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.BROADCASTER;

              this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption);
              this.rtcEngine?.startPreview()
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
