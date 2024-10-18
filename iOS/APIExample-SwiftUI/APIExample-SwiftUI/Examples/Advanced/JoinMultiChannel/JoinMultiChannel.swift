//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct JoinMultiChannelEntry: View {
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
            NavigationLink(destination: JoinMultiChannel(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct JoinMultiChannelEntryPreviews: PreviewProvider {
    static var previews: some View {
        JoinMultiChannelEntry()
    }
}

struct JoinMultiChannel: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = JoinMultiChannelRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    var remote2View = VideoView(type: .remote,
                               audioOnly: false)
    @State private var isShowLeaveAlert: Bool = false
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            remote2View.frame(height: 200)
            HStack {
                Button("LeaveChannelEx") {
                    isShowLeaveAlert.toggle()
                }
                .disabled(!agoraKit.joinEx)
                .colorMultiply(agoraKit.joinEx ? .white : .gray)
                .padding(5)
                .adaptiveBackground(.blue)
                .adaptiveForegroundStyle(.white)
#if os(iOS) && swift(>=5.7)
                .clipShape(.rect(cornerRadius: 5))
#else
                .clipShape(RoundedRectangle(cornerRadius: 5))
#endif
                .alert(isPresented: $isShowLeaveAlert) {
                    Alert(title: Text("stopMicrophoneRecording".localized), primaryButton: .default(Text("Stop".localized), action: {
                        agoraKit.onTapLeaveChannelEx(isStop: true)
                    }), secondaryButton: .cancel(Text("Cancel".localized), action: {
                        agoraKit.onTapLeaveChannelEx(isStop: false)
                    }))
                }
                
                Spacer()
                Button("take Snapshot Ex") {
                    agoraKit.onTapTakeSnapshotEx()
                }
                .disabled(!agoraKit.joinEx)
                .colorMultiply(agoraKit.joinEx ? .white : .gray)
                .padding(5)
                .adaptiveBackground(.blue)
                .adaptiveForegroundStyle(.white)
#if os(iOS) && swift(>=5.7)
                .clipShape(.rect(cornerRadius: 5))
#else
                .clipShape(RoundedRectangle(cornerRadius: 5))
#endif
                .alert(isPresented: $agoraKit.isShowAlert) {
                    Alert(title: Text(agoraKit.snapshotPath), dismissButton: .default(Text("Sure".localized)))
                }
            }.padding()
            Spacer()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView,
                              remote2View: remote2View.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct JoinMultiChannelPreviews: PreviewProvider {
    static var previews: some View {
        JoinMultiChannel(configs: [:])
    }
}
