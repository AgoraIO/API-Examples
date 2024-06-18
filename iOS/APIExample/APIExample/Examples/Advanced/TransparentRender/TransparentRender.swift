//
//  TransparentRender.swift
//  APIExample
//
//  Created by wushengtao on 2024/6/17.
//  Copyright © 2024 Agora Corp. All rights reserved.
//

import Foundation

class TransparentRenderEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "TransparentRender"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class TransparentRenderViewController: BaseViewController {
    private lazy var agoraKit: AgoraRtcEngineKit = {
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        let agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        return agoraKit
    }()
        
    private lazy var mediaPlayer: AgoraRtcMediaPlayerProtocol? = {
        let mediaPlayer = agoraKit.createMediaPlayer(with: self)
        mediaPlayer?.setVideoFrameDelegate(self)
        return mediaPlayer
    }()
    
    @IBOutlet private weak var topCanvasView: UIView?
    @IBOutlet private weak var leftCanvasView: UIView?
    @IBOutlet private weak var rightCanvasView: UIView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let channelName = self.configs["channelName"] as? String ?? ""
        
        openMedia()
        
        agoraKit.setExternalVideoSource(true, useTexture: false, sourceType: .videoFrame)
        agoraKit.enableVideo()
        agoraKit.startPreview()
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishMicrophoneTrack = false
        option.clientRoleType = .broadcaster
        option.publishCustomVideoTrack = true
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option) { channel, uid, elapsed in
                LogUtils.log(message: "join channel[\(channelName)] success", level: .info)
            }
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
        setupLocalCavans()
        setupLocalAlphaCavans()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        agoraKit.leaveChannel()
    }
    
    private func openMedia() {
        let url = Bundle.main.path(forResource: "yuv_limit_range_alpha_1280_540_right", ofType: "mp4") ?? ""
        let ret = try? mediaPlayer?.open(url, startPos: 0)
        LogUtils.log(message: "openMedia: open ret:\(ret) url: \(url)", level: .info)
    }
    
    private func setupLocalCavans() {
        mediaPlayer?.setView(topCanvasView)
    }
    
    private func setupLocalAlphaCavans() {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = leftCanvasView
        videoCanvas.renderMode = .fit
        videoCanvas.sourceType = .custom
        videoCanvas.enableAlphaMask = true
        agoraKit.setupLocalVideo(videoCanvas)
    }
    
    private func setupRemoteAlphaCavans(uid: UInt) {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = rightCanvasView
        videoCanvas.uid = uid
        videoCanvas.renderMode = .fit
        videoCanvas.enableAlphaMask = true
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}

extension TransparentRenderViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        setupRemoteAlphaCavans(uid: uid)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = nil
        videoCanvas.uid = uid
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "didOccurError: \(errorCode)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "didOccurWarning: \(warningCode)", level: .warning)
    }
}

extension TransparentRenderViewController: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, 
                             didChangedTo state: AgoraMediaPlayerState,
                             reason: AgoraMediaPlayerReason) {
        LogUtils.log(message: "didChangedTo state: \(state.rawValue), reason: \(reason.rawValue)", level: .info)
        switch state {
        case .openCompleted, .playBackCompleted, .playBackAllLoopsCompleted:
            let ret = mediaPlayer?.play()
            LogUtils.log(message: "play media ret:\(ret ?? -1)", level: .info)
        default:
            break
        }
    }
    
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo positionMs: Int, atTimestamp timestampMs: TimeInterval) {
        LogUtils.log(message: "didChangedTo positionMs: \(positionMs), timestampMs: \(timestampMs)", level: .info)
    }
}

extension TransparentRenderViewController: AgoraRtcMediaPlayerVideoFrameDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didReceiveVideoFrame videoFrame: AgoraOutputVideoFrame) {
        let newVideoFrame = AgoraVideoFrame()
        newVideoFrame.format = 12
        newVideoFrame.textureBuf = videoFrame.pixelBuffer
        newVideoFrame.rotation = videoFrame.rotation
        newVideoFrame.alphaStitchMode = 4
        let _ = agoraKit.pushExternalVideoFrame(newVideoFrame)
//        LogUtils.log(message: "pushExternalVideoFrame: \(ret)", level: .info)
    }
}
