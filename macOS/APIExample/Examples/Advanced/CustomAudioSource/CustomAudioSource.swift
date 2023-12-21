//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class CustomAudioSource: BaseViewController {
    @IBOutlet weak var Container: AGEVideoContainer!
    @IBOutlet weak var pushPcmSwitch: NSSwitch!

    var videos: [VideoView] = []

    var agoraKit: AgoraRtcEngineKit!
    var exAudio: ExternalAudio = ExternalAudio.shared()
    var pcmSourcePush: AgoraPcmSourcePush!
    private var trackId: Int32 = 0
    
    /**
     --- Microphones Picker ---
     */
    @IBOutlet weak var selectMicsPicker: Picker!
    var mics: [AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.selectMicsPicker.picker.addItems(withTitles: self.mics.map {$0.deviceName ?? "unknown"})
            }
        }
    }
    var selectedMicrophone: AgoraRtcDeviceInfo? {
        let index = self.selectMicsPicker.indexOfSelectedItem
        if index >= 0 && index < mics.count {
            return mics[index]
        } else {
            return nil
        }
    }
    func initSelectMicsPicker() {
        selectMicsPicker.label.stringValue = "Microphone".localized
        // find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.mics = self.agoraKit.enumerateDevices(.audioRecording) ?? []
        }
        
        selectMicsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let micId = self.selectedMicrophone?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
    }
    
    /**
     --- Layout Picker ---
     */
    @IBOutlet weak var selectLayoutPicker: Picker!
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    var selectedLayout: Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
    }
    func initSelectLayoutPicker() {
        layoutVideos(2)
        selectLayoutPicker.label.stringValue = "Layout".localized
        selectLayoutPicker.picker.addItems(withTitles: layouts.map { $0.label })
        selectLayoutPicker.onSelectChanged {
            if self.isJoined {
                return
            }
            guard let layout = self.selectedLayout else { return }
            self.layoutVideos(layout.value)
        }
    }
    
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            selectLayoutPicker.isEnabled = !isJoined
            if isJoined == false {
                pushPcmSwitch.state = .off
                pcmSourcePush.stop()
            }
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinChannelButton.isEnabled = !isProcessing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableAudio()
        
        initSelectMicsPicker()
        initSelectLayoutPicker()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            self.exAudio.stopWork()
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func pushPcmSwitchChanged(_ sender: NSSwitch) {
        if !isJoined {
            return
        }
        if sender.state == .on {
            pcmSourcePush.start()
        } else {
            pcmSourcePush.stop()
        }
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCustomAudioTrack = sender.state == .on
        agoraKit.updateChannel(with: mediaOption)
    }
    
    let sampleRate:UInt = 44100, audioChannel:UInt = 1, bitPerSample = 16, samples = 441 * 10
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            guard let micId = selectedMicrophone?.deviceId else {
                return
            }
            
            agoraKit.setDevice(.audioRecording, deviceId: micId)
            // disable video module in audio scene
            agoraKit.disableVideo()
            
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream audio
            agoraKit.setClientRole(.broadcaster)
            
            // setup external audio source
            exAudio.setupExternalAudio(withAgoraKit: agoraKit,
                                       sampleRate: UInt32(sampleRate),
                                       channels: UInt32(audioChannel),
                                       audioCRMode: .exterCaptureSDKRender,
                                       ioType: .remoteIO)
            

            guard let filepath = Bundle.main.path(forResource: "output", ofType: "raw") else {return}

            pcmSourcePush = AgoraPcmSourcePush(delegate: self, filePath: filepath, sampleRate: Int(sampleRate),
                                               channelsPerFrame: Int(audioChannel), bitPerSample: bitPerSample, samples: samples)
            let trackConfig = AgoraAudioTrackConfig()
            trackConfig.enableLocalPlayback = true
            trackId = agoraKit.createCustomAudioTrack(.mixable, config: trackConfig)
                        
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            
            let option = AgoraRtcChannelMediaOptions()
            option.publishMicrophoneTrack = true
            option.publishCustomAudioTrack = true
            option.publishCameraTrack = false
            option.publishCustomAudioTrackId = Int(trackId)
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    self.isProcessing = false
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            agoraKit.destroyCustomAudioTrack(Int(trackId))
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
                self.isProcessing = false
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
    }
    
    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
            } else {
                view.placeholder.stringValue = "Remote \(i)"
            }
            videos.append(view)
        }
        // layout render view
        Container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension CustomAudioSource: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isProcessing = false
        isJoined = true
        let localVideo = videos[0]
        localVideo.uid = uid
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        exAudio.startWork()
        
        if pushPcmSwitch.state == .on {
            pcmSourcePush.start()
        }
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            remoteVideo.uid = uid
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
        }
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
        if let remoteVideo = videos.first(where: { $0.uid == uid }) {
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
}

extension CustomAudioSource: AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: UnsafeMutablePointer<UInt8>) {
        agoraKit.pushExternalAudioFrameRawData(data,
                                               samples: samples,
                                               sampleRate: Int(sampleRate),
                                               channels: Int(audioChannel),
                                               trackId: Int(trackId),
                                               timestamp: 0)
    }
}
