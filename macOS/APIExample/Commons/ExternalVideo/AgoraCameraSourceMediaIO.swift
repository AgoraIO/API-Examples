//
//  AgoraCamera.swift
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/11/10.
//  Copyright © 2017年 Agora.io All rights reserved.
//


import Cocoa
import AgoraRtcKit

extension AVCaptureDevice.Position {
    func reverse() -> AVCaptureDevice.Position {
        switch self {
        case .front:              return .back
        case .back, .unspecified: return .front
        default: return .front
        }
    }
    
    func isFront() -> Bool {
        return self == .front
    }
}

class AgoraCameraSourceMediaIO: NSObject {
    var consumer: AgoraVideoFrameConsumer?
    
    var isFront: Bool {
        get {
            return position.isFront()
        }
    }
    
    private var position = AVCaptureDevice.Position.front
    private var captureSession: AVCaptureSession?
    private var captureQueue: DispatchQueue?
    private var currentOutput: AVCaptureVideoDataOutput? {
        if let outputs = self.captureSession?.outputs as? [AVCaptureVideoDataOutput] {
            return outputs.first
        } else {
            return nil
        }
    }
}

private extension AgoraCameraSourceMediaIO {
    func initialize() -> Bool {
        let captureSession = AVCaptureSession()
        let captureOutput = AVCaptureVideoDataOutput()
        captureOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_420YpCbCr8BiPlanarFullRange]
        if captureSession.canAddOutput(captureOutput) {
            captureSession.addOutput(captureOutput)
        }
        
        self.captureSession = captureSession
        captureQueue = DispatchQueue(label: "Agora-Custom-Video-Capture-Queue")
        
        return true
    }
    
    func startCapture() {
        guard let currentOutput = currentOutput, let captureQueue = captureQueue else {
            return
        }
        
        currentOutput.setSampleBufferDelegate(self, queue: captureQueue)
        captureQueue.async { [weak self] in
            guard let strongSelf = self, let captureSession = strongSelf.captureSession else {
                return
            }
            strongSelf.changeCaptureDevice(toPosition: strongSelf.position, ofSession: captureSession)
            captureSession.beginConfiguration()
            if captureSession.canSetSessionPreset(.vga640x480) {
                captureSession.sessionPreset = .vga640x480
            }
            captureSession.commitConfiguration()
            captureSession.startRunning()
        }
    }
    
    func stopCapture() {
        currentOutput?.setSampleBufferDelegate(nil, queue: nil)
        captureQueue?.async { [weak self] in
            self?.captureSession?.stopRunning()
        }
    }
    
    func dispose() {
        captureQueue = nil
        captureSession = nil
    }
}

private extension AgoraCameraSourceMediaIO {
    func changeCaptureDevice(toPosition position: AVCaptureDevice.Position, ofSession captureSession: AVCaptureSession) {
        guard let captureDevice = captureDevice(atPosition: position) else {
            return
        }
        
        let currentInputs = captureSession.inputs as? [AVCaptureDeviceInput]
        let currentInput = currentInputs?.first
        
        if let currentInput = currentInput, currentInput.device.localizedName == captureDevice.uniqueID {
            return
        }
        
        guard let newInput = try? AVCaptureDeviceInput(device: captureDevice) else {
            return
        }
        
        captureSession.beginConfiguration()
        if let currentInput = currentInput {
            captureSession.removeInput(currentInput)
        }
        if captureSession.canAddInput(newInput) {
            captureSession.addInput(newInput)
        }
        captureSession.commitConfiguration()
    }
    
    func captureDevice(atPosition position: AVCaptureDevice.Position) -> AVCaptureDevice? {
        let devices = AVCaptureDevice.DiscoverySession(deviceTypes: [.builtInWideAngleCamera], mediaType: .video, position: position).devices
        return devices.first
    }
}

extension AgoraCameraSourceMediaIO: AgoraVideoSourceProtocol {
    func shouldInitialize() -> Bool {
        return initialize()
    }
    
    func shouldStart() {
        startCapture()
    }
    
    func shouldStop() {
        stopCapture()
    }
    
    func shouldDispose() {
        dispose()
    }
    
    func bufferType() -> AgoraVideoBufferType {
        return .pixelBuffer
    }
    
    func contentHint() -> AgoraVideoContentHint {
        return .none
    }
    
    func captureType() -> AgoraVideoCaptureType {
        return .camera
    }
}

extension AgoraCameraSourceMediaIO: AVCaptureVideoDataOutputSampleBufferDelegate {
    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        guard let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer), CVPixelBufferLockBaseAddress(pixelBuffer, .readOnly) == kCVReturnSuccess else {
            return
        }
        defer {
            CVPixelBufferUnlockBaseAddress(pixelBuffer, .readOnly)
        }
        
        let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
        consumer?.consumePixelBuffer(pixelBuffer, withTimestamp: time, rotation: .rotationNone)
    }
}
