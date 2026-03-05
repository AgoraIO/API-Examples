# add-new-case

Create a new example in the APIExample-SwiftUI project following the Entry/RTC pattern.

## When to Use

- User wants to add a new API demonstration in SwiftUI
- User needs to create a sample for a specific Agora feature
- Porting an example from another platform to SwiftUI

## Prerequisites

Before creating a new case:
1. Run `query-cases` to verify the example doesn't already exist
2. Confirm the target category (Basic or Advanced)
3. Determine the example name (PascalCase, e.g., `MyNewFeature`)

## Instructions

### Step 1: Create the Example Folder

Create folder at `APIExample-SwiftUI/Examples/[Basic|Advanced]/<ExampleName>/`

### Step 2: Create the RTC Wrapper Class

Create `<ExampleName>RTC.swift`:

```swift
import AgoraRtcKit
import SwiftUI

class <ExampleName>RTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    
    func setupRTC(configs: [String: Any], localView: VideoUIView, remoteView: VideoUIView) {
        self.localView = localView
        self.remoteView = remoteView
        
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        guard let channelName = configs["channelName"] as? String else { return }
        
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.clientRoleType = .broadcaster
        
        NetworkManager.shared.generateToken(channelName: channelName) { token in
            self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
        }
    }
    
    func onDestroy() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel(nil)
        }
        AgoraRtcEngineKit.destroy()
    }
}

extension <ExampleName>RTC: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Joined channel: \(channel)", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error: \(errorCode)", level: .error)
    }
}
```

### Step 3: Create the SwiftUI Views

Create `<ExampleName>.swift`:

```swift
import SwiftUI

struct <ExampleName>Entry: View {
    @State private var channelName: String = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]
    
    var body: some View {
        VStack {
            Spacer()
            TextField("Enter channel name".localized, text: $channelName)
                .textFieldStyle(.roundedBorder)
                .padding()
            Button {
                configs = ["channelName": channelName]
                isActive = true
            } label: {
                Text("Join".localized)
            }.disabled(channelName.isEmpty)
            Spacer()
            NavigationLink(destination: <ExampleName>(configs: configs), isActive: $isActive) {
                EmptyView()
            }
            Spacer()
        }
    }
}

struct <ExampleName>: View {
    @State var configs: [String: Any] = [:]
    @ObservedObject private var agoraKit = <ExampleName>RTC()
    
    var localView = VideoView(type: .local, audioOnly: false)
    var remoteView = VideoView(type: .remote, audioOnly: false)
    
    var body: some View {
        VStack {
            HStack {
                localView
                remoteView
            }.frame(maxHeight: 200)
            Spacer()
        }
        .onAppear {
            agoraKit.setupRTC(configs: configs,
                             localView: localView.videoView,
                             remoteView: remoteView.videoView)
        }
        .onDisappear {
            agoraKit.onDestroy()
        }
    }
}
```

### Step 4: Register the Example

Add to `menus` array in `APIExample-SwiftUI/ContentView.swift`:

```swift
MenuItem(name: "<Display Name>".localized, view: AnyView(<ExampleName>Entry()))
```

## Architecture Rules

- RTC class inherits `NSObject`, conforms to `ObservableObject` and `AgoraRtcEngineDelegate`
- Entry view collects configuration and navigates to Main view
- Main view holds RTC object as `@ObservedObject`
- Setup RTC in `.onAppear`, cleanup in `.onDisappear`
- Always call `leaveChannel()` then `AgoraRtcEngineKit.destroy()` in `onDestroy()`
- Dispatch UI updates to main thread from delegate callbacks

## Verification Checklist

- [ ] Example folder created in correct category
- [ ] RTC wrapper class created with proper lifecycle
- [ ] Entry and Main views created
- [ ] MenuItem added to ContentView.swift
- [ ] Engine lifecycle properly managed
- [ ] Build succeeds without errors
