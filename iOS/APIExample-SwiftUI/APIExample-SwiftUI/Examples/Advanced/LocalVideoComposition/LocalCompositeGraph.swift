//
//  LocalVideoComposition.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2025/11/13.
//

import SwiftUI

struct LocalCompositeGraphEntry: View {
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
            NavigationLink(destination: LocalCompositeGraph(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct LocalCompositeGraph: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = LocalCompositeGraphRTC()
    @State private var toggleState: Bool = false
    
    var localView = VideoView()

    var body: some View {
        VStack {
            HStack{
                localView
            }
            Spacer()
            HStack {
                Toggle("", isOn: $toggleState)
                    .onChange(of: toggleState) { newValue in
                        agoraKit.updateToggleState(isOn: newValue)
                    }
                Spacer()
            }
            .padding()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}
