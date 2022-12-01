//
//  ContentInspect.swift
//  APIExample
//
//  Created by Arlin on 2022/5/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//


/// Content Inspect
/// This module show how to use sdk ability to inspect sexy video content.
/// 1.Enable content inspect: agoraKit.enableContentInspect(true, config:inspectConfig).
/// 2.Call back mathod contentInspectResult of AgoraVideoFrameDelegate to get the inspect result.
///
/// More detail: https://docs.agora.io/cn/content-moderation/landing-page?platform=macOS


import AgoraRtcKit
import AGEVideoLayout

class ContentInspectViewController: BaseViewController {
    @IBOutlet weak var channelField: Input!
    @IBOutlet weak var joinChannelButton: NSButton!
    @IBOutlet weak var videoContainer: AGEVideoContainer!
    
    @IBOutlet weak var inspectResultButton: NSButton!
    @IBOutlet weak var inspectNoitceLabel: NSTextField!
    
    var localVideo: VideoView!
    var remoteVideo: VideoView!
    
    var isJoinChannel: Bool = false {
        didSet {
            channelField.isEnabled = !isJoinChannel
            joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
            inspectResultButton.isHidden = !isJoinChannel
            inspectNoitceLabel.isHidden = !isJoinChannel
        }
    }
    
    var agoraKit: AgoraRtcEngineKit?

    
    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupUI()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        agoraKit?.leaveChannel(nil)
        AgoraRtcEngineKit.destroy()
    }
    
    // MARK: - UI
    @IBAction func joinBtnClicked(_ sender: NSButton) {
        if isJoinChannel {
            agoraKit?.leaveChannel(nil)
            isJoinChannel = false
            return
        }
        
        let channelId = channelField.stringValue
        guard !channelId.isEmpty else {return}

        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit?.enableVideo()
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit?.setupLocalVideo(videoCanvas)
        
        // Enable content inspect with local video view
        let moderateModule = AgoraContentInspectModule()
        moderateModule.type = .moderation
        moderateModule.interval = 1
        
        let inspectConfig = AgoraContentInspectConfig()
        inspectConfig.modules = [moderateModule]
        agoraKit?.enableContentInspect(true, config:inspectConfig)
        
        let options = AgoraRtcChannelMediaOptions()
        options.publishCameraTrack = true
        options.clientRoleType = .broadcaster
        NetworkManager.shared.generateToken(channelName: channelId, success: { token in
            let result = self.agoraKit?.joinChannel(byToken: token, channelId: channelId, uid: 0, mediaOptions: options)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }

    func setupUI() {
        localVideo = VideoView.createFromNib()!
        localVideo.placeholder.stringValue = "Local Host".localized
        remoteVideo = VideoView.createFromNib()!
        remoteVideo.placeholder.stringValue = "Remote Host".localized
        videoContainer.layoutStream(views: [localVideo, remoteVideo])

        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
        joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
        inspectNoitceLabel.stringValue = "You can use the camera point to picture and check content inspect effect".localized
        inspectResultButton.title = "Video Neutral".localized
    }
}

// MARK: - AgoraRtcEngineDelegate
extension ContentInspectViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, contentInspectResult result: AgoraContentInspectResult) {
        var resultText = "Video Neutral".localized
        switch result {
        case .neutral:
            resultText = "Video Neutral".localized
        case .sexy:
            resultText = "Video Sexy".localized
        case .porn:
            resultText = "Video Porn".localized
        default:
            break
        }
                
        // For animation effects
        if resultText == inspectResultButton.title && resultText == "Video Neutral".localized {
            inspectResultButton.title = ""
        } else {
            inspectResultButton.title = resultText
        }
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoinChannel = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)

        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        videoCanvas.view = remoteVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit?.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user \(uid) offline with reason \(reason)", level: .info)
        
        // Stop render remote user video frame
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = nil
        videoCanvas.uid = uid
        agoraKit?.setupRemoteVideo(videoCanvas)
    }
}

