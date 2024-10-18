//
//  JoinchannelVideoEnter.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI
import AgoraRtcKit

struct JoinChannelAudioEntry: View {
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
                }.confirmationDialog("Set Audio Scenario".localized, isPresented: $isShowAudioProfile, titleVisibility: .visible) {
                    ForEach(AgoraAudioScenario.allValues(), id: \.self) { item in
                        getAudioScenarioAction(item)
                    }
                    Button("Cancel".localized, role: .cancel) {}
                }
            }.padding(EdgeInsets(top: 0, leading: 35, bottom: 15, trailing: 35))
            HStack {
                Text("Audio Profile".localized)
                Spacer()
                Button(profile.description()) {
                    self.isShowAudioScence = true
                }.confirmationDialog("Set Audio Profile".localized, isPresented: $isShowAudioScence, titleVisibility: .visible) {
                    ForEach(AgoraAudioProfile.allValues(), id: \.self) { item in
                        getAudioProfileAction(item)
                    }
                    Button("Cancel".localized, role: .cancel) {}
                }
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
            NavigationLink(destination: JoinChannelAudio(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
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

struct JoinChannelAudioEntryPreviews: PreviewProvider {
    static var previews: some View {
        JoinChannelAudioEntry()
    }
}

struct JoinChannelAudio: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = JoinChannelAudioRTC()
    @State private var isShowAudioProfile: Bool = false
    @State private var scenario: AgoraAudioScenario = .default
    @State private var recordingVolumeSlider: Double = 100
    @State private var playbackVolumeSlider: Double = 100
    @State private var inEarMonitoringVolumeSlider: Double = 100
    @State private var inEarMonitoringSwitch: Bool = false
    
    var localView = VideoView(type: .local,
                              audioOnly: true)
    var remoteView = VideoView(type: .remote,
                               audioOnly: true)
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            Spacer()
            HStack {
                Text("Audio Scenario".localized)
                Spacer()
                Button(scenario.description()) {
                    isShowAudioProfile = true
                }.confirmationDialog("Set Audio Scenario".localized, isPresented: $isShowAudioProfile, titleVisibility: .visible) {
                    ForEach(AgoraAudioScenario.allValues(), id: \.self) { item in
                        getAudioScenarioAction(item)
                    }
                    Button("Cancel".localized, role: .cancel) {}
                }.padding(.trailing, 30)
            }.padding(EdgeInsets(top: 0, leading: 15, bottom: 10, trailing: 15))
            HStack {
                Text("RecordingVolume".localized)
                Spacer()
                Slider(value: $recordingVolumeSlider, in: 0...400, step: 1).onChange(of: recordingVolumeSlider) { value in
                    agoraKit.onChangeRecordingVolume(value)
                }.frame(maxWidth: 150)
            }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
            HStack {
                Text("PlaybackVolume".localized)
                Spacer()
                Slider(value: $playbackVolumeSlider, in: 0...400, step: 1).onChange(of: playbackVolumeSlider) { value in
                    agoraKit.onChangePlaybackVolume(value)
                }.frame(maxWidth: 150)
            }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
            HStack {
                Text("InEar Monitoring".localized)
                Spacer()
                Toggle("", isOn: $inEarMonitoringSwitch).onChange(of: inEarMonitoringSwitch) { value in
                    agoraKit.toggleInEarMonitoring(value)
                }.padding(.trailing, 20)
            }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
            HStack {
                Text("InEar Monitoring Volume".localized)
                Spacer()
                Slider(value: $inEarMonitoringVolumeSlider, in: 0...100, step: 1).onChange(of: inEarMonitoringVolumeSlider) { value in
                    agoraKit.onChangeInEarMonitoringVolume(value)
                }.frame(maxWidth: 150).disabled(!inEarMonitoringSwitch)
            }.padding(EdgeInsets(top: 0, leading: 15, bottom: 0, trailing: 15))
        }.onAppear(perform: {
            scenario = configs["audioScenario"] as! AgoraAudioScenario
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
    
    private func getAudioScenarioAction(_ scenario: AgoraAudioScenario) -> some View {
        Button(action: {
            self.scenario = scenario
            self.agoraKit.setAudioScenario(scenario: scenario)
        }, label: {
            Text(scenario.description())
        })
     }
}

struct JoinChannelAudioPreviews: PreviewProvider {
    static var previews: some View {
        JoinChannelAudio(configs: [:])
    }
}
