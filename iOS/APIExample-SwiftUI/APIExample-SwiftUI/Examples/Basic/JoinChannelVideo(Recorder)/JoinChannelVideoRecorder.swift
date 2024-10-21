//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct JoinChannelVideoRecorderEntry: View {
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
            NavigationLink(destination: JoinChannelVideoRecorder(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct JoinChannelVideoRecorderEntryPreviews: PreviewProvider {
    static var previews: some View {
        JoinChannelVideoRecorderEntry()
    }
}

struct JoinChannelVideoRecorder: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @State private var isLocalStart: Bool = false
    @State private var isRemoteStart: Bool = false
    @State private var localRecord: String = "Recording".localized
    @State private var remoteRecord: String = "Recording".localized
    @ObservedObject private var agoraKit = JoinChannelVideoRecorderRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    var body: some View {
        VStack {
            HStack{
                ZStack(alignment: .bottomTrailing) {
                    localView
                    Button(localRecord) {
                        isLocalStart.toggle()
                        localRecord = isLocalStart ? "Stop Recording".localized : "Recording".localized
                        agoraKit.onTapLocalRecordButton(isStart: isLocalStart)
                    }.overlay(
                        RoundedRectangle(cornerRadius: 5).stroke(Color.red, lineWidth: 2).padding(-5)
                    )
                    .adaptiveForegroundStyle(.red)
                        .padding(.bottom, 20)
                        .padding(.trailing, 20)
                 }
                ZStack(alignment: .bottomTrailing) {
                    remoteView
                    Button(remoteRecord) {
                        isRemoteStart.toggle()
                        remoteRecord = isRemoteStart ? "Stop Recording".localized : "Recording".localized
                        agoraKit.onTapRemoteRecordButton(isStart: isRemoteStart)
                    }.overlay(
                        RoundedRectangle(cornerRadius: 5).stroke(Color.red, lineWidth: 2).padding(-5)
                    ).adaptiveForegroundStyle(.red)
                        .padding(.bottom, 20)
                        .padding(.trailing, 20)
                        .opacity(agoraKit.isRemoteJoind ? 1.0 : 0)
                 }
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

struct JoinChannelVideoRecorderPreviews: PreviewProvider {
    static var previews: some View {
        JoinChannelVideoRecorder(configs: [:])
    }
}
