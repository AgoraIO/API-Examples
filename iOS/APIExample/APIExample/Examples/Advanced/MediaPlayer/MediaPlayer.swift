//
//  MediaPlayer.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

let CAMERA_UID = UInt.random(in: 1001...2000)
let PLAYER_UID = UInt.random(in: 2001...3000)

class MediaPlayerEntry : UIViewController
{
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "MediaPlayer"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
    
}

class MediaPlayerMain: BaseViewController, UITextFieldDelegate {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var mediaUrlField: UITextField!
    @IBOutlet weak var playerControlStack: UIStackView!
    @IBOutlet weak var playerProgressSlider: UISlider!
    @IBOutlet weak var playoutVolume: UISlider!
    @IBOutlet weak var publishVolume: UISlider!
    @IBOutlet weak var playerDurationLabel: UILabel!
    
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayerKit: AgoraRtcMediaPlayerProtocol!
    
    private var originY: CGFloat = 0
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    @objc func keyboardWillAppear(notification: NSNotification) {
        let keyboardinfo = notification.userInfo![UIResponder.keyboardFrameEndUserInfoKey]
        let keyboardheight:CGFloat = (keyboardinfo as AnyObject).cgRectValue.size.height
   
        if self.originY == 0 {
            self.originY = self.view.centerY_CS
        }
        let rect = self.mediaUrlField.convert(self.mediaUrlField.bounds, to: self.view)
        let y = self.view.bounds.height - rect.origin.y - self.mediaUrlField.bounds.height - keyboardheight

        if y < 0 {
            let animator = UIViewPropertyAnimator(duration: 0.2, curve: .easeOut) {
                self.view.centerY_CS = y + self.originY
            }
            animator.startAnimation()
        }
    }
    
    @objc func keyboardWillDisappear(notification:NSNotification){
        let animator = UIViewPropertyAnimator(duration: 0.2, curve: .easeOut) {
            self.view.centerY_CS = self.originY
            self.originY = 0
        }
        animator.startAnimation()
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
    
    func textView(_ textView: UITextView, shouldChangeTextIn range: NSRange, replacementText text: String) -> Bool {
        if text == "\n" {
            textView.resignFirstResponder()
            return true
        }
        return false
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        mediaUrlField.delegate = self
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillAppear), name: UIResponder.keyboardWillShowNotification, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillDisappear), name: UIResponder.keyboardWillHideNotification, object: nil)
        // layout render view
        localVideo.setPlaceholder(text: "No Player Loaded")
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream1x2(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
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
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else { return }
                
        // prepare media player
        mediaPlayerKit = agoraKit.createMediaPlayer(with: self)
        mediaPlayerKit.setView(localVideo.videoView)
        mediaPlayerKit.setRenderMode(.fit)
        
        playoutVolume.minimumValue = 0
        playoutVolume.maximumValue = 400
        playoutVolume.integerValue = Int(mediaPlayerKit.getPlayoutVolume())
        
        publishVolume.minimumValue = 0
        publishVolume.maximumValue = 100
        mediaPlayerKit.adjustPublishSignalVolume(50)
        publishVolume.integerValue = 50
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        videoCanvas.sourceType = .mediaPlayer
        videoCanvas.mediaPlayerId = mediaPlayerKit.getMediaPlayerId()
        agoraKit.setupLocalVideo(videoCanvas)
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishCustomAudioTrack = true
        option.publishMicrophoneTrack = true
        option.autoSubscribeAudio = true
        option.autoSubscribeVideo = true
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, uid: CAMERA_UID, success: { token in
            self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: CAMERA_UID, mediaOptions: option, joinSuccess: nil)
        })
        agoraKit.muteRemoteAudioStream(PLAYER_UID, mute: true)

        let option1 = AgoraRtcChannelMediaOptions()
        option1.autoSubscribeAudio = false
        option1.autoSubscribeVideo = false
        option1.clientRoleType = GlobalSettings.shared.getUserRole()
        option1.enableAudioRecordingOrPlayout = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        NetworkManager.shared.generateToken(channelName: channelName, uid: PLAYER_UID) { token in
            let result1 = self.agoraKit.joinChannelEx(byToken: token, connection: connection, delegate: self, mediaOptions: option1, joinSuccess: nil)
            if result1 != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed, please check your params")
            }
            self.doOpenMediaUrl(sender: UIButton())
        }
    }
    
    @IBAction func doOpenMediaUrl(sender: UIButton) {
        guard let url = mediaUrlField.text else { return }
        //resign text field
        mediaUrlField.resignFirstResponder()
        mediaPlayerKit.open(url, startPos: 0)
    }
    
    @IBAction func doPlay(sender: UIButton) {
        mediaPlayerKit.play()// get channel name from configs
    }
    
    @IBAction func doStop(sender: UIButton) {
        mediaPlayerKit.stop()
    }
    
    @IBAction func doPause(sender: UIButton) {
        mediaPlayerKit.pause()
    }
    
    @IBAction func doPublish(sender: UIButton) {
        guard let channelName = configs["channelName"] as? String else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.publishMediaPlayerVideoTrack = true
        option.publishMediaPlayerAudioTrack = true
        option.publishMediaPlayerId = Int(mediaPlayerKit.getMediaPlayerId())
        option.publishCameraTrack = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        agoraKit.updateChannelEx(with: option, connection: connection)

    }
    @IBAction func dloStopPublish(_ sender: Any) {
        guard let channelName = configs["channelName"] as? String else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.publishMediaPlayerVideoTrack = false
        option.publishMediaPlayerAudioTrack = false
        option.publishMediaPlayerId = Int(mediaPlayerKit.getMediaPlayerId())
        option.publishCameraTrack = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        agoraKit.updateChannelEx(with: option, connection: connection)
    }
    
    @IBAction func doAdjustPlayoutVolume(sender: UISlider) {
        mediaPlayerKit.adjustPlayoutVolume(Int32(Int(sender.value)))
    }
    
    @IBAction func doAdjustPublishVolume(sender: UISlider) {
        mediaPlayerKit.adjustPublishSignalVolume(Int32(Int(sender.value)))
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.disableVideo()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            agoraKit.destroyMediaPlayer(mediaPlayerKit)
            mediaPlayerKit = nil
            AgoraRtcEngineKit.destroy()
        }
    }
}

