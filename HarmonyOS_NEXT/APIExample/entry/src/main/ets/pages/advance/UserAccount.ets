import {
  ChannelMediaOptions,
  Constants,
  IRtcEngineEventHandler,
  RtcConnection,
  RtcEngine,
  RtcEngineConfig,
  RtcEngineEx,
  UserInfo,
  VideoCanvas,
  VideoEncoderConfiguration
} from '@shengwang/rtc-full';
import { common } from '@kit.AbilityKit';
import { TitleBar } from '../../common/TitleBar';
import { AppID } from '../../common/KeyCenter';
import { GlobalInfo } from '../Data';
import Logger from '../../util/Logger';
import PermissionHelper from '../../util/PermissionHelper';
import ShowToast from '../../util/ShowToast';
import { TokenUtils } from '../../util/TokenUtils';

const TAG: string = 'UserAccount'
const VIDEO_PREVIEW_ASPECT_RATIO: number = 1.2676

@Entry
@Component
struct UserAccount {
  private rtcEngine: RtcEngineEx | undefined = undefined
  private rtcConnection: RtcConnection = new RtcConnection('', 0)
  private localXComponentController: XComponentController = new XComponentController()
  private remoteXComponentController: XComponentController = new XComponentController()

  @State channelName: string = ''
  @State userAccount: string = ''
  @State isJoined: boolean = false
  @State localUid: number = 0
  @State remoteUid: number = 0
  @State mappedUid: number = 0

  private showResult(apiName: string, result: number): boolean {
    Logger.info(TAG, `${apiName} result:${result}`)
    if (result == Constants.ErrorCode.ERR_OK) {
      ShowToast.shortToast(`${apiName}: success`)
      return true
    }
    ShowToast.longToast(`${apiName} failed: ${result} : ${RtcEngine.getErrorDescription(result)}`)
    return false
  }

  private validateInput(): boolean {
    if (this.channelName.length == 0) {
      ShowToast.shortToast($r('app.string.channel_name_empty'))
      return false
    }
    if (this.userAccount.length == 0) {
      ShowToast.shortToast($r('app.string.user_account_empty'))
      return false
    }
    return true
  }

