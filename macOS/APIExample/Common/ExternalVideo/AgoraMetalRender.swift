//
//  AgoraMetalRender.swift
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/11/15.
//  Copyright © 2017年 Agora.io All rights reserved.
//

import CoreMedia
import Metal
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
import MetalKit
#endif
import AgoraRtcKit

protocol AgoraMetalRenderMirrorDataSource: NSObjectProtocol {
    func renderViewShouldMirror(renderView: AgoraMetalRender) -> Bool
}

enum AgoraVideoRotation:Int {
    /** 0: No rotation */
    case rotationNone = 0
    /** 1: 90 degrees */
    case rotation90 = 1
    /** 2: 180 degrees */
    case rotation180 = 2
    /** 3: 270 degrees */
    case rotation270 = 3
}


class AgoraMetalRender: NSView {
    weak var mirrorDataSource: AgoraMetalRenderMirrorDataSource?
    
    fileprivate var videoFrame: CVPixelBuffer? = nil
    fileprivate var textures: [MTLTexture]?
    fileprivate var vertexBuffer: MTLBuffer?
    fileprivate var viewSize = CGSize.zero
    
    fileprivate var device = MTLCreateSystemDefaultDevice()
    fileprivate var renderPipelineState: MTLRenderPipelineState?
    fileprivate let semaphore = DispatchSemaphore(value: 1)
    fileprivate var metalDevice = MTLCreateSystemDefaultDevice()
    fileprivate var rotation = AgoraVideoRotation.rotationNone
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
    fileprivate var metalView: MTKView!
    fileprivate var textureCache: CVMetalTextureCache?
#endif
    fileprivate var commandQueue: MTLCommandQueue?
    
    fileprivate var userId: UInt = 0
    
    init() {
        super.init(frame: CGRect(x: 0, y: 0, width: 100, height: 100))
        initializeMetalView()
        initializeTextureCache()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        initializeMetalView()
        initializeTextureCache()
    }
    
    override init(frame frameRect: CGRect) {
        super.init(frame: frameRect)
        initializeMetalView()
        initializeTextureCache()
    }
    
    
    override func layout() {
        super.layout()
        viewSize = bounds.size
    }
    
    func startRender(uid: UInt) {
        userId = uid
        initializeRenderPipelineState()
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
        metalView.delegate = self
#endif
    }
    
    func stopRender() {
        userId = 0
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
        metalView.delegate = nil
#endif
    }
}

func getAgoraRotation(rotation: Int32) -> AgoraVideoRotation? {
    switch rotation {
    case 0:
        return .rotationNone
    case 90:
        return .rotation90
    case 180:
        return .rotation180
    case 270:
        return .rotation270
    default:
        return nil
    }
}


extension AgoraMetalRender: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        if uid != userId {
            return false
        }
        
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
        guard let rotation = getAgoraRotation(rotation: videoFrame.rotation) else {
            return false
        }
        self.rotation = rotation
        guard let pixelBuffer = videoFrame.pixelBuffer else { return false }
        
        let res = semaphore.wait(timeout: .now() + 0.1)
        guard res == DispatchTimeoutResult.success else {
            return true
        }
        self.videoFrame = pixelBuffer
        semaphore.signal()
#endif
        return true
    }
    
    
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .cvPixelNV12
    }
    
    func getVideoFrameProcessMode() -> AgoraVideoFrameProcessMode {
        .readOnly
    }
    func onPreEncode(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        true
    }
}

private extension AgoraMetalRender {
    func initializeMetalView() {
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
        metalView = MTKView(frame: bounds, device: device)
        metalView.framebufferOnly = true
        metalView.colorPixelFormat = .bgra8Unorm
        metalView.autoresizingMask = [.width, .height]
        addSubview(metalView)
        commandQueue = device?.makeCommandQueue()
#endif
    }
    
    func initializeRenderPipelineState() {
        guard let device = device, let library = device.makeDefaultLibrary() else {
            return
        }
        
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
        pipelineDescriptor.sampleCount = 1
        pipelineDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        pipelineDescriptor.depthAttachmentPixelFormat = .invalid
        
        pipelineDescriptor.vertexFunction = library.makeFunction(name: "mapTexture")
        pipelineDescriptor.fragmentFunction = library.makeFunction(name: "displayNV12Texture")
        
        renderPipelineState = try? device.makeRenderPipelineState(descriptor: pipelineDescriptor)
    }
    
    func initializeTextureCache() {
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
        guard let metalDevice = metalDevice,
              CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, metalDevice, nil, &textureCache) == kCVReturnSuccess else {
            return
        }
#endif
    }
    
#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
    func texture(pixelBuffer: CVPixelBuffer, textureCache: CVMetalTextureCache?, planeIndex: Int = 0, pixelFormat: MTLPixelFormat = .bgra8Unorm) -> MTLTexture? {
        guard let textureCache = textureCache, CVPixelBufferLockBaseAddress(pixelBuffer, .readOnly) == kCVReturnSuccess else {
            return nil
        }
        defer {
            CVPixelBufferUnlockBaseAddress(pixelBuffer, .readOnly)
        }
        
        let isPlanar = CVPixelBufferIsPlanar(pixelBuffer)
        let width = isPlanar ? CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex) : CVPixelBufferGetWidth(pixelBuffer)
        let height = isPlanar ? CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex) : CVPixelBufferGetHeight(pixelBuffer)
        
        var imageTexture: CVMetalTexture?
        let result = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, textureCache, pixelBuffer, nil, pixelFormat, width, height, planeIndex, &imageTexture)
        
        guard let unwrappedImageTexture = imageTexture,
              let texture = CVMetalTextureGetTexture(unwrappedImageTexture),
              result == kCVReturnSuccess
        else {
            return nil
        }
        
        return texture
    }
