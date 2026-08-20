import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class <ExampleName>Main: BaseViewController {
    
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt = 0
    
    @IBOutlet weak var Container: AGEVideoContainer!
    
    // MARK: - Lifecycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        initializeAgoraEngine()
    }
    
    override func viewWillClose() {
        leaveChannel()
        super.viewWillClose()
    }
    
    // MARK: - Agora Engine Setup
    
    func initializeAgoraEngine() {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = .global
        
        agoraKit = AgoraRtcEngineKit(config: config, delegate: self)
        agoraKit.enableVideo()
        agoraKit.enableAudio()
    }
    
    func joinChannel() {
        let token = KeyCenter.Token(channelName: "test")
        agoraKit.joinChannel(byToken: token, channelName: "test", info: nil, uid: 0)
    }
    
    func leaveChannel() {
        agoraKit.leaveChannel(nil)
        agoraKit.destroy()
    }
}

// MARK: - AgoraRtcEngineDelegate

extension <ExampleName>Main: AgoraRtcEngineDelegate {
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        // Handle join success
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        // Handle remote user joined
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        // Handle remote user offline
    }
}
