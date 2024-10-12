//
//  RawVideoData.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import SwiftUI

struct RawVideoDataEntry: View {
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
            NavigationLink(destination: RawVideoData(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct RawVideoData: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = RawVideoDataRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }
            .frame(maxHeight: 200)
            .padding(.bottom, 50)
            
            Button("screenshot".localized) {
                agoraKit.isSnapShoting = true
            }
            .disabled(!agoraKit.isJoined)
            .padding(.bottom, 10)
            
            if let image = agoraKit.screenshot {
                Image(uiImage: image)
                    .resizable()
                    .scaledToFit()
                    .frame(height: 400)
            }
            Spacer()
        }
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

//#Preview {
//    RawVideoData()
//}
