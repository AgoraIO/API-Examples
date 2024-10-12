//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct MutliCameraEntry: View {
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
            NavigationLink(destination: MutliCamera(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

//#Preview {
//    MutliCameraEntry()
//}

struct MutliCamera: View {
    @State var configs: [String: Any] = [:]
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = MutliCameraRTC()
    @State private var isOpenCamera: Bool = false
    @State private var title: String = "Open Rear Camera".localized
    
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
            Spacer()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
            
        }).toolbar(content: toobarItems)
    }
    
    func toobarItems() -> some ToolbarContent {
        let placement: ToolbarItemPlacement = {
#if os(iOS) && swift(>=5.7)
            return .topBarTrailing
#else
            return .navigationBarTrailing
#endif
        }()
        
        return ToolbarItem(placement: placement) {
            Button(title) {
                isOpenCamera.toggle()
                title = isOpenCamera ? "Close Rear Camera".localized : "Open Rear Camera".localized
                agoraKit.onTapBackCameraButton(isOpenCamera)
            }
        }
    }
}

//#Preview {
//    MutliCamera(configs: [:])
//}
