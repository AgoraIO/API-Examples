//
//  JoinChannelVideoView.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import SwiftUI

struct MediaPlayerEntry: View {
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
            NavigationLink(destination: MediaPlayer(configs: configs).navigationTitle(channelName).navigationBarTitleDisplayMode(.inline), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
        .navigationBarTitleDisplayMode(.inline)
    }
}

struct MediaPlayerEntryPreviews: PreviewProvider {
    static var previews: some View {
        MediaPlayerEntry()
    }
}

struct MediaPlayer: View {
    @State var configs: [String: Any] = [:]
    
    @State private var statsInfo: String = ""
    @ObservedObject private var agoraKit = MediaPlayerRTC()
    
    var localView = VideoView(type: .local,
                              audioOnly: false)
    var remoteView = VideoView(type: .remote,
                               audioOnly: false)
    
    @State private var isShowPlayPickerView: Bool = false
    @State private var isShowPublishPickerView: Bool = false
    
    var body: some View {
        VStack {
            VStack {
                localView
                remoteView
            }.frame(height: 300)
            ScrollView(.vertical) {
                HStack {
                    TextField("", text: $agoraKit.url).textFieldStyle(.roundedBorder)
                    Button("Open".localized) {
                        agoraKit.doOpenMediaUrl()
                    }.padding(.leading, 10)
                }
                .padding(.horizontal, 20)
                .padding(.top, 15)
                HStack {
                    Slider(value: $agoraKit.playerProgressSlider).disabled(true)
                    Text(agoraKit.playerDurationLabel)
                }.padding(.horizontal, 20)
                HStack(alignment: .center, spacing: 10, content: {
                    Button("Play".localized) {
                        agoraKit.doPlay()
                    }.disabled(agoraKit.isPlaying)
                    Spacer()
                    Button("Stop".localized) {
                        agoraKit.doStop()
                    }.disabled(!agoraKit.isPlaying)
                    Spacer()
                    Button("Pause".localized) {
                        agoraKit.doPause()
                    }.disabled(!agoraKit.isPlaying)
                    Spacer()
                    Button("Publish".localized) {
                        agoraKit.doPublish()
                    }.disabled(!agoraKit.isPlaying || agoraKit.isPublished)
                    Spacer()
                    Button("stopPublish".localized) {
                        agoraKit.dloStopPublish()
                    }.disabled(!agoraKit.isPublished)
                })
                .padding(.horizontal, 20)
                .padding(.top, 20)
                HStack {
                    Text("PlaybackVolume".localized).padding(.trailing, 15)
                    Slider(value: $agoraKit.playoutVolume, in: 0...400).onChange(of: agoraKit.playoutVolume) { value in
                        agoraKit.doAdjustPlayoutVolume(value: value)
                    }
                }.padding(20)
                HStack {
                    Text("Publish Volume".localized).padding(.trailing, 15)
                    Slider(value: $agoraKit.publishVolume, in: 0...100).onChange(of: agoraKit.publishVolume) { value in
                        agoraKit.doAdjustPublishVolume(value: value)
                    }
                }
                .padding(.horizontal, 20)
                .padding(.bottom, 20)
                    
                HStack {
                    Text("play Track:".localized).padding(.trailing, 5)
                    Button(agoraKit.codecName) {
                        withAnimation {
                            isShowPlayPickerView.toggle()
                        }
                    }.foregroundStyle(.gray)
                    Spacer()
                    Text("publish Track:".localized).padding(.trailing, 5)
                    Button(agoraKit.codecName) {
                        withAnimation {
                            isShowPublishPickerView.toggle()
                        }
                    }.foregroundStyle(.gray)
                }.padding(20)
            }
            Spacer()
            if isShowPlayPickerView {
                let data = agoraKit.trackList?.compactMap({ $0.codecName })
                PickerView(dataArray: .constant(data ?? []), onTapCancel: {
                    withAnimation {
                        isShowPlayPickerView.toggle()
                    }
                }, onTapSure: { value in
                    withAnimation {
                        isShowPlayPickerView.toggle()
                    }
                    agoraKit.onClickPlayAudioTrack(value: value)
                })
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowPlayPickerView)
            }
            if isShowPublishPickerView {
                let data = agoraKit.trackList?.compactMap({ $0.codecName })
                PickerView(dataArray: .constant(data ?? []), onTapCancel: {
                    withAnimation {
                        isShowPublishPickerView.toggle()
                    }
                }, onTapSure: { value in
                    withAnimation {
                        isShowPublishPickerView.toggle()
                    }
                    agoraKit.onClickPublishAudioTrack(value: value)
                })
                .transition(.move(edge: .bottom))
                .animation(.spring(), value: isShowPublishPickerView)
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

struct MediaPlayerPreviews: PreviewProvider {
    static var previews: some View {
        MediaPlayer(configs: [:])
    }
}
