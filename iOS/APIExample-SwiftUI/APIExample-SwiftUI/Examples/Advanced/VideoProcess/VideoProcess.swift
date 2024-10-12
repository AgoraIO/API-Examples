//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct VideoProcessEntry: View {
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
            NavigationLink(destination: VideoProcess(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

//#Preview {
//    VideoProcessEntry()
//}

struct VideoProcess: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = VideoProcessRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    @State private var beautySwitch: Bool = false
    @State private var colorEnhanceSwitch: Bool = false
    @State private var virtualBgSwitch: Bool = false
    @State private var lightenSlider: Float = 0.5
    @State private var rednessSlider: Float = 0.5
    @State private var sharpnessSlider: Float = 0.5
    @State private var smoothnessSlider: Float = 0.5
    @State private var lowLightEnhanceSwitch: Bool = false
    @State private var videoDenoiseSwitch: Bool = false
    @State private var strengthSlider: Float = 0.5
    @State private var skinProtectSlider: Float = 0.5
    @State private var selectedSegment = 0
    private let segments = ["Image".localized,
                            "Color".localized,
                            "Blur".localized,
                            "Video".localized]
    
    var body: some View {
        ScrollView {
            VStack {
                HStack{
                    localView
                    remoteView
                }.frame(height: 200)
                HStack {
                    Text("Face Beauty".localized)
                    Spacer()
                    Toggle("", isOn: $beautySwitch)
                        .frame(width: 50)
                        .onChange(of: beautySwitch) { value in
                            agoraKit.onChangeBeauty(value)
                        }
                    Spacer()
                }
                .padding(.leading, 20)
                HStack {
                    VStack {
                        Text("Lightening".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $lightenSlider).onChange(of: lightenSlider) { value in
                            agoraKit.onLightenSlider(value,
                                                     isOn: beautySwitch)
                        }
                    }
                    VStack {
                        Text("Redness".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $rednessSlider).onChange(of: rednessSlider) { value in
                            agoraKit.onRednessSlider(value,
                                                     isOn: beautySwitch)
                        }
                    }
                }.padding(.horizontal, 20)
                HStack {
                    VStack {
                        Text("Sharpness".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $sharpnessSlider).onChange(of: sharpnessSlider) { value in
                            agoraKit.onSharpnessSlider(value, isOn: beautySwitch)
                        }
                    }
                    VStack {
                        Text("Smoothness".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $smoothnessSlider).onChange(of: smoothnessSlider) { value in
                            agoraKit.onSmoothSlider(value, isOn: beautySwitch)
                        }
                    }
                }.padding(.horizontal, 20)
                HStack {
                    Text("Low light Enhancement".localized)
                    Spacer()
                    Toggle("", isOn: $lowLightEnhanceSwitch)
                        .frame(width: 50)
                        .onChange(of: lowLightEnhanceSwitch) { value in
                            agoraKit.onChangeLowLightEnhance(value)
                        }
                    Spacer()
                }
                .padding(.horizontal, 20)
                .padding(.top, 20)
                HStack {
                    Text("Video Dencise".localized)
                    Spacer()
                    Toggle("", isOn: $videoDenoiseSwitch)
                        .frame(width: 50)
                        .onChange(of: videoDenoiseSwitch) { value in
                            agoraKit.onChangeVideoDenoise(value)
                        }
                    Spacer()
                }.padding(.horizontal, 20)
                HStack {
                    Text("Color Enhancement".localized)
                    Spacer()
                    Toggle("", isOn: $colorEnhanceSwitch)
                        .frame(width: 50)
                        .onChange(of: colorEnhanceSwitch) { value in
                            agoraKit.onChangeColorEnhance(value)
                        }
                    Spacer()
                }
                .padding(.horizontal, 20)
                .padding(.top, 20)
                HStack {
                    VStack {
                        Text("Strength".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $strengthSlider).onChange(of: strengthSlider) { value in
                            agoraKit.onStrengthSlider(value,
                                                      isOn: colorEnhanceSwitch)
                        }
                    }
                    VStack {
                        Text("Skin Protect".localized)
                            .frame(maxWidth: .infinity,
                                   alignment: .leading)
                        Slider(value: $skinProtectSlider).onChange(of: skinProtectSlider) { value in
                            agoraKit.onSkinProtectSlider(value,
                                                         isOn: colorEnhanceSwitch)
                        }
                    }
                }
                .padding(.horizontal, 20)
                .padding(.bottom, 20)
                HStack {
                    Text("Virtual Background".localized)
                    Spacer()
                    Toggle("", isOn: $virtualBgSwitch)
                        .frame(width: 50)
                        .onChange(of: virtualBgSwitch) { value in
                            agoraKit.onChangeVirtualBgSwtich(value,
                                                             index: selectedSegment)
                        }
                    Spacer()
                }.padding(.horizontal, 20)
                Picker(selection: $selectedSegment, label: Text("")) {
                    ForEach(Array(segments.indices), id: \.self) { index in
                        Text(segments[index])
                    }
                }
                .pickerStyle(SegmentedPickerStyle())
                .onChange(of: selectedSegment, perform: { value in
                    agoraKit.onChangeVirtualBackground(index: value,
                                                       isOn: virtualBgSwitch)
                })
                .disabled(!virtualBgSwitch)
                .padding(.horizontal, 20)
                .padding(.bottom, 20)
            }.onAppear(perform: {
                agoraKit.setupRTC(configs: configs,
                                  localView: localView.videoView,
                                  remoteView: remoteView.videoView)
            }).onDisappear(perform: {
                agoraKit.onDestory()
            })
        }
    }
}

//#Preview {
//    VideoProcess(configs: [:])
//}
