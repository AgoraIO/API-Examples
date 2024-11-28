//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct ScreenShareEntry: View {
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
            NavigationLink(destination: ScreenShare(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct ScreenShareEntryPreviews: PreviewProvider {
    static var previews: some View {
        ScreenShareEntry()
    }
}

struct ScreenShare: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = ScreenShareRTC()
    @State private var isStart: Bool = false
    @State private var isCaptureAudio: Bool = false
    @State private var isCaptureVideo: Bool = false
    @State private var signalVolumeValue: Float = 0.5
    @State private var isShowPickerView: Bool = false
    @State private var fpsDataSources: [Int] = [15, 30, 60]
    @State private var scenarioTypeDataSources: [String: Int] = ["document": 1, "gaming": 2, "video": 3, "RDC": 4]
    @State private var scenarioType: String = "video"
    @State private var isShowFps: Bool = false
    @State private var fps: String = "60"
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    var body: some View {
        VStack {
            HStack{
                localView
                remoteView
            }.frame(maxHeight: 200)
            HStack {
                Text("captureAudio")
                Toggle("", isOn: $isCaptureAudio)
                    .frame(width: 50)
                    .onChange(of: isCaptureAudio) { value in
                        agoraKit.clickCaptureAudio(value)
                    }
                Spacer()
                Text("captureVideo")
                Toggle("", isOn: $isCaptureVideo)
                    .frame(width: 50)
                    .onChange(of: isCaptureVideo) { value in
                        agoraKit.clickCaptureVideo(value)
                    }
            }
            .padding(.horizontal, 20)
            .padding(.top, 20)
            HStack {
                Text("captureSignalVolume")
                Slider(value: $signalVolumeValue)
                    .onChange(of: signalVolumeValue) { value in
                        agoraKit.captureSignalVolumeSlider(value)
                    }
            }.padding(.horizontal, 20)
            HStack {
                Text("captureFrameRate")
                Spacer()
                Button(fps + "fps") {
                    withAnimation {
                        isShowFps.toggle()
                    }
                }
            }
            .padding(.horizontal, 20)
            .padding(.top, 15)
            HStack {
                Text("ScreenScenarioType")
                Spacer()
                Button(scenarioType) {
                    withAnimation {
                        isShowPickerView.toggle()
                    }
                }
            }
            .padding(.horizontal, 20)
            .padding(.top, 15)
            
            HStack {
                Button("stopScreen") {
                    isStart.toggle()
                    agoraKit.stopScreenCapture()
                }.disabled(!isStart)
                Spacer()
                Button("startScreen") {
                    isStart.toggle()
                    agoraKit.startScreenCapture()
                }.disabled(isStart)
                Spacer()
                Button("updateScreen") {
                    agoraKit.updateScreenCapture()
                }.disabled(!isStart)
            }
            .padding(.horizontal, 20)
            .padding(.top, 25)
            Spacer()
            if isShowPickerView {
                let data = scenarioTypeDataSources.map({ $0.key })
                PickerView(dataArray: .constant(data), onTapCancel: {
                    withAnimation {
                        isShowPickerView.toggle()
                    }
                }, onTapSure: { value in
                    withAnimation {
                        isShowPickerView.toggle()
                    }
                    scenarioType = value
                    let type = scenarioTypeDataSources[value] ?? 3
                    agoraKit.setScreenScenario(type: type)
                })
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowPickerView)
            }
            if isShowFps {
                let data = fpsDataSources.map({ "\($0)"})
                PickerView(dataArray: .constant(data), onTapCancel: {
                    withAnimation {
                        isShowFps.toggle()
                    }
                }, onTapSure: { value in
                    withAnimation {
                        isShowFps.toggle()
                    }
                    fps = value
                    agoraKit.setFps(fps: value)
                })
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowFps)
            }
            
        }.onAppear(perform: {
            agoraKit.setupRTC(configs: configs,
                              localView: localView.videoView,
                              remoteView: remoteView.videoView)
        }).onDisappear(perform: {
            agoraKit.onDestory()
        })
    }
}

struct ScreenSharePreviews: PreviewProvider {
    static var previews: some View {
        ScreenShare(configs: [:])
    }
}
