---
name: upsert-case
description: >
  Add a new audio API demo case or modify an existing one in the APIExample-Audio project.
  Uses AgoraAudio_iOS SDK — no video APIs available. Covers folder creation, Entry/Main Swift file,
  storyboard, MenuItem registration, Xcode target membership, and Case Index update.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-Audio

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — follow the Modify Existing Case flow below

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

> **Audio-only constraint**: this project uses `AgoraAudio_iOS` SDK. The video module is not available.
> Do NOT add any video API calls. See the NEVER list below.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `.swift` file + `.storyboard`, `ViewController.swift` (MenuItem), `APIExample-Audio.xcodeproj/project.pbxproj` (target membership), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `.swift` file(s), optionally `.storyboard`, `ARCHITECTURE.md` (Case Index); update the project file only for new or moved build inputs |

---

## Modify Existing Case

1. Locate the existing Swift implementation and change the actual runtime behavior first.
2. Update storyboard outlets, actions, and controller identifiers when the behavior change needs it.
3. Adjust registration in `ViewController.swift` only when the menu or navigation wiring changes.
4. Update Xcode target membership for new or moved build inputs, then synchronize the Case Index.
5. Build and review the changed behavior, lifecycle, and registration. Reusing an existing folder does not replace implementation work.

## Step 1 — Create the Example Folder

```
APIExample-Audio/Examples/[Basic|Advanced]/<ExampleName>/
```

## Step 2 — Create the Swift File

Create `<ExampleName>.swift` with Entry and Main classes:

```swift
import UIKit
import AgoraRtcKit

class <ExampleName>Entry: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!

    @IBAction func onJoinPressed(_ sender: UIButton) {
        guard let channelName = channelTextField.text, !channelName.isEmpty else { return }
        let storyboard = UIStoryboard(name: "<ExampleName>", bundle: nil)
        guard let mainVC = storyboard.instantiateViewController(
            withIdentifier: "<ExampleName>") as? <ExampleName>Main else { return }
        mainVC.configs = ["channelName": channelName]
        navigationController?.pushViewController(mainVC, animated: true)
    }
}

class <ExampleName>Main: BaseViewController {
    private(set) var agoraKit: AgoraRtcEngineKit?
    private var tokenRequestID = 0

    override func viewDidLoad() {
        super.viewDidLoad()
        setupRTC()
    }

    func setupRTC() {
        precondition(Thread.isMainThread)
        guard agoraKit == nil else { return }
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit?.setAudioProfile(.default)
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

    override func willMove(toParent parent: UIViewController?) {
        super.willMove(toParent: parent)
        if parent == nil { onDestroy() }
    }
}

extension <ExampleName>Main: AgoraRtcEngineDelegate {
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

## Step 3 — Create the Storyboard

Create `APIExample-Audio/Base.lproj/<ExampleName>.storyboard` with two scenes:

| Scene | Storyboard ID | Class |
|-------|--------------|-------|
| Entry | `EntryViewController` | `<ExampleName>Entry` |
| Main  | `<ExampleName>` | `<ExampleName>Main` |

UI should contain only audio controls — no video rendering views.

## Step 4 — Register the MenuItem

Add to the `menus` array in `APIExample-Audio/ViewController.swift`:

```swift
MenuItem(name: "<Display Name>".localized,
         storyboard: "<ExampleName>",
         controller: "<ExampleName>")
```

## Step 5 — Add Files to the Xcode Target

This project uses explicit Xcode groups and build phases. For a new case, update
`APIExample-Audio.xcodeproj/project.pbxproj` so the Swift file belongs to the
`APIExample-Audio` target's Sources build phase and the storyboard belongs to its Resources
build phase. Add any new localized or audio resources to Resources as well. Existing-file
edits do not require a project-file change unless a build input was added or moved.

## Step 6 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.swift` | `keyApi1()`, `keyApi2()` | One-line description |
```

---

The lifecycle code is a skeleton: wire `requestJoin(channelName:requestPermission:)` from
setup or the Join action, passing the channel from `configs` and the case's actual permission
request. Never replace the permission closure with an unconditional grant in a real case.
Keep setup, join, leave and destroy on the main queue. A user Leave action must call
`leaveChannel()` so pending requests are invalidated, even before the SDK reports joined.

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] Both Entry and Main classes exist in the Swift file
- [ ] Main inherits `BaseViewController`
- [ ] Storyboard has correct scene IDs
- [ ] No video rendering views in the storyboard
- [ ] MenuItem added to `ViewController.swift`
- [ ] New Swift files are in the `APIExample-Audio` target's Sources build phase
- [ ] New storyboards, localized files, and audio assets are in the target's Resources build phase
- [ ] `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called in `willMove(toParent:)` when `parent == nil`
- [ ] UI updates inside delegate callbacks dispatched to `DispatchQueue.main`
- [ ] Microphone permission requested before `joinChannel()`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Permission/Token pending → leave/destroy → delayed callback does not join
- [ ] Repeat cleanup, reopen and out-of-order Token responses preserve only the active request
- [ ] Missing required Token and nonzero join return produce a failure state without logging credentials
- [ ] Project builds without errors

---

## NEVER

- NEVER call `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, or `startPreview()` — the SDK has no video module
- NEVER add `AgoraRtcVideoCanvas` or `VideoView` to any storyboard or code in this project
- NEVER create `AgoraRtcEngineKit` in the Entry VC
- NEVER call `leaveChannel` or `destroy` in `viewDidDisappear` — use `willMove(toParent:)` with `parent == nil`
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `DispatchQueue.main.async { }`
- NEVER add a source or resource file without adding it to the `APIExample-Audio` target
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
