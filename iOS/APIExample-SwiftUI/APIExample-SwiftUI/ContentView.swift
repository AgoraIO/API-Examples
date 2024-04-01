//
//  ContentView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/18.
//

import SwiftUI

struct MenuSection: Identifiable {
    let id = UUID()
    var name: String
    var rows: [MenuItem]
}

struct MenuItem: Identifiable {
    let id = UUID()
    var name: String
    var view: AnyView
    init(name: String, view: AnyView) {
        self.name = name
        self.view = view
    }
}

struct ContentView: View {
    var menus: [MenuSection] = [
        MenuSection(name: "Basic", rows: [
            MenuItem(name: "Join a channel (Token)".localized, 
                     view: AnyView(JoinChannelVideoTokenEntry())),
            MenuItem(name: "Join a channel (Video)".localized,
                     view: AnyView(JoinChannelVideoEntry())),
            MenuItem(name: "Join a channel (Audio)".localized,
                     view: AnyView(JoinChannelAudioEntry())),
            MenuItem(name: "Local or remote recording".localized,
                     view: AnyView(JoinChannelVideoRecorderEntry()))
        ]),
        MenuSection(name: "Anvanced", rows: [
//            MenuItem(name: "Group Video Chat".localized, storyboard: "VideoChat", controller: "VideoChat"),
//            MenuItem(name: "Live Streaming".localized, controller: "LiveStreaming"),
//            MenuItem(name: "RTMP Streaming".localized, controller: "RTMPStreaming"),
//            MenuItem(name: "Fusion CDN Streaming".localized, controller: "FusionCDN"),
//            MenuItem(name: "Video Metadata".localized, controller: "VideoMetadata".localized),
//            MenuItem(name: "Voice Changer".localized, controller: ""),
//            MenuItem(name: "Custom Audio Source".localized, controller: "CustomPcmAudioSource"),
//            MenuItem(name: "Custom Audio Render".localized, controller: "CustomAudioRender"),
//            MenuItem(name: "Custom Video Source(Push)".localized, controller: "CustomVideoSourcePush"),
//            MenuItem(name: "Custom Video Source(Multi)".localized,
//                     controller: "CustomVideoSourcePushMulti"),
//            MenuItem(name: "Custom Video Render".localized, controller: "CustomVideoRender"),
//            MenuItem(name: "Raw Audio Data".localized, controller: ""),
//            MenuItem(name: "Raw Video Data".localized, controller: ""),
//            MenuItem(name: "Picture In Picture(require iOS15)".localized, controller: "PictureInPicture"),
//            MenuItem(name: "Simple Filter Extension".localized, controller: "SimpleFilter"),
//            MenuItem(name: "Quick Switch Channel".localized, controller: "QuickSwitchChannel"),
//            MenuItem(name: "Join Multiple Channels".localized, controller: "JoinMultiChannel"),
//            MenuItem(name: "Stream Encryption".localized, controller: ""),
//            MenuItem(name: "Audio Mixing".localized, controller: ""),
//            MenuItem(name: "Precall Test".localized, controller: ""),
//            MenuItem(name: "Media Player".localized, controller: ""),
            MenuItem(name: "Screen Share".localized,
                     view: AnyView(ScreenShareEntry())),
            MenuItem(name: "Video Process".localized,
                     view: AnyView(VideoProcessEntry())),
            MenuItem(name: "Rhythm Player".localized,
                     view: AnyView(RhythmPlayerEntry())),
            MenuItem(name: "Create Data Stream".localized,
                     view: AnyView(CreateDataStreamEntry())),
            MenuItem(name: "Media Channel Relay".localized,
                     view: AnyView(MediaChannelRelayEntry())),
            MenuItem(name: "Spatial Audio".localized,
                     view: AnyView(SpatialAudioEntry())),
            MenuItem(name: "Content Inspect".localized,
                     view: AnyView(ContentInspectEntry())),
            MenuItem(name: "Mutli Camera(iOS13.0+)".localized,
                     view: AnyView(MutliCameraEntry())),
            MenuItem(name: "Ktv copyright music".localized,
                     view: AnyView(KtvCopyrightMusic())),
            MenuItem(name: "ARKit".localized,
                     view: AnyView(ARKitEntry())),
            MenuItem(name: "Audio Waveform".localized,
                     view: AnyView(AudioWaveformEntry())),
            MenuItem(name: "Face Capture".localized,
                     view: AnyView(FaceCaptureEntry()))
        ])
    ]

    var body: some View {
        NavigationView {
            List {
                ForEach(menus) { section in
                    Section(header: Text(section.name)) {
                        ForEach(section.rows) { item in
                            NavigationLink(destination: {
                                item.view.navigationTitle(item.name)
                            }) {
                                Text(item.name)
                            }
                        }
                    }
                }
            }
            .listStyle(GroupedListStyle())
            .navigationTitle("Agora API Example")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    NavigationLink(destination: {
                        SettingsView()
                    }) {
                        Text("Settings").foregroundStyle(.black)
                    }
                }
            }
        }
    }
}

#Preview {
    ContentView()
}
