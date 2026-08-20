// ❌ INCORRECT: Direct UI update from callback
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
    self.statusLabel.stringValue = "Joined channel"  // WRONG: may crash
}
