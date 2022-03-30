//
//  SampleHandler.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class SampleHandler: RPBroadcastSampleHandler {
    
    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
        print("broadcastStarted")
        AgoraReplayKitExt.shareInstance().start(self)
    }
    
    override func broadcastPaused() {
        print("broadcastPaused")
        AgoraReplayKitExt.shareInstance().pause()
    }
    
    override func broadcastResumed() {
        print("broadcastResumed")
        AgoraReplayKitExt.shareInstance().resume()
    }
    
    override func broadcastFinished() {
        print("broadcastFinished")
        AgoraReplayKitExt.shareInstance().stop()
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        AgoraReplayKitExt.shareInstance().push(sampleBuffer, with: sampleBufferType)
    }
}

extension SampleHandler: AgoraReplayKitExtDelegate {
    func broadcastFinished(_ broadcast: AgoraReplayKitExt, reason: AgoraReplayKitExtReason) {
        print("broadcastFinished:\(reason.rawValue)")
        switch reason {
        case .connectFail:
            let error = NSError(domain: "", code: 0, userInfo: nil)
            finishBroadcastWithError(error)
            break
        case .disconnect:
            let error = NSError(domain: "", code: 0, userInfo: nil)
            finishBroadcastWithError(error)
            break
        case .initiativeStop:
            let error = NSError(domain: "", code: 0, userInfo: nil)
            finishBroadcastWithError(error)
            break
        default: break
        }
    }
}
