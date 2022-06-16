//
//  RawAudioData.swift
//  APIExample
//
//  Created by Arlin on 2022/4/13.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

/// Raw Audio Data
/// This module show how to get origin raw audio frame data.
/// 1.Register obesever: agoraKit.setAudioFrameDelegate(self)
/// 2.Call back AgoraAudioFrameDelegate to get raw audio frame data
///
/// More detail: https://docs.agora.io/en/Interactive%20Broadcast/raw_data_audio_apple?platform=iOS

import AgoraRtcKit
import AGEVideoLayout

class RawAudioDataViewController: BaseViewController {
    @IBOutlet weak var videoContainer: AGEVideoContainer!
    
    let localVideo = Bundle.loadVideoView(type: .local, audioOnly: true)
    let remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: true)
    
    var agoraKit: AgoraRtcEngineKit!

    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        
        guard let channelId = configs["channelName"] as? String else {return}
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        agoraKit.setClientRole(.broadcaster)
        
        // Setup raw auido data frame observer
        agoraKit.setAudioFrameDelegate(self)
                
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelId, info: nil, uid: 0)
        if result != 0 {
            /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
            self.showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
        }
    }
        
    override func didMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.setAudioFrameDelegate(nil)
            agoraKit.leaveChannel(nil)
        }
    }
       
    // MARK: - UI
    func setupUI () {
        localVideo.setPlaceholder(text: self.getAudioLabel(uid: 0, isLocal: true))
        remoteVideo.setPlaceholder(text: self.getAudioLabel(uid: 0, isLocal: false))
        localVideo.statsInfo = nil
        remoteVideo.statsInfo = nil
        videoContainer.layoutStream(views: [localVideo, remoteVideo])
    }
}

// MARK: - AgoraAudioFrameDelegate
extension RawAudioDataViewController: AgoraAudioFrameDelegate {
    func onRecord(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onPlaybackAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onMixedAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onPlaybackAudioFrame(beforeMixing frame: AgoraAudioFrame, channelId: String, uid: UInt) -> Bool {
        return true
    }
    
    func onEarMonitoringAudioFrame(_ frame: AgoraAudioFrame) -> Bool {
        return true
    }
}

// MARK: - AgoraRtcEngineDelegate
extension RawAudioDataViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        localVideo.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)
        remoteVideo.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user \(uid) offline with reason \(reason)", level: .info)
        remoteVideo.setPlaceholder(text: self.getAudioLabel(uid: 0, isLocal: false))
    }
}

class RawAudioDataEntryViewController: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!

    @IBAction func joinBtnClicked(sender: UIButton) {
        guard let channelName = channelTextField.text,
              channelName.lengthOfBytes(using: .utf8) > 0 else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "RawAudioData"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}
