//
//  SampleHandler.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class SampleHandler: AgoraReplayKitHandler, AgoraReplayKitExtDelegate {
    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
        // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
        AgoraReplayKitExt.shareInstance().start(self)
    }
    override func broadcastPaused() {
        // User has requested to pause the broadcast. Samples will stop being delivered.
        AgoraReplayKitExt.shareInstance().pause()
    }
    
    override func broadcastResumed() {
        // User has requested to resume the broadcast. Samples delivery will resume.
        AgoraReplayKitExt.shareInstance().resume()
    }
    
    override func broadcastFinished() {
        // User has requested to finish the broadcast.
        AgoraReplayKitExt.shareInstance().stop()
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        AgoraReplayKitExt.shareInstance().push(sampleBuffer, with: sampleBufferType)
    }
    
    func broadcastFinished(_ broadcast: AgoraReplayKitExt, reason: AgoraReplayKitExtReason) {
        switch reason {
        case AgoraReplayKitExtReasonInitiativeStop:
            print("AgoraReplayKitExtReasonInitiativeStop")
            
        case AgoraReplayKitExtReasonConnectFail:
            print("AgoraReplayKitExReasonConnectFail")
            
        case AgoraReplayKitExtReasonDisconnect:
            print("AgoraReplayKitExReasonDisconnect")
            
        default: break
        }
    }
}
