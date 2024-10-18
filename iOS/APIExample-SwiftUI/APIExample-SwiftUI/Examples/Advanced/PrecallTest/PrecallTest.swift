//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct PrecallTest: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = PrecallTestRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    
    var body: some View {
        GeometryReader { geo in
            ZStack {
                VStack {
                    ActivityIndicator(isAnimating: $agoraKit.startActivityView,
                                      style: .large).opacity(agoraKit.isStartLastmileTest ? 1.0 : 0.0)
                    Text("Echo Pretest")
                        .frame(width: 300, alignment: .leading)
                        .padding(.top, 50)
                        .font(.title)
                    
                    Button("Start Audio Device Loopback Test") {
                        agoraKit.doEchoTest()
                    }
                    .frame(width: 300, alignment: .leading)
                    .padding(.vertical, 10)
                    
                    Button("Start Lastmile Test") {
                        agoraKit.doLastmileTest()
                    }
                    .frame(width: 300, alignment: .leading)
                    .padding(.vertical, 10)
                    
                    Button(agoraKit.isStartCameraTest ? "Stop Camera Test" : "Start Camera Test") {
                        agoraKit.doCameraTest()
                    }
                    .frame(width: 300, alignment: .leading)
                    .padding(.vertical, 10)
                    
                    ZStack {
                        Text(agoraKit.lastmileResultLabel)
                            .frame(width: 300, alignment: .leading)
                            .padding(.vertical, 10)
                            .opacity(agoraKit.isStartLastmileTest ? 1.0 : 0.0)
                        
                        localView.opacity(agoraKit.isStartCameraTest ? 1.0 : 0.0)
                    }
                    
                    Spacer()
                }
                VStack {
                    Text("\(agoraKit.countDown)").font(.title)
                    Text(agoraKit.isRecordEnd ? "Now you should hear what you said..." : "Please say something...")
                }
                .frame(width: geo.size.width,
                       height: geo.size.height)
                .background(.black.opacity(0.3))
                .opacity(agoraKit.isEchoTestEnd ? 0.0 : 1.0)
            }.onAppear(perform: {
                agoraKit.setupRTC(configs: configs,
                                  localView: localView.videoView)
            }).onDisappear(perform: {
                agoraKit.onDestory()
            }).navigationBarTitleDisplayMode(.inline)
        }
    }
}

struct PrecallTestPreviews: PreviewProvider {
    static var previews: some View {
        PrecallTest(configs: [:])
    }
}

struct ActivityIndicator: UIViewRepresentable {
    @Binding var isAnimating: Bool
    let style: UIActivityIndicatorView.Style

    func makeUIView(context: UIViewRepresentableContext<ActivityIndicator>) -> UIActivityIndicatorView {
        return UIActivityIndicatorView(style: style)
    }

    func updateUIView(_ uiView: UIActivityIndicatorView, context: UIViewRepresentableContext<ActivityIndicator>) {
        isAnimating ? uiView.startAnimating() : uiView.stopAnimating()
    }
}
