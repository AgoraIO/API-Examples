//
//  LiveStreaming.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/25.
//

import SwiftUI
import Combine

struct LiveStreamingEntry: View {
    @State private var channelName: String = ""
    @State private var channelButtonIsActive = false
    @State private var selectedCamertOption = ""
    @State private var firstFrameToggleIsOn = false
    @State private var preloadIsOn = false
    @State private var roleSheetIsShow = false
    @State private var cameraSheetIsShow = false
    @State private var role: AgoraClientRole = .broadcaster

    @State private var cameraOptions: [AgoraFocalLengthInfo] = []
    @State private var configs: [String: Any] = [:]
    @State var selectedColor: BackgroundColors = .Red
    @ObservedObject private var liveStreamRTCKit = LiveStreamingRTC()
    @Environment(\.presentationMode) var presentationMode

    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName)
                .textFieldStyle(.roundedBorder)
                .padding()
                .padding(.bottom, 10)
            
            //Default background color
            HStack {
                Text("Default background Color".localized)
                Picker("", selection: $selectedColor) {
                    ForEach(BackgroundColors.allCases) { e in
                        Text(e.suggestedColor).tag(e)
                    }
                }
            }
            
            //First frame optimization
            Toggle("First Frame Optimization", isOn: $firstFrameToggleIsOn)
                .padding(.bottom, 10)
                .alert(isPresented: $firstFrameToggleIsOn) {
                    Alert(title: Text("After this function is enabled, it cannot be disabled and takes effect only when both the primary and secondary ends are enabled".localized),
                          primaryButton: .default(Text("Sure".localized), action: {
                                    firstFrameToggleIsOn = true
                                }),
                          secondaryButton: .cancel(Text("Cancel".localized)))
                }
                .fixedSize()
            
            //Preload
            Button(preloadIsOn ? "cancel preload".localized : "preload Channel".localized) {
                preloadIsOn.toggle()
            }
            .padding(.bottom, 10)
    
            //Camera
            HStack {
                Text("Camera Selected".localized)
                Button(selectedCamertOption) {
                    self.cameraSheetIsShow = true
                }
                .sheet(isPresented: $cameraSheetIsShow, content: {
                    let params = cameraOptions.flatMap({ $0.value })
                    let keys = params.map({ $0.key })
                    
                    PickerSheetView(selectedOption: $selectedCamertOption, options: keys, isPresented: $cameraSheetIsShow) { selectedOption in
                        self.selectedCamertOption = selectedOption
                        for camera in params {
                            if camera.key == self.selectedCamertOption {
                                let config = AgoraCameraCapturerConfiguration()
                                config.cameraFocalLengthType = camera.value
                                config.cameraDirection = camera.key.contains("Front camera".localized) ? .front : .rear
                                liveStreamRTCKit.agoraKit.setCameraCapturerConfiguration(config)
                                break
                            }
                        }
                    }
                })
            }
            .padding(.bottom, 10)
            
            Button {
                self.roleSheetIsShow = true
            } label: {
                Text("Join".localized)
            }
            .adaptiveDialog(title: "Pick Role".localized, isPresented: $roleSheetIsShow, actions: {
                Button("Broadcaster".localized) {
                    self.role = .broadcaster
                    prepareConfig()
                    self.channelButtonIsActive = true
                }
                Button("Audience".localized) {
                    self.role = .audience
                    prepareConfig()
                    self.channelButtonIsActive = true
                }
                adaptiveCancelStyleButton(title: "Cancel".localized) {
                    
                }
            }, actionSheetActions: [
                .default(Text("Broadcaster".localized)) {
                    self.role = .broadcaster
                    prepareConfig()
                    self.channelButtonIsActive = true
                },
                .default(Text("Audience".localized)) {
                    self.role = .audience
                    prepareConfig()
                    self.channelButtonIsActive = true
                },
                
                .cancel(Text("Cancel".localized))
            ])
            .disabled(channelName.isEmpty)
            
            Spacer()
            NavigationLink(destination: LiveStreaming(liveStreamRTCKit: liveStreamRTCKit, configs: configs)
                            .navigationTitle(channelName)
                            .navigationBarTitleDisplayMode(.inline),
                           isActive: $channelButtonIsActive) {
                EmptyView()
            }
            Spacer()
        }
        .onDisappear(perform: {
            UIApplication.shared.sendAction(#selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
        })
        .onAppear(perform: {
            guard let infos = liveStreamRTCKit.agoraKit.queryCameraFocalLengthCapability() else { return }
            let params = infos.flatMap({ $0.value })
            let keys = params.map({ $0.key })
            cameraOptions = infos
            
            selectedCamertOption = keys.first ?? ""
        })
        .navigationBarBackButtonHidden(true)
        .navigationBarItems(leading: Button(action: {
            liveStreamRTCKit.onDestory()
            presentationMode.wrappedValue.dismiss()
        }) {
            HStack {
                Image(systemName: "chevron.left") // Custom back button icon
            }
        })
        .navigationBarTitleDisplayMode(.inline)
    }
    
    func prepareConfig() {
        configs["role"] = self.role
        configs["isFirstFrame"] = firstFrameToggleIsOn
        configs["channelName"] = channelName
        configs["isPreloadChannel"] = preloadIsOn
        configs["cameraKey"] = selectedCamertOption
        configs["backgroundColor"] = selectedColor.value
        print("")
    }
}

