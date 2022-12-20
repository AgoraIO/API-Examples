//
//  AgoraPcmSourcePush.swift
//  APIExample
//
//  Created by XC on 2021/1/10.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import Foundation

protocol AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: UnsafeMutablePointer<UInt8>) -> Void
}

class AgoraPcmSourcePush: NSObject {
    fileprivate var delegate: AgoraPcmSourcePushDelegate?
    private let filePath: String
    private let sampleRate: Int
    private let channelsPerFrame: Int
    private let bitPerSample: Int
    private let samples: Int
    
    private var state: State = .Stop
    private let captureQueue: DispatchQueue

    enum State {
        case Play
        case Stop
    }
    
    init(delegate: AgoraPcmSourcePushDelegate?, filePath: String, sampleRate: Int, channelsPerFrame: Int, bitPerSample: Int, samples: Int) {
        self.delegate = delegate
        self.filePath = filePath
        self.sampleRate = sampleRate
        self.channelsPerFrame = channelsPerFrame
        self.bitPerSample = bitPerSample
        self.samples = samples
        captureQueue = DispatchQueue(label: "MyCaptureQueue")
    }
    
    func start() {
        if state == .Stop {
            state = .Play
            play()
        }
    }
    
    func stop() {
        if state == .Play {
            state = .Stop
        }
    }

    private func play() {
        captureQueue.async {
            guard let input = InputStream(fileAtPath: self.filePath) else { return }
            input.open()
                        
            let bufferSize = self.samples * self.bitPerSample / 8 * self.channelsPerFrame
            let audioIntervalPreCall = TimeInterval(self.samples) / TimeInterval(self.sampleRate)
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: bufferSize)
            var index: Double = 0
            var firstPush: Double = 0
            while input.hasBytesAvailable && self.state == .Play {
                if firstPush == 0 {
                    firstPush = CFAbsoluteTimeGetCurrent()
                }
                input.read(buffer, maxLength: bufferSize)
                self.delegate?.onAudioFrame(data: buffer)
                index += 1
                let nextPush = firstPush + index * audioIntervalPreCall
                let sleep = nextPush - CFAbsoluteTimeGetCurrent()
                Thread.sleep(forTimeInterval: sleep)
            }
            buffer.deallocate()
            input.close()
        }
    }
}
