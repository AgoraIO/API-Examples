//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct CreateDataStreamEntry: View {
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
            NavigationLink(destination: CreateDataStream(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .adaptiveNavigationBarTitleDisplayMode()
    }
}

struct CreateDataStreamEntryPreview: PreviewProvider {
    static var previews: some View {
        CreateDataStreamEntry()
    }
}

struct CreateDataStream: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = CreateDataStreamRTC()
    @State private var textfield: String = ""
    
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
            Text("Send Message".localized)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.leading, 20)
                .padding(.top, 20)
            HStack(spacing: 20) {
                TextField("Input message".localized, text: $textfield).textFieldStyle(.roundedBorder)
                Button("Send".localized) {
                    agoraKit.onSendPress(message: textfield)
                    textfield = ""
                }.disabled(textfield.isEmpty)
            }
            .padding(.horizontal, 20)
            .alert(isPresented: $agoraKit.isShowAlert) {
                Alert(title: Text(agoraKit.alertMessage),
                      message: nil,
                      dismissButton: .default(Text("OK")))
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

struct CreateDataStreamPreviews: PreviewProvider {
    static var previews: some View {
        CreateDataStream(configs: [:])
    }
}
