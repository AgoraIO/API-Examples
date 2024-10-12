//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct FaceCaptureEntry: View {
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
            NavigationLink(destination: FaceCapture(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

//#Preview {
//    FaceCaptureEntry()
//}

struct FaceCapture: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = FaceCaptureRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    
    var body: some View {
        VStack {
            HStack{
                localView
            }
            Spacer()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        }).alert(isPresented: $agoraKit.isFaceLicenseEmpty, content: {
            Alert(title: Text("Please contact Agora customer service to obtain a face capture certificate".localized))
        })
    }
}

//#Preview {
//    FaceCapture(configs: [:])
//}
