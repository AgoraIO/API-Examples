//
//  RawVideoData.swift
//  APIExample
//
//  Created by Arlin on 2022/4/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit
import AGEVideoLayout

class RawVideoData: BaseViewController {
    @IBOutlet weak var channelField: Input!
    @IBOutlet weak var joinChannelButton: NSButton!
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var snapShotButton: NSButton!
    @IBOutlet weak var imageView: NSImageView!
    
    var localVideo: VideoView!
    var remoteVideo: VideoView!
    var agoraKit: AgoraRtcEngineKit!
    
    var isSnapShoting = false
    var isJoinChannel: Bool = false {
        didSet {
            channelField.isEnabled = !isJoinChannel
            joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
            snapShotButton.isEnabled = isJoinChannel
            imageView.isHidden = !isJoinChannel
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        setupAgoraKit()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        agoraKit.leaveChannel(nil)
        AgoraRtcEngineKit.destroy()
    }
    
    func setupAgoraKit() {
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        agoraKit.setClientRole(.broadcaster)
    }
    
    // MARK: - UI
    @IBAction func joinBtnClicked(_ sender: NSButton) {
        if isJoinChannel {
            agoraKit.leaveChannel(nil)
            isJoinChannel = false
            imageView.image = nil
            return
        }
        
        let channelId = channelField.stringValue
        guard !channelId.isEmpty else {return}
        
        // Setup raw video data frame observer
        agoraKit.setVideoFrameDelegate(self)
        
        let encoderConfig = AgoraVideoEncoderConfiguration()
        encoderConfig.dimensions = CGSize(width: 960, height: 720)
        encoderConfig.frameRate = .fps30
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(encoderConfig)
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelId, info: nil, uid: 0)
        if result != 0 {
            /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
            self.showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
        }
    }
    
    @IBAction func snapShotBtnClicked(_ sender: Any) {
        isSnapShoting = true
    }
    
    func setupUI() {
        localVideo = VideoView.createFromNib()!
        localVideo.placeholder.stringValue = "Local Host".localized
        remoteVideo = VideoView.createFromNib()!
        remoteVideo.placeholder.stringValue = "Remote Host".localized
        container.layoutStream(views: [localVideo, remoteVideo])

        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
        joinChannelButton.title = isJoinChannel ? "Leave Channel".localized : "Join Channel".localized
        snapShotButton.title = "SnapShot".localized
    }
}

// MARK: - AgoraVideoFrameDelegate
extension RawVideoData: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
        if isSnapShoting {
            isSnapShoting = false
            let image = MediaUtils.i420(toImage: videoFrame.yBuffer, srcU: videoFrame.uBuffer, srcV: videoFrame.vBuffer, width: videoFrame.width, height: videoFrame.height)
            
            DispatchQueue.main.async {
                self.imageView.image = image
            }
        }
        return true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        return true
    }
}

extension RawVideoData: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
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
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user \(uid) offline with reason \(reason)", level: .info)
        
        // Stop render remote user video frame
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = nil
        videoCanvas.uid = uid
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}

