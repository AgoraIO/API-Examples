//
//  SampleHandler.swift
//  Agora-ScreenShare-Extension
//
//  Created by zhaoyongqiang on 2024/3/29.
//

import ReplayKit

class SampleHandler: AgoraReplayKitHandler {

    override func broadcastStarted(withSetupInfo setupInfo: [String: NSObject]?) {
        // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
        super.broadcastStarted(withSetupInfo: setupInfo)
    }
    override func broadcastPaused() {
        // User has requested to pause the broadcast. Samples will stop being delivered.
        super.broadcastPaused()
    }
    
    override func broadcastResumed() {
        // User has requested to resume the broadcast. Samples delivery will resume.
        super.broadcastResumed()
    }
    
    override func broadcastFinished() {
        // User has requested to finish the broadcast.
        super.broadcastFinished()
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        super.processSampleBuffer(sampleBuffer, with: sampleBufferType)
    }
}
