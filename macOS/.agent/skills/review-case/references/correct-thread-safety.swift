// Correct: Thread-safe UI updates

func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
    // Callback may arrive on background thread
    DispatchQueue.main.async {
        self.statusLabel.stringValue = "Joined channel"
    }
}
