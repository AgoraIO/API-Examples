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

class ScreenShare: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var channelField: NSTextField!
    @IBOutlet weak var joinBtn: NSButton!
    @IBOutlet weak var leaveBtn: NSButton!
    @IBOutlet weak var resolutionPicker: NSPopUpButton!
    @IBOutlet weak var fpsPicker: NSPopUpButton!
    @IBOutlet weak var displayPicker: NSPopUpButton!
    @IBOutlet weak var displayShareBtn: NSButton!
    @IBOutlet weak var stopDisplayShareBtn: NSButton!
    @IBOutlet weak var windowPicker: NSPopUpButton!
    @IBOutlet weak var windowShareBtn: NSButton!
    @IBOutlet weak var stopWindowShareBtn: NSButton!
    @IBOutlet weak var layoutPicker: NSPopUpButton!
    @IBOutlet weak var updateConfigBtn: NSButton!
    @IBOutlet weak var shareHalfScreenBtn: NSButton!
    @IBOutlet weak var displayHintPicker: NSPopUpButton!
    var agoraKit: AgoraRtcEngineKit!
    var windowManager: WindowList = WindowList()
    var windowlist:[Window] = [], screenlist:[Window] = []

    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            joinBtn.isHidden = isJoined
            leaveBtn.isHidden = !isJoined
            layoutPicker.isEnabled = !isJoined
            windowShareBtn.isEnabled = isJoined
            displayShareBtn.isEnabled = isJoined
        }
    }
    
    var isWindowSharing: Bool = false {
        didSet{
            stopWindowShareBtn.isHidden = !isWindowSharing
            windowShareBtn.isHidden = isWindowSharing
            displayShareBtn.isEnabled = !isWindowSharing
            shareHalfScreenBtn.isEnabled = isWindowSharing
            updateConfigBtn.isEnabled = isWindowSharing
        }
    }
    
    var isDisplaySharing: Bool = false {
        didSet{
            stopDisplayShareBtn.isHidden = !isDisplaySharing
            displayShareBtn.isHidden = isDisplaySharing
            windowShareBtn.isEnabled = !isDisplaySharing
            shareHalfScreenBtn.isEnabled = isDisplaySharing
            updateConfigBtn.isEnabled = isDisplaySharing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        
        // prepare resolution picker
        resolutionPicker.addItems(withTitles: Configs.Resolutions.map({ (res:Resolution) -> String in
            return res.name()
        }))
        resolutionPicker.selectItem(at: Configs.defaultResolutionIdx)
        
        // prepare fps picker
        fpsPicker.addItems(withTitles: Configs.Fps.map({ (fps:Int) -> String in
            return "\(fps)fps"
        }))
        fpsPicker.selectItem(at: Configs.defaultFpsIdx)
        
        // prepare window manager and list
        windowManager.getList()
        screenlist = windowManager.items.filter({$0.type == .screen})
        windowlist = windowManager.items.filter({$0.type == .window})
        
        // prepare display picker
        displayPicker.addItems(withTitles: screenlist.map({"\($0.name ?? "Unknown")(\($0.id))"}))
        
        // prepare window picker
        windowPicker.addItems(withTitles: windowlist.map({"\($0.name ?? "Unknown")(\($0.id))"}))
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        //config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // this is mandatory to get camera list
        agoraKit.enableVideo()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if(isJoined) {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        // set myself as broadcaster to stream video/audio
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        let resolution = Configs.Resolutions[resolutionPicker.indexOfSelectedItem]
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution.size(),
                                                                             frameRate: AgoraVideoFrameRate(rawValue: Configs.Fps[fpsPicker.indexOfSelectedItem]) ?? .fps15,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        
        // set up local video to render your local camera preview
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelField.stringValue, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            localVideo.uid = uid
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
    
    @IBAction func onLeavePressed(_ sender: Any) {
        agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
            LogUtils.log(message: "Left channel", level: .info)
            self.videos[0].uid = nil
            self.isJoined = false
        }
    }
    
    @IBAction func onUpdateDisplayConfig(_ sender: Any) {
        //TODO
//        let params = AgoraScreenCaptureParameters()
//        let resolution = Configs.Resolutions[resolutionPicker.indexOfSelectedItem]
//        params.frameRate = Configs.Fps[fpsPicker.indexOfSelectedItem]
//        params.dimensions = resolution.size()
//        agoraKit.update(params)
//        agoraKit.setScreenCapture(Configs.VideoContentHints[displayHintPicker.indexOfSelectedItem])
    }
    
    @IBAction func onStartShareHalfScreen(_ sender: Any) {
        let rect = NSScreen.main?.frame
        let region = NSMakeRect(0,0,rect!.width/2,rect!.height/2)
        agoraKit.updateScreenCaptureRegion(region)
    }
    
    @IBAction func onLayoutChanged(_ sender: NSPopUpButton) {
        switch(sender.indexOfSelectedItem) {
            //1x1
        case 0:
            layoutVideos(2)
            break
            //1x3
        case 1:
            layoutVideos(4)
            break
            //1x8
        case 2:
            layoutVideos(9)
            break
            //1x15
        case 3:
            layoutVideos(16)
            break
        default:
            layoutVideos(2)
        }
    }
    
    @IBAction func onDisplayShare(_ sender: NSButton) {
        //TODO
//        let screen = screenlist[displayPicker.indexOfSelectedItem]
//        let params = AgoraScreenCaptureParameters()
//        let result = agoraKit.startScreenCapture(byDisplayId: UInt(screen.id), rectangle: .zero, parameters:params)
//        if result != 0 {
//            // Usually happens with invalid parameters
//            // Error code description can be found at:
//            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
//            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
//            self.showAlert(title: "Error", message: "startScreenCapture call failed: \(result), please check your params")
//        }
//        isDisplaySharing = true
    }
    
    @IBAction func onWindowShare(_ sender: NSButton) {
        //TODO
//        let window = windowlist[windowPicker.indexOfSelectedItem]
//        let params = AgoraScreenCaptureParameters()
//        let result = agoraKit.startScreenCapture(byWindowId: UInt(window.id), rectangle: .zero, parameters: params)
//        if result != 0 {
//            // Usually happens with invalid parameters
//            // Error code description can be found at:
//            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
//            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
//            self.showAlert(title: "Error", message: "startScreenCapture call failed: \(result), please check your params")
//        }
//        isWindowSharing = true
    }
    
    @IBAction func stopWindowShare(_ send: NSButton) {
        agoraKit.stopScreenCapture()
        isWindowSharing = false
    }
    
    @IBAction func stopDisplayShare(_ send: NSButton) {
        agoraKit.stopScreenCapture()
        isDisplaySharing = false
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
        container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension ScreenShare: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = remoteVideo.videocanvas
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
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
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = nil
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
}
