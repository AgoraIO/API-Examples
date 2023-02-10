//
//  ARVideoRenderer.swift
//  Agora-Video-With-ARKit
//
//  Created by GongYuhua on 2017/12/27.
//  Copyright © 2017年 Agora.io All rights reserved.
//

import Foundation
import MetalKit
import SceneKit
import AgoraRtcKit

class ARVideoRenderer : NSObject {
    fileprivate var yTexture: MTLTexture?
    fileprivate var uTexture: MTLTexture?
    fileprivate var vTexture: MTLTexture?
    fileprivate var rgbTexture: MTLTexture?
    
    fileprivate let device = MTLCreateSystemDefaultDevice()
    fileprivate var commandQueue: MTLCommandQueue?
    
    fileprivate var defaultLibrary: MTLLibrary?
    
    fileprivate var threadsPerThreadgroup = MTLSizeMake(16, 16, 1)
    fileprivate var threadgroupsPerGrid = MTLSizeMake(128, 96, 1)
    fileprivate var pipelineState: MTLComputePipelineState?
    
    var renderNode: SCNNode?
    var pixelBuffer: CVPixelBuffer?
    
    override init() {
        super.init()
        
        defaultLibrary = device?.makeDefaultLibrary()
        
        if let device = device, let function = defaultLibrary?.makeFunction(name: "writeRGBFromYUV") {
            pipelineState = try? device.makeComputePipelineState(function: function)
        }
        
        commandQueue = device?.makeCommandQueue()
    }
    
    func shouldDispose() {
        yTexture = nil
        uTexture = nil
        vTexture = nil
        rgbTexture = nil
        renderNode?.geometry?.firstMaterial?.diffuse.contents = createEmptyRGBTexture(width: 1, height: 1)
    }
}

extension ARVideoRenderer: AgoraVideoFrameDelegate {
//    func onCapture(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
//
//        videoFrame.pixelBuffer = pixelBuffer
//
//        return true
//    }
    
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .cvPixelI420
    }
    
    func getVideoFrameProcessMode() -> AgoraVideoFrameProcessMode {
        .readWrite
    }

    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        guard let node = renderNode, let rawData = videoFrame.alphaBuffer else {
            return true
        }
        
        let width = Int(videoFrame.width)
        let height = Int(videoFrame.height)
        
        yTexture = createTexture(withData: rawData,
                                 width: width,
                                 height: height)
        uTexture = createTexture(withData: rawData + width * height,
                                 width: width / 2,
                                 height: height / 2)
        vTexture = createTexture(withData: rawData + width * height * 5 / 4,
                                 width: width / 2,
                                 height: height / 2)
        
        rgbTexture = createEmptyRGBTexture(width: width, height: height)
        
        node.geometry?.firstMaterial?.diffuse.contents = rgbTexture
        renderRGBTexture()

        return true
    }
}

private extension ARVideoRenderer {
    func createTexture(withData data: UnsafeMutableRawPointer, width: Int, height: Int) -> MTLTexture? {
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r8Uint,
                                                                  width: width,
                                                                  height: height,
                                                                  mipmapped: false)
        let texture = device?.makeTexture(descriptor: descriptor)
        texture?.replace(region: MTLRegionMake2D(0, 0, width, height),
                         mipmapLevel: 0,
                         withBytes: data,
                         bytesPerRow: width)
        
        return texture
    }
    
    func createEmptyRGBTexture(width: Int, height: Int) -> MTLTexture? {
        let rgbaDescriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba16Float,
                                                                      width: width,
                                                                      height: height,
                                                                      mipmapped: false)
        rgbaDescriptor.usage = [.shaderWrite, .shaderRead]
        let rgbTexture = device?.makeTexture(descriptor: rgbaDescriptor)
        return rgbTexture
    }
    
    func renderRGBTexture() {
        guard let state = pipelineState,
            let buffer = commandQueue?.makeCommandBuffer(),
            let encoder = buffer.makeComputeCommandEncoder() else {
            return
        }
        
        encoder.setComputePipelineState(state)
        encoder.setTexture(yTexture, index: 0)
        encoder.setTexture(uTexture, index: 1)
        encoder.setTexture(vTexture, index: 2)
        encoder.setTexture(rgbTexture, index: 3)
        encoder.dispatchThreadgroups(threadgroupsPerGrid,
                                     threadsPerThreadgroup: threadsPerThreadgroup)
        encoder.endEncoding()
        
        buffer.commit()
    }
}
