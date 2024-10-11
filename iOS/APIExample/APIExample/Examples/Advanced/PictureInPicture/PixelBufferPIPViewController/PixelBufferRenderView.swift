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
        self.displayLayer.removeFromSuperlayer()
        self.displayLayer = createLayer()
        self.layer.addSublayer(displayLayer)
    }

    func renderFromVideoFrameData(videoData: AgoraOutputVideoFrame, uid: Int) {
        let width = videoData.width
        let height = videoData.height
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
            
            self.renderVideoPixelBuffer(pixelBuffer: pixelBuffer, width: width, height: height)
        }
    }
    
    func renderVideoPixelBuffer(pixelBuffer: CVPixelBuffer, width: Int32, height: Int32) {
        DispatchQueue.main.async {
            self.videoWidth = width
            self.videoHeight = height
            self.layoutDisplayer()
        }
        
        // 创建 CMVideoFormatDescription
        var videoInfo: CMVideoFormatDescription?
        let status = CMVideoFormatDescriptionCreateForImageBuffer(allocator: kCFAllocatorDefault, 
                                                                  imageBuffer: pixelBuffer,
                                                                  formatDescriptionOut: &videoInfo)
        guard status == noErr, let videoInfo = videoInfo else {
            print("Error creating video format description")
            return
        }

        // 创建 CMSampleTimingInfo
        var timingInfo = CMSampleTimingInfo()
        timingInfo.duration = CMTime.zero
        timingInfo.decodeTimeStamp = CMTime.invalid
        timingInfo.presentationTimeStamp = CMTime(seconds: CACurrentMediaTime(), preferredTimescale: 1000)

        // 创建 CMSampleBuffer
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

        // 将样本缓冲区排队到显示层
        self.displayLayer.enqueue(sampleBuffer)
        CMSampleBufferInvalidate(sampleBuffer)
    }
    
    private func layoutDisplayer() {
        guard videoWidth > 0, videoHeight > 0, !self.frame.size.equalTo(CGSize.zero) else {
            return
        }

        let viewWidth = self.frame.size.width
        let viewHeight = self.frame.size.height

        let videoRatio = CGFloat(videoWidth) / CGFloat(videoHeight)
        let viewRatio = viewWidth / viewHeight

        var videoSize = CGSize.zero
        if videoRatio >= viewRatio {
            videoSize.height = viewHeight
            videoSize.width = videoSize.height * videoRatio
        } else {
            videoSize.width = viewWidth
            videoSize.height = videoSize.width / videoRatio
        }

        let xOffset = max(0, (viewWidth - videoSize.width) / 2)
        let yOffset = max(0, (viewHeight - videoSize.height) / 2)
        let renderRect = CGRect(x: xOffset, y: yOffset, width: videoSize.width, height: videoSize.height)

        if !renderRect.equalTo(displayLayer.frame) {
            displayLayer.frame = renderRect
        }
    }
    
}
