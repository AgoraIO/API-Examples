// Correct: Thread-safe UI updates

func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
    // Callback may arrive on background thread
    DispatchQueue.main.async { [weak self] in
        guard let self = self, self.agoraKit === engine else { return }
        self.statusLabel.stringValue = "Joined channel"
    }
}
