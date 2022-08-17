//
//  RawVideoData.swift
//  APIExample
//
//  Created by Arlin on 2022/4/11.
//  Copyright © 2022 Agora Corp. All rights reserved.
//


/// Raw Video Data
/// This module show how to get origin raw video frame data.
/// 1.Register obesever: agoraKit.setVideoDataFrame(self)
/// 2.Call back AgoraVideoDataFrameProtocol to get raw video frame data
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
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation))
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideo.videoView
        videoCanvas.uid = UserInfo.userId
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Setup raw video data frame observer
        agoraKit.setVideoDataFrame(self)
        
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelId, info: nil, uid: UserInfo.userId)
        if result != 0 {
            /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
            self.showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
        }
    }
        
    override func didMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.setVideoDataFrame(nil)
            agoraKit.leaveChannel(nil)
        }
    }
       
    // MARK: - UI
    @IBAction func snapShotBtnClicked(_ sender: Any) {
        isSnapShoting = true
    }

    func setupUI () {
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        videoContainer.layoutStream(views: [localVideo, remoteVideo])
    }
}

// MARK: - AgoraVideoDataFrameProtocol
extension RawVideoDataViewController: AgoraVideoDataFrameProtocol {
    func onCaptureVideoFrame(_ videoFrame: AgoraVideoDataFrame!) -> Bool {
        if isSnapShoting {
            isSnapShoting = false
            let image = MediaUtils.i420(toImage: videoFrame.yBuffer, srcU: videoFrame.uBuffer, srcV: videoFrame.vBuffer, width: Int32(videoFrame.width), height: Int32(videoFrame.height))
            DispatchQueue.main.async {
                self.imageView.image = image
            }
        }
        return true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraVideoDataFrame!, forUid uid: UInt32) -> Bool {
        return true
    }
}

// MARK: - AgoraRtcEngineDelegate
extension RawVideoDataViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)
        remoteVideo.uid = uid
        
        // Render remote user video frame at a UIView
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.uid = uid
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
        NetworkManager.shared.generateToken(channelName: channelName) {
            self.navigationController?.pushViewController(newViewController, animated: true)            
        }
    }
}


