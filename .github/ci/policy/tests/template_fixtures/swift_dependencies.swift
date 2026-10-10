// Controlled SDK/UI doubles: these check template control flow, not RTC SDK compatibility.
import Foundation
protocol ObservableObject {}
class UIViewController: NSObject {
    func viewDidLoad() {}
    func willMove(toParent parent: UIViewController?) {}
}
class BaseViewController: UIViewController {}
protocol AgoraRtcEngineDelegate: AnyObject {}
enum AgoraErrorCode: Int { case failed = 1 }
enum Role { case broadcaster }
enum Profile { case `default` }
class AgoraRtcEngineConfig { var appId = "" }
class AgoraRtcChannelMediaOptions {
    var clientRoleType = Role.broadcaster
    var publishMicrophoneTrack = false
}
class AgoraRtcEngineKit {
    static var instances: [AgoraRtcEngineKit] = []
    static var destroys = 0
    static var joinResult: Int32 = 0
    var destroyed = false
    var leaves = 0
    var joins: [(String, UInt, String?)] = []
    static func sharedEngine(with config: AgoraRtcEngineConfig, delegate: AgoraRtcEngineDelegate) -> AgoraRtcEngineKit {
        let engine = AgoraRtcEngineKit()
        instances.append(engine)
        return engine
    }
    static func destroy() {
        destroys += 1
        instances.last?.destroyed = true
    }
    func leaveChannel(_ callback: (() -> Void)? = nil) {
        precondition(!destroyed, "leave after destroy")
        leaves += 1
    }
    func joinChannel(byToken token: String?, channelId: String, uid: UInt,
                     mediaOptions: AgoraRtcChannelMediaOptions) -> Int32 {
        precondition(Thread.isMainThread, "join off main")
        precondition(!destroyed, "join after destroy")
        joins.append((channelId, uid, token))
        return Self.joinResult
    }
    func setAudioProfile(_ profile: Profile) {}
}
enum KeyCenter { static let AppId = ""; static var Certificate: String? }
enum LogUtils {
    enum Level { case info, error }
    static var messages: [String] = []
    static func log(message: String, level: Level) { messages.append(message) }
}
class NetworkManager {
    static let shared = NetworkManager()
    var requests: [(String, UInt, (String?) -> Void)] = []
    // The runner inserts the project's real generateToken declaration here.
    __TOKEN_DECLARATION__ {
        requests.append((channelName, uid, success))
    }
}
