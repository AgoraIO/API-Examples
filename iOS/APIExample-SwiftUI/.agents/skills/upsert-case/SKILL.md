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
    var agoraKit: AgoraRtcEngineKit!
    private var isJoined = false

    func setupRTC(configs: [String: Any]) {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)

        guard let channelName = configs["channelName"] as? String else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.clientRoleType = .broadcaster

        NetworkManager.shared.generateToken(channelName: channelName) { [weak self] token in
            self?.agoraKit.joinChannel(byToken: token, channelId: channelName,
                                       uid: 0, mediaOptions: option)
        }
    }

    func onDestroy() {
        if isJoined { agoraKit.leaveChannel(nil) }
        AgoraRtcEngineKit.destroy()
    }
}

extension <ExampleName>RTC: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String,
                   withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Joined: \(channel)", level: .info)
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error: \(errorCode)", level: .error)
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
