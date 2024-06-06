import {
  AgoraFacePositionInfo,
  AudioVolumeInfo,
  ChannelMediaOptions,
  Constants,
  RtcEngine,
  RtcEngineConfig,
  RtcStats,
  VideoCanvas
} from 'AgoraRtcSdk';
import { TitleBar } from '../../common/TitleBar';
import Logger from '../../util/Logger';
import { abilityAccessCtrl, bundleManager, common, Permissions } from '@kit.AbilityKit';
import ShowToast from '../../util/ShowToast';
import { BusinessError } from '@kit.BasicServicesKit';
import { AppID } from '../../common/KeyCenter';

const TAG: string = 'JoinVideoChannel'

@Entry
@Component
struct JoinVideoChannel {
  private rtcEngine: RtcEngine | undefined = undefined;
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

    this.rtcEngine.enableVideo()
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

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_joinvideochannel'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
            XComponent({
              id: 'preview_local',
              type: 'surface',
              libraryname: Constants.AGORA_LIB_NAME,
            })
              .onLoad(() => {
                let localCanvas = new VideoCanvas("preview_local");
                localCanvas.uid = this.localUid;
                localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
                localCanvas.mirrorMode = 0;
                this.rtcEngine?.setupLocalVideo(localCanvas);
              })
              .width('50%')
          }

          if (this.remoteUid != 0) {
            XComponent({
              id: 'preview_remote',
              type: 'surface',
              libraryname: Constants.AGORA_LIB_NAME,
            })
              .onLoad(() => {
                let localCanvas = new VideoCanvas("preview_remote");
                localCanvas.uid = this.remoteUid;
                localCanvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN;
                localCanvas.mirrorMode = 0;
                this.rtcEngine?.setupRemoteVideo(localCanvas);
              })
              .width('50%')
          }
        }
        .align(Alignment.TopStart)
        .height("50%")
        .width('100%')
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      Row() {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id("input_channel_name")
          .type(InputType.Number)
          .enabled(!this.isJoin)
          .enableKeyboardOnFocus(false)
          .onChange((value: string) => {
            this.channelName = value
          })
          .width('80%')
        if (this.isJoin) {
          Button($r('app.string.leave'))
            .id('button_to_leave')
            .onClick(() => {
              this.rtcEngine?.stopPreview()
              this.rtcEngine?.leaveChannel()
              focusControl.requestFocus("button_to_leave")
            })
            .width('20%')
        } else {
          Button($r('app.string.join'))
            .id('button_to_join')
            .onClick(async () => {
              if (this.channelName == "") {
                ShowToast.shortToast("The channel name is empty!")
                return
              }
              if (!(await checkPermissions(getContext(this) as common.UIAbilityContext))) {
                ShowToast.shortToast("Permission leak!")
                return
              }
              let mediaOption: ChannelMediaOptions = new ChannelMediaOptions();
              mediaOption.publishCameraTrack = true;
              mediaOption.publishMicrophoneTrack = true;
              mediaOption.autoSubscribeVideo = true;
              mediaOption.autoSubscribeAudio = true;
              mediaOption.channelProfile = Constants.ChannelProfile.CHANNEL_PROFILE_LIVE_BROADCASTING;
              mediaOption.clientRoleType = Constants.ClientRole.CLIENT_ROLE_BROADCASTER;

              this.rtcEngine?.joinChannelWithOptions("", this.channelName, 0, mediaOption);
              this.rtcEngine?.startPreview()
              focusControl.requestFocus("button_to_join")
            })
            .width('20%')
        }
      }
      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}

async function checkPermissions(context: common.UIAbilityContext): Promise<boolean> {
  let atManager = abilityAccessCtrl.createAtManager();
  let grant: boolean = false;
  let permissions: Array<Permissions> = [
    'ohos.permission.MICROPHONE',
    "ohos.permission.CAMERA"
  ];

  // 获取应用程序的accessTokenID
  let tokenId: number = 0;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager.getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (error) {
    let err: BusinessError = error as BusinessError;
    Logger.error(TAG, `Failed to get bundle info for self. Code is ${err.code}, message is ${err.message}`);
  }

  let pending: Array<Permissions> = new Array<Permissions>();

  for (let i = 0; i < permissions.length; i++) {
    let permission = permissions[i];
    // 校验应用是否被授予权限
    try {
      grant = await atManager.checkAccessToken(tokenId, permission) == abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
    } catch (error) {
      let err: BusinessError = error as BusinessError;
      Logger.error(`Failed to check access token. Code is ${err.code}, message is ${err.message}`);
    }
    if (!grant) {
      pending.push(permission)
    }
  }

  if (pending.length > 0) {
    let data = await atManager.requestPermissionsFromUser(context, pending)
    let grantStatusList: Array<number> = data.authResults;
    let length: number = grantStatusList.length;
    for (let i = 0; i < length; i++) {
      if (grantStatusList[i] === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
        // 用户授权，可以继续访问目标操作
        grant = true
      } else {
        // 用户拒绝授权，提示用户必须授权才能访问当前页面的功能，并引导用户到系统设置中打开相应的权限
        Logger.info(TAG, 'Permission denied');
        grant = false
        break
      }
    }
  }

  return grant;
}

