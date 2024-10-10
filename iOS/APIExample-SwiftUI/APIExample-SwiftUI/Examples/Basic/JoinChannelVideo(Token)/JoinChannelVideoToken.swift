//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct JoinChannelVideoTokenEntry: View {
    @State private var channelName: String = ""
    @State private var appId: String = ""
    @State private var token: String = ""
    @State private var isActive = false
    @State private var showAlert = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack(alignment: .leading) {
            Spacer()
            Text("AppID")
            TextField("Please the input APPID".localized, text: $appId).textFieldStyle(.roundedBorder)
            Text("Token")
            TextField("Please the input Token".localized, text: $token).textFieldStyle(.roundedBorder)
            Text("Channel Name")
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder)
            HStack {
                Spacer()
                Button {
                    configs = ["channelName": channelName,
                               "appId": appId,
                               "token": token]
                    UIApplication.shared.sendAction(#selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
                    self.isActive = true
                } label: {
                    Text("Join".localized)
                }.disabled(appId.isEmpty || token.isEmpty || channelName.isEmpty)
                Spacer()
            }.padding(.top, 20)
            Spacer()
            NavigationLink(destination: JoinChannelVideoToken(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }.padding(EdgeInsets(top: 0, leading: 50, bottom: 0, trailing: 50))

            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Button(action: {
                        showAlert = true
                    }) {
                        Image("tips")
                    }.alert(isPresented: $showAlert) {
                        Alert(title: Text("Quick input APPID and Token methods".localized), message: Text("I: the mobile phone and Mac log in to the same Apple account. After copying the Mac, it will automatically synchronize other terminals with the same account. The mobile phone can directly click the input box to paste.\n\n II: use https://cl1p.net/ online clipboard:\n\n1.Enter in a URL that starts with cl1p.net. Example cl1p.net/uqztgjnqcalmd\n\n2.Paste in anything you want.\n\n3.On another computer enter the same URL and get your stuff.".localized), dismissButton: .default(Text("OK")))
                    }
                }
            }
    }
}
#Preview {
    JoinChannelVideoTokenEntry()
}

struct JoinChannelVideoToken: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = JoinChannelVideoTokenRTC()
    
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

#Preview {
    JoinChannelVideoToken(configs: [:])
}
