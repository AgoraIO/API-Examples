//
//  LocalVideoTranscoding.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/29.
//

import SwiftUI

struct LocalVideoTranscodingEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName]
                self.isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: LocalVideoTranscoding(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct LocalVideoTranscoding: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = LocalVideoTranscodingRTC()

    var localView = SampleBufferView()
    var remoteView = VideoView(type: .remote)

    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            Spacer()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct LocalVideoTranscodingPreviews: PreviewProvider {
    static var previews: some View {
        LocalVideoTranscoding()
    }
}

