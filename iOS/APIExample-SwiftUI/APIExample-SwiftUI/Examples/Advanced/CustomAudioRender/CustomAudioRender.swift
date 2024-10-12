//
//  CustomAudioRender.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/10/9.
//

import SwiftUI

struct CustomAudioRenderEntry: View {
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
            NavigationLink(destination: CustomAudioRender(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct CustomAudioRender: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = CustomAudioRenderRTC()
    
    var body: some View {
        VStack {
            GeometryReader { geometry in
                ScrollView {
                    LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible()), GridItem(.flexible())]) {
                        ForEach(agoraKit.audioInfos, id: \.self) { info in
                            VStack {
                                Text(info.content)
                                    .font(.system(size: 12))
                                    .foregroundColor(.gray)
                                    .frame(maxWidth: .infinity, maxHeight: .infinity)
                                    .background(.gray.opacity(0.5))
                            }
                            .frame(width: geometry.size.width / 3, height: 200)
                        }
                    }
                }
            }
        }
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

//#Preview {
//    CustomAudioRender()
//}