struct LiveStreaming: View {
    var liveStreamRTCKit: LiveStreamingRTC
    @State var configs: [String: Any] = [:]
    @State var selectStabilizationMode: AntiShakeLevel = .off
    @State var selectEncodingType: CodeType = .auto
    @State var centerStage: Bool = false
    @State var bFpsState: Bool = false
    @State var waterMarkState: Bool = false
    @State var gasketPushFlow: Bool = false
    @State var showCenterStageAlert: Bool = false
    @State var simulcastStreamState: Bool = false
    @State private var selectedCamertOption = ""
    @State private var cameraOptions: [AgoraFocalLengthInfo] = []
    @State private var cameraSheetIsShow = false
    @State private var showSnapshot: Bool = false
    @State private var showSnapShotTitle: String = ""
    @State private var showSnapShotMessage: String = ""
    @State private var fastLiveState: Bool = false
    @State private var linkStreamState: Bool = false
    @State var cameraDirection: AgoraCameraDirection = .front
    
    var backgroundView = VideoView(type: .local,
                              audioOnly: false)
    var foregroundView = VideoView(type: .remote,
                               audioOnly: false)
    
    var body: some View {
        ZStack {
            backgroundView
            VStack {
                HStack {
                    Spacer()
                    foregroundView
                        .frame(width: 136, height: 182)
                        .offset(x: -20)
                        .onTapGesture {
                            liveStreamRTCKit.onTapForegroundVideo()
                        }
                }
       
                //Anti-shake
                HStack {
                    Text("anti shake".localized)
                        .adaptiveForegroundStyle(.white)
                    Picker("", selection: $selectStabilizationMode) {
                        ForEach(AntiShakeLevel.allCases) { e in
                            Text(e.suggestedLevel).tag(e)
                        }
                    }
                    .onChange(of: selectStabilizationMode) { newValue in
                        liveStreamRTCKit.agoraKit.setCameraStabilizationMode(newValue.value)
                    }
                }
                .adaptiveBackground(.gray.opacity(0.3))
                .padding(.top, 30)
                
                if liveStreamRTCKit.role == .broadcaster {
                    //centerStage, camera focus
                    HStack {
                        Toggle("CenterStage", isOn: $centerStage)
                            .adaptiveForegroundStyle(.white)
                            .onChange(of: centerStage) { newValue in
                                let centerStageNotSupported = liveStreamRTCKit.agoraKit.isCameraCenterStageSupported()
                                if newValue && !centerStageNotSupported {
                                    showCenterStageAlert = true
                                    return
                                }
                                
                                if newValue {
                                    liveStreamRTCKit.agoraKit.enableCameraCenterStage(newValue)
                                }
                            }
                            .frame(height: 30)
                            .alert(isPresented: $showCenterStageAlert, content: {
                                Alert(
                                    title: Text("This device does not support Center Stage".localized),
                                    dismissButton: .destructive(Text("ok"), action: {
                                        centerStage = false
                                    })
                                )
                            })
                        
                        Text("Camera Selected".localized)
                            .adaptiveForegroundStyle(.white)
                        Button(selectedCamertOption) {
                            self.cameraSheetIsShow = true
                        }
                        .sheet(isPresented: $cameraSheetIsShow, content: {
                            let params = cameraOptions.flatMap({ $0.value })
                            let keys = params.map({ $0.key })
                            
                            PickerSheetView(selectedOption: $selectedCamertOption, options: keys, isPresented: $cameraSheetIsShow) { selectedOption in
                                self.selectedCamertOption = selectedOption
                                for camera in params {
                                    if camera.key == self.selectedCamertOption {
                                        let config = AgoraCameraCapturerConfiguration()
                                        config.cameraFocalLengthType = camera.value
                                        config.cameraDirection = camera.key.contains("Front camera".localized) ? .front : .rear
                                        if (config.cameraDirection != self.cameraDirection) {
                                            liveStreamRTCKit.agoraKit.switchCamera()
                                        }
                                        liveStreamRTCKit.agoraKit.setCameraCapturerConfiguration(config)
                                        self.cameraDirection = config.cameraDirection
                                        break
                                    }
                                }
                            }
                        })
                    }
                    .fixedSize()
                    .adaptiveBackground(.gray.opacity(0.3))
                    
                    //B-frame, encoding method    
                    HStack {
                        Toggle("B Fps".localized, isOn: $bFpsState)
                            .adaptiveForegroundStyle(.white)
                            .onChange(of: bFpsState) { newValue in
                                let encoderConfig = AgoraVideoEncoderConfiguration()
                                let videoOptions = AgoraAdvancedVideoOptions()
                                videoOptions.compressionPreference = newValue ? .quality : .lowLatency
                                encoderConfig.advancedVideoOptions = videoOptions
                                
                                liveStreamRTCKit.agoraKit.setVideoEncoderConfiguration(encoderConfig)
                            }
                        Text("Code Type".localized)
                            .adaptiveForegroundStyle(.white)
                        Picker("", selection: $selectEncodingType) {
                            ForEach(CodeType.allCases) { e in
                                Text(e.suggestedType)
                                    .tag(e)
                            }
                        }
                        .onChange(of: selectEncodingType) { newValue in
                            let encoderConfig = AgoraVideoEncoderConfiguration()
                            let advancedOptions = AgoraAdvancedVideoOptions()
                            advancedOptions.encodingPreference = newValue.value
                            encoderConfig.advancedVideoOptions = advancedOptions
                            
                            liveStreamRTCKit.agoraKit.setVideoEncoderConfiguration(encoderConfig)
                        }
                        .pickerStyle(SegmentedPickerStyle())
                    
                    }
                    .fixedSize()
                    .adaptiveBackground(.gray.opacity(0.3))
                    
                    //Watermark, gasket push flow
                    HStack {
                        Toggle("Water Mark".localized, isOn: $waterMarkState)
                            .adaptiveForegroundStyle(.white)
                            .onChange(of: waterMarkState) { newValue in
                                if newValue {
                                    if let filepath = Bundle.main.path(forResource: "agora-logo", ofType: "png") {
                                        if let url = URL.init(string: filepath) {
                                            let waterMark = WatermarkOptions()
                                            waterMark.visibleInPreview = true
                                            waterMark.positionInPortraitMode = CGRect(x: 10, y: 80, width: 60, height: 60)
                                            waterMark.positionInLandscapeMode = CGRect(x: 10, y: 80, width: 60, height: 60)
                                            liveStreamRTCKit.agoraKit.addVideoWatermark(url, options: waterMark)
                                        }
                                    }
                                } else {
                                    liveStreamRTCKit.agoraKit.clearVideoWatermarks()
                                }

                            }
                        
                        Toggle("Gasket push flow".localized, isOn: $gasketPushFlow)
                            .adaptiveForegroundStyle(.white)
                            .onChange(of: gasketPushFlow) { newValue in
                                let options = AgoraImageTrackOptions()
                                let imgPath = Bundle.main.path(forResource: "agora-logo", ofType: "png")
                                options.imageUrl = imgPath
                                
                                liveStreamRTCKit.agoraKit.enableVideoImageSource(newValue, options: options)
                            }

                    }
                    .fixedSize()
                    .adaptiveBackground(.gray.opacity(0.3))
                }
                
                //Screenshot, dual stream
                HStack {
                    Button("screenshot".localized) {
                        showSnapshot = true

                        guard let remoteUid = liveStreamRTCKit.remoteUid else {
                            showSnapShotTitle = "remote user has not joined, and cannot take a screenshot".localized;
                            showSnapShotMessage = ""
                            return
                        }
                        
                        let path = NSTemporaryDirectory().appending("1.png")
                        liveStreamRTCKit.agoraKit.takeSnapshot(Int(remoteUid), filePath: path)
                        showSnapShotTitle = "Screenshot successful".localized
                        showSnapShotMessage = path
                    }
                    .alert(isPresented: $showSnapshot, content: {
                        Alert(title: Text(showSnapShotTitle), message: Text(showSnapShotMessage))
                    })
                    .padding(.trailing, 20)
                    
                    Toggle(isOn: $simulcastStreamState, label: {
                        VStack {
                            Text("Simulcast Stream Title".localized)
                            Text(simulcastStreamState ? "Opened State".localized : "Default State".localized)
                                .font(.system(size: 11))
                        }
                        .adaptiveForegroundStyle(.white)
                    })
                    .onChange(of: simulcastStreamState) { newValue in
                        simulcastStreamState = newValue
                        liveStreamRTCKit.agoraKit.setDualStreamMode(newValue ? .enableSimulcastStream : .disableSimulcastStream)
                    }
                }
                .fixedSize()
                .adaptiveBackground(.gray.opacity(0.3))
                
                //Live streaming at lightning speed, real-time interaction
                HStack {
                    if liveStreamRTCKit.showUltraLowEntry {
                        Toggle("Fast Live".localized, isOn: $fastLiveState)
                            .onChange(of: fastLiveState) { newValue in
                                liveStreamRTCKit.onToggleUltraLowLatency(enabled: newValue)
                            }
                            .disabled(linkStreamState)
                    }
                    
                        
                    if liveStreamRTCKit.showLinkStreamEntry {
                        Toggle("Link Stream".localized, isOn: $linkStreamState)
                            .onChange(of: linkStreamState) { newValue in
                                liveStreamRTCKit.onToggleClientRole(state: newValue)
                            }
                    }
                }
                .fixedSize()
                .adaptiveBackground(.gray.opacity(0.3))
                
                Spacer()
            }
        }
        .onAppear {
            liveStreamRTCKit.setupRTC(configs: configs, localView: backgroundView.videoView, remoteView: foregroundView.videoView)
            guard let infos = liveStreamRTCKit.agoraKit.queryCameraFocalLengthCapability() else { return }
            
            cameraOptions = infos
            
            if let cameraKey = configs["cameraKey"] as? String {
                selectedCamertOption = cameraKey
                cameraDirection = cameraKey.contains("Front camera".localized) ? .front : .rear
            }
            
            
        }.onDisappear {
            liveStreamRTCKit.leaveChannel()
        }
    }
}

