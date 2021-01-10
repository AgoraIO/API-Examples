//
//  VideoMetadata.swift
//  APIExample
//
//  Created by Dong Yifan on 2020/5/27.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import Foundation
import UIKit
import AgoraRtcKit
import AGEVideoLayout

class VideoMetadataEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "VideoMetadata"
    
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

class VideoMetadataMain: BaseViewController {
    @IBOutlet weak var sendMetadataButton: UIButton!
    
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    @IBOutlet weak var container: AGEVideoContainer!
    
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            sendMetadataButton.isHidden = !isJoined
        }
    }
    
    // video metadata to be sent later
    var metadata: Data?
    // metadata lenght limitation
    let MAX_META_LENGTH = 1024
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        sendMetadataButton.isHidden = true
        
        // layout render view
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // register metadata delegate and datasource
        agoraKit.setMediaMetadataDataSource(self, with: .video)
        agoraKit.setMediaMetadataDelegate(self, with: .video)
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
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
        if(result != 0) {
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
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    /// callback when send metadata button hit
    @IBAction func onSendMetadata() {
        self.metadata = "\(Date())".data(using: .utf8)
    }
    
}

/// agora rtc engine delegate events
extension VideoMetadataMain: AgoraRtcEngineDelegate {
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

/// AgoraMediaMetadataDelegate and AgoraMediaMetadataDataSource
extension VideoMetadataMain : AgoraMediaMetadataDelegate, AgoraMediaMetadataDataSource {
    func metadataMaxSize() -> Int {
        // the data to send should not exceed this size
        return MAX_META_LENGTH
    }
    
    /// Callback when the SDK is ready to send metadata.
    /// You need to specify the metadata in the return value of this method.
    /// Ensure that the size of the metadata that you specify in this callback does not exceed the value set in the metadataMaxSize callback.
    /// @param timestamp The timestamp (ms) of the current metadata.
    /// @return The metadata that you want to send in the format of Data
    func readyToSendMetadata(atTimestamp timestamp: TimeInterval, sourceType: AgoraVideoSourceType) -> Data? {
        guard let metadata = self.metadata else {return nil}
        
        // clear self.metadata to nil after any success send to avoid redundancy
        self.metadata = nil
        
        if(metadata.count > MAX_META_LENGTH) {
            //if data exceeding limit, return nil to not send anything
            LogUtils.log(message: "invalid metadata: length exceeds \(MAX_META_LENGTH)", level: .info)
            return nil
        }
        LogUtils.log(message: "metadata sent", level: .info)
        self.metadata = nil
        return metadata
    }
    
    /// Callback when the local user receives the metadata.
    /// @param data The received metadata.
    /// @param uid The ID of the user who sends the metadata.
    /// @param timestamp The timestamp (ms) of the received metadata.
    func receiveMetadata(_ data: Data, fromUser uid: Int, atTimestamp timestamp: TimeInterval) {
        DispatchQueue.main.async {
            LogUtils.log(message: "metadata received", level: .info)
            let alert = UIAlertController(title: "Metadata received", message: String(data: data, encoding: .utf8), preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            self.present(alert, animated: true, completion: nil)
        }
    }
    
}
