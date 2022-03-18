//
//  AgoraPcmSourcePush.swift
//  APIExample
//
//  Created by XC on 2021/1/10.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import Foundation

protocol AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: Data) -> Void
}

class AgoraPcmSourcePush: NSObject {
    fileprivate var delegate: AgoraPcmSourcePushDelegate?
    private let filePath: String
    private let sampleRate: Int
    private let channelsPerFrame: Int
    private let bitPerSample: Int
    private let samples: Int
    private var audioTimer: DispatchSourceTimer?
    private var audioSentFrames: Int64 = 0
    private var audioSendStartMS: Int64 = 0
    private var input: InputStream?
    private var buffer: UnsafeMutablePointer<UInt8>?
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
      audioSentFrames = 0
      audioSendStartMS = 0
      if let buffer = buffer, let input = input {
        buffer.deallocate()
        input.close()
      }
      buffer = nil
      input = nil
      guard let timer = audioTimer else {
          return
      }
      timer.cancel()
      
      audioTimer = nil
      return
    }

//    private func play() {
//        captureQueue.async {
//            guard let input = InputStream(fileAtPath: self.filePath) else { return }
//            input.open()
//
//            let bufferSize = self.samples * self.bitPerSample / 8 * self.channelsPerFrame
//            let sleepTime = TimeInterval(self.samples) / TimeInterval(self.sampleRate)
//            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: bufferSize)
//            while input.hasBytesAvailable && self.state == .Play {
//                input.read(buffer, maxLength: bufferSize)
//                self.delegate?.onAudioFrame(data: Data(bytes: buffer, count: bufferSize))
//                Thread.sleep(forTimeInterval: sleepTime)
//            }
//            buffer.deallocate()
//            input.close()
//        }
//    }
  
  private func currentTimeMillis() -> Int64 {
     var darwinTime : timeval = timeval(tv_sec: 0, tv_usec: 0)
     gettimeofday(&darwinTime, nil)
     return (Int64(darwinTime.tv_sec) * 1000) + Int64(darwinTime.tv_usec / 1000)
  }
  
  func play() {
      let bufferSize = self.samples * self.bitPerSample / 8 * self.channelsPerFrame
      input = InputStream(fileAtPath: filePath)
      input?.open()
      buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: bufferSize)
      guard let input = input, let buffer = buffer  else { return }
      let timer = DispatchSource.makeTimerSource(queue: captureQueue)
      timer.schedule(wallDeadline: .now(), repeating: 0.1)
      
      timer.setEventHandler { [weak self] in
          guard let self = self else {
              return
          }
          
          if (self.audioSentFrames == 0) {
              self.audioSendStartMS = self.currentTimeMillis()
          }
          
          let currentMS = self.currentTimeMillis()
          
        let targetSentFrames = ((currentMS - self.audioSendStartMS) / 100) + 1;
        while (targetSentFrames > self.audioSentFrames && (input.hasBytesAvailable && self.state == .Play)) {
          let size = input.read(buffer, maxLength: bufferSize)
          self.delegate?.onAudioFrame(data: Data(bytes: buffer, count: size))
          self.audioSentFrames += 1
        }
      }
      timer.resume()
      audioTimer = timer
  }
}
