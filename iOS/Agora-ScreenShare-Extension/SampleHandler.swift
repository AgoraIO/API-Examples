//
//  SampleHandler.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class SampleHandler: RPBroadcastSampleHandler {
    
    var bufferCopy: CMSampleBuffer?
    var lastSendTs: Int64 = Int64(Date().timeIntervalSince1970 * 1000)
    var timer: Timer?
    
    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
        
        if let setupInfo = setupInfo, let channel = setupInfo["channelName"] as? String {
            //In-App Screen Capture
            AgoraUploader.startBroadcast(to: channel)
        } else {
            // iOS Screen Record and Broadcast
            // IMPORTANT
            // You have to use App Group to pass information/parameter
            // from main app to extension
            // in this demo we don't introduce app group as it increases complexity
            // this is the reason why channel name is hardcoded to be ScreenShare
            // You may use a dynamic channel name through keychain or userdefaults
            // after enable app group feature
            AgoraUploader.startBroadcast(to: "ScreenShare")
        }
        DispatchQueue.main.async {
            self.timer = Timer.scheduledTimer(withTimeInterval: 0.1, repeats: true) {[weak self] (timer:Timer) in
                guard let weakSelf = self else {return}
                let elapse = Int64(Date().timeIntervalSince1970 * 1000) - weakSelf.lastSendTs
                print("elapse: \(elapse)")
                // if frame stopped sending for too long time, resend the last frame
                // to avoid stream being frozen when viewed from remote
                if(elapse > 300) {
                    if let buffer = weakSelf.bufferCopy {
                        weakSelf.processSampleBuffer(buffer, with: .video)
                    }
                }
            }
        }
    }
    
    override func broadcastPaused() {
        // User has requested to pause the broadcast. Samples will stop being delivered.
    }
    
    override func broadcastResumed() {
        // User has requested to resume the broadcast. Samples delivery will resume.
    }
    
    override func broadcastFinished() {
        timer?.invalidate()
        timer = nil
        AgoraUploader.stopBroadcast()
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        DispatchQueue.main.async {[weak self] in
            switch sampleBufferType {
            case .video:
                if let weakSelf = self {
                    weakSelf.bufferCopy = sampleBuffer
                    weakSelf.lastSendTs = Int64(Date().timeIntervalSince1970 * 1000)
                }
                AgoraUploader.sendVideoBuffer(sampleBuffer)
            case .audioApp:
                AgoraUploader.sendAudioAppBuffer(sampleBuffer)
                break
            case .audioMic:
                AgoraUploader.sendAudioMicBuffer(sampleBuffer)
                break
            @unknown default:
                break
            }
        }
    }
}
