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
        agoraKit.enableVideo()
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localVideoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Enable content inspect with local video view
        let moderateModule = AgoraContentInspectModule()
        moderateModule.vendor = .agora
        moderateModule.token = nil
        moderateModule.type = .moderation
        moderateModule.frequency = 1
        
        let inspectConfig = AgoraContentInspectConfig()
        inspectConfig.modules = [moderateModule]
        agoraKit.enableContentInspect(true, config:inspectConfig)
        
        let options = AgoraRtcChannelMediaOptions()
        options.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelId, uid: 0, mediaOptions: options)
        if result != 0 {
            /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
            showAlert(title: "Error", message: "Join channel failed with errorCode: \(result)")
        }
    }
    
    override func didMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.leaveChannel(nil)
        }
    }
    
    // MARK: - UI
    func setupUI () {
        let rightBarButton = UIBarButtonItem(title: "SwitchCamera".localized, style: .plain, target: self, action: #selector(switchCameraBtnClicked))
        self.navigationItem.rightBarButtonItem = rightBarButton
    }
    
    @objc func switchCameraBtnClicked() {
        agoraKit.switchCamera()
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
        if resultText == inspectResultLabel.text && resultText == "Video Neutral".localized {
            inspectResultLabel.text = ""
        } else {
            inspectResultLabel.text = resultText
        }
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        /// Error code description: https://docs.agora.io/en/Interactive%20Broadcast/error_rtc
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
