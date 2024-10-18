//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI
import ARKit

struct ARKitEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    @State private var isOn: Bool = false
    
    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            HStack(spacing: 15, content: {
                HStack(spacing: 10, content: {
                    Text("First Frame Optimization".localized)
                    Toggle("", isOn: $isOn).frame(width: 50)
                }).padding(.leading, 25)
                Spacer()
                Button {
                    configs = ["channelName": channelName,
                               "isFirstFrame": isOn]
                    self.isActive = true
                } label: {
                    Text("Join".localized)
                }.disabled(channelName.isEmpty)
                Spacer()
            })
            Spacer()
            NavigationLink(destination: ARKit(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct ARKitEntryPreviews: PreviewProvider {
    static var previews: some View {
        ARKitEntry()
    }
}

struct ARViewWrapper: UIViewRepresentable {
    let arView = ARSCNView()
    var statsLabel: UILabel = {
        let label = UILabel()
        label.text = ""
        label.textColor = .black
        label.font = .systemFont(ofSize: 14)
        label.numberOfLines = 0
        label.preferredMaxLayoutWidth = 200
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    var infoLabel: UILabel = {
        let label = UILabel()
        label.text = "Move Camera to find a planar \n(Shown as Red Rectangle)".localized
        label.textColor = .white
        label.font = .systemFont(ofSize: 14)
        label.numberOfLines = 0
        label.textAlignment = .center
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    func makeUIView(context: Context) -> ARSCNView {
        // Set up ARSCNView configuration here
        arView.addSubview(infoLabel)
        arView.addSubview(statsLabel)
        infoLabel.leadingAnchor.constraint(equalTo: arView.leadingAnchor, constant: 35).isActive = true
        infoLabel.trailingAnchor.constraint(equalTo: arView.trailingAnchor, constant: -35).isActive = true
        infoLabel.centerYAnchor.constraint(equalTo: arView.centerYAnchor).isActive = true
        
        statsLabel.leadingAnchor.constraint(equalTo: arView.leadingAnchor, constant: 15).isActive = true
        statsLabel.bottomAnchor.constraint(equalTo: arView.bottomAnchor, constant: -15).isActive = true
        
        return arView
    }
    
    func updateUIView(_ uiView: ARSCNView, context: Context) {
        // Update ARSCNView here if needed
    }
}

struct ARKit: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = ARKitRTC()
    @GestureState private var tappedLocation: CGPoint = .zero
    
    let sceneView = ARViewWrapper()
    
    var body: some View {
        VStack {
            sceneView.background(ignoresSafeAreaEdges: .all).alert(isPresented: $agoraKit.isSupportedAR) {
                let message = "This app requires world tracking, which is available only on iOS devices with the A9 processor or later.".localized
                return Alert(title: Text("ARKit is not available on this device.".localized), message: Text(message), dismissButton: .cancel())
            }.onReceive(agoraKit.$planarDetected, perform: { _ in
                if agoraKit.planarDetected {
                    sceneView.infoLabel.text = "Tap to place remote video canvas".localized
                } else {
                    sceneView.infoLabel.text = "Move Camera to find a planar\n(Shown as Red Rectangle)".localized
                }
            }).onReceive(agoraKit.$isHiddenStatsLabel, perform: { _ in
                sceneView.statsLabel.isHidden = agoraKit.isHiddenStatsLabel
                sceneView.statsLabel.text = agoraKit.stats
            }).onTapGesture {
                agoraKit.doSceneViewTapped(tappedLocation)
            }.gesture(DragGesture(minimumDistance: 0)
                .updating($tappedLocation) { (value, state, _) in
                    state = value.startLocation
                }
            )
            
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              sceneView: sceneView.arView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct ARKitPreviews: PreviewProvider {
    static var previews: some View {
        ARKit(configs: [:])
    }
}
