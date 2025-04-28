//
//  AgoraPCMPlayer.swift
//  APIExample
//
//  Created by wushengtao on 2024/9/26.
//  Copyright © 2024 Agora Corp. All rights reserved.
//

import Foundation

class AgoraPCMPlayer {
    private var audioEngine: AVAudioEngine
    private var playerNode: AVAudioPlayerNode
    private var sampleRate: Double
    private var channels: AVAudioChannelCount

    init(sampleRate: Double, channels: AVAudioChannelCount) {
        self.sampleRate = sampleRate
        self.channels = channels
        
        audioEngine = AVAudioEngine()
        playerNode = AVAudioPlayerNode()
        
        audioEngine.attach(playerNode)
        
        let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: channels)
        audioEngine.connect(playerNode, to: audioEngine.mainMixerNode, format: format)
        
        do {
            try audioEngine.start()
        } catch {
            print("Audio Engine failed to start: \(error)")
        }
    }
    
    func playPCMData(pcmData: UnsafeMutablePointer<UInt8>, count: UInt) {
        guard let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: channels),
              let audioBuffer = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: AVAudioFrameCount(count / 4)), 
              let channelData = audioBuffer.floatChannelData else {
            return
        }
        
        audioBuffer.frameLength = AVAudioFrameCount(count / 4)
        
        for frame in 0..<Int(audioBuffer.frameLength) {
            let leftSample = Int16(pcmData[frame * 4]) | (Int16(pcmData[frame * 4 + 1]) << 8)
            let rightSample = Int16(pcmData[frame * 4 + 2]) | (Int16(pcmData[frame * 4 + 3]) << 8)
            
            channelData[0][frame] = Float(leftSample) / Float(Int16.max)
            channelData[1][frame] = Float(rightSample) / Float(Int16.max)
        }
        
        playerNode.scheduleBuffer(audioBuffer, completionHandler: nil)
        playerNode.play()
    }
}
