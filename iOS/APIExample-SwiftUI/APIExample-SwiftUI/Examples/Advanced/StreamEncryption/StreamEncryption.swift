//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct StreamEncryptionEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    @State private var encryptSecretField: String = ""
    @State private var isShowActionSheet: Bool = false
    @State private var mode: AgoraEncryptionMode = .AES128GCM2
    @State private var useCustom: Bool = false
    
    var body: some View {
        VStack {
            Spacer()
            HStack {
                Text("Encryption Mode".localized)
                Spacer()
                Button(useCustom ? "Custom" : mode.description()) {
                    isShowActionSheet.toggle()
                }.confirmationDialog("Set Encryption Mode".localized, isPresented: $isShowActionSheet, titleVisibility: .visible) {
                    ForEach(AgoraEncryptionMode.allValues(), id: \.self) { item in
                        Button(item.description()) {
                            mode = item
                            useCustom = false
                        }
                    }
                    Button("Custom") {
                        useCustom = true
                    }
                    Button("Cancel".localized, role: .cancel) {}
                }
                Spacer()
            }.padding(.leading, 15)
            TextField("Enter encryption secret".localized, text: $encryptSecretField).textFieldStyle(.roundedBorder)
                .padding(.horizontal, 15)
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName,
                           "mode": mode,
                           "secret": encryptSecretField,
                           "useCustom": useCustom]
                self.isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: StreamEncryption(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct StreamEncryptionEntryPreviews: PreviewProvider {
    static var previews: some View {
        StreamEncryptionEntry()
    }
}

struct StreamEncryption: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = StreamEncryptionRTC()
    
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
        })
    }
}

struct StreamEncryptionPreviews: PreviewProvider {
    static var previews: some View {
        StreamEncryption(configs: [:])
    }
}
