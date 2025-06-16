//
//  PictureInPicture.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import SwiftUI

struct PipItem: Identifiable {
    let id = UUID()
    var name: String
}

struct PictureInPictureEntry: View {
    var items: [PipItem] = [
        PipItem(name: "Pixel Buffer Render"),
    ]
    
    var body: some View {
        List {
            ForEach(items) { item in
                NavigationLink(destination: {
                    let entry = PictureInPictureJoinEntry()
                    entry.navigationTitle(item.name)
                }) {
                    Text(item.name)
                }
            }
        }
    }
}

struct PictureInPictureJoinEntry: View {
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
            if #available(iOS 15.0, *) {
                NavigationLink(destination: customRenderView().navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                    EmptyView()
                }
            } else {
                // Fallback on earlier versions
            }
            
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
    
    @available(iOS 15.0, *)
    private func customRenderView() -> PixelBufferRenderExample {
        return PixelBufferRenderExample(configs: configs)
    }
    
}

struct PictureInPictureJoinEntryPreviews: PreviewProvider {
    static var previews: some View {
        PictureInPictureJoinEntry()
    }
}
