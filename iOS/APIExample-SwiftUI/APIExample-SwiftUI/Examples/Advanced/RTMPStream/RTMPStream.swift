//
//  RTMStream.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/10/8.
//

import SwiftUI

struct RTMPStreamEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            Text("Ensure that you enable the RTMP Converter service at Agora Dashboard before using this function.")
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName]
                self.isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: RTMPStream(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .padding(.horizontal, 40)
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct RTMPStream: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = RTMPStreamRTC()
    
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
            
            if agoraKit.isJoined {
                VStack {
                    HStack {
                        TextField("", text: $agoraKit.rtmpUrl)
                            .disabled(agoraKit.isPublished)
                            
                        Button(agoraKit.isPublished ? "stopPublish".localized : "Publish".localized) {
                            agoraKit.onPublish()
                        }
                    }
                    HStack {
                        Spacer()
                        Toggle("Transcoding".localized, isOn: $agoraKit.transcodingState)
                            .disabled(agoraKit.isPublished)
                            .frame(width: 100)
                    }
                }
                .padding(.horizontal, 40)
            }
            
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

struct RTMPStreamPreviews: PreviewProvider {
    static var previews: some View {
        RTMPStream()
    }
}
