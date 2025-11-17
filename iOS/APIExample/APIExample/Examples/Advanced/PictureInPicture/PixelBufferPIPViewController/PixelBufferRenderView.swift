//
//  PixelBufferRenderView.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/8.
//

import UIKit
import AVFoundation
import AgoraRtcKit

enum VideoPosition {
    case topLeft
    case topRight
    case bottomLeft
    case bottomRight
}

class PixelBufferRenderView: UIView {
    var uid: UInt = 0
    private var videoWidth: Int32 = 0
    private var videoHeight: Int32 = 0
    private var videoRotation: Int32 = 0
    
    lazy var displayLayer: AVSampleBufferDisplayLayer = {
        let layer = AVSampleBufferDisplayLayer()
        return layer
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        configLayers()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func configLayers() {
        self.layer.addSublayer(displayLayer)
        displayLayer.frame = self.bounds
    }
    
    func createLayer() -> AVSampleBufferDisplayLayer {
        let layer = AVSampleBufferDisplayLayer()
        return layer
    }
    
    func clean() {
        uid = 0
        videoRotation = 0
        self.displayLayer.removeFromSuperlayer()
        self.displayLayer = createLayer()
        self.layer.addSublayer(displayLayer)
    }

    func renderFromVideoFrameData(videoData: AgoraOutputVideoFrame) {
        let width = videoData.width
        let height = videoData.height
        let rotation = videoData.rotation
        let yStride = videoData.yStride
        let uStride = videoData.uStride
        let vStride = videoData.vStride
        
        let yBuffer = videoData.yBuffer
        let uBuffer = videoData.uBuffer
        let vBuffer = videoData.vBuffer
        
        autoreleasepool {
            var pixelBuffer: CVPixelBuffer?
            let pixelAttributes: [String: Any] = [kCVPixelBufferIOSurfacePropertiesKey as String: [:]]
            
            let result = CVPixelBufferCreate(kCFAllocatorDefault, 
                                             Int(width),
                                             Int(height),
                                             kCVPixelFormatType_420YpCbCr8Planar,
                                             pixelAttributes as CFDictionary,
                                             &pixelBuffer)
            
            guard result == kCVReturnSuccess, let pixelBuffer = pixelBuffer else {
                print("Unable to create CVPixelBuffer: \(result)")
                return
            }
            
            CVPixelBufferLockBaseAddress(pixelBuffer, .init(rawValue: 0))
            let yPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0)
            let pixelBufferYBytes = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0)

            for i in 0..<height {
                memcpy(yPlane?.advanced(by: pixelBufferYBytes * Int(i)), yBuffer?.advanced(by: Int(yStride * i)), Int(yStride))
            }

            let uPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1)
            let pixelBufferUBytes = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1)

            for i in 0..<height / 2 {
                memcpy(uPlane?.advanced(by: pixelBufferUBytes * Int(i)), uBuffer?.advanced(by: Int(uStride * i)), Int(uStride))
            }

            let vPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 2)
            let pixelBufferVBytes = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 2)

            for i in 0..<height / 2 {
                memcpy(vPlane?.advanced(by: pixelBufferVBytes * Int(i)), vBuffer?.advanced(by: Int(vStride * i)), Int(vStride))
            }
            
            CVPixelBufferUnlockBaseAddress(pixelBuffer, .init(rawValue: 0))
            
            self.renderVideoPixelBuffer(pixelBuffer: pixelBuffer, width: width, height: height, rotation: rotation)
        }
    }
    
    func renderVideoPixelBuffer(pixelBuffer: CVPixelBuffer, width: Int32, height: Int32, rotation: Int32) {
        DispatchQueue.main.async {
            self.videoWidth = width
            self.videoHeight = height
            self.videoRotation = rotation
            self.layoutDisplayer()
        }
        
        var videoInfo: CMVideoFormatDescription?
        let status = CMVideoFormatDescriptionCreateForImageBuffer(allocator: kCFAllocatorDefault, 
                                                                  imageBuffer: pixelBuffer,
                                                                  formatDescriptionOut: &videoInfo)
        guard status == noErr, let videoInfo = videoInfo else {
            print("Error creating video format description")
            return
        }

        var timingInfo = CMSampleTimingInfo()
        timingInfo.duration = CMTime.zero
        timingInfo.decodeTimeStamp = CMTime.invalid
        timingInfo.presentationTimeStamp = CMTime(seconds: CACurrentMediaTime(), preferredTimescale: 1000)

        var sampleBuffer: CMSampleBuffer?
        let sampleBufferStatus = CMSampleBufferCreateReadyWithImageBuffer(allocator: kCFAllocatorDefault, 
                                                                          imageBuffer: pixelBuffer,
                                                                          formatDescription: videoInfo,
                                                                          sampleTiming: &timingInfo,
                                                                          sampleBufferOut: &sampleBuffer)
        guard sampleBufferStatus == noErr, let sampleBuffer = sampleBuffer else {
            print("Error creating sample buffer")
            return
        }

        self.displayLayer.enqueue(sampleBuffer)
        CMSampleBufferInvalidate(sampleBuffer)
    }
    
    private func layoutDisplayer() {
        guard videoWidth > 0, videoHeight > 0, !self.frame.size.equalTo(CGSize.zero) else {
            return
        }

        let viewWidth = self.frame.size.width
        let viewHeight = self.frame.size.height

        // 根据旋转角度调整实际的视频宽高
        let (actualWidth, actualHeight) = getActualVideoDimensions()
        
        let videoRatio = CGFloat(actualWidth) / CGFloat(actualHeight)
        let viewRatio = viewWidth / viewHeight

        var videoSize = CGSize.zero
        
        // 保持宽高比，完整显示视频（AspectFit 模式）
        if videoRatio > viewRatio {
            // 视频更宽，以宽度为准
            videoSize.width = viewWidth
            videoSize.height = videoSize.width / videoRatio
        } else {
            // 视频更高，以高度为准
            videoSize.height = viewHeight
            videoSize.width = videoSize.height * videoRatio
        }

        let xOffset = (viewWidth - videoSize.width) / 2
        let yOffset = (viewHeight - videoSize.height) / 2
        let renderRect = CGRect(x: xOffset, y: yOffset, width: videoSize.width, height: videoSize.height)

        if !renderRect.equalTo(displayLayer.frame) {
            displayLayer.frame = renderRect
        }
        
        // 应用旋转变换
        applyRotationTransform()
    }
    
    /// 根据旋转角度获取实际的视频尺寸
    private func getActualVideoDimensions() -> (width: Int32, height: Int32) {
        if videoRotation == 90 || videoRotation == 270 {
            return (width: videoHeight, height: videoWidth)
        } else {
            return (width: videoWidth, height: videoHeight)
        }
    }
    
    /// 应用旋转变换
    private func applyRotationTransform() {
        let rotationAngle: CGFloat
        
        switch videoRotation {
        case 90:
            rotationAngle = .pi / 2
        case 180:
            rotationAngle = .pi
        case 270:
            rotationAngle = .pi * 3 / 2
        default:
            rotationAngle = 0
        }
        
        // 应用旋转变换
        if rotationAngle != 0 {
            displayLayer.setAffineTransform(CGAffineTransform(rotationAngle: rotationAngle))
        } else {
            displayLayer.setAffineTransform(CGAffineTransform.identity)
        }
    }
    
}
