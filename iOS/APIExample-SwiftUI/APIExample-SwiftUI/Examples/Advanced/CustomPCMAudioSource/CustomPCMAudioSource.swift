//
//  CustomPCMAudioSource.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import SwiftUI

struct CustomPCMAudioSourceEntry: View {
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
            NavigationLink(destination: CustomPCMAudioSource(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct CustomPCMAudioSource: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = CustomPCMAudioSourceRTC()
    @State private var displayPcm = false
    
    var body: some View {
        VStack {
            DynamicGridView(
                items: agoraKit.audioViews
            )
            .frame(height: 200)
            .padding()
            
            Spacer()
            
            Toggle("Push PCM".localized, isOn: $displayPcm)
                .onChange(of: displayPcm) { newValue in
                    agoraKit.pushPCM(state: newValue)
                }
                .disabled(!agoraKit.isJoined)
                .padding(.bottom, 300)
        }
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct DynamicGridView: View {
    let items: [AudioStreamInfo]
    
    var body: some View {
        GeometryReader { geometry in
            let columns = 3
            let itemWidth = geometry.size.width / CGFloat(columns)
            
            LazyVGrid(columns: Array(repeating: GridItem(.fixed(itemWidth)), count: columns)) {
                ForEach(items, id: \.self) { item in
                    VideoView(type: item.local ? .local : .remote, audioOnly: true)
                        .adaptiveBackground(.gray)
                        .frame(width: itemWidth, height: 200)
                        .padding(4)
                }
            }
        }
    }
}

struct CustomPCMAudioSourcePreviews: PreviewProvider {
    static var previews: some View {
        CustomPCMAudioSource()
    }
}
