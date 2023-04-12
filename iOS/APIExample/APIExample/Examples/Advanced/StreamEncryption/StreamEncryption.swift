//
//  StreamEncryption.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class StreamEncryptionEntry : UIViewController
{
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    @IBOutlet weak var encryptSecretField: UITextField!
    @IBOutlet weak var encryptModeBtn: UIButton!
    var mode:AgoraEncryptionMode = .AES128GCM2
    var useCustom:Bool = false
    let identifier = "StreamEncryption"
    
    override func viewDidLoad() {
        super.viewDidLoad()

        encryptModeBtn.setTitle("\(mode.description())", for: .normal)
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        guard let channelName = channelTextField.text, let secret = encryptSecretField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        encryptSecretField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "mode":mode, "secret":secret, "useCustom": useCustom]
        navigationController?.pushViewController(newViewController, animated: true)
    }
    
    func getEncryptionModeAction(_ mode:AgoraEncryptionMode) -> UIAlertAction{
        return UIAlertAction(title: "\(mode.description())", style: .default, handler: {[unowned self] action in
            self.mode = mode
            self.useCustom = false
            self.encryptModeBtn.setTitle("\(mode.description())", for: .normal)
        })
    }
    
    @IBAction func setEncryptionMode(){
        let alert = UIAlertController(title: "Set Encryption Mode".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        for profile in AgoraEncryptionMode.allValues(){
            alert.addAction(getEncryptionModeAction(profile))
        }
        // add custom option
        alert.addAction(UIAlertAction(title: "Custom", style: .default, handler: { (action:UIAlertAction) in
            self.useCustom = true
            self.encryptModeBtn.setTitle("Custom", for: .normal)
        }))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
}

class StreamEncryptionMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
            let secret = configs["secret"] as? String,
            let mode = configs["mode"] as? AgoraEncryptionMode,
            let useCustom = configs["useCustom"] as? Bool else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable encryption
        if(!useCustom) {
            // sdk encryption
            let config = AgoraEncryptionConfig()
            config.encryptionMode = mode
            config.encryptionKey = secret
            config.encryptionKdfSalt = getEncryptionSaltFromServer()
            let ret = agoraKit.enableEncryption(true, encryptionConfig: config)
            if ret != 0 {
                // for errors please take a look at:
                // CN https://docs.agora.io/cn/live-streaming-premium-legacy/API%20Reference/oc/Classes/AgoraRtcChannel.html?platform=iOS#//api/name/enableEncryption:encryptionConfig:
                // EN https://docs.agora.io/en/video-calling/develop/media-stream-encryption#implement--media-stream-encryption
                self.showAlert(title: "Error", message: "enableEncryption call failed: \(ret), please check your params")
            }
        } else {
            // your own custom algorithm encryption
            AgoraCustomEncryption.registerPacketProcessing(agoraKit)
        }
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token,
                                                   channelId: channelName,
                                                   info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
                self.isJoined = true
                LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
            }
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    func getEncryptionSaltFromServer() -> Data {

        return "EncryptionKdfSaltInBase64Strings".data(using: .utf8)!
    }

    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            
            agoraKit.leaveChannel()
            agoraKit.enableEncryption(false, encryptionConfig: AgoraEncryptionConfig())
            guard let useCustom = configs["useCustom"] as? Bool else { return }
            if useCustom {
                // deregister packet processing
                AgoraCustomEncryption.deregisterPacketProcessing(agoraKit)
                if isJoined {
                    agoraKit.leaveChannel { (stats) -> Void in
                        LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                    }
                }
            }
        }
    }
}

/// agora rtc engine delegate events
extension StreamEncryptionMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode.description)", level: .error)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}
