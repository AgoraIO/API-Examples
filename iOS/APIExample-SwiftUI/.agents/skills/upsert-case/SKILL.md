---
name: upsert-case
description: >
  Add a new API demo case or modify an existing one in the APIExample-SwiftUI project.
  Covers folder creation, Entry view, RTC class, MenuItem registration, Xcode target membership,
  and Case Index update.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-SwiftUI

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — follow the Modify Existing Case flow below

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `<ExampleName>RTC.swift` + `<ExampleName>.swift`, `ContentView.swift` (MenuItem), `APIExample-SwiftUI.xcodeproj/project.pbxproj` (target membership), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `*RTC.swift` and/or `*.swift` view files, `ARCHITECTURE.md` (Case Index); update the project file only for new or moved build inputs |

---

## Modify Existing Case

1. Locate the existing RTC class and SwiftUI view files and change the actual runtime behavior first.
2. Update view state, actions, and navigation when the behavior change needs it.
3. Adjust registration in `ContentView.swift` only when the menu or navigation wiring changes.
4. Update Xcode target membership for new or moved build inputs, then synchronize the Case Index.
5. Build and review the changed behavior, lifecycle, and registration. Reusing an existing folder does not replace implementation work.

## Step 1 — Create the Example Folder

```
APIExample-SwiftUI/Examples/[Basic|Advanced]/<ExampleName>/
```

## Step 2 — Create the RTC Class

Create `<ExampleName>RTC.swift` — owns the engine lifecycle:

```swift
import AgoraRtcKit
import SwiftUI

class <ExampleName>RTC: NSObject, ObservableObject {
    private(set) var agoraKit: AgoraRtcEngineKit?
    private var tokenRequestID = 0

    func setupRTC(configs: [String: Any]) {
        precondition(Thread.isMainThread)
        guard agoraKit == nil else { return }
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configure this case's media, then call requestJoin(channelName:requestPermission:).
        // Supply the project's permission flow; its completion must report granted/denied.
    }

    // Main-queue entry point. A new request supersedes any pending permission/Token response.
    func requestJoin(channelName: String,
                     requestPermission: (@escaping (Bool) -> Void) -> Void) {
        precondition(Thread.isMainThread)
        guard let engine = agoraKit, !channelName.isEmpty else { return }
        tokenRequestID += 1
        let requestID = tokenRequestID
        let uid: UInt = 0
        requestPermission { [weak self, weak engine] granted in
            DispatchQueue.main.async { [weak self, weak engine] in
                guard let self = self, let engine = engine,
                      self.tokenRequestID == requestID, self.agoraKit === engine else { return }
                guard granted else {
                    LogUtils.log(message: "Permission denied", level: .error)
                    return
                }
                NetworkManager.shared.generateToken(channelName: channelName, uid: uid) { [weak self, weak engine] token in
                    DispatchQueue.main.async { [weak self, weak engine] in
                        guard let self = self, let engine = engine,
                              self.tokenRequestID == requestID, self.agoraKit === engine else { return }
                        if !(KeyCenter.Certificate ?? "").isEmpty && (token ?? "").isEmpty {
                            LogUtils.log(message: "Token request failed", level: .error)
                            return
                        }
                        let option = AgoraRtcChannelMediaOptions()
                        option.clientRoleType = .broadcaster
                        option.publishMicrophoneTrack = true
                        // Configure camera publication/canvases for video cases after camera permission.
                        let result = engine.joinChannel(byToken: token, channelId: channelName,
                                                        uid: uid, mediaOptions: option)
                        if result != 0 {
                            LogUtils.log(message: "joinChannel failed: \(result)", level: .error)
                        }
                    }
                }
            }
        }
    }

    func leaveChannel() {
        precondition(Thread.isMainThread)
        tokenRequestID += 1
        agoraKit?.leaveChannel(nil)
    }

    func onDestroy() {
        precondition(Thread.isMainThread)
        leaveChannel() // Invalidate callbacks even while Token/permission/join is pending.
        guard agoraKit != nil else { return }
        // Stop case-owned capture, players, timers and observers here.
        AgoraRtcEngineKit.destroy()
        agoraKit = nil
    }
}

extension <ExampleName>RTC: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String,
                   withUid uid: UInt, elapsed: Int) {
        DispatchQueue.main.async { [weak self] in
            guard let self = self, self.agoraKit === engine else { return }
            LogUtils.log(message: "Joined: \(channel) uid: \(uid)", level: .info)
        }
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        DispatchQueue.main.async { [weak self] in
            guard let self = self, self.agoraKit === engine else { return }
            LogUtils.log(message: "Error: \(errorCode.rawValue)", level: .error)
        }
    }
}
```

