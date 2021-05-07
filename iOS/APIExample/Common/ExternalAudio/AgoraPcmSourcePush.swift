//
//  AgoraPcmSourcePush.swift
//  APIExample
//
//  Created by XC on 2021/5/7.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import Foundation

protocol AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: UnsafeMutablePointer<UInt8>, samples: UInt) -> Void
    func onStop()
}

class AgoraPcmSourcePush: NSObject {
    fileprivate var delegate: AgoraPcmSourcePushDelegate?
    private let filePath: String
    private let sampleRate: Int
    private let channelsPerFrame: Int
    private let bitPerSample: Int
    private let samples: Int
    
    private var state: State = .Stop
    private let playerQueue: DispatchQueue

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
        playerQueue = DispatchQueue(label: "player")
    }
    
    func start() {
        if state == .Stop {
            state = .Play
            play()
        }
    }
    
    func stop() {
        DispatchQueue.main.async {
            if self.state == .Play {
                self.state = .Stop
            }
            self.delegate?.onStop()
        }
    }

    private func play() {
        guard let input = InputStream(fileAtPath: self.filePath) else { return }
        input.open()
        let bufferSize = self.samples * self.bitPerSample / 8 * self.channelsPerFrame
        let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: bufferSize)
        let timeInterval = TimeInterval(self.samples) / TimeInterval(self.sampleRate)
        dispatchTimer(timeInterval: timeInterval, handler: { (timer: DispatchSourceTimer?) in
            if (timer != nil) {
                input.read(buffer, maxLength: bufferSize)
                self.delegate?.onAudioFrame(data: buffer, samples: UInt(self.samples))
            } else {
                buffer.deallocate()
                input.close()
                self.stop()
            }
        }, needRepeat: {
            return input.hasBytesAvailable && self.state == .Play
        })
    }
    
    private func dispatchTimer(timeInterval: Double, handler: @escaping (DispatchSourceTimer?) -> Void, needRepeat: @escaping () -> Bool) {
        let timer = DispatchSource.makeTimerSource(flags: [], queue: playerQueue)
        timer.schedule(deadline: .now(), repeating: timeInterval)
        timer.setEventHandler {
            self.playerQueue.async {
                if needRepeat() {
                    handler(timer)
                } else {
                    timer.cancel()
                    handler(nil)
                }
            }
        }
        timer.resume()
    }
}
