//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

//TODO
//#if false

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
        self.navigationController?.pushViewController(newViewController, animated: true)
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
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension960x720,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else { return }
                
        // prepare media player
        mediaPlayerKit = agoraKit.createMediaPlayer(with: self)
        mediaPlayerKit.setView(localVideo.videoView)
//        mediaPlayerKit.setRenderMode(.fit)
        
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
        videoCanvas.sourceId = mediaPlayerKit.getMediaPlayerId()
        agoraKit.setupLocalVideo(videoCanvas)
//        agoraKit.startPreview()
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = .of(false)
        option.publishScreenTrack = .of(false)
        option.publishCustomVideoTrack = .of(false)
        option.publishEncodedVideoTrack = .of(false)
        option.publishMediaPlayerVideoTrack = .of(true)
        
        option.publishMediaPlayerAudioTrack = .of(true)
        option.publishAudioTrack = .of(false)
        option.autoSubscribeAudio = .of(true)
        option.autoSubscribeVideo = .of(true)
        option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        option.publishMediaPlayerId = .of((Int32)(mediaPlayerKit.getMediaPlayerId()))
        option.defaultVideoStreamType = .of((Int32)(AgoraVideoStreamType.high.rawValue))
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: option)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    @IBAction func doOpenMediaUrl(sender: UIButton) {
        guard let url = mediaUrlField.text else { return }
        //resign text field
        mediaUrlField.resignFirstResponder()
        mediaPlayerKit.open(url, startPos: 0)
    }
    
    @IBAction func doPlay(sender: UIButton) {
        mediaPlayerKit.play()
    }
    
    @IBAction func doStop(sender: UIButton) {
        mediaPlayerKit.stop()
    }
    
    @IBAction func doPause(sender: UIButton) {
        mediaPlayerKit.pause()
    }
    
    @IBAction func doSeek(sender: UISlider) {
        //let position = Int(sender.value * Float(mediaPlayerKit.getDuration()))
        //let result = mediaPlayerKit.seek(toPosition: position)
    }
    
    @IBAction func doAdjustPlayoutVolume(sender: UISlider) {
        //AgoraRtcChannelPublishHelper.shareInstance().adjustPlayoutSignalVolume(Int32(Int(sender.value)))
        mediaPlayerKit.adjustPlayoutVolume(Int32(Int(sender.value)))
    }
    
    @IBAction func doAdjustPublishVolume(sender: UISlider) {
        //AgoraRtcChannelPublishHelper.shareInstance().adjustPublishSignalVolume(Int32(Int(sender.value)))
        mediaPlayerKit.adjustPublishSignalVolume(Int32(Int(sender.value)))
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            agoraKit.destroyMediaPlayer(mediaPlayerKit)
            mediaPlayerKit = nil
        }
    }
}

/// agora rtc engine delegate events
extension MediaPlayerMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
    func agoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, error: AgoraMediaPlayerError) {
        LogUtils.log(message: "player rtc channel publish helper state changed to: \(state.rawValue), error: \(error.rawValue)", level: .info)
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            switch state {
            case .failed:
                weakself.showAlert(message: "media player error: \(error.rawValue)")
                break
            case .openCompleted:
                let duration = weakself.mediaPlayerKit.getDuration()
                weakself.playerControlStack.isHidden = false
                weakself.playerDurationLabel.text = "\(String(format: "%02d", duration / 60)) : \(String(format: "%02d", duration % 60))"
                weakself.playerProgressSlider.setValue(0, animated: true)
                break
            case .stopped:
                weakself.playerControlStack.isHidden = true
                weakself.playerProgressSlider.setValue(0, animated: true)
                weakself.playerDurationLabel.text = "00 : 00"
                break
            default: break
            }
        }
    }
    
    func agoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedToPosition position: Int) {
        let duration = Float(mediaPlayerKit.getDuration() * 1000)
        var progress: Float = 0
        var left: Int = 0
        if duration > 0 {
            progress = Float(mediaPlayerKit.getPosition()) / duration
            left = Int((mediaPlayerKit.getDuration() * 1000 - mediaPlayerKit.getPosition()) / 1000)
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

//#endif
