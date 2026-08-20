// Thread Safety Pattern - Always dispatch UI updates to main thread

extension ExampleMain: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        // Callbacks may arrive on background thread
        DispatchQueue.main.async {
            self.updateUI()
        }
    }
}
