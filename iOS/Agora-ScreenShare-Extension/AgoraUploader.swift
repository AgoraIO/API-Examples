//
//  AgoraUploader.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/1/16.
//  Copyright © 2017年 Agora. All rights reserved.
//

import Foundation
import CoreMedia
import ReplayKit
import AgoraRtcKit

class AgoraUploader {
    private static let videoDimension : CGSize = {
        let screenSize = UIScreen.main.currentMode!.size
        var boundingSize = CGSize(width: 720, height: 1280)
        let mW = boundingSize.width / screenSize.width
        let mH = boundingSize.height / screenSize.height
        if( mH < mW ) {
            boundingSize.width = boundingSize.height / screenSize.height * screenSize.width
        }
        else if( mW < mH ) {
            boundingSize.height = boundingSize.width / screenSize.width * screenSize.height
        }
        return boundingSize
    }()
    
    private static let audioSampleRate: UInt = 48000
    private static let audioChannels: UInt = 2
    
    private static var sharedAgoraEngine: AgoraRtcEngineKit?
    
    private static var connectionId: UInt32 = 0
    private static var channelId: String?
    
    private static func initAgoraEngine() -> AgoraRtcEngineKit {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.audioScenario = .default
        config.channelProfile = .liveBroadcasting
        
        let kit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: nil)
        kit.setClientRole(.broadcaster)
        
        kit.enableVideo()
        let videoConfig = AgoraVideoEncoderConfiguration(size: videoDimension,
                                                         frameRate: .fps24,
                                                         bitrate: AgoraVideoBitrateStandard,
                                                         orientationMode: .adaptative,
                                                         mirrorMode: .auto)
        kit.setVideoEncoderConfiguration(videoConfig)
        kit.setAudioProfile(.musicStandardStereo)
        // MIGRATED
        //kit.setExternalAudioSource(true, sampleRate: Int(audioSampleRate), channels: Int(audioChannels))
        kit.setExternalVideoSource(true, useTexture: true, encodedFrame: true)
        kit.setExternalAudioSource(true, sampleRate: Int(audioSampleRate), channels: Int(audioChannels))
        
        //kit.muteAllRemoteVideoStreams(true)
        //kit.muteAllRemoteAudioStreams(true)
        return kit
    }
    
    static func startBroadcast(to channel: String) {
        print("joining \(channel)")
        if sharedAgoraEngine == nil {
            sharedAgoraEngine = initAgoraEngine()
        }
        guard let agoraEngine = sharedAgoraEngine else {
            return
        }
        channelId = channel
        let option = AgoraRtcChannelMediaOptions()
        
        option.publishCameraTrack = .of(false)
        option.publishAudioTrack = .of(false)
        option.autoSubscribeAudio = .of(false)
        option.autoSubscribeVideo = .of(false)

        option.publishCustomVideoTrack = .of(true)
        option.publishCustomAudioTrack = .of(true)
        option.enableAudioRecordingOrPlayout = .of(false)
        option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))

        //sharedAgoraEngine.startPreview()
        //sharedAgoraEngine.joinChannel(byToken: nil, channelId: channel, uid: SCREEN_SHARE_UID, mediaOptions: option, joinSuccess: nil)
        agoraEngine.joinChannelEx(
            byToken: KeyCenter.Token,
            channelId: channel,
            uid: SCREEN_SHARE_UID,
            connectionId: &connectionId,
            delegate: Listener(),
            mediaOptions: option)
        { channel, uid, elapsed in
            print("joinChannelEx \(channel) with uid \(uid) elapsed \(elapsed)ms")
            let videoConfig = AgoraVideoEncoderConfiguration(size: videoDimension,
                                                             frameRate: .fps24,
                                                             bitrate: AgoraVideoBitrateStandard,
                                                             orientationMode: .adaptative,
                                                             mirrorMode: .auto)
            agoraEngine.setVideoEncoderConfigurationEx(videoConfig, connectionId: connectionId)
        }
    }
    
    static func sendVideoBuffer(_ sampleBuffer: CMSampleBuffer) {
        guard let videoFrame = CMSampleBufferGetImageBuffer(sampleBuffer),
              let agoraEngine = sharedAgoraEngine else { return }
        
        var rotation : Int32 = 0
        if let orientationAttachment = CMGetAttachment(sampleBuffer, key: RPVideoSampleOrientationKey as CFString, attachmentModeOut: nil) as? NSNumber {
            if let orientation = CGImagePropertyOrientation(rawValue: orientationAttachment.uint32Value) {
                switch orientation {
                case .up,    .upMirrored:    rotation = 0
                case .down,  .downMirrored:  rotation = 180
                case .left,  .leftMirrored:  rotation = 90
                case .right, .rightMirrored: rotation = 270
                default:   break
                }
            }
        }
        
        //let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
        let time = CMTime(seconds: CACurrentMediaTime(), preferredTimescale: 1000)
        
        let frame = AgoraVideoFrame()
        frame.format = 12
        frame.time = time
        frame.textureBuf = videoFrame
        frame.rotation = rotation
        agoraEngine.pushExternalVideoFrame(frame, connectionId: connectionId)
    }
    
    static func sendAudioAppBuffer(_ sampleBuffer: CMSampleBuffer) {
        guard let agoraEngine = sharedAgoraEngine else {
            return
        }
        AgoraAudioTube.agoraKit(agoraEngine,
                                pushAudioCMSampleBuffer: sampleBuffer,
                                resampleRate: audioSampleRate,
                                type: .app)
    }
    
    static func sendAudioMicBuffer(_ sampleBuffer: CMSampleBuffer) {
        guard let agoraEngine = sharedAgoraEngine else {
            return
        }
        AgoraAudioTube.agoraKit(agoraEngine,
                                pushAudioCMSampleBuffer: sampleBuffer,
                                resampleRate: audioSampleRate,
                                type: .mic)
    }
    
    static func stopBroadcast() {
        //sharedAgoraEngine.leaveChannel(nil)
        guard let agoraEngine = sharedAgoraEngine else {
            return
        }
        if connectionId != 0 && channelId != nil {
            agoraEngine.leaveChannelEx(channelId!, connectionId: connectionId, leaveChannelBlock: nil)
            channelId = nil
            connectionId = 0
            sharedAgoraEngine = nil
        }
    }
    
    class Listener: NSObject, AgoraRtcEngineDelegate {
        /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
        /// what is happening
        /// Warning code description can be found at:
        /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
        /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
        /// @param warningCode warning code of the problem
        func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
            print("\(warningCode.rawValue)")
        }
        
        /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
        /// to let user know something wrong is happening
        /// Error code description can be found at:
        /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
        /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
        /// @param errorCode error code of the problem
        func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
            print("\(errorCode.rawValue)")
        }
    }
}