/// agora rtc engine delegate events
extension MediaPlayerMain: AgoraRtcEngineDelegate {
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
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "local user join: \(uid) \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if uid == CAMERA_UID || uid == PLAYER_UID {
            return
        }
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

extension MediaPlayerMain: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, error: AgoraMediaPlayerError) {
        LogUtils.log(message: "player rtc channel publish helper state changed to: \(state.rawValue), error: \(error.rawValue)", level: .info)
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            switch state.rawValue {
            case 100: // failed
                weakself.showAlert(message: "media player error: \(error.rawValue)")
                break
            case 2: // openCompleted
                let duration = weakself.mediaPlayerKit.getDuration()
                weakself.playerControlStack.isHidden = false
                weakself.playerDurationLabel.text = "\(String(format: "%02d", duration / 60000)) : \(String(format: "%02d", duration % 60000 / 1000))"
                weakself.playerProgressSlider.setValue(0, animated: true)
                break
            case 7: // stopped
                weakself.playerControlStack.isHidden = true
                weakself.playerProgressSlider.setValue(0, animated: true)
                weakself.playerDurationLabel.text = "00 : 00"
                guard let channelName = weakself.configs["channelName"] as? String else { return }
                let option = AgoraRtcChannelMediaOptions()
                option.publishMediaPlayerVideoTrack = false
                option.publishMediaPlayerAudioTrack = false
                let connection = AgoraRtcConnection()
                connection.channelId = channelName
                connection.localUid = PLAYER_UID
                weakself.agoraKit.updateChannelEx(with: option, connection: connection)
                break
            default: break
            }
        }
    }
    
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo position: Int) {
        let duration = Float(mediaPlayerKit.getDuration())
        var progress: Float = 0
        var left: Int = 0
        if duration > 0 {
            progress = Float(position) / duration
            left = Int((mediaPlayerKit.getDuration() - position)) / 1000
        }
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            weakself.playerDurationLabel.text = "\(String(format: "%02d", left / 60)) : \(String(format: "%02d", left % 60))"
            if !weakself.playerProgressSlider.isTouchInside {
                weakself.playerProgressSlider.setValue(progress, animated: true)
            }
        }
    }
}
