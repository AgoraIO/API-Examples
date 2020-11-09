//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class RawMediaDataEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
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
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

class RawMediaDataMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    var agoraMediaDataPlugin: AgoraMediaDataPlugin?
    
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
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        // setup raw media data observers
        agoraMediaDataPlugin = AgoraMediaDataPlugin(agoraKit: agoraKit)
        
        // Register audio observer
        let audioType:ObserverAudioType = ObserverAudioType(rawValue: ObserverAudioType.recordAudio.rawValue | ObserverAudioType.playbackAudioFrameBeforeMixing.rawValue | ObserverAudioType.mixedAudio.rawValue | ObserverAudioType.playbackAudio.rawValue);
        agoraMediaDataPlugin?.registerAudioRawDataObserver(audioType)
        agoraMediaDataPlugin?.audioDelegate = self
        
        agoraKit.setRecordingAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 4410)
        agoraKit.setMixedAudioFrameParametersWithSampleRate(44100, channel: 1, samplesPerCall: 4410)
        agoraKit.setPlaybackAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 4410)

        // Register video observer
        let videoType:ObserverVideoType = ObserverVideoType(rawValue: ObserverVideoType.captureVideo.rawValue | ObserverVideoType.renderVideo.rawValue | ObserverVideoType.preEncodeVideo.rawValue)
        agoraMediaDataPlugin?.registerVideoRawDataObserver(videoType)
        agoraMediaDataPlugin?.videoDelegate = self;

        // Register packet observer
        let packetType:ObserverPacketType = ObserverPacketType(rawValue: ObserverPacketType.sendAudio.rawValue | ObserverPacketType.sendVideo.rawValue | ObserverPacketType.receiveAudio.rawValue | ObserverPacketType.receiveVideo.rawValue)
        agoraMediaDataPlugin?.registerPacketRawDataObserver(packetType)
        agoraMediaDataPlugin?.packetDelegate = self;
        
        
        
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
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelName, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        }
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                // deregister observers
                agoraMediaDataPlugin?.deregisterAudioRawDataObserver(ObserverAudioType(rawValue: 0))
                agoraMediaDataPlugin?.deregisterVideoRawDataObserver(ObserverVideoType(rawValue: 0))
                agoraMediaDataPlugin?.deregisterPacketRawDataObserver(ObserverPacketType(rawValue: 0))
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

// audio data plugin, here you can process raw audio data
// note this all happens in CPU so it comes with a performance cost
extension RawMediaDataMain : AgoraAudioDataPluginDelegate
{
    /// Retrieves the recorded audio frame.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didRecord audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the audio playback frame for getting the audio.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPlaybackAudioRawData audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the audio frame of a specified user before mixing.
    /// The SDK triggers this callback if isMultipleChannelFrameWanted returns false.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPlaybackBeforeMixing audioRawData: AgoraAudioRawData, ofUid uid: uint) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the mixed recorded and playback audio frame.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didMixedAudioRawData audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
}

// video data plugin, here you can process raw video data
// note this all happens in CPU so it comes with a performance cost
extension RawMediaDataMain : AgoraVideoDataPluginDelegate
{
    /// Occurs each time the SDK receives a video frame captured by the local camera.
    /// After you successfully register the video frame observer, the SDK triggers this callback each time a video frame is received. In this callback, you can get the video data captured by the local camera. You can then pre-process the data according to your scenarios.
    /// After pre-processing, you can send the processed video data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didCapturedVideoRawData videoRawData: AgoraVideoRawData) -> AgoraVideoRawData {
        return videoRawData
    }
    
    /// Occurs each time the SDK receives a video frame before sending to encoder
    /// After you successfully register the video frame observer, the SDK triggers this callback each time a video frame is going to be sent to encoder. In this callback, you can get the video data before it is sent to enoder. You can then pre-process the data according to your scenarios.
    /// After pre-processing, you can send the processed video data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPreEncode videoRawData: AgoraVideoRawData) -> AgoraVideoRawData {
        return videoRawData
    }
    
    /// Occurs each time the SDK receives a video frame sent by the remote user.
    ///After you successfully register the video frame observer and isMultipleChannelFrameWanted return false, the SDK triggers this callback each time a video frame is received. In this callback, you can get the video data sent by the remote user. You can then post-process the data according to your scenarios.
    ///After post-processing, you can send the processed data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willRenderVideoRawData videoRawData: AgoraVideoRawData, ofUid uid: uint) -> AgoraVideoRawData {
        return videoRawData
    }
}

// packet data plugin, here you can process raw network packet(before decoding/encoding)
// note this all happens in CPU so it comes with a performance cost
extension RawMediaDataMain : AgoraPacketDataPluginDelegate
{
    /// Occurs when the local user sends a video packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willSendVideoPacket videoPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return videoPacket
    }
    
    /// Occurs when the local user sends an audio packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willSendAudioPacket audioPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return audioPacket
    }
    
    /// Occurs when the local user receives a video packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didReceivedVideoPacket videoPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return videoPacket
    }
    
    /// Occurs when the local user receives an audio packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didReceivedAudioPacket audioPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return audioPacket
    }
}
