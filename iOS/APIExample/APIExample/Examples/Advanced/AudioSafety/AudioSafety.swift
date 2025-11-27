//
//  CustomAudio.swift
//  APIExample
//
//  Created by qinhui on 2025/11/26.
//  Copyright © 2025 Agora Corp. All rights reserved.
//

import UIKit
import AGEVideoLayout
import AgoraRtcKit

class AudioSafetyEntry: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "AudioSafety"
    var profile: AgoraAudioProfile = .default
    var scenario: AgoraAudioScenario = .default
    
    @IBOutlet weak var durationTextField: UITextField!
    @IBOutlet weak var recordSwitch: UISwitch!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        durationTextField.keyboardType = .numberPad
    }

    @IBAction func doJoinPressed(sender: UIButton) {
        guard let channelName = channelTextField.text else { return }
        let recordSeconds = Int(durationTextField.text ?? "") ?? 0
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else { return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName,
                                     "audioProfile": profile,
                                     "audioScenario": scenario,
                                     "enabelRecordLocal": recordSwitch.isOn,
                                     "recordSeconds": recordSeconds
        ]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class AudioSafety: BaseViewController {
    var audioViews: [UInt: VideoView] = [:]
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var container: AGEVideoContainer!
    
    // AudioSafetyManager for audio recording and reporting
    private var audioSafetyManager: AudioSafetyManager?
    
    // Random UID known in advance, used for both config.localUid and joinChannel
    private let myUid: UInt = UInt.random(in: 10000...100000000)
    
    // Debounce for reportUser to prevent rapid consecutive calls
    private var lastReportTime: TimeInterval = 0
    private let REPORT_DEBOUNCE_MS: TimeInterval = 1.0  // Minimum 1 second between reports
    
    override func viewDidLoad() {
        super.viewDidLoad()
        guard let channelName = configs["channelName"] as? String,
              let audioProfile = configs["audioProfile"] as? AgoraAudioProfile,
              let audioScenario = configs["audioScenario"] as? AgoraAudioScenario,
              let enabelRecordLocal = configs["enabelRecordLocal"] as? Bool,
              let recordSeconds = configs["recordSeconds"] as? Int
            else { return }
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        // set audio scenario
        config.audioScenario = audioScenario
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        
        agoraKit.enableAudio()
        
        // Initialize AudioSafetyManager (must be done before joining channel)
        // AudioSafetyManager will register itself as audio frame delegate
        let audioSafetyConfig = AudioSafetyManagerConfig(
            rtcEngine: agoraKit,
            localUid: myUid,
            enableRegisterLocal: enabelRecordLocal,
            bufferDurationSeconds: recordSeconds
        )
        audioSafetyManager = AudioSafetyManager(config: audioSafetyConfig)
        
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // set audio profile
        agoraKit.setAudioProfile(audioProfile)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // enable volume indicator
        agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishMicrophoneTrack = true
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            // Use the same UID that was set in config (myUid was set when creating config)
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: self.myUid, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    func sortedViews() -> [VideoView] {
        return Array(audioViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    /**
     * Report a user and generate WAV file with audio evidence
     * @param uid User ID to report
     */
    func reportUser(uid: UInt) {
        // Debounce: prevent rapid consecutive reports
        let currentTime = Date().timeIntervalSince1970
        if currentTime - lastReportTime < REPORT_DEBOUNCE_MS {
            showAlert(title: "Info", message: "Please wait before reporting again")
            return
        }
        lastReportTime = currentTime
        
        audioSafetyManager?.reportUser(targetUid: uid, callback: AudioSafetyReportCallback(
            onSuccess: { wavFileUrl in
                DispatchQueue.main.async {
                    ToastView.hidden()
                    ToastView.show(text: "Audio evidence saved: \(wavFileUrl.path)")
                    LogUtils.log(message: "Audio evidence saved: \(wavFileUrl.path)", level: .info)
                    
                    // Here you can upload the WAV file to your moderation service
                    // For example:
                    // uploadToModerationService(wavFileUrl, uid)
                }
            },
            onError: { error in
                DispatchQueue.main.async {
                    ToastView.hidden()
                    ToastView.show(text: "Failed to report user \(uid): \(error)")
                    LogUtils.log(message: "Failed to report user \(uid): \(error)", level: .error)
                }
            }
        ))
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        agoraKit.enable(inEarMonitoring: false)
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        agoraKit.leaveChannel()
        
        AgoraRtcEngineKit.destroy()
        
        audioSafetyManager?.stopRecording()
        audioSafetyManager?.release()
        audioSafetyManager = nil
    }
}

/**
 * Helper class to implement AudioSafetyManager.ReportCallback
 */
private class AudioSafetyReportCallback: AudioSafetyManager.ReportCallback {
    let onSuccess: (URL) -> Void
    let onError: (String) -> Void
    
    init(onSuccess: @escaping (URL) -> Void, onError: @escaping (String) -> Void) {
        self.onSuccess = onSuccess
        self.onError = onError
    }
    
    func onSuccess(wavFileUrl: URL) {
        onSuccess(wavFileUrl)
    }
    
    func onError(error: String) {
        onError(error)
    }
}

extension AudioSafety: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
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
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // Start recording (observer already registered before joining channel)
        audioSafetyManager?.startRecording()
        
        // set up local audio view, this view will not show video but just a placeholder
        // Use actual uid (same as myUid) instead of 0, to match Android behavior
        let view = Bundle.loadVideoView(type: .local, audioOnly: true)
        view.uid = uid  // Set the actual UID, not 0
        self.audioViews[uid] = view  // Use uid as key, not 0
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
        
        // Add tap gesture to report user (same as Android's onClick)
        let tap = UITapGestureRecognizer(target: self, action: #selector(handleTap(_:)))
        view.addGestureRecognizer(tap)
        
        self.container.layoutStream3x2(views: self.sortedViews())
    }
    
    @objc private func handleTap(_ gesture: UITapGestureRecognizer) {
        if let videoView = gesture.view as? VideoView {
            // Now videoView.uid is the actual UID (myUid for local, or remote uid)
            reportUser(uid: videoView.uid)
        }
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // Register remote user for audio monitoring
        audioSafetyManager?.registerUser(uid)

        // set up remote audio view, this view will not show video but just a placeholder
        let view = Bundle.loadVideoView(type: .remote, audioOnly: true)
        view.uid = uid
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
        
        // Add tap gesture to report user (same as Android's onClick)
        let tap = UITapGestureRecognizer(target: self, action: #selector(handleTap(_:)))
        view.addGestureRecognizer(tap)
        
        self.container.layoutStream3x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // Unregister user when user leaves
        audioSafetyManager?.unregisterUser(uid)
        
        // remove remote audio view
        self.audioViews.removeValue(forKey: uid)
        self.container.layoutStream3x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didLeaveChannelWith stats: AgoraChannelStats) {
        LogUtils.log(message: "local user \(myUid) leaveChannel!", level: .info)
        
        // Stop recording when leaving channel
        audioSafetyManager?.stopRecording()
    }
    
    /// Reports which users are speaking, the speakers' volumes, and whether the local user is speaking.
    /// @params speakers volume info for all speakers
    /// @params totalVolume Total volume after audio mixing. The value range is [0,255].
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
//        for speaker in speakers {
//            if let audioView = audioViews[speaker.uid] {
//                audioView.setInfo(text: "Volume:\(speaker.volume)")
//            }
//        }
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        audioViews[myUid]?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        audioViews[myUid]?.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        audioViews[stats.uid]?.statsInfo?.updateAudioStats(stats)
    }
}

// Note: AgoraAudioFrameDelegate is now handled by AudioSafetyManager
// No need to implement it here as AudioSafetyManager will handle all audio frame callbacks
