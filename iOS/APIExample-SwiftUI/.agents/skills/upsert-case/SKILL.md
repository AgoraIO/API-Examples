---
name: upsert-case
description: >
  Add a new API demo case or modify an existing one in the APIExample-SwiftUI project.
  Covers folder creation, Entry view, RTC class, MenuItem registration, and Case Index update.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-SwiftUI

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — skip Steps 1–3, go directly to Step 4+

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `<ExampleName>RTC.swift` + `<ExampleName>.swift`, `ContentView.swift` (MenuItem), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `*RTC.swift` and/or `*.swift` view files, `ARCHITECTURE.md` (Case Index) |

---

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
    @ObservedObject private var rtc = <ExampleName>RTC()

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

## Step 5 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/` | `keyApi1()`, `keyApi2()` | One-line description |
```

---

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] RTC class inherits `NSObject`, conforms to `ObservableObject` and `AgoraRtcEngineDelegate`
- [ ] Engine created in `setupRTC`, destroyed in `onDestroy`
- [ ] Main view uses `@ObservedObject` (not `@StateObject`) for the RTC object
- [ ] `setupRTC` called in `.onAppear`, `onDestroy` called in `.onDisappear`
- [ ] `leaveChannel` + `AgoraRtcEngineKit.destroy()` called in `onDestroy`
- [ ] UI updates inside delegate callbacks dispatched to `DispatchQueue.main`
- [ ] MenuItem added to `ContentView.swift`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Project builds without errors

---

## NEVER

- NEVER create `AgoraRtcEngineKit` in the Entry view
- NEVER use `@StateObject` for the RTC object in the Main view — the Main view does not own its lifetime
- NEVER call SDK APIs inside SwiftUI `body` — only in `.onAppear`, `.onDisappear`, or explicit user action handlers
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `DispatchQueue.main.async { }`
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER call `joinChannel` before requesting camera/microphone permissions
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
