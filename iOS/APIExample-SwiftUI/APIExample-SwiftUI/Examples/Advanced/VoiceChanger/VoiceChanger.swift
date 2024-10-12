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
    @State var selectedVoiceConversion: AgoraVoiceConversionPreset = .off
    @State var selectedAinsModel: AUDIO_AINS_MODE = .AINS_MODE_BALANCED
    @State var currentAudioEffects: AgoraAudioEffectPreset = .off
    @State var selectedVoiceEqualizationFreq: AgoraAudioEqualizationBandFrequency = .band31
    @State var selectedReverbKay: AgoraAudioReverbType = .dryLevel
    
    @State var tonicModeValue: Float = 1
    @State var tonicPitchValue: Float = 1
    @State var voiceFormantValue: Float = 0
    @State var bandGainValue: Float = 0
    @State var pitchValue: Float = 1
    @State var reverbValue: Float = 0.5
    
    @State var shouldPreset = true
    @State private var tonicModeEnable = false
    @State private var tonicPitchEnable = false

    @State private var tonicModeValueSliderRange: ClosedRange<Float> = 1.0...3.0
    @State private var tonicPitchValueSliderRange: ClosedRange<Float> = 1.0...12.0
    @State private var voiceFormantValueSliderRange: ClosedRange<Float> = -1.0...1.0
    @State private var pitchValueSliderRange: ClosedRange<Float> = 0.5...2.0
    @State private var bandGainSliderRange: ClosedRange<Float> = -15.0...15.0
    @State private var reverbValueSliderRange: ClosedRange<Float> = 0.0...1.0

    @State var ainsModelCases: [AUDIO_AINS_MODE] = [
        .AINS_MODE_BALANCED,
        .AINS_MODE_AGGRESSIVE,
        .AINS_MODE_ULTRALOWLATENCY
    ]
    
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
    
    @State var voiceConversion: [AgoraVoiceConversionPreset] = [
        .off,
        .neutral,
        .sweet,
        .changerSolid,
        .changerBass
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
    
    @State var voiceConversionCases: [AgoraVoiceConversionPreset] = [
        .off,
        .neutral,
        .sweet,
        .changerSolid,
        .changerBass
    ]
    
    @State var voiceEqualizationFreqCases: [AgoraAudioEqualizationBandFrequency] = [
        .band31,
        .band62,
        .band125,
        .band250,
        .band500,
        .band1K,
        .band2K,
        .band4K,
        .band8K,
        .band16K
    ]
    
    @State var reverbKeyCases: [AgoraAudioReverbType] = [
        .dryLevel,
        .wetLevel,
        .roomSize,
        .wetDelay,
        .strength
    ]
    
    var reverbMap: [AgoraAudioReverbType: Int] = [
        .dryLevel: 0,
        .wetLevel: 0,
        .roomSize: 0,
        .wetDelay: 0,
        .strength: 0
    ]
    
    var localView = VideoView(type: .local,
                              audioOnly: true)
    var remoteView = VideoView(type: .remote,
                               audioOnly: true)
    
    var voiceBeautifierViews: some View {
        Group {
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
                Text(selectedPitchCorrection == .off ? "N/A" : "Tonic Mode".localized)
                    .foregroundStyle(tonicModeEnable ? .black : .gray)
                Slider(value: $tonicModeValue, in: tonicModeValueSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setAudioEffectParameters(currentAudioEffects, param1: Int32(tonicModeValue), param2: Int32(tonicPitchValue))
                    }
                }
                .tint(tonicModeEnable ? .blue : .gray)
                .disabled(!tonicModeEnable)

                Text(selectedPitchCorrection == .off ? "N/A" : "Tonic Pitch".localized)
                    .foregroundStyle(tonicModeEnable ? .black : .gray)
                Slider(value: $tonicPitchValue, in: tonicPitchValueSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setAudioEffectParameters(currentAudioEffects, param1: Int32(tonicModeValue), param2: Int32(tonicPitchValue))
                    }
                }
                .tint(tonicModeEnable ? .blue : .gray)
                .disabled(!tonicPitchEnable)
            }

            HStack {
                Text("Voice Conversion".localized)
                Picker("", selection: $selectedVoiceConversion) {
                    ForEach(voiceConversionCases) { e in
                        Text(e.description()).tag(e)
                    }
                }
                .onChange(of: selectedVoiceConversion) { newValue in
                    if shouldPreset {
                        voiceChangerRTC.agoraKit.setVoiceConversionPreset(newValue)
                        resetAllSelectors(except: "VoiceConversion")
                        updateAudioEffectsControls(.off)
                    }
                }
            }

            VStack {
                Text("Voice Formant".localized)
                Slider(value: $voiceFormantValue, in: voiceFormantValueSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setLocalVoiceFormant(Double(voiceFormantValue))
                    }
                }
            }
        }
    }
    
    var voiceEffectViews: some View {
        Group {
            Text("Customize Voice Effects".localized)
                .font(.system(size: 15, weight: .bold))
                .frame(maxWidth: .infinity, alignment: .leading)
            HStack {
                Text("Pitch".localized)
                Slider(value: $pitchValue, in: pitchValueSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setLocalVoicePitch(Double(pitchValue))
                    }
                }
            }

            HStack {
                Text("BandFreq".localized)
                Picker("", selection: $selectedVoiceEqualizationFreq) {
                    ForEach(voiceEqualizationFreqCases) { e in
                        Text(e.description()).tag(e)
                    }
                }
                .onChange(of: selectedVoiceEqualizationFreq) { newValue in
                    voiceChangerRTC.agoraKit.setLocalVoiceEqualizationOf(selectedVoiceEqualizationFreq, withGain: Int(bandGainValue))
                }
            }

            HStack {
                Text("BandGain".localized)
                Slider(value: $bandGainValue, in: bandGainSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setLocalVoiceEqualizationOf(selectedVoiceEqualizationFreq, withGain: Int(bandGainValue))
                    }
                }
            }

            HStack {
                Text("ReverbKey".localized)
                Picker("", selection: $selectedReverbKay) {
                    ForEach(reverbKeyCases) { e in
                        Text(e.description()).tag(e)
                    }
                }
                .onChange(of: selectedReverbKay) { newValue in
                    updateReverbValueRange(reverbKey: newValue)
                }
            }

            HStack {
                Text("ReverbValue".localized)
                Slider(value: $reverbValue, in: reverbValueSliderRange) { isEditing in
                    if !isEditing {
                        voiceChangerRTC.agoraKit.setLocalVoiceReverbOf(selectedReverbKay, withValue: Int(reverbValue))
                    }
                }
            }
        }
    }
    
    var anisModelViews: some View {
        Group {
            HStack {
                Text("AINSModel".localized)
                Picker("", selection: $selectedAinsModel) {
                    ForEach(ainsModelCases) { e in
                        Text(e.description()).tag(e)
                    }
                }
                .onChange(of: selectedAinsModel) { newValue in
                    if shouldPreset {
                        voiceChangerRTC.agoraKit.setAINSMode(true, mode: newValue)
                        resetAllSelectors(except: "AINSModel")
                        updateAudioEffectsControls(.off)
                    }
                }
            }
        }
    }
    
    var body: some View {
        VStack {
            HStack {
                localView
                    .background(.gray)
                remoteView
                    .background(.gray)
            }
            .frame(maxHeight: 200)
            .padding(.bottom, 50)
            
            ScrollView {
                VStack(alignment: .leading) {
                    anisModelViews
                    voiceBeautifierViews
                    voiceEffectViews
                }
                .padding(.all, 20)
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
        if selected != "VoiceConversion" {
            selectedVoiceConversion = .off
        }
        if selected != "AINSModel" {
            selectedAinsModel = .AINS_MODE_BALANCED
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
    
    private func updateReverbValueRange(reverbKey: AgoraAudioReverbType) {
        var min: Float = 0, max: Float = 0
        switch reverbKey {
        case .dryLevel:
            min = -20
            max = 10
            
        case .wetLevel:
            min = -20
            max = 10
            
        case .roomSize:
            min = 0
            max = 100
            
        case .wetDelay:
            min = 0
            max = 200
            
        case .strength:
            min = 0
            max = 100

        default: break
        }
        reverbValueSliderRange = min...max
        reverbValue = Float(reverbMap[reverbKey] ?? 0)
    }
}

extension AgoraVoiceBeautifierPreset: Identifiable {
    public var id: Int { rawValue }
}

extension AgoraAudioEffectPreset: Identifiable {
    public var id: Int { rawValue }
}

extension AgoraVoiceConversionPreset: Identifiable {
    public var id: Int { rawValue }
}

extension AUDIO_AINS_MODE: Identifiable {
    public var id: Int { rawValue }
}

extension AgoraAudioEqualizationBandFrequency: Identifiable {
    public var id: Int { rawValue }
}

extension AgoraAudioReverbType: Identifiable {
    public var id: Int { rawValue }
}
//#Preview {
//    VoiceChanger()
//}