  private queryUserInfo(uid: number, userAccount: string): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    const infoByAccount = new UserInfo(0, '')
    const accountResult = engine.getUserInfoByUserAccountEx(userAccount, infoByAccount, this.rtcConnection)
    const infoByUid = new UserInfo(0, '')
    const uidResult = engine.getUserInfoByUidEx(uid, infoByUid, this.rtcConnection)
    Logger.info(TAG, `getUserInfoByUserAccountEx result:${accountResult}, uid:${infoByAccount.uid}`)
    Logger.info(TAG, `getUserInfoByUidEx result:${uidResult}, userAccount:${infoByUid.userAccount}`)
    if (accountResult == Constants.ErrorCode.ERR_OK && uidResult == Constants.ErrorCode.ERR_OK) {
      ShowToast.longToast(`User Account: ${infoByUid.userAccount} -> ${infoByAccount.uid}`)
    } else {
      ShowToast.longToast(`Get UserInfo failed: account=${accountResult}, uid=${uidResult}`)
    }
  }

  private createExEventHandler(): IRtcEngineEventHandler {
    const eventHandler: IRtcEngineEventHandler = {}
    eventHandler.onJoinChannelSuccess = (channel: string, uid: number, elapsed: number) => {
      Logger.info(TAG, `onJoinChannelSuccess channel:${channel}, uid:${uid}, elapsed:${elapsed}`)
      this.rtcConnection.channelId = channel
      this.rtcConnection.localUid = uid
      this.localUid = uid
      this.mappedUid = uid
      this.isJoined = true
    }
    eventHandler.onLocalUserRegistered = (uid: number, userAccount: string) => {
      Logger.info(TAG, `onLocalUserRegistered userAccount:${userAccount}, uid:${uid}`)
      this.mappedUid = uid
      ShowToast.longToast(`onLocalUserRegistered: ${userAccount} -> ${uid}`)
    }
    eventHandler.onUserJoined = (uid: number, elapsed: number) => {
      Logger.info(TAG, `onUserJoined uid:${uid}, elapsed:${elapsed}`)
      if (this.remoteUid == 0) {
        this.remoteUid = uid
      }
    }
    eventHandler.onUserInfoUpdated = (uid: number, userInfo: UserInfo) => {
      Logger.info(TAG, `onUserInfoUpdated userAccount:${userInfo.userAccount}, uid:${uid}`)
      this.queryUserInfo(uid, userInfo.userAccount)
    }
    eventHandler.onUserOffline = (uid: number, reason: number) => {
      Logger.info(TAG, `onUserOffline uid:${uid}, reason:${reason}`)
      if (this.remoteUid == uid) {
        this.remoteUid = 0
      }
    }
    eventHandler.onLeaveChannel = () => {
      Logger.info(TAG, 'onLeaveChannel')
      this.isJoined = false
      this.localUid = 0
      this.remoteUid = 0
      this.mappedUid = 0
      this.rtcConnection.localUid = 0
    }
    eventHandler.onError = (err: number, message: string) => {
      Logger.error(TAG, `onError err:${err}, message:${message}`)
      ShowToast.longToast(`onError ${err}: ${message}`)
    }
    return eventHandler
  }

  private registerUserAccount(): void {
    if (this.userAccount.length == 0) {
      ShowToast.shortToast($r('app.string.user_account_empty'))
      return
    }
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    this.showResult('registerLocalUserAccount', engine.registerLocalUserAccount(AppID, this.userAccount))
  }

  private async preloadChannel(): Promise<void> {
    if (!this.validateInput()) {
      return
    }
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    const token = await TokenUtils.genRtcToken(this.channelName, this.userAccount)
    this.showResult('preloadChannelWithUserAccount',
      engine.preloadChannelWithUserAccount(token, this.channelName, this.userAccount))
  }

  private async joinChannelEx(): Promise<void> {
    if (!this.validateInput()) {
      return
    }
    if (!(await PermissionHelper.checkPermissions(
      getContext(this) as common.UIAbilityContext,
      ['ohos.permission.MICROPHONE', 'ohos.permission.CAMERA']
    ))) {
      ShowToast.shortToast('Permission leak!')
      return
    }
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    const options = new ChannelMediaOptions()
    options.publishCameraTrack = true
    options.publishMicrophoneTrack = true
    options.autoSubscribeVideo = true
    options.autoSubscribeAudio = true
    options.channelProfile = Constants.ChannelProfile.LIVE_BROADCASTING
    options.clientRoleType = Constants.ClientRole.BROADCASTER
    this.rtcConnection.channelId = this.channelName
    this.rtcConnection.localUid = 0
    const token = await TokenUtils.genRtcToken(this.channelName, this.userAccount)
    const result = engine.joinChannelWithUserAccountEx(
      token, this.channelName, this.userAccount, options, this.createExEventHandler())
    if (this.showResult('joinChannelWithUserAccountEx', result)) {
      engine.startPreview()
    }
  }

  private leaveChannelEx(): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    Logger.info(TAG,
      `leaveChannelEx channel:${this.rtcConnection.channelId}, localUid:${this.rtcConnection.localUid}`)
    const result = engine.leaveChannelEx(this.rtcConnection)
    if (this.showResult('leaveChannelEx', result)) {
      engine.stopPreview()
    }
  }

  aboutToAppear(): void {
    const config = new RtcEngineConfig()
    config.mAppId = AppID
    config.mContext = getContext(this) as common.UIAbilityContext
    config.mEventHandler = {}
    config.mEventHandler.onLocalUserRegistered = (uid: number, userAccount: string) => {
      Logger.info(TAG, `onLocalUserRegistered userAccount:${userAccount}, uid:${uid}`)
      this.mappedUid = uid
      ShowToast.longToast(`onLocalUserRegistered: ${userAccount} -> ${uid}`)
    }
    config.mEventHandler.onError = (err: number, message: string) => {
      Logger.error(TAG, `onError err:${err}, message:${message}`)
      ShowToast.longToast(`onError ${err}: ${message}`)
    }
    this.rtcEngine = RtcEngine.create(config) as RtcEngineEx
    const encoderConfig = new VideoEncoderConfiguration()
    encoderConfig.dimensions = GlobalInfo.settings.dimensions
    encoderConfig.frameRate = GlobalInfo.settings.frameRate
    encoderConfig.orientationMode = GlobalInfo.settings.orientationMode
    this.rtcEngine.setVideoEncoderConfiguration(encoderConfig)
    this.rtcEngine.enableVideo()
  }

  aboutToDisappear(): void {
    const engine = this.rtcEngine
    if (engine == undefined) {
      return
    }
    if (this.isJoined) {
      engine.stopPreview()
      engine.leaveChannelEx(this.rtcConnection)
    }
    RtcEngine.destroy().then(() => {
      Logger.info(TAG, 'destroy done')
    })
    this.rtcEngine = undefined
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      TitleBar({
        title: $r('app.string.item_useraccount'),
        showBack: true
      }).flexGrow(0)

      Column() {
        Row() {
          if (this.localUid != 0) {
            XComponent({
              id: 'user_account_local',
              type: XComponentType.SURFACE,
              controller: this.localXComponentController
            })
              .onLoad(() => {
                const surfaceId = this.localXComponentController.getXComponentSurfaceId()
                const canvas = new VideoCanvas(surfaceId, true)
                canvas.uid = this.localUid
                canvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN
                canvas.mirrorMode = 0
                this.rtcEngine?.setupLocalVideo(canvas)
              })
              .width('50%')
          }

          if (this.remoteUid != 0) {
            XComponent({
              id: 'user_account_remote',
              type: XComponentType.SURFACE,
              controller: this.remoteXComponentController
            })
              .onLoad(() => {
                const surfaceId = this.remoteXComponentController.getXComponentSurfaceId()
                const canvas = new VideoCanvas(surfaceId, true)
                canvas.uid = this.remoteUid
                canvas.renderMode = VideoCanvas.RENDER_MODE_HIDDEN
                canvas.mirrorMode = 0
                this.rtcEngine?.setupRemoteVideoEx(canvas, this.rtcConnection)
              })
              .width('50%')
          }
        }
        .align(Alignment.TopStart)
        .width('100%')
        .aspectRatio(VIDEO_PREVIEW_ASPECT_RATIO)
      }
      .align(Alignment.TopStart)
      .flexGrow(1)

      Column({ space: 8 }) {
        TextInput({ placeholder: $r('app.string.text_input_channel_name') })
          .id('input_user_account_channel')
          .enabled(!this.isJoined)
          .enableKeyboardOnFocus(false)
          .maxLength(64)
          .onChange((value: string) => {
            this.channelName = value
          })

        TextInput({ placeholder: $r('app.string.text_input_user_account') })
          .id('input_user_account')
          .enabled(!this.isJoined)
          .enableKeyboardOnFocus(false)
          .maxLength(255)
          .onChange((value: string) => {
            this.userAccount = value
            this.mappedUid = 0
          })

        if (this.mappedUid != 0) {
          Row() {
            Text($r('app.string.mapped_int_uid'))
            Text(`: ${this.mappedUid}`)
          }
          .width('100%')
        }

        Row({ space: 8 }) {
          Button($r('app.string.register_user_account'))
            .id('button_register_user_account')
            .enabled(!this.isJoined)
            .onClick(() => {
              this.registerUserAccount()
            })
            .layoutWeight(1)

          Button($r('app.string.preload_user_account_channel'))
            .id('button_preload_user_account_channel')
            .enabled(!this.isJoined)
            .onClick(async () => {
              await this.preloadChannel()
            })
            .layoutWeight(1)
        }
        .width('100%')

        Button(this.isJoined ? $r('app.string.leave') : $r('app.string.join'))
          .id('button_user_account_join')
          .onClick(async () => {
            focusControl.requestFocus('button_user_account_join')
            if (this.isJoined) {
              this.leaveChannelEx()
            } else {
              await this.joinChannelEx()
            }
          })
          .width('100%')
      }
      .flexGrow(0)
      .width('100%')
      .padding({ left: 12, right: 12, top: 10, bottom: 12 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}
