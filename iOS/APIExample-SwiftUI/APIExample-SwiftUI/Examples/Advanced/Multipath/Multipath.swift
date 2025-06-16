import SwiftUI
import AgoraRtcKit

struct MultipathEntry: View {
    @State private var channelName: String = ""
    @State private var roleIndex: Int = 0
    @State private var modeIndex: Int = 0
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
            
            HStack {
                Text("Mode".localized)
                Picker("Mode", selection: $modeIndex) {
                    Text("dynamic").tag(0)
                    Text("duplicate").tag(1)
                }
                .pickerStyle(SegmentedPickerStyle())
            }
            .padding(.horizontal)
            
            Button {
                configs = [
                    "channelName": channelName,
                    "role_index": roleIndex,
                    "mode_index": modeIndex
                ]
                self.isActive = true
            } label: {
                Text("Join".localized)
                    .frame(width: 200)
                    .padding()
            }
            .disabled(channelName.isEmpty)
            
            Spacer()
            NavigationLink(destination: Multipath(configs: configs)
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

struct MultipathEntryPreviews: PreviewProvider {
    static var previews: some View {
        NavigationView {
            MultipathEntry()
        }
    }
}

struct Multipath: View {
    @State var configs: [String: Any] = [:]
    
    @State private var multipathEnabled = true
    @ObservedObject private var agoraKit = MultipathRTC()
    
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
                    VStack {
                        HStack {
                            Text("Current Mode:")
                            Spacer()
                            Text(configs["mode_index"] as? Int == 0 ? "dynamic" : "duplicate")
                        }
                        .padding()
                        
                        Toggle("Enable Multipath", isOn: $multipathEnabled)
                            .onChange(of: multipathEnabled) { newValue in
                                agoraKit.updateMultipath(enabled: newValue)
                            }
                            .padding()
                    }
                    .padding(.bottom)
                } else {
                    Spacer()
                }
            }
        }
        .onAppear {
            agoraKit.setupRTC(configs: configs,
                             localView: localView.videoView,
                             remoteView: remoteView.videoView)
        }
        .onDisappear {
            agoraKit.onDestory()
        }
    }
}

struct MultipathPreviews: PreviewProvider {
    static var previews: some View {
        NavigationView {
            Multipath(configs: [
                "channelName": "test",
                "role_index": 0,
                "mode_index": 0
            ])
        }
    }
} 
