//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct QuickSwitchChannelEntry: View {
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
            NavigationLink(destination: QuickSwitchChannel(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

//#Preview {
//    QuickSwitchChannelEntry()
//}

struct QuickSwitchChannel: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = QuickSwitchChannelRTC()
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    @State private var currentIndex: Int = 0
    
    var body: some View {
        VStack {
            ZStack {
                remoteView.zIndex(0)
                TabView(selection: $currentIndex) {
                    ForEach(1...20, id: \.self) { index in
                        let channelName = configs["channelName"] as? String
                        Text("channelName: \(channelName ?? "")\(index)")
                            .frame(width: 375,
                                   height: 667,
                                   alignment: .topLeading)
                    }
                }
                .zIndex(1)
                .tabViewStyle(PageTabViewStyle(indexDisplayMode: .never))
                .onChange(of: currentIndex) { value in
                    joinChannel(index: value)
                }
            }
        }.onAppear(perform: {
            currentIndex = 1
            agoraKit.setupRTC(configs: configs,
                              remoteView: remoteView.videoView)
            joinChannel(index: currentIndex)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
    
    private func joinChannel(index: Int) {
        let channelName = configs["channelName"] as? String
        let channel = "\(channelName ?? "")\(index)"
        agoraKit.joinChannel(channelName: channel)
    }
}

//#Preview {
//    QuickSwitchChannel(configs: [:])
//}
