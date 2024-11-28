//
//  JoinchannelVideoEnter.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI
import AgoraRtcKit

struct AudioWaveformEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var isShowAudioScence: Bool = false
    @State private var isShowAudioProfile: Bool = false
    @State private var profile: AgoraAudioProfile = .default
    @State private var scenario: AgoraAudioScenario = .default
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            HStack {
                Text("Audio Scenario".localized)
                Spacer()
                Button {
                    self.isShowAudioProfile = true
                } label: {
                    Text(scenario.description())
                }
                .adaptiveDialog(title: "Set Audio Scenario".localized, isPresented: $isShowAudioProfile, actions: {
                    ForEach(AgoraAudioScenario.allValues(), id: \.self) { item in
                        getAudioScenarioAction(item)
                    }
                    adaptiveCancelStyleButton(title: "Cancel".localized) {}
                }, actionSheetActions: AgoraAudioScenario.allValues().map { item in
                        .default(Text(item.description())) {
                            self.scenario = item
                        }
                    } + [.cancel(Text("Cancel".localized))])
            }.padding(EdgeInsets(top: 0, leading: 35, bottom: 15, trailing: 35))
            HStack {
                Text("Audio Profile".localized)
                Spacer()
                Button(profile.description()) {
                    self.isShowAudioScence = true
                }
                .adaptiveDialog(title: "Set Audio Profile".localized, isPresented: $isShowAudioScence, actions: {
                    ForEach(AgoraAudioProfile.allValues(), id: \.self) { item in
                        getAudioProfileAction(item)
                    }
                    adaptiveCancelStyleButton(title: "Cancel".localized) {}
                }, actionSheetActions: AgoraAudioProfile.allValues().map { item in
                        .default(Text(item.description())) {
                            self.profile = item
                        }
                    } + [.cancel(Text("Cancel".localized))])
            }.padding(EdgeInsets(top: 0, leading: 35, bottom: 0, trailing: 35))
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName,
                           "audioProfile": profile,
                           "audioScenario": scenario]
                self.isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: AudioWaveform(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
    
   private func getAudioProfileAction(_ profile: AgoraAudioProfile) -> some View {
        Button(profile.description()){
            self.profile = profile
        }
    }
    
   private func getAudioScenarioAction(_ scenario: AgoraAudioScenario) -> some View {
        Button(scenario.description()){
            self.scenario = scenario
        }
    }
}

struct AudioWaveFromEntryPreivew: PreviewProvider {
    static var previews: some View {
        AudioWaveformEntry()
    }
}

struct BoxingViewWrapper: UIViewRepresentable {
    let boxingview = ZSNBoxingView()
        
    func makeUIView(context: Context) -> UIView {
        boxingview.middleInterval = -10
        return boxingview
    }

    func updateUIView(_ uiView: UIView, context: Context) {
    }
}

struct AudioWaveform: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = AudioWaveformRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: true)
    var remoteView = VideoView(type: .remote,
                               audioOnly: true)
    var viewWrapper = BoxingViewWrapper()
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            Spacer(minLength: 200)
            viewWrapper.onReceive(agoraKit.$volume, perform: { _ in
                viewWrapper.boxingview.level = agoraKit.volume
            })
            Text("\(agoraKit.volume)")
            
        }.onAppear(perform: {
            viewWrapper.boxingview.start()
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            viewWrapper.boxingview.stop()
            agoraKit.onDestory()
        })
    }
}

struct AudioWaveFormPreview: PreviewProvider {
    static var previews: some View {
        AudioWaveform(configs: [:])
    }
}
