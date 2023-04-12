//
//  MediaInfo.swift
//  OpenVideoCall
//
//  Created by GongYuhua on 4/11/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import Foundation
import AgoraRtcKit

struct StatisticsInfo {
    struct LocalInfo {
        var channelStats = AgoraChannelStats()
        var videoStats = AgoraRtcLocalVideoStats()
        var audioStats = AgoraRtcLocalAudioStats()
    }
    
    struct RemoteInfo {
        var videoStats = AgoraRtcRemoteVideoStats()
        var audioStats = AgoraRtcRemoteAudioStats()
    }
    
    enum StatisticsType {
        case local(LocalInfo), remote(RemoteInfo)
        
        var isLocal: Bool {
            switch self {
            case .local:  return true
            case .remote: return false
            }
        }
    }
    
    var dimension = CGSize.zero
    var fps:UInt = 0
    var firstFrameElapsedTime: Double = 0
    
    var type: StatisticsType
    
    init(type: StatisticsType) {
        self.type = type
    }
    
    mutating func updateChannelStats(_ stats: AgoraChannelStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.channelStats = stats
            self.type = .local(new)
        default:
            break
        }
    }
    
    mutating func updateLocalVideoStats(_ stats: AgoraRtcLocalVideoStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.videoStats = stats
            self.type = .local(new)
        default:
            break
        }
        dimension = CGSize(width: Int(stats.encodedFrameWidth), height: Int(stats.encodedFrameHeight))
        fps = stats.sentFrameRate
    }
    
    mutating func updateLocalAudioStats(_ stats: AgoraRtcLocalAudioStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.audioStats = stats
            self.type = .local(new)
        default:
            break
        }
    }
    
    mutating func updateVideoStats(_ stats: AgoraRtcRemoteVideoStats) {
        switch type {
        case .remote(let info):
            var new = info
            new.videoStats = stats
            dimension = CGSize(width: Int(stats.width), height: Int(stats.height))
            fps = UInt(stats.rendererOutputFrameRate)
            self.type = .remote(new)
        default:
            break
        }
    }
    
    mutating func updateAudioStats(_ stats: AgoraRtcRemoteAudioStats) {
        switch type {
        case .remote(let info):
            var new = info
            new.audioStats = stats
            self.type = .remote(new)
        default:
            break
        }
    }
    mutating func updateFirstFrameInfo(_ info: AgoraVideoRenderingTracingInfo) {
        firstFrameElapsedTime = Double(info.elapsedTime)
    }
    
    func description(audioOnly:Bool) -> String {
        var full: String
        switch type {
        case .local(let info):  full = localDescription(info: info, audioOnly: audioOnly)
        case .remote(let info): full = remoteDescription(info: info, audioOnly: audioOnly)
        }
        return full
    }
    
    func localDescription(info: LocalInfo, audioOnly: Bool) -> String {
        
        let dimensionFps = "\(Int(dimension.width))×\(Int(dimension.height)),\(fps)fps"
        
        let lastmile = "LM Delay: \(info.channelStats.lastmileDelay)ms"
        let videoSend = "VSend: \(info.videoStats.sentBitrate)kbps"
        let audioSend = "ASend: \(info.audioStats.sentBitrate)kbps"
        let cpu = "CPU: \(info.channelStats.cpuAppUsage)%/\(info.channelStats.cpuTotalUsage)%"
        //TODO
//        let vSendLoss = "VSend Loss: \(info.videoStats.txPacketLossRate)%"
//        let aSendLoss = "ASend Loss: \(info.audioStats.txPacketLossRate)%"
        let vSendLoss = "VSend Loss: MISSING%"
        let aSendLoss = "ASend Loss: MISSING%"
        
        let firstFrame = "firstFrameTime: \(firstFrameElapsedTime)"
        
        if(audioOnly) {
            let array = firstFrameElapsedTime > 0 ? [firstFrame, lastmile,audioSend,cpu,aSendLoss] : [lastmile,audioSend,cpu,aSendLoss]
            return array.joined(separator: "\n")
        }
        let array = firstFrameElapsedTime > 0 ? [firstFrame, dimensionFps,lastmile,videoSend,audioSend,cpu,vSendLoss,aSendLoss] : [dimensionFps,lastmile,videoSend,audioSend,cpu,vSendLoss,aSendLoss]
        return array.joined(separator: "\n")
    }
    
    func remoteDescription(info: RemoteInfo, audioOnly: Bool) -> String {
        let dimensionFpsBit = "\(Int(dimension.width))×\(Int(dimension.height)), \(fps)fps"
        
        var audioQuality: AgoraNetworkQuality
        if let quality = AgoraNetworkQuality(rawValue: info.audioStats.quality) {
            audioQuality = quality
        } else {
            audioQuality = AgoraNetworkQuality.unknown
        }
        
        let firstFrame = "firstFrameTime: \(firstFrameElapsedTime)"
        let videoRecv = "VRecv: \(info.videoStats.receivedBitrate)kbps"
        let audioRecv = "ARecv: \(info.audioStats.receivedBitrate)kbps"
        
        let videoLoss = "VLoss: \(info.videoStats.packetLossRate)%"
        let audioLoss = "ALoss: \(info.audioStats.audioLossRate)%"
        let aquality = "AQuality: \(audioQuality.description())"
        if(audioOnly) {
            let array = firstFrameElapsedTime > 0 ? [firstFrame, audioRecv,audioLoss,aquality] : [audioRecv,audioLoss,aquality]
            return array.joined(separator: "\n")
        }
        let array = firstFrameElapsedTime > 0 ? [firstFrame, dimensionFpsBit,videoRecv,audioRecv,videoLoss,audioLoss,aquality] : [dimensionFpsBit,videoRecv,audioRecv,videoLoss,audioLoss,aquality]
        return array.joined(separator: "\n")
    }
}
