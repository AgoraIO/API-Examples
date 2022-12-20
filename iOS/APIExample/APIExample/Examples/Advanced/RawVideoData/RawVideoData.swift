//
//  RawVideoData.swift
//  APIExample
//
//  Created by Arlin on 2022/4/11.
//  Copyright © 2022 Agora Corp. All rights reserved.
//


/// Raw Video Data
/// This module show how to get origin raw video frame data.
/// 1.Register obesever: agoraKit.setVideoFrameDelegate(self)
/// 2.Call back AgoraVideoFrameDelegate to get raw video frame data
///
/// More detail: https://docs.agora.io/en/Interactive%20Broadcast/raw_data_video_apple?platform=iOS

import AgoraRtcKit
import AGEVideoLayout

class RawVideoDataViewController: BaseViewController {
    @IBOutlet weak var videoContainer: AGEVideoContainer!
    @IBOutlet weak var imageView: UIImageView!
    
    let localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    let remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    var agoraKit: AgoraRtcEngineKit!
    var isSnapShoting = false

    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        
        guard let channelId = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {return}
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // Setup raw video data frame observer
        agoraKit.setVideoFrameDelegate(self)
        
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        agoraKit.startPreview()
        
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        
        NetworkManager.shared.generateToken(channelName: channelId, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelId, uid: 0, mediaOptions: option, joinSuccess: nil)
            if result != 0 {
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
            }
        })
    }
        
    override func didMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.disableAudio()
            agoraKit.disableVideo()
            agoraKit.setVideoFrameDelegate(nil)
            agoraKit.leaveChannel(nil)
        }
    }
       
    // MARK: - UI
    func setupUI () {
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        videoContainer.layoutStream(views: [localVideo, remoteVideo])
    }
    
    @IBAction func snapShotBtnClicked(_ sender: Any) {
        isSnapShoting = true
    }
}

// MARK: - AgoraVideoFrameDelegate
extension RawVideoDataViewController: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
        if isSnapShoting {
            isSnapShoting = false
            let image = MediaUtils.pixelBuffer(toImage: videoFrame.pixelBuffer!)
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

// MARK: - AgoraRtcEngineDelegate
extension RawVideoDataViewController: AgoraRtcEngineDelegate {
    // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)
        
        // Render remote user video frame at a UIView
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.uid = uid
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
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localVideo.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteVideo.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteVideo.statsInfo?.updateAudioStats(stats)
    }
}

class RawVideoDataEntryViewController: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!

    @IBAction func joinBtnClicked(sender: UIButton) {
        guard let channelName = channelTextField.text,
              channelName.lengthOfBytes(using: .utf8) > 0 else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "RawVideoData"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}


