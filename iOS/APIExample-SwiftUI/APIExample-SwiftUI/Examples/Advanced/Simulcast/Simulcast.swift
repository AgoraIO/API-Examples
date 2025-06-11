import SwiftUI
import AgoraRtcKit

struct SimulcastEntry: View {
    @State private var channelName: String = ""
    @State private var roleIndex: Int = 0
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName)
                .textFieldStyle(.roundedBorder)
                .padding()
            
            HStack {
                Text("Role".localized)
                Picker("Role", selection: $roleIndex) {
                    Text("broadcaster").tag(0)
                    Text("audience").tag(1)
                }
                .pickerStyle(SegmentedPickerStyle())
            }
            .padding(.horizontal)
            
            Button {
                configs = [
                    "channelName": channelName,
                    "role_index": roleIndex
                ]
                self.isActive = true
            } label: {
                Text("Join".localized)
                    .frame(width: 200)
                    .padding()
            }
            .disabled(channelName.isEmpty)
            
            Spacer()
            NavigationLink(destination: Simulcast(configs: configs)
                .navigationTitle(channelName)
                .navigationBarTitleDisplayMode(.inline),
                         isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct SimulcastEntryPreviews: PreviewProvider {
    static var previews: some View {
        NavigationView {
            SimulcastEntry()
        }
    }
}

struct Simulcast: View {
    @State var configs: [String: Any] = [:]
    
    @State private var layer1Enabled = true
    @State private var layer2Enabled = true
    @State private var layer3Enabled = true
    @State private var layer4Enabled = true
    @State private var selectedLayer = 0
    
    @ObservedObject private var agoraKit = SimulcastRTC()
    
    var localView = VideoView(type: .local, audioOnly: false)
    var remoteView = VideoView(type: .remote, audioOnly: false)
    
    var body: some View {
        VStack {
            HStack {
                localView
                remoteView
            }
            .frame(maxHeight: 200)
            
            if let roleIndex = configs["role_index"] as? Int {
                if roleIndex == 0 {
                    Spacer()
                    // Broadcaster controls
                    VStack(spacing: 12) {
                        Toggle("Layer1:720p30fps", isOn: $layer1Enabled)
                            .onChange(of: layer1Enabled) { _ in
                                agoraKit.updateSimulcastConfig(layer: .layer1, enable: layer1Enabled)
                            }
                        Toggle("Layer2:540p15fps", isOn: $layer2Enabled)
                            .onChange(of: layer2Enabled) { _ in
                                agoraKit.updateSimulcastConfig(layer: .layer2, enable: layer2Enabled)
                            }
                        Toggle("Layer3:360p15fps", isOn: $layer3Enabled)
                            .onChange(of: layer3Enabled) { _ in
                                agoraKit.updateSimulcastConfig(layer: .layer3, enable: layer3Enabled)
                            }
                        Toggle("Layer4:270p15fps", isOn: $layer4Enabled)
                            .onChange(of: layer4Enabled) { _ in
                                agoraKit.updateSimulcastConfig(layer: .layer4, enable: layer4Enabled)
                            }
                    }
                    .padding()
                    .padding(.bottom)
                } else {
                    // Audience controls
                    Picker("Select Layer", selection: $selectedLayer) {
                        Text("Layer1").tag(0)
                        Text("Layer2").tag(1)
                        Text("Layer3").tag(2)
                        Text("Layer4").tag(3)
                    }
                    .pickerStyle(SegmentedPickerStyle())
                    .padding()
                    .onChange(of: selectedLayer) { newValue in
                        let streamType: AgoraVideoStreamType
                        switch newValue {
                        case 0: streamType = .layer1
                        case 1: streamType = .layer2
                        case 2: streamType = .layer3
                        case 3: streamType = .layer4
                        default: streamType = .layer1
                        }
                        agoraKit.setRemoteVideoStream(type: streamType)
                    }
                    Spacer()
                }
            }
        }
        .onAppear {
            // Only setup RTC if we have valid configs
            if let _ = configs["channelName"] as? String,
               let _ = configs["role_index"] as? Int {
                agoraKit.setupRTC(configs: configs,
                                  localView: localView.videoView,
                                  remoteView: remoteView.videoView)
            }
        }
        .onDisappear {
            agoraKit.onDestory()
        }
    }
}

struct SimulcastPreviews: PreviewProvider {
    static var previews: some View {
        NavigationView {
            Simulcast(configs: [
                "channelName": "test",
                "role_index": 0
            ])
        }
    }
} 
