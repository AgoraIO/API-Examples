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
        AgoraReplayKitExt.shareInstance().start(self)
    }
    
    override func broadcastPaused() {
        AgoraReplayKitExt.shareInstance().pause()
    }
    
    override func broadcastResumed() {
        AgoraReplayKitExt.shareInstance().resume()
    }
    
    override func broadcastFinished() {
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
            let error = NSError(domain: "ConnectFail", code: 0, userInfo: nil)
            finishBroadcastWithError(error)
            break
        case .disconnect:
            let error = NSError(domain: "Disconnect", code: 0, userInfo: nil)
            finishBroadcastWithError(error)
            break
        case .initiativeStop:
            // Pass nil in objc mathod to avoid showing alert view
            SampleHandlerUtil.finishBroadcast(withNilError: self)
            break
        default: break
        }
    }
}
