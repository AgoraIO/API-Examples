//
//  JoinchannelVideoEnter.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI
import AgoraRtcKit

struct RhythmPlayerEntry: View {
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
            NavigationLink(destination: RhythmPlayer(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
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

struct RhythmPlayerEntryPreviews: PreviewProvider {
    static var previews: some View {
        RhythmPlayerEntry()
    }
}

struct RhythmPlayer: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = RhythmPlayerRTC()
    @State private var beatsMinute: Float = 60
    @State private var beatsMeasure: Float = 4
    @State private var isPlaying: Bool = false
    
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
                Text("Beats per Minute".localized)
                Spacer()
                Slider(value: $beatsMinute, in: 60...360).onChange(of: beatsMinute) { value in
                    agoraKit.onChangeBeatsMinute(value)
                }
                .frame(width: 150)
                .disabled(agoraKit.isEnabelBeatsMinute)
            }.padding(.horizontal, 20)
            HStack {
                Text("Beats per Measure".localized)
                Spacer()
                Slider(value: $beatsMeasure, in: 1...9).onChange(of: beatsMeasure) { value in
                    agoraKit.onChangeBeatsMeasure(value)
                }
                .frame(width: 150)
                .disabled(agoraKit.isEnabelBeatsMeasure)
            }.padding(.horizontal, 20)
            HStack {
                Button("Play".localized) {
                    agoraKit.onPlay()
                    isPlaying = true
                }.disabled(isPlaying)
                Spacer()
                Button("Stop".localized) {
                    agoraKit.onStop()
                    isPlaying = false
                }.disabled(!isPlaying)
            }
            .padding(.horizontal, 90)
            .padding(.top, 30)
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

struct RhythmPlayerPreviews: PreviewProvider {
    static var previews: some View {
        RhythmPlayer(configs: [:])
    }
}
