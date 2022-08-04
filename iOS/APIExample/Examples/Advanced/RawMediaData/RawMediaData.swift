//
//  RawMediaData.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

var audioRecording = false

class RawMediaDataEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var audioRecordSwitch: UISwitch!
    let identifier = "RawMediaData"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        audioRecording = audioRecordSwitch.isOn
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)
    }
}

class RawMediaDataMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
//        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        
        agoraKit.setVideoFrameDelegate(self)
        // enable video module and set up video encoding configs
        // Enable video module should be after calling registerVideoFrameObserver
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        
        // setup raw media data observers
        agoraKit.setAudioFrameDelegate(self)
        
        agoraKit.setRecordingAudioFrameParametersWithSampleRate(44100, channel: 2, mode: .readOnly, samplesPerCall: 1024)
        agoraKit.setMixedAudioFrameParametersWithSampleRate(44100, channel: 1, samplesPerCall: 1024)
        agoraKit.setPlaybackAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 1024)
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: option)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
        
        if audioRecording {
            // start audio recording
            let config = AgoraAudioRecordingConfiguration()
            let tempPath = NSTemporaryDirectory() as NSString
            config.filePath = "\(tempPath)/audio.mp4"
            config.fileRecordOption = .mic
            config.quality = .medium
            config.sampleRate = 44100
            config.codec = false
            let ret = agoraKit.startAudioRecording(withConfig: config)
            LogUtils.log(message: "recording status \(ret)", level: .info)
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                if audioRecording {
                    agoraKit.stopAudioRecording()
                }
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

/// agora rtc engine delegate events
extension RawMediaDataMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
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
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}

extension RawMediaDataMain: AgoraAudioFrameDelegate {
    /**
     * Occurs when the recorded audio frame is received.
     * @param audioFrame A pointer to the audio frame: AgoraAudioFrame.
     * @return
     * - true: The recorded audio frame is valid and is encoded and sent.
     * - false: The recorded audio frame is invalid and is not encoded or sent.
     */
    func onRecord(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return false
    }
    
    func onPlaybackAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return false
    }
    
    func onMixedAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return false
    }
    
    func onPlaybackAudioFrame(beforeMixing frame: AgoraAudioFrame, channelId: String, uid: UInt) -> Bool {
        return false
    }

    
}

extension RawMediaDataMain: AgoraVideoFrameDelegate {
    /**
     * Occurs each time the SDK receives a video frame captured by the local camera.
     * Notes: If getVideoFrameProcessMode is read-only, use MUST implement this method
     *
     * After you successfully register the video frame observer, the SDK triggers this callback each time
     * a video frame is received. In this callback, you can get the video data captured by the local
     * camera. You can then pre-process the data according to your scenarios.
     *
     * After pre-processing, you can send the processed video data back to the SDK by setting the
     * `videoFrame` parameter in this callback.
     *
     * @param videoFrame A pointer to the video frame: AgoraOutputVideoFrame
     * @return Determines whether to ignore the current video frame if the pre-processing fails:
     * - true: Do not ignore.
     * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
     */
    func onCapture(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
//        LogUtils.log(message: "get captured video frame type:\(videoFrame.type), height: \(videoFrame.height), width: \(videoFrame.width)", level: .info)
        return false
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        return false
    }
    
    func onPreEncode(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
        return true
    }
    
    func getVideoFrameProcessMode() -> AgoraVideoFrameProcessMode {
        return .readOnly
    }
    
    func getVideoPixelFormatPreference() -> AgoraVideoFormat {
        return .cvPixel
    }
    
    func getMirrorApplied() -> Bool {
        return true
    }
}