## Step 3 — Create the SwiftUI Views

Create `<ExampleName>.swift` with Entry and Main views:

```swift
import SwiftUI

struct <ExampleName>Entry: View {
    @State private var channelName = ""
    @State private var isActive = false
    @State private var configs: [String: Any] = [:]

    var body: some View {
        VStack {
            TextField("Enter channel name".localized, text: $channelName)
                .textFieldStyle(.roundedBorder).padding()
            Button("Join".localized) {
                configs = ["channelName": channelName]
                isActive = true
            }.disabled(channelName.isEmpty)
            NavigationLink(destination: <ExampleName>(configs: configs),
                           isActive: $isActive) { EmptyView() }
        }
    }
}

struct <ExampleName>: View {
    @State var configs: [String: Any] = [:]
    @StateObject private var rtc = <ExampleName>RTC()

    var body: some View {
        VStack { /* UI here */ }
            .onAppear { rtc.setupRTC(configs: configs) }
            .onDisappear { rtc.onDestroy() }
    }
}
```

## Step 4 — Register the MenuItem

Add to the `menus` array in `APIExample-SwiftUI/ContentView.swift`:

```swift
MenuItem(name: "<Display Name>".localized, view: AnyView(<ExampleName>Entry()))
```

## Step 5 — Add Files to the Xcode Target

This project uses explicit Xcode groups and build phases. For a new case, update
`APIExample-SwiftUI.xcodeproj/project.pbxproj` so both Swift files belong to the
`APIExample-SwiftUI` target's Sources build phase. Add any new assets or localized files to
the same target's Resources build phase. Existing-file edits do not require a project-file
change unless a build input was added or moved.

## Step 6 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/` | `keyApi1()`, `keyApi2()` | One-line description |
```

---

The lifecycle code is a skeleton: wire `requestJoin(channelName:requestPermission:)` from
setup or the Join action, passing the channel from `configs` and the case's actual permission
request. Never replace the permission closure with an unconditional grant in a real case.
Keep setup, join, leave and destroy on the main queue. A user Leave action must call
`leaveChannel()` so pending requests are invalidated, even before the SDK reports joined.

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] RTC class inherits `NSObject`, conforms to `ObservableObject` and `AgoraRtcEngineDelegate`
- [ ] Engine created in `setupRTC`, destroyed in `onDestroy`
- [ ] Main view uses `@StateObject` when it constructs the RTC object; `@ObservedObject` is used only for an externally owned, injected object
- [ ] `setupRTC` called in `.onAppear`, `onDestroy` called in `.onDisappear`
- [ ] `leaveChannel` + `AgoraRtcEngineKit.destroy()` called in `onDestroy`
- [ ] UI updates inside delegate callbacks dispatched to `DispatchQueue.main`
- [ ] MenuItem added to `ContentView.swift`
- [ ] New Swift files are in the `APIExample-SwiftUI` target's Sources build phase
- [ ] New assets or localized files are in the target's Resources build phase, when applicable
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Permission/Token pending → leave/destroy → delayed callback does not join
- [ ] Repeat cleanup, reopen and out-of-order Token responses preserve only the active request
- [ ] Missing required Token and nonzero join return produce a failure state without logging credentials
- [ ] Project builds without errors

---

## NEVER

- NEVER create `AgoraRtcEngineKit` in the Entry view
- NEVER construct an RTC object in a view and store it as `@ObservedObject` — use `@StateObject`; reserve `@ObservedObject` for an externally owned, injected object
- NEVER add a source or resource file without adding it to the `APIExample-SwiftUI` target
- NEVER call SDK APIs inside SwiftUI `body` — only in `.onAppear`, `.onDisappear`, or explicit user action handlers
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `DispatchQueue.main.async { }`
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER call `joinChannel` before requesting camera/microphone permissions
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
