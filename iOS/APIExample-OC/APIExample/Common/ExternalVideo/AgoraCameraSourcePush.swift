//
//  MyVideoCapture.swift
//  Agora-Video-Source
//
//  Created by GongYuhua on 2017/4/11.
//  Copyright © 2017年 Agora. All rights reserved.
//

import UIKit
import AVFoundation

protocol AgoraCameraSourcePushDelegate {
    func myVideoCapture(_ capture: AgoraCameraSourcePush, didOutputSampleBuffer pixelBuffer: CVPixelBuffer, rotation: Int, timeStamp: CMTime)
}

enum Camera: Int {
    case front = 1
    case back = 0
    
    static func defaultCamera() -> Camera {
        return .front
    }
    
    func next() -> Camera {
        switch self {
        case .back: return .front
        case .front: return .back
        }
    }
}

class CustomVideoSourcePreview : UIView {
    private var previewLayer: AVCaptureVideoPreviewLayer?
    
    func insertCaptureVideoPreviewLayer(previewLayer: AVCaptureVideoPreviewLayer) {
        self.previewLayer?.removeFromSuperlayer()
        
        previewLayer.frame = bounds
        layer.insertSublayer(previewLayer, below: layer.sublayers?.first)
        self.previewLayer = previewLayer
    }
    
    override func layoutSublayers(of layer: CALayer) {
        super.layoutSublayers(of: layer)
        previewLayer?.frame = bounds
    }
}

class AgoraCameraSourcePush: NSObject {
    
    fileprivate var delegate: AgoraCameraSourcePushDelegate?
    private var videoView: CustomVideoSourcePreview
    
    private var currentCamera = Camera.defaultCamera()
    private let captureSession: AVCaptureSession
    private let captureQueue: DispatchQueue
    private var currentOutput: AVCaptureVideoDataOutput? {
        if let outputs = self.captureSession.outputs as? [AVCaptureVideoDataOutput] {
            return outputs.first
        } else {
            return nil
        }
    }
    
    init(delegate: AgoraCameraSourcePushDelegate?, videoView: CustomVideoSourcePreview) {
        self.delegate = delegate
        self.videoView = videoView
        
        captureSession = AVCaptureSession()
        captureSession.usesApplicationAudioSession = false
        
        let captureOutput = AVCaptureVideoDataOutput()
        captureOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_420YpCbCr8BiPlanarFullRange]
        if captureSession.canAddOutput(captureOutput) {
            captureSession.addOutput(captureOutput)
        }
        
        captureQueue = DispatchQueue(label: "MyCaptureQueue")
        
        let previewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
        videoView.insertCaptureVideoPreviewLayer(previewLayer: previewLayer)
    }
    
    deinit {
        captureSession.stopRunning()
    }
    
    func startCapture(ofCamera camera: Camera) {
        guard let currentOutput = currentOutput else {
            return
        }
        
        currentCamera = camera
        currentOutput.setSampleBufferDelegate(self, queue: captureQueue)
        
        captureQueue.async { [weak self] in
            guard let strongSelf = self else {
                return
            }
            strongSelf.changeCaptureDevice(toIndex: camera.rawValue, ofSession: strongSelf.captureSession)
            strongSelf.captureSession.beginConfiguration()
            if strongSelf.captureSession.canSetSessionPreset(AVCaptureSession.Preset.vga640x480) {
                strongSelf.captureSession.sessionPreset = AVCaptureSession.Preset.vga640x480
            }
            strongSelf.captureSession.commitConfiguration()
            strongSelf.captureSession.startRunning()
        }
    }
    
    func stopCapture() {
        currentOutput?.setSampleBufferDelegate(nil, queue: nil)
        captureQueue.async { [weak self] in
            self?.captureSession.stopRunning()
        }
    }
    
    func switchCamera() {
        stopCapture()
        currentCamera = currentCamera.next()
        startCapture(ofCamera: currentCamera)
    }
}

private extension AgoraCameraSourcePush {
    func changeCaptureDevice(toIndex index: Int, ofSession captureSession: AVCaptureSession) {
        guard let captureDevice = captureDevice(atIndex: index) else {
            return
        }
        
        let currentInputs = captureSession.inputs as? [AVCaptureDeviceInput]
        let currentInput = currentInputs?.first
        
        if let currentInputName = currentInput?.device.localizedName,
            currentInputName == captureDevice.uniqueID {
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
    
    func captureDevice(atIndex index: Int) -> AVCaptureDevice? {
        let deviceDiscoverySession = AVCaptureDevice.DiscoverySession(deviceTypes: [.builtInWideAngleCamera], mediaType: AVMediaType.video, position: .back)
        let devices = deviceDiscoverySession.devices
        
        let count = devices.count
        guard count > 0, index >= 0 else {
            return nil
        }
        
        let device: AVCaptureDevice
        if index >= count {
            device = devices.last!
        } else {
            device = devices[index]
        }
        
        return device
    }
}

extension AgoraCameraSourcePush: AVCaptureVideoDataOutputSampleBufferDelegate {
    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        guard let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) else {
            return
        }
        let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
        DispatchQueue.main.async {[weak self] in
            guard let weakSelf = self else {
                return
            }
            
            weakSelf.delegate?.myVideoCapture(weakSelf, didOutputSampleBuffer: pixelBuffer, rotation: 90, timeStamp: time)
        }
    }
}
