//
//  VideoMetadata.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import SwiftUI

struct VideoMetadataEntry: View {
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
            NavigationLink(destination: VideoMetadata(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct VideoMetadata: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = VideoMetaDataRTC()

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
            
            Button("Send Message".localized) {
                agoraKit.metadata = "\(Date())".data(using: .utf8)
            }
            .padding(.bottom, 150)
        }
        .alert(isPresented: $agoraKit.showAlert) {
            Alert(
                title: Text("Metadata received"),
                message: Text(agoraKit.receivedMsg)
                
            )
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
//    VideoMetadata()
//}
