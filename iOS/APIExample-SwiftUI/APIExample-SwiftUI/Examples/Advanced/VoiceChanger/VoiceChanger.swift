//
//  VoiceChanger.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/29.
//

import SwiftUI

struct VoiceChangerEntry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName).textFieldStyle(.roundedBorder).padding()
            Button {
                configs = ["channelName": channelName]
                self.isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: VoiceChanger(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct VoiceChanger: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var voiceChangerRTC = VoiceChangerRTC()
    @State var selectedChatBeautifier: AgoraVoiceBeautifierPreset = .presetOff
    @State var selectedTimbreTransformation: AgoraVoiceBeautifierPreset = .presetOff
    @State var selectedVoiceChanger: AgoraAudioEffectPreset = .off
    @State var selectedStyleTransformation: AgoraAudioEffectPreset = .off
    @State var selectedRoomAcoustics: AgoraAudioEffectPreset = .off
    @State var selectedPitchCorrection: AgoraAudioEffectPreset = .off
    @State var currentAudioEffects: AgoraAudioEffectPreset = .off

    @State var shouldPreset = true
    @State var tonicModeValue: Float = 1
    @State var tonicPitchValue: Float = 1
    @State private var tonicModeEnable = false
    @State private var tonicPitchEnable = false

    @State private var tonicModeValueSliderRange: ClosedRange<Float> = 1.0...3.0
    @State private var tonicPitchValueSliderRange: ClosedRange<Float> = 1.0...12.0

    @State var chatBeautifierCases: [AgoraVoiceBeautifierPreset] = [
        .presetOff,
        .presetChatBeautifierFresh,
        .presetChatBeautifierVitality,
        .presetChatBeautifierMagnetic
    ]
    
    @State var timbreTransformationCases: [AgoraVoiceBeautifierPreset] = [
        .presetOff,
        .timbreTransformationVigorous,
        .timbreTransformationDeep,
        .timbreTransformationMellow,
        .timbreTransformationFalsetto,
        .timbreTransformationFull,
        .timbreTransformationClear,
        .timbreTransformationResounding,
        .timbreTransformatRinging
    ]
    
    @State var voiceChangerCases: [AgoraAudioEffectPreset] = [
        .off,
        .voiceChangerEffectUncle,
        .voiceChangerEffectOldMan,
        .voiceChangerEffectBoy,
        .voiceChangerEffectSister,
        .voiceChangerEffectGirl,
        .voiceChangerEffectPigKin,
        .voiceChangerEffectHulk
    ]
    
    @State var styleTransformationCases: [AgoraAudioEffectPreset] = [
        .off,
        .styleTransformationPopular,
        .styleTransformationRnb
    ]
    
    @State var roomAcousticsCases: [AgoraAudioEffectPreset] = [
        .off,
        .roomAcousticsKTV,
        .roomAcousVocalConcer,
        .roomAcousStudio,
        .roomAcousPhonograph,
        .roomAcousVirtualStereo,
        .roomAcousSpatial,
        .roomAcousEthereal,
        .roomAcous3DVoice
    ]
    
    @State var pitchCorrectionCases: [AgoraAudioEffectPreset] = [
        .off,
        .pitchCorrection
    ]
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    var body: some View {
        VStack {
            HStack {
                localView
                remoteView
            }
            .frame(maxHeight: 200)
            .padding(.bottom, 50)
            
            ScrollView {
                VStack(alignment: .leading) {
                    Text("Voice Beautifier & Effects Preset")
                        .font(.system(size: 15, weight: .bold))
                        .frame(maxWidth: .infinity, alignment: .leading)
                    HStack {
                        Text("Set Chat Beautifier".localized)
                        Picker("", selection: $selectedChatBeautifier) {
                            ForEach(chatBeautifierCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedChatBeautifier) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setVoiceBeautifierPreset(newValue)
                                resetAllSelectors(except: "ChatBeautifier")
                                updateAudioEffectsControls(.off)
                            }
                        }
                    }

                    HStack {
                        Text("Set Timbre Transformation".localized)
                        Picker("", selection: $selectedTimbreTransformation) {
                            ForEach(timbreTransformationCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedTimbreTransformation) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setVoiceBeautifierPreset(newValue)
                                resetAllSelectors(except: "TimbreTransformation")
                                updateAudioEffectsControls(.off)
                            }
                        }
                    }

                    HStack {
                        Text("Set Voice Changer".localized)
                        Picker("", selection: $selectedVoiceChanger) {
                            ForEach(voiceChangerCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedVoiceChanger) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setAudioEffectPreset(newValue)
                                resetAllSelectors(except: "VoiceChanger")
                                updateAudioEffectsControls(newValue)
                            }
                        }
                    }

                    HStack {
                        Text("Set Style Transformation".localized)
                        Picker("", selection: $selectedStyleTransformation) {
                            ForEach(styleTransformationCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedStyleTransformation) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setAudioEffectPreset(newValue)
                                resetAllSelectors(except: "StyleTransformation")
                                updateAudioEffectsControls(newValue)
                            }
                        }
                    }

                    HStack {
                        Text("Set Room Acoustics".localized)
                        Picker("", selection: $selectedRoomAcoustics) {
                            ForEach(roomAcousticsCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedRoomAcoustics) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setAudioEffectPreset(newValue)
                                resetAllSelectors(except: "RoomAcoustics")
                                updateAudioEffectsControls(newValue)
                            }
                        }
                    }

                    HStack {
                        Text("Pitch Correction".localized)
                        Picker("", selection: $selectedPitchCorrection) {
                            ForEach(pitchCorrectionCases) { e in
                                Text(e.description()).tag(e)
                            }
                        }
                        .onChange(of: selectedPitchCorrection) { newValue in
                            if shouldPreset {
                                voiceChangerRTC.agoraKit.setAudioEffectPreset(newValue)
                                resetAllSelectors(except: "PitchCorrection")
                                updateAudioEffectsControls(newValue)
                            }
                        }
                    }
                    
                    HStack {
                        Text("Tonic Mode".localized)
                        Slider(value: $tonicModeValue, in: tonicModeValueSliderRange) { isEditing in
                            if !isEditing {
                                voiceChangerRTC.agoraKit.setAudioEffectParameters(currentAudioEffects, param1: Int32(tonicModeValue), param2: Int32(tonicPitchValue))
                            }
                        }
                        .disabled(!tonicModeEnable)
                        
                        Text("Tonic Pitch".localized)
                        Slider(value: $tonicPitchValue, in: tonicPitchValueSliderRange) { isEditing in
                            if !isEditing {
                                voiceChangerRTC.agoraKit.setAudioEffectParameters(currentAudioEffects, param1: Int32(tonicModeValue), param2: Int32(tonicPitchValue))
                            }
                        }
                        .disabled(!tonicPitchEnable)
                    }
                }
                .padding(.leading, 20)
                .frame(maxWidth: .infinity, alignment: .leading)
            }
        }
        .onAppear(perform: {
            voiceChangerRTC.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            voiceChangerRTC.onDestory()
        })
    }
}

