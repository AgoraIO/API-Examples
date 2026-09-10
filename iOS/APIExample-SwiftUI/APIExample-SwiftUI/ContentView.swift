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
    var requiresIOS15: Bool
    init(name: String, view: AnyView, requiresIOS15: Bool = false) {
        self.name = name
        self.view = view
        self.requiresIOS15 = requiresIOS15
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
        MenuSection(name: "Advanced", rows: [
            MenuItem(name: "Live Streaming".localized,
                     view: AnyView(LiveStreamingEntry())),
            MenuItem(name: "RTMP Streaming".localized,
                     view: AnyView(RTMPStreamEntry())),
            MenuItem(name: "Media Metadata".localized,
                     view: AnyView(VideoMetadataEntry())),
            MenuItem(name: "Voice Effects".localized,
                     view: AnyView(VoiceChangerEntry())),
            MenuItem(name: "Custom Audio Source (PCM)".localized,
                     view: AnyView(CustomPCMAudioSourceEntry())),
            MenuItem(name: "Custom Audio Render".localized,
                     view: AnyView(CustomAudioRenderEntry())),
            MenuItem(name: "Raw Audio Data".localized,
                     view: AnyView(RawAudioDataEntry())),
            MenuItem(name: "Audio Mixing".localized,
                     view: AnyView(AudioMixingEntry())),
            MenuItem(name: "Audio Waveform".localized,
                     view: AnyView(AudioWaveformEntry())),
            MenuItem(name: "Raw Video Data".localized,
                     view: AnyView(RawVideoDataEntry())),
            MenuItem(name: "Picture In Picture".localized,
                     view: AnyView(PictureInPictureEntry()),
                     requiresIOS15: true),
            MenuItem(name: "Video Process".localized,
                     view: AnyView(VideoProcessEntry())),
            MenuItem(name: "Local Video Transcoding".localized,
                     view: AnyView(LocalVideoTranscodingEntry())),
            MenuItem(name: "Local Composite Graph".localized,
                     view: AnyView(LocalCompositeGraphEntry())),
            MenuItem(name: "Quick Switch Channel".localized,
                     view: AnyView(QuickSwitchChannelEntry())),
            MenuItem(name: "Join Multiple Channels".localized,
                     view: AnyView(JoinMultiChannelEntry())),
            MenuItem(name: "Stream Encryption".localized,
                     view: AnyView(StreamEncryptionEntry())),
            MenuItem(name: "Pre-call Test".localized,
                     view: AnyView(PrecallTest())),
            MenuItem(name: "Media Player".localized,
                     view: AnyView(MediaPlayerEntry())),
            MenuItem(name: "Screen Share".localized,
                     view: AnyView(ScreenShareEntry())),
            MenuItem(name: "Send Data Stream".localized,
                     view: AnyView(CreateDataStreamEntry())),
            MenuItem(name: "Media Channel Relay".localized,
                     view: AnyView(MediaChannelRelayEntry())),
            MenuItem(name: "Spatial Audio".localized,
                     view: AnyView(SpatialAudioEntry())),
            MenuItem(name: "Content Inspect".localized,
                     view: AnyView(ContentInspectEntry())),
            MenuItem(name: "Multi Camera".localized,
                     view: AnyView(MutliCameraEntry())),
            MenuItem(name: "KTV Copyright Music".localized,
                     view: AnyView(KtvCopyrightMusic())),
            MenuItem(name: "ARKit".localized,
                     view: AnyView(ARKitEntry())),
            MenuItem(name: "Simulcast".localized,
                     view: AnyView(SimulcastEntry())),
            MenuItem(name: "Multipath".localized,
                     view: AnyView(MultipathEntry()))
        ])
    ]

    var body: some View {
        NavigationView {
            List {
                ForEach(menus) { section in
                    Section(header: Text(section.name)) {
                        ForEach(section.rows) { item in
                            MenuItemView(item: item)
                        }
                    }
                }
                
                Text("SDK Version: \(AgoraRtcEngineKit.getSdkVersion())")
            }
            .listStyle(GroupedListStyle())
            .navigationTitle("Agora API Example")
            .navigationBarTitleDisplayMode(.inline)
        }
    }
}

struct MenuItemView: View {
    var item: MenuItem
    var body: some View {
        if item.requiresIOS15 {
            if #available(iOS 15.0, *) {
                NavigationLink(destination: {
                    item.view.navigationTitle(item.name)
                }) {
                    Text(item.name)
                }
            } else {
                Button(action: {
                    showAlert()
                }) {
                    Text(item.name)
                }
            }
        } else {
            NavigationLink(destination: {
                item.view.navigationTitle(item.name)
            }) {
                Text(item.name)
            }
        }
    }
    
    func showAlert() {
        let alert = UIAlertController(title: "Unsupported", message: "Picture in Picture is not supported on this version of iOS.", preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default))
        
        if let rootViewController = UIApplication.shared.windows.first?.rootViewController {
            rootViewController.present(alert, animated: true, completion: nil)
        }
    }
}

struct ContentViewPreviews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
