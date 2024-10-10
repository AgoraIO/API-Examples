//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct ContentInspectEntry: View {
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
            NavigationLink(destination: ContentInspect(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

#Preview {
    ContentInspectEntry()
}

struct ContentInspect: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = ContentInspectRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    
    var body: some View {
        ZStack(alignment: .top) {
            localView
            ZStack(alignment: .center) {
                Color.black.opacity(0.5).frame(height: 55)
                Text("Please obtain the pornographic identification results through the console webhook.".localized).foregroundStyle(.white)
            }
        }
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        }).toolbar {
            ToolbarItem(placement: .topBarTrailing) {
                Button("SwitchCamera".localized) {
                    agoraKit.switchCamera()
                }
            }
        }
    }
}

#Preview {
    ContentInspect(configs: [:])
}