#endif
}

#if os(macOS) || (os(iOS) && (!arch(i386) && !arch(x86_64)))
extension AgoraMetalRender: MTKViewDelegate {
    public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        
    }
    
    public func draw(in: MTKView) {
        guard viewSize.width > 0 && viewSize.height > 0 else {
            return
        }
        
        let res = semaphore.wait(timeout: .now())
        guard res == DispatchTimeoutResult.success else {
            return;
        }
        
        guard let pixelBuffer = self.videoFrame else {
            semaphore.signal()
            return }
        guard CVPixelBufferLockBaseAddress(pixelBuffer, .readOnly) == kCVReturnSuccess else {
            semaphore.signal()
            return
        }
        defer {
            CVPixelBufferUnlockBaseAddress(pixelBuffer, .readOnly)
        }
        
        let isPlanar = CVPixelBufferIsPlanar(pixelBuffer)
        let width = isPlanar ? CVPixelBufferGetWidthOfPlane(pixelBuffer, 0) : CVPixelBufferGetWidth(pixelBuffer)
        let height = isPlanar ? CVPixelBufferGetHeightOfPlane(pixelBuffer, 0) : CVPixelBufferGetHeight(pixelBuffer)
        let size = CGSize(width: width, height: height)
        
        let mirror = mirrorDataSource?.renderViewShouldMirror(renderView: self) ?? false
        if let renderedCoordinates = self.rotation.renderedCoordinates(mirror: mirror,
                                                                       videoSize: size,
                                                                       viewSize: viewSize) {
            let byteLength = 4 * MemoryLayout.size(ofValue: renderedCoordinates[0])
            vertexBuffer = device?.makeBuffer(bytes: renderedCoordinates, length: byteLength, options: [.storageModeShared])
        }
        
        if let yTexture = texture(pixelBuffer: pixelBuffer, textureCache: textureCache, planeIndex: 0, pixelFormat: .r8Unorm),
           let uvTexture = texture(pixelBuffer: pixelBuffer, textureCache: textureCache, planeIndex: 1, pixelFormat: .rg8Unorm) {
            
            self.textures = [yTexture, uvTexture]
        }
        
        guard let textures = textures, let device = device,
              let commandBuffer = commandQueue?.makeCommandBuffer(), let vertexBuffer = vertexBuffer else {
            semaphore.signal()
            return
        }
        
        render(textures: textures, withCommandBuffer: commandBuffer, device: device, vertexBuffer: vertexBuffer)
    }
    
    private func render(textures: [MTLTexture], withCommandBuffer commandBuffer: MTLCommandBuffer, device: MTLDevice, vertexBuffer: MTLBuffer) {
        guard let currentRenderPassDescriptor = metalView.currentRenderPassDescriptor,
              let currentDrawable = metalView.currentDrawable,
              let renderPipelineState = renderPipelineState,
              let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: currentRenderPassDescriptor) else {
            semaphore.signal()
            return
        }
        
        encoder.pushDebugGroup("Agora-Custom-Render-Frame")
        encoder.setRenderPipelineState(renderPipelineState)
        encoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        
        
        guard textures.count == 2 else {
            semaphore.signal()
            return;
        }
        
        if let textureY = textures.first, let textureUV = textures.last {
            encoder.setFragmentTexture(textureY, index: 0)
            encoder.setFragmentTexture(textureUV, index: 1)
            encoder.drawPrimitives(type: .triangleStrip, vertexStart: 0, vertexCount: 4)
        }
        
        encoder.popDebugGroup()
        encoder.endEncoding()
        
        commandBuffer.addScheduledHandler { [weak self] (buffer) in
            self?.semaphore.signal()
        }
        commandBuffer.present(currentDrawable)
        commandBuffer.commit()
        
    }
}
#endif

extension AgoraVideoRotation {
    func renderedCoordinates(mirror: Bool, videoSize: CGSize, viewSize: CGSize) -> [simd_float4]? {
        guard viewSize.width > 0, viewSize.height > 0, videoSize.width > 0, videoSize.height > 0 else {
            return nil
        }
        
        let widthAspito: Float
        let heightAspito: Float
        if self == .rotation90 || self == .rotation270 {
            widthAspito = Float(videoSize.height / viewSize.width)
            heightAspito = Float(videoSize.width / viewSize.height)
        } else {
            widthAspito = Float(videoSize.width / viewSize.width)
            heightAspito = Float(videoSize.height / viewSize.height)
        }
        
        let x: Float
        let y: Float
        if widthAspito < heightAspito {
            x = 1
            y = heightAspito / widthAspito
        } else {
            x = widthAspito / heightAspito
            y = 1
        }
        
        let A = simd_float4(  x, -y, 0.0, 1.0 )
        let B = simd_float4( -x, -y, 0.0, 1.0 )
        let C = simd_float4(  x,  y, 0.0, 1.0 )
        let D = simd_float4( -x,  y, 0.0, 1.0 )
        
        switch self {
        case .rotationNone:
            if mirror {
                return [A, B, C, D]
            } else {
                return [B, A, D, C]
            }
        case .rotation90:
            if mirror {
                return [C, A, D, B]
            } else {
                return [D, B, C, A]
            }
        case .rotation180:
            if mirror {
                return [D, C, B, A]
            } else {
                return [C, D, A, B]
            }
        case .rotation270:
            if mirror {
                return [B, D, A, C]
            } else {
                return [A, C, B, D]
            }
        }
    }
}
