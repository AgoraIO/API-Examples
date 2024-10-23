//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct SpatialAudioEntry: View {
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
            NavigationLink(destination: SpatialAudio(configs: configs).adaptiveNavigationTitle(channelName), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .adaptiveNavigationBarTitleDisplayMode()
    }
}

struct SpatialAudioEntryPreviews: PreviewProvider {
    static var previews: some View {
        SpatialAudioEntry()
    }
}

struct SpatialAudio: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = SpatialAudioRTC()
    @State private var position = CGPoint(x: 0.0, y: 0.0)
    @State private var isAttenuation: Bool = false
    @State private var isMic: Bool = true
    @State private var attenuationFrame: CGRect = .zero
    @State private var selfFrame: CGRect = .zero
    @State private var isShowVoice1: Bool = false
    @State private var isShowVoice2: Bool = false
    @State private var isVoice1Mute: Bool = false
    @State private var isVoice1VoiceBlur: Bool = false
    @State private var isVoice1Airborne: Bool = false
    @State private var voice1AttenuationValue: Float = 0.5
    @State private var isVoice2Mute: Bool = false
    @State private var isVoice2VoiceBlur: Bool = false
    @State private var isVoice2Airborne: Bool = false
    @State private var voice2AttenuationValue: Float = 0.5
    
    var body: some View {
        ZStack {
            VStack {
                Text("Move the red icon to experience the 3D audio effect".localized).padding(.vertical, 30)
                HStack {
                    ZStack {
                        GeometryReader {_ in
                            
                        }
                        GeometryReader { g in
                            Color.yellow.onAppear(perform: {
                                attenuationFrame = g.frame(in: .named("OuterV"))
                            }).opacity(isAttenuation ? 1.0 : 0.0)
                        }
                        VStack {
                            GeometryReader { g in
                                Button(action: {
                                    withAnimation {
                                        isShowVoice1.toggle()
                                    }
                                }, label: {
#if os(iOS) && swift(>=5.7)
                                    Image(.spatialSound2)
#else
                                    Image("spatial_sound2")
#endif
                                }).padding().onAppear {
                                    agoraKit.setupVoice1Frame(frame: g.frame(in: .named("OuterV")))
                                }
                            }
                            Spacer()
                            Text("Room".localized).padding(.bottom, 10).opacity(isAttenuation ? 1.0 : 0.0)
                        }
                    }
                    .frame(width: 76, height: 166)
                    .padding(.leading, 30)
                    
                    Spacer()
                    ZStack {
                        VStack {
                            GeometryReader { g in
                                Button(action: {
                                    withAnimation {
                                        isShowVoice2.toggle()
                                    }
                                }, label: {
#if os(iOS) && swift(>=5.7)
                                    Image(.spatialSound2)
#else
                                    Image("spatial_sound2")
#endif
                                }).padding().onAppear {
                                    agoraKit.setupVoice2Frame(frame: g.frame(in: .named("OuterV")))
                                }
                            }
                        }
                    }.frame(width: 76, height: 166).padding(.trailing, 30)
                }
                Spacer()
                GeometryReader { g in
#if os(iOS) && swift(>=5.7)
                    Image(.spatialSelf)
                        .position(position)
                        .gesture(
                            DragGesture()
                                .onChanged { value in
                                    let frame = g.frame(in: .named("OuterV")).origin
                                    let point = CGPointMake(value.location.x + frame.x, value.location.y + frame.y)

                                    position = value.location
                                    selfFrame = CGRect(origin: point, size: CGSize(width: 61, height: 47))
                                    agoraKit.updatePosition(frame: selfFrame)
                                }
                        ).onAppear {
                            let rect = g.frame(in: .named("OuterV"))
                            selfFrame = rect
                        }.zIndex(2.0)
#else
                    Image("spatial_self")
                        .position(position)
                        .gesture(
                            DragGesture()
                                .onChanged { value in
                                    let frame = g.frame(in: .named("OuterV")).origin
                                    let point = CGPoint(x: value.location.x + frame.x, y: value.location.y + frame.y)
                                                                    
                                    position = value.location
                                    selfFrame = CGRect(origin: point, size: CGSize(width: 61, height: 47))
                                    agoraKit.updatePosition(frame: selfFrame)
                                }
                        ).onAppear {
                            let rect = g.frame(in: .named("OuterV"))
                            selfFrame = rect
                        }.zIndex(2.0)
#endif
                }
                .frame(width: 61, height: 47)

                Spacer()
                HStack {
                    GeometryReader { g in
                        HStack {
#if os(iOS) && swift(>=5.7)
                            Image(.spatialRemote)
#else
                            Image("spatial_remote")
#endif
                            Text("\(agoraKit.remoteUser1)")
                        }
                        .opacity(agoraKit.remoteUser1 == 0 ? 0.0 : 1.0)
                        .adaptiveBackground(.clear)
                        .onAppear {
                            let frame = g.frame(in: .named("OuterV"))
                            agoraKit.remoteUser2Frame = frame
                        }
                    }
                    
                    Spacer()
                    GeometryReader { g in
                        HStack {
#if os(iOS) && swift(>=5.7)
                            Image(.spatialRemote)
#else
                            Image("spatial_remote")
#endif
                            Text("\(agoraKit.remoteUser2)")
                        }
                        .onAppear {
                            let frame = g.frame(in: .named("OuterV"))
                            agoraKit.remoteUser2Frame = frame
                        }
                        .opacity(agoraKit.remoteUser2 == 0 ? 0.0 : 1.0)
                    }
                }
                .padding(EdgeInsets(top: 0, leading: 50, bottom: 30, trailing: 50))
                
                HStack {
                    Spacer()
                    Text("Sound barrier".localized)
                    Toggle("", isOn: $isAttenuation).onChange(of: isAttenuation) { value in
                        agoraKit.onTapAudioAttenuationSwitch(value,
                                                             frame: attenuationFrame,
                                                             selfFrame: selfFrame)
                    }.frame(width: 50).zIndex(1)
                }.padding(.trailing, 30)
                HStack {
                    Spacer()
                    Text("Microphone".localized)
                    Toggle("", isOn: $isMic).onChange(of: isMic) { value in
                        agoraKit.onTapMicSwitch(value)
                    }.frame(width: 50).zIndex(1)
                }.padding(EdgeInsets(top: 0, leading: 0, bottom: 15, trailing: 30))
            }
            if isShowVoice1 {
                ActionSheetView(isShow: $isShowVoice1,
                                isMute: $isVoice1Mute,
                                isVoiceBlur: $isVoice1VoiceBlur,
                                isAirborne: $isVoice1Airborne,
                                attenuationValue: $voice1AttenuationValue,
                                isVoice1: .constant(true),
                                agoraKit: agoraKit)
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowVoice1)
            }
            if isShowVoice2 {
                ActionSheetView(isShow: $isShowVoice2,
                                isMute: $isVoice2Mute,
                                isVoiceBlur: $isVoice2VoiceBlur,
                                isAirborne: $isVoice2Airborne,
                                attenuationValue: $voice2AttenuationValue,
                                isVoice1: .constant(false),
                                agoraKit: agoraKit)
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowVoice2)
            }
            
        }.frame(minWidth: 0,
                maxWidth: .infinity,
                minHeight: 0,
                maxHeight: .infinity)
        .onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              selfFrame: selfFrame)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        }).coordinateSpace(name: "OuterV")
    }
}