extension VoiceChanger {
    private func resetAllSelectors(except selected: String) {
        shouldPreset = false
        if selected != "ChatBeautifier" {
            selectedChatBeautifier = .presetOff
        }
        if selected != "TimbreTransformation" {
            selectedTimbreTransformation = .presetOff
        }
        if selected != "VoiceChanger" {
            selectedVoiceChanger = .off
        }
        if selected != "StyleTransformation" {
            selectedStyleTransformation = .off
        }
        if selected != "RoomAcoustics" {
            selectedRoomAcoustics = .off
        }
        if selected != "PitchCorrection" {
            selectedPitchCorrection = .off
        }
        DispatchQueue.main.async {
            shouldPreset = true
        }
    }
    
    func updateAudioEffectsControls(_ effect: AgoraAudioEffectPreset) {
        currentAudioEffects = effect
        if effect == .roomAcous3DVoice {
            tonicModeEnable = true
            tonicPitchEnable = false
            tonicModeValueSliderRange = 0...60
            tonicModeValue = 10
        } else if effect == .pitchCorrection {
            tonicModeEnable = true
            tonicPitchEnable = true
            tonicModeValueSliderRange = 1...3
            tonicModeValue = 1
            
            tonicPitchValueSliderRange = 1...12
            tonicPitchValue = 4
        } else {
            tonicModeEnable = false
            tonicPitchEnable = false
        }
    }
}

extension AgoraVoiceBeautifierPreset: Identifiable {
    public var id: Int { rawValue }
}

extension AgoraAudioEffectPreset: Identifiable {
    public var id: Int { rawValue }
}

#Preview {
    VoiceChanger()
}
