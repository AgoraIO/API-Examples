import Cocoa
import AgoraRtcKit

// Skeleton: replace <ExampleName> and wire the case UI/storyboard and permissions.
// This minimal join publishes microphone audio. Configure video/canvases only if needed.
class <ExampleName>Main: BaseViewController {
    var agoraKit: AgoraRtcEngineKit?
    private var tokenRequestID = 0

    override func viewDidLoad() {
        super.viewDidLoad()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
    }

    // Call on the main queue after the case has obtained microphone permission.
    func joinChannel(channelName: String) {
        guard !channelName.isEmpty, agoraKit != nil else { return }
        tokenRequestID += 1
        let requestID = tokenRequestID
        NetworkManager.shared.generateToken(channelName: channelName, uid: 0) { [weak self] token in
            DispatchQueue.main.async { [weak self] in
                guard let self = self, self.tokenRequestID == requestID,
                      let engine = self.agoraKit else { return }
                if !(KeyCenter.Certificate ?? "").isEmpty && (token ?? "").isEmpty {
                    LogUtils.log(message: "Token request failed", level: .error)
                    return
                }
                let options = AgoraRtcChannelMediaOptions()
                options.channelProfile = .liveBroadcasting
                options.clientRoleType = .broadcaster
                options.publishMicrophoneTrack = true
                options.publishCameraTrack = false
                let result = engine.joinChannel(byToken: token, channelId: channelName,
                                                uid: 0, mediaOptions: options)
                if result != 0 {
                    LogUtils.log(message: "joinChannel failed: \(result)", level: .error)
                }
            }
        }
    }

    func leaveChannel() {
        tokenRequestID += 1 // Discard Token responses after leave or scene removal.
        agoraKit?.leaveChannel(nil)
    }

    override func viewWillBeRemovedFromSplitView() {
        leaveChannel()
        // Stop any case-owned preview, capture, player, timer or observer here.
        if agoraKit != nil {
            AgoraRtcEngineKit.destroy()
            agoraKit = nil
        }
        super.viewWillBeRemovedFromSplitView()
    }
}

extension <ExampleName>Main: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String,
                   withUid uid: UInt, elapsed: Int) {
        DispatchQueue.main.async { [weak self] in
            guard let self = self, self.agoraKit === engine else { return }
            LogUtils.log(message: "Joined channel with UID \(uid)", level: .info)
        }
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        DispatchQueue.main.async { [weak self] in
            guard let self = self, self.agoraKit === engine else { return }
            LogUtils.log(message: "RTC error: \(errorCode.rawValue)", level: .error)
        }
    }

    // For expiring tokens, implement tokenPrivilegeWillExpire using the same channel/UID,
    // validate the pending request, then call renewToken(_:). See review-case/SKILL.md.
}