struct ActionSheetView: View {
    @Binding var isShow: Bool
    @Binding var isMute: Bool
    @Binding var isVoiceBlur: Bool
    @Binding var isAirborne: Bool
    @Binding var attenuationValue: Float
    @Binding var isVoice1: Bool
    @StateObject var agoraKit: SpatialAudioRTC
    @State private var opacity: Double = 0
    var body: some View {
        ZStack {
            Color.black.opacity(opacity)
                .edgesIgnoringSafeArea(.all)
                .onTapGesture {
                    withAnimation {
                        isShow.toggle()
                        opacity = isShow ? 0.4 : 0.0
                    }
                }.onAppear {
                    withAnimation {
                        opacity = isShow ? 0.4 : 0.0
                    }
                }
            VStack {
                HStack {
                    Text("Mute".localized)
                    Toggle("", isOn: $isMute).onChange(of: isMute) { value in
                        agoraKit.mediaPlayerMute(isVoice1: isVoice1, isMute: value)
                    }
                }.padding(.horizontal, 15)
                HStack {
                    Text("Voice Blur")
                    Toggle("", isOn: $isVoiceBlur).onChange(of: isVoiceBlur) { value in
                        agoraKit.mediaPlayerVoiceBlur(isVoice1: isVoice1, isOn: value)
                    }
                }.padding(.horizontal, 15)
                HStack {
                    Text("Airborne Simulation")
                    Toggle("", isOn: $isAirborne).onChange(of: isAirborne) { value in
                        agoraKit.mediaPlayerAirborne(isVoice1: isVoice1, isOn: value)
                    }
                }.padding(.horizontal, 15)
                HStack {
                    Text("Attenuation")
                    Slider(value: $attenuationValue)
                        .padding(.leading, 20)
                        .onChange(of: attenuationValue) { value in
                            agoraKit.mediaPlayerAttenuation(isVoice1: isVoice1, value: value)
                        }
                }.padding(.horizontal, 15)
            }.frame(height: 200)
            .offset(x: 0, y: isShow ? 200 : 0)
        }
    }
}

struct SpatialAudioPreviews: PreviewProvider {
    static var previews: some View {
        SpatialAudio(configs: [:])
    }
}
