//
//  AgoraCamera.swift
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/11/10.
//  Copyright © 2017年 Agora.io All rights reserved.
//


import UIKit
import AgoraRtcKit
import AVFoundation

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
//    var consumer: AgoraVideoFrameConsumer?
    
    var isFront: Bool {
        get {
            return position.isFront()
        }
    }
    
    private var viewOrientation = UIInterfaceOrientation.portrait
    private var viewOrientationObserver: NSObjectProtocol?
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
    
    override init() {
        super.init()
        viewOrientationObserver = NotificationCenter.default.addObserver(forName: UIApplication.willChangeStatusBarOrientationNotification, object: nil, queue: nil) { [weak self] (notify) in
            guard let value = notify.userInfo?[UIApplication.statusBarOrientationUserInfoKey] as? Int, let orientation = UIInterfaceOrientation(rawValue: value) else {
                return
            }
            self?.viewOrientation = orientation
        }
    }
    
    deinit {
        if let viewOrientationObserver = viewOrientationObserver {
            NotificationCenter.default.removeObserver(viewOrientationObserver)
            self.viewOrientationObserver = nil
        }
    }
    
    func switchCamera() {
        position = position.reverse()
        captureQueue?.async { [weak self] in
            guard let strongSelf = self, let captureSession = strongSelf.captureSession, captureSession.isRunning else {
                return
            }
            
            strongSelf.stopCapture()
            strongSelf.startCapture()
        }
    }
}

private extension AgoraCameraSourceMediaIO {
    func initialize() -> Bool {
        let captureSession = AVCaptureSession()
        captureSession.usesApplicationAudioSession = false
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

//extension AgoraCameraSourceMediaIO: AgoraVideoSourceProtocol {
//    func shouldInitialize() -> Bool {
//        return initialize()
//    }
//
//    func shouldStart() {
//        startCapture()
//    }
//
//    func shouldStop() {
//        stopCapture()
//    }
//
//    func shouldDispose() {
//        dispose()
//    }
//}

extension AgoraCameraSourceMediaIO: AVCaptureVideoDataOutputSampleBufferDelegate {
    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        guard let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer), CVPixelBufferLockBaseAddress(pixelBuffer, .readOnly) == kCVReturnSuccess else {
            return
        }
        defer {
            CVPixelBufferUnlockBaseAddress(pixelBuffer, .readOnly)
        }
        
        let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
//        let rotation = viewOrientation.agoraRotation()
//        consumer?.consumePixelBuffer(pixelBuffer, withTimestamp: time, rotation: rotation)
    }
}

private extension UIInterfaceOrientation {
//    func agoraRotation() -> AgoraVideoRotation {
//        switch self {
//        case .portrait:             return .rotation90
//        case .portraitUpsideDown:   return .rotation270
//        case .landscapeLeft:        return .rotationNone
//        case .landscapeRight:       return .rotation180
//        case .unknown:              return .rotation90
//        default:                    return .rotation90
//        }
//    }
}
