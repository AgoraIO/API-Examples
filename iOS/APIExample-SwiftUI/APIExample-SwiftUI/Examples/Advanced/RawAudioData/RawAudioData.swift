//
//  RawAudioData.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import SwiftUI

struct RawAudioDataEntry: View {
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
            NavigationLink(destination: RawAudioData(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct RawAudioData: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = RawAudioDataRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: true)
    var remoteView = VideoView(type: .remote,
                               audioOnly: true)
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            
            HStack {
                TextField("Please Enter Data".localized, text: $agoraKit.audioData)
                    .frame(height: 35)
                    .border(Color(UIColor.separator))
                    .padding(.leading, 40)

                Button("Send".localized) {
                    agoraKit.sendData()
                }
                .padding(.trailing, 20)
                .disabled(agoraKit.audioData.count == 0)
                Spacer()
            }
            .padding(.top, 50)
            
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

#Preview {
    RawAudioData()
}
