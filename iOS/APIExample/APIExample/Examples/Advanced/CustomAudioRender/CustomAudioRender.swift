//
//  CustomAudioSource.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit
import AGEVideoLayout

private let bitsPerSample: UInt = 16
private let samples: UInt = 441 * 10
private let sampleRate: UInt = 44100
private let channels: UInt = 2

class PCMPlayer {
    private var audioEngine: AVAudioEngine
    private var playerNode: AVAudioPlayerNode
    private var sampleRate: Double
    private var channels: AVAudioChannelCount

    init(sampleRate: Double, channels: AVAudioChannelCount) {
        self.sampleRate = sampleRate
        self.channels = channels
        
        audioEngine = AVAudioEngine()
        playerNode = AVAudioPlayerNode()
        
        audioEngine.attach(playerNode)
        
        let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: channels)
        audioEngine.connect(playerNode, to: audioEngine.mainMixerNode, format: format)
        
        do {
            try audioEngine.start()
        } catch {
            print("Audio Engine failed to start: \(error)")
        }
    }
    
    func playPCMData(pcmData: UnsafeMutablePointer<UInt8>, count: UInt) {
        guard let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: channels),
              let audioBuffer = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: AVAudioFrameCount(count / 4)), // 16位立体声每帧4字节
              let channelData = audioBuffer.floatChannelData else {
            return
        }
        
        audioBuffer.frameLength = AVAudioFrameCount(count / 4)
        
        for frame in 0..<Int(audioBuffer.frameLength) {
            let leftSample = Int16(pcmData[frame * 4]) | (Int16(pcmData[frame * 4 + 1]) << 8)
            let rightSample = Int16(pcmData[frame * 4 + 2]) | (Int16(pcmData[frame * 4 + 3]) << 8)
            
            channelData[0][frame] = Float(leftSample) / Float(Int16.max)
            channelData[1][frame] = Float(rightSample) / Float(Int16.max)
        }
        
        playerNode.scheduleBuffer(audioBuffer, completionHandler: nil)
        playerNode.play()
    }
}

class CustomAudioRenderEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomAudioRender"
    
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

class CustomAudioRenderMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!

    @IBOutlet weak var container: AGEVideoContainer!
    var audioViews: [UInt: VideoView] = [:]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    lazy var player = PCMPlayer(sampleRate: Double(sampleRate), channels: AVAudioChannelCount(channels))

    override func viewDidLoad() {
        super.viewDidLoad()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        agoraKit.enableAudio()
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishMicrophoneTrack = false
        option.autoSubscribeAudio = true
        option.autoSubscribeVideo = false
        option.channelProfile = .liveBroadcasting
        option.clientRoleType = .broadcaster //GlobalSettings.shared.getUserRole()
        
        agoraKit.enableExternalAudioSink(true, sampleRate: sampleRate, channels: channels)
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.enableExternalAudioSink(false, sampleRate: sampleRate, channels: channels)
                isJoined = false
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
    
    private func startPullAudio() {
        DispatchQueue.global().async {
            let pullMs: TimeInterval = 10
            let lengthInByte = sampleRate / 1000 * 2 * channels * UInt(pullMs)
            let pointer = UnsafeMutablePointer<UInt8>.allocate(capacity: Int(lengthInByte))
            while self.isJoined {
                let date = Date()
                memset(pointer, 0, Int(lengthInByte))
                let ret = self.agoraKit.pullPlaybackAudioFrameRawData(pointer, lengthInByte: lengthInByte)
                self.player.playPCMData(pcmData: pointer, count: lengthInByte)
                let cost = -date.timeIntervalSinceNow * 1000
                // multiply 0.4 to make sure thread process time is less than pullMs
                usleep(UInt32(max((pullMs * 1000 - cost) * 0.4, 0)))
//                print("pullPlaybackAudioFrameRawData: \(ret) lengthInByte: \(lengthInByte) cost: \(-date.timeIntervalSinceNow * 1000) ms")
            }
            pointer.deallocate()
        }
    }
}


/// agora rtc engine delegate events
extension CustomAudioRenderMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en:https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // set up local audio view, this view will not show video but just a placeholder
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
        startPullAudio()
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // set up remote audio view, this view will not show video but just a placeholder
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // remove remote audio view
        self.audioViews.removeValue(forKey: uid)
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
}
