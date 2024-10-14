//
//  PlayerObserver.swift
//  APIExample
//
//  Created by wushengtao on 2024/10/13.
//  Copyright © 2024 Agora Corp. All rights reserved.
//

import AgoraRtcKit

@objc protocol PlayerObserverDelegate: AnyObject {
    
    @objc optional func onStateChanged(oldState: AgoraRtePlayerState, newState: AgoraRtePlayerState, error: AgoraRteError?)
    
    @objc optional func onPositionChanged(currentTime: UInt64, utcTime: UInt64)
    
    @objc optional func onResolutionChanged(width: Int, height: Int)
    
    @objc optional func onEvent(event: AgoraRtePlayerEvent)
    
    @objc optional func onMetadata(type: AgoraRtePlayerMetadataType, data: Data)
    
    @objc optional func onPlayerInfoUpdated(info: AgoraRtePlayerInfo)
    
    @objc optional func onAudioVolumeIndication(volume: Int32)
}

class PlayerObserver: AgoraRtePlayerObserver {
    weak var delegate: PlayerObserverDelegate?
    
    func onStateChanged(oldState: AgoraRtePlayerState, newState: AgoraRtePlayerState, error: AgoraRteError?) {
        self.delegate?.onStateChanged?(oldState: oldState, newState: newState, error: error)
    }
    
    func onPositionChanged(currentTime: UInt64, utcTime: UInt64) {
        self.delegate?.onPositionChanged?(currentTime: currentTime, utcTime: utcTime)
    }
    
    func onResolutionChanged(width: Int, height: Int) {
        self.onResolutionChanged(width: width, height: height)
    }
    
    func onEvent(event: AgoraRtePlayerEvent) {
        self.onEvent(event: event)
    }
    
    func onMetadata(type: AgoraRtePlayerMetadataType, data: Data) {
        self.delegate?.onMetadata?(type: type, data: data)
    }
    
    func onPlayerInfoUpdated(info: AgoraRtePlayerInfo) {
        self.delegate?.onPlayerInfoUpdated?(info: info)
    }
    
    func onAudioVolumeIndication(volume: Int32) {
        
    }
}
