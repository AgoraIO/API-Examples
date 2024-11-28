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

struct ContentInspectEntryPreviews: PreviewProvider {
    static var previews: some View {
        ContentInspectEntry()
    }
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
                Text("Please obtain the pornographic identification results through the console webhook.".localized)
                    .adaptiveForegroundStyle(.white)
            }
        }
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        }).toolbar(content: toobarItems)
    }
    
    @ToolbarContentBuilder // 使用 @ToolbarContentBuilder 声明
    func toobarItems() -> some ToolbarContent {
        let placement: ToolbarItemPlacement = {
#if os(iOS) && swift(>=5.7)
            return .topBarTrailing
#else
            return .navigationBarTrailing
#endif
        }()
        
        ToolbarItem(placement: placement) {
            Button("SwitchCamera".localized) {
                agoraKit.switchCamera()
            }
        }
    }
}

struct ContentInspectPreviews: PreviewProvider {
    static var previews: some View {
        ContentInspect(configs: [:])
    }
}
