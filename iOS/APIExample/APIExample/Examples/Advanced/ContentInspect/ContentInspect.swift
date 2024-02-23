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
/// More detail: https://docs.agora.io/cn/content-moderation/landing-page?platform=iOS

import AgoraRtcKit

class ContentInspectViewController: BaseViewController {
    @IBOutlet weak var localVideoView: UIView!
    @IBOutlet weak var inspectResultLabel: UILabel!
    
    var agoraKit: AgoraRtcEngineKit!

    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()

        guard let channelId = configs["channelName"] as? String else {return}
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Enable content inspect with local video view
        let moderateModule = AgoraContentInspectModule()
        moderateModule.type = .imageModeration
        moderateModule.interval = 1
        
        let inspectConfig = AgoraContentInspectConfig()
        inspectConfig.modules = [moderateModule]
        agoraKit.enableContentInspect(true, config: inspectConfig)
        
        let options = AgoraRtcChannelMediaOptions()
        options.publishCameraTrack = true
        options.publishMicrophoneTrack = true
        options.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelId, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelId, uid: 0, mediaOptions: options)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
            }
        })
    }
    
    override func didMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.disableAudio()
            agoraKit.disableVideo()
            agoraKit.leaveChannel(nil)
            AgoraRtcEngineKit.destroy()
        }
    }
    
    // MARK: - UI
    func setupUI () {
        let rightBarButton = UIBarButtonItem(title: "SwitchCamera".localized, 
                                             style: .plain,
                                             target: self,
                                             action: #selector(switchCameraBtnClicked))
        self.navigationItem.rightBarButtonItem = rightBarButton
    }
    
    @objc func switchCameraBtnClicked() {
        agoraKit.switchCamera()
    }
}

// MARK: - AgoraRtcEngineDelegate
extension ContentInspectViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        // Usually happens with invalid parameters
        // Error code description can be found at:
        // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
        // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
        showAlert(title: "Error", message: "Error: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
}

class ContentInspectEntryViewController: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!

    @IBAction func joinBtnClicked(sender: UIButton) {
        guard let channelName = channelTextField.text,
              channelName.lengthOfBytes(using: .utf8) > 0 else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "ContentInspect"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}
