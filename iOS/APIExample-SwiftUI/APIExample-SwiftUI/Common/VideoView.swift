//
//  VideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/18.
//

import SwiftUI

struct VideoView: UIViewRepresentable {
    @State var type: StreamType = .local
    @State var audioOnly: Bool = false
    let videoView = VideoUIView()
        
    func makeUIView(context: Context) -> UIView {
        return videoView
    }

    func updateUIView(_ uiView: UIView, context: Context) {
        let videoView = uiView as? VideoUIView
        videoView?.audioOnly = audioOnly
        videoView?.type = type
    }
}

struct SampleBufferView: UIViewRepresentable {
    @State var type: StreamType = .local
    @State var audioOnly: Bool = false
    let videoView = SampleBufferDisplayView()
        
    func makeUIView(context: Context) -> UIView {
        return videoView
    }

    func updateUIView(_ uiView: UIView, context: Context) {
        let videoView = uiView as? SampleBufferDisplayView
        videoView?.audioOnly = audioOnly
        videoView?.type = type
    }
}

struct MetalRenderView: UIViewRepresentable {
    @State var type: StreamType = .local
    @State var audioOnly: Bool = false
    let videoView = MetalVideoView()
        
    func makeUIView(context: Context) -> UIView {
        return videoView
    }

    func updateUIView(_ uiView: UIView, context: Context) {
        let videoView = uiView as? MetalVideoView
        videoView?.audioOnly = audioOnly
        videoView?.type = type
    }
}
