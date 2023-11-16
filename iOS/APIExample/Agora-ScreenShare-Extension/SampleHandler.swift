//
//  SampleHandler.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class SampleHandler: AgoraReplayKitHandler {
    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
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
