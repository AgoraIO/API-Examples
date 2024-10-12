//
//  PictureInPicture.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import SwiftUI

enum PipType {
    case sdkRender, customRender
}

struct PipItem: Identifiable {
    let id = UUID()
    var name: String
    var type: PipType
}

struct PictureInPictureEntry: View {
    var items: [PipItem] = [
        PipItem(name: "PIP Custom Render", type: .customRender),
        PipItem(name: "PIP RTC SDK Render", type: .sdkRender)
    ]
    
    var body: some View {
        List {
            ForEach(items) { item in
                NavigationLink(destination: {
                    let entry = PictureInPictureJoinEntry(pipType: item.type)
                    entry.navigationTitle(item.name)
                }) {
                    Text(item.name)
                }
            }
        }
    }
}

struct PictureInPictureJoinEntry: View {
    var pipType: PipType
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]

    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name", text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName]
                self.isActive = true
            } label: {
                Text("Join Channel")
            }.disabled(channelName.isEmpty)
            Spacer()
            if pipType == .sdkRender {
                NavigationLink(destination: sdkRenderView().navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                    EmptyView()
                }
            } else {
                NavigationLink(destination: customRenderView().navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                    EmptyView()
                }
            }
            
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
    
    private func customRenderView() -> CustomRenderExample {
        return CustomRenderExample(configs: configs)
    }
    
    private func sdkRenderView() -> SDKRenderExample{
        return SDKRenderExample(configs: configs)
    }
    
}

//#Preview {
//    PictureInPictureJoinEntry(pipType: .customRender)
//}
