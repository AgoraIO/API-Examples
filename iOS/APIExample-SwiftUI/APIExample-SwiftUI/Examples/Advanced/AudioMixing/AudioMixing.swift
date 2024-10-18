//
//  JoinchannelVideoEnter.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI
import AgoraRtcKit

struct AudioMixingEntry: View {
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
                .adaptiveDialog(
                    title: "Set Audio Scenario".localized,
                    isPresented: $isShowAudioProfile,
                    actions: {
                        ForEach(AgoraAudioScenario.allValues(), id: \.self) { item in
                            getAudioScenarioAction(item)
                        }
                        
                        adaptiveCancelStyleButton(title: "Cancel".localized) { }
                    },
                    actionSheetActions: AgoraAudioScenario.allValues().map { item in
                            .default(Text(item.description())) {
                                // Handle item selection
                                self.scenario = item
                            }
                        } + [.cancel(Text("Cancel".localized))]
                )
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
                    adaptiveCancelStyleButton(title: "Cancel".localized) { }
                }, actionSheetActions: AgoraAudioProfile.allValues().map { item in
                        .default(Text(item.description())) {
                            self.profile = profile
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
            NavigationLink(destination: AudioMixing(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
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

struct AudioMixingEntryPreviews: PreviewProvider {
    static var previews: some View {
        AudioMixingEntry()
    }
}

struct AudioMixing: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = AudioMixingRTC()
    @State private var audioMixingVolumeSlider: Double = 50
    
    var localView = VideoView(type: .local,
                              audioOnly: true)
    var remoteView = VideoView(type: .remote,
                               audioOnly: true)
    
    @State private var isStartAudioMix: Bool = false
    @State private var isPauseAudioMix: Bool = false
    @State private var isStartEffect: Bool = false
    @State private var isPauseEffect: Bool = false
    
    var body: some View {
        VStack {
            ScrollView {
                HStack{
                    localView
                    remoteView
                }.frame(height: 200)
                HStack {
                    Text("Audio Mixing Controls".localized)
                        .font(.title)
                        .padding(15)
                    Spacer()
                }
                HStack {
                    ProgressView(value: agoraKit.progress,
                                 total: 1.0)
                    Text(agoraKit.audioMixingDuration).padding(.horizontal, 10)
                }.padding(.horizontal, 15)
                
                HStack {
                    Button("Start".localized) {
                        agoraKit.onStartAudioMixing()
                        isStartAudioMix = true
                    }.disabled(isStartAudioMix)
                    Spacer()
                    Button("Resume".localized) {
                        agoraKit.onResumeAudioMixing()
                        isPauseAudioMix = false
                    }.disabled(!isPauseAudioMix)
                    Spacer()
                    Button("Pause".localized) {
                        agoraKit.onPauseAudioMixing()
                        isPauseAudioMix = true
                    }.disabled(!isStartAudioMix || isPauseAudioMix)
                    Spacer()
                    Button("Stop".localized) {
                        agoraKit.onStopAudioMixing()
                        isStartAudioMix = false
                        isPauseAudioMix = false
                    }.disabled(!isStartAudioMix)
                }
                .padding(.horizontal, 15)
                .padding(.vertical, 10)
                
                HStack {
                    Text("MixingVolume".localized)
                    Spacer()
                    Slider(value: $audioMixingVolumeSlider, in: 0...100, step: 1).onChange(of: audioMixingVolumeSlider) { value in
                        agoraKit.onChangeAudioMixingVolume(value)
                    }.frame(maxWidth: 150)
                }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
                
                HStack {
                    Text("MixingPlaybackVolume".localized)
                    Spacer()
                    Slider(value: $agoraKit.audioMixingPlaybackVolumeSlider, in: 0...100, step: 1).onChange(of: agoraKit.audioMixingPlaybackVolumeSlider) { value in
                        agoraKit.onChangeAudioMixingPlaybackVolume(value)
                    }.frame(maxWidth: 150)
                }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
                
                HStack {
                    Text("MixingPublishVolume".localized)
                    Spacer()
                    Slider(value: $agoraKit.audioMixingPublishVolumeSlider, in: 0...100, step: 1).onChange(of: agoraKit.audioMixingPublishVolumeSlider) { value in
                        agoraKit.onChangeAudioMixingPublishVolume(value)
                    }.frame(maxWidth: 150)
                }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
                
                HStack {
                    Text("Audio Effect Controls".localized)
                        .font(.title)
                        .padding(15)
                    Spacer()
                }
                HStack {
                    Button("Play".localized) {
                        agoraKit.onPlayEffect()
                        isStartEffect = true
                    }.disabled(isStartEffect)
                    Spacer()
                    Button("Resume".localized) {
                        agoraKit.onResumeEffect()
                        isPauseEffect = false
                    }.disabled(!isPauseEffect)
                    Spacer()
                    Button("Pause".localized) {
                        agoraKit.onPauseEffect()
                        isPauseEffect = true
                    }.disabled(!isStartEffect || isPauseEffect)
                    Spacer()
                    Button("Stop".localized) {
                        agoraKit.onStopEffect()
                        isStartEffect = false
                        isPauseEffect = false
                    }.disabled(!isStartEffect)
                }
                .padding(.horizontal, 15)
                .padding(.vertical, 10)
                HStack {
                    Text("EffectVolume".localized)
                    Spacer()
                    Slider(value: $agoraKit.audioEffectVolumeSlider, in: 0...100, step: 1).onChange(of: agoraKit.audioEffectVolumeSlider) { value in
                        agoraKit.onChangeAudioEffectVolume(value)
                    }.frame(maxWidth: 150)
                }.padding(EdgeInsets(top: 0, leading: 15, bottom: 40, trailing: 15))
            }
            Spacer()
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct AudioMixingPreviews: PreviewProvider {
    static var previews: some View {
        AudioMixing(configs: [:])
    }
}
