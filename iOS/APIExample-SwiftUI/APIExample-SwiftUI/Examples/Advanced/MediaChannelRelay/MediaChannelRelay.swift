//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct MediaChannelRelayEntry: View {
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
            NavigationLink(destination: MediaChannelRelay(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

#Preview {
    MediaChannelRelayEntry()
}

struct MediaChannelRelay: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = MediaChannelRelayRTC()
    @State private var relayChannelName: String = ""
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            Text("Send stream to another channel".localized)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.top, 20)
                .padding(.leading, 20)
            TextField("Enter target relay channel name".localized,
                      text: $relayChannelName)
            .textFieldStyle(.roundedBorder)
            .padding(.horizontal, 15)
            HStack {
                Button("Relay".localized) {
                    agoraKit.doRelay(destinationChannelName: relayChannelName)
                }.disabled(relayChannelName.isEmpty || agoraKit.isRelaying)
                Spacer()
                Button("Pause".localized) {
                    agoraKit.doPause()
                }.opacity(agoraKit.isRelaying ? 1.0 : 0.0)
                Spacer()
                Button("Resume".localized) {
                    agoraKit.doResume()
                }.opacity(agoraKit.isRelaying ? 1.0 : 0.0)
                Spacer()
                Button("Stop".localized) {
                    agoraKit.doStop()
                }.opacity(agoraKit.isRelaying ? 1.0 : 0.0)
            }
            .padding(.top, 20)
            .padding(.horizontal, 20)
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

#Preview {
    MediaChannelRelay(configs: [:])
}