extension AgoraFocalLengthInfo {
    var value: [String: AgoraFocalLength] {
        let title = cameraDirection == 1 ? "Front camera".localized + " - " : "Rear camera".localized + " - "
        switch focalLengthType {
        case .default: return [title + "Default".localized: focalLengthType]
        case .wide: return [title + "Wide".localized: focalLengthType]
        case .ultraWide: return [title + "Length Wide".localized: focalLengthType]
        case .telephoto: return [title + "Telephoto".localized: focalLengthType]
        @unknown default: return [title + "Default".localized: focalLengthType]
        }
    }
}

enum BackgroundColors: String, CaseIterable, Identifiable {
    case Red
    case Blue
    case Pink
    case Purple
    case Yellow

    var id: String { self.rawValue }
    
    var value: UInt32 {
        switch self {
        case .Red:
            return UInt32(0xff0d00ff)
        case .Blue:
            return UInt32(0x0400ffff)
        case .Pink:
            return UInt32(0xff006aff)
        case .Purple:
            return UInt32(0xff00d9ff)
        case .Yellow:
            return UInt32(0xeaff00ff)
        }
    }
    
    var suggestedColor: String {
        switch self {
        case .Red: return "Red".localized
        case .Blue: return "Blue".localized
        case .Pink: return  "Pink".localized
        case .Purple: return "Purple".localized
        case .Yellow: return "Yellow".localized
        }
    }
}

enum AntiShakeLevel: String, CaseIterable, Identifiable {
    case off
    case auto
    case level1
    case level2
    case level3

    var id: String { self.rawValue }
    
    var value: AgoraCameraStabilizationMode {
        switch self {
        case .off:
            return .off
        case .auto:
            return .auto
        case .level1:
            return .level1
        case .level2:
            return .level2
        case .level3:
            return .level3
        }
    }
    
    var suggestedLevel: String {
        return self.rawValue
    }
}

enum CodeType: String, CaseIterable, Identifiable {
    case auto
    case software
    case hardware
    
    var id: String { self.rawValue }
    
    var value: AgoraEncodingPreference {
        switch self {
        case .auto:
            return .preferAuto
        case .software:
            return .prefersoftware
        case .hardware:
            return .preferhardware
        }
    }
    
    var suggestedType: String {
        switch self {
        case .auto: return "auto".localized
        case .software: return "software".localized
        case .hardware: return  "hardware".localized
        }
    }
}

