import Foundation
func drain() {
    var done = false
    DispatchQueue.main.async { done = true }
    while !done { RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.001)) }
}
func request(_ sut: TestSubject, _ channel: String) {
    sut.requestJoin(channelName: channel) { $0(true) }
    drain()
}
func respond(_ index: Int, _ token: String? = "test-response") {
    // The callback may arrive off main. The template must serialize it with teardown.
    DispatchQueue.global().sync { NetworkManager.shared.requests[index].2(token) }
    drain()
}
func totalJoins() -> Int { AgoraRtcEngineKit.instances.reduce(0) { $0 + $1.joins.count } }
let sut = TestSubject()
setup(sut)
let first = AgoraRtcEngineKit.instances.last!
setup(sut)
precondition(AgoraRtcEngineKit.instances.count == 1, "setup must be idempotent")

// Exit while a token is pending; leave is required even without didJoinChannel.
request(sut, "old")
sut.onDestroy()
sut.onDestroy()
respond(0)
precondition(totalJoins() == 0 && first.leaves == 1 && AgoraRtcEngineKit.destroys == 1)

// Reopen the same retained owner. An older response cannot use the new engine.
setup(sut)
request(sut, "fresh")
respond(0)
precondition(totalJoins() == 0)
respond(1)
precondition(totalJoins() == 1)
let second = AgoraRtcEngineKit.instances.last!
precondition(second.joins[0].0 == "fresh" && second.joins[0].1 == 0)

// Leave without destroy also invalidates pending requests.
sut.leaveChannel()
request(sut, "leaving")
sut.leaveChannel()
respond(2)
precondition(totalJoins() == 1)

// Latest request wins even when older replies arrive last.
request(sut, "superseded")
request(sut, "latest")
respond(4)
respond(3)
precondition(totalJoins() == 2 && second.joins.last!.0 == "latest")

// Delayed permission grant after teardown must not even request a token.
var permission: ((Bool) -> Void)?
sut.requestJoin(channelName: "permission-old") { permission = $0 }
let requestsBeforeExit = NetworkManager.shared.requests.count
sut.onDestroy()
setup(sut)
permission?(true)
drain()
precondition(NetworkManager.shared.requests.count == requestsBeforeExit)

// Denial and certificate-enabled token failure never join; no-certificate nil is valid.
sut.requestJoin(channelName: "denied") { $0(false) }
drain()
precondition(NetworkManager.shared.requests.count == requestsBeforeExit)
KeyCenter.Certificate = "enabled-in-test"
request(sut, "missing-token")
respond(requestsBeforeExit, nil)
precondition(totalJoins() == 2)
KeyCenter.Certificate = nil
request(sut, "no-certificate")
respond(requestsBeforeExit + 1, nil)
precondition(totalJoins() == 3)
AgoraRtcEngineKit.joinResult = -1
request(sut, "join-error")
respond(requestsBeforeExit + 2)
precondition(LogUtils.messages.contains { $0.contains("joinChannel failed") })
precondition(!LogUtils.messages.contains { $0.contains("test-response") })
sut.onDestroy()
print("PASS: pending exit, repeated cleanup/setup, reopen, leave, reordered responses, permission and token failures, join error")
