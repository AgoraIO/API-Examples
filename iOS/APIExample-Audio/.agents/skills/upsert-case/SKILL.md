---
name: upsert-case
description: >
  Add a new audio API demo case or modify an existing one in the APIExample-Audio project.
  Uses AgoraAudio_iOS SDK — no video APIs available. Covers folder creation, Entry/Main Swift file,
  storyboard, MenuItem registration, and Case Index update.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-Audio

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — skip Steps 1–3, go directly to Step 4+

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

> **Audio-only constraint**: this project uses `AgoraAudio_iOS` SDK. The video module is not available.
> Do NOT add any video API calls. See the NEVER list below.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `.swift` file + `.storyboard`, `ViewController.swift` (MenuItem), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `.swift` file(s), optionally `.storyboard`, `ARCHITECTURE.md` (Case Index) |

---

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
    var agoraKit: AgoraRtcEngineKit?

    override func viewDidLoad() {
        super.viewDidLoad()
        guard let channelName = configs["channelName"] as? String else { return }
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit?.setAudioProfile(.default)
        // request microphone permission, then join
        NetworkManager.shared.generateToken(channelName: channelName) { [weak self] token in
            let option = AgoraRtcChannelMediaOptions()
            option.publishMicrophoneTrack = true
            self?.agoraKit?.joinChannel(byToken: token, channelId: channelName,
                                        uid: 0, mediaOptions: option)
        }
    }

    override func willMove(toParent parent: UIViewController?) {
        super.willMove(toParent: parent)
        if parent == nil {
            agoraKit?.leaveChannel()
            AgoraRtcEngineKit.destroy()
        }
    }
}

extension <ExampleName>Main: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String,
                   withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Joined: \(channel) uid: \(uid)", level: .info)
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error: \(errorCode.rawValue)", level: .error)
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

## Step 5 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.swift` | `keyApi1()`, `keyApi2()` | One-line description |
```

---

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] Both Entry and Main classes exist in the Swift file
- [ ] Main inherits `BaseViewController`
- [ ] Storyboard has correct scene IDs
- [ ] No video rendering views in the storyboard
- [ ] MenuItem added to `ViewController.swift`
- [ ] `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called in `willMove(toParent:)` when `parent == nil`
- [ ] UI updates inside delegate callbacks dispatched to `DispatchQueue.main`
- [ ] Microphone permission requested before `joinChannel()`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Project builds without errors

---

## NEVER

- NEVER call `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, or `startPreview()` — the SDK has no video module
- NEVER add `AgoraRtcVideoCanvas` or `VideoView` to any storyboard or code in this project
- NEVER create `AgoraRtcEngineKit` in the Entry VC
- NEVER call `leaveChannel` or `destroy` in `viewDidDisappear` — use `willMove(toParent:)` with `parent == nil`
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `DispatchQueue.main.async { }`
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
