---
name: upsert-case
description: >
  Add a new API demo case or modify an existing one in the APIExample (UIKit + Swift) project.
  Covers folder creation, Entry/Main Swift file, storyboard, MenuItem registration, and Case Index update.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — update the existing `.swift` and storyboard first, then check registration and docs

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `.swift` file + `Base.lproj/<ExampleName>.storyboard`, `ViewController.swift` (MenuItem), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `.swift` file(s), optionally `Base.lproj/<ExampleName>.storyboard`, `ViewController.swift` if registration/wiring changed, `ARCHITECTURE.md` (Case Index) |

---

## Modify Existing Case

When repairing or rebuilding an existing case, use this order instead of the new-case flow:

1. Locate the existing `.swift` implementation and update the actual runtime logic first
2. Update the existing storyboard if scene wiring, outlets, actions, or controller identifiers changed
3. Check `APIExample/ViewController.swift` and fix the `MenuItem` only if registration or `controller` / `storyboard` wiring is wrong
4. Update `ARCHITECTURE.md` last if the case path, APIs, or description changed

Do not skip implementation edits just because the case folder already exists.

## Step 1 — Create the Example Folder

```
APIExample/Examples/[Basic|Advanced]/<ExampleName>/
```

Use `Basic/` for fundamental channel join demos, `Advanced/` for everything else.

## Step 2 — Create the Swift File

Create `<ExampleName>.swift` containing both Entry and Main classes:

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
        // configure engine, request permissions, then join
        NetworkManager.shared.generateToken(channelName: channelName) { [weak self] token in
            let option = AgoraRtcChannelMediaOptions()
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

Create `APIExample/Examples/[Basic|Advanced]/<ExampleName>/Base.lproj/<ExampleName>.storyboard` with two scenes:

| Scene | Storyboard ID | Class |
|-------|--------------|-------|
| Entry | `EntryViewController` | `<ExampleName>Entry` |
| Main  | `<ExampleName>` | `<ExampleName>Main` |

Connect a `Show` segue or use the manual push in `onJoinPressed`.

Keep the storyboard inside the example folder. Do not place new case storyboards in the shared `APIExample/Base.lproj/` directory.

## Default Entry UI Convention

Unless the user explicitly asks for a different flow, use this default Entry layout and interaction:

- One channel input field (`UITextField`) with placeholder `"Enter channel name".localized`
- One join button (`UIButton`) with title `"Join".localized`
- Join action validates non-empty channel name, dismisses keyboard, and pushes Main VC
- Pass config via `configs = ["channelName": channelName]`

Rationale:
- This matches the dominant pattern used by existing APIExample cases
- Keeps new cases consistent with existing user interaction and navigation
- Minimizes refactor cost when RTC join logic is added later

## Step 4 — Register the MenuItem

Add to the `menus` array in `APIExample/ViewController.swift`:

```swift
MenuItem(name: "<Display Name>".localized,
         storyboard: "<ExampleName>",
         controller: "<ExampleName>")
```

Place it in the correct section (Basic / Advanced).

## Step 5 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.swift` | `keyApi1()`, `keyApi2()` | One-line description |
```

Key APIs: list 2–5 core SDK methods the case demonstrates. Do not list `joinChannel`, `leaveChannel`, `destroy`, or `sharedEngine` unless they are the primary focus.

---

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] Both Entry and Main classes exist in the Swift file
- [ ] Main inherits `BaseViewController`
- [ ] Storyboard has correct scene IDs
- [ ] Entry scene follows default UI convention (channel input + Join button), unless the user requested otherwise
- [ ] MenuItem added to `ViewController.swift`
- [ ] `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called in `willMove(toParent:)` when `parent == nil`
- [ ] UI updates inside delegate callbacks dispatched to `DispatchQueue.main`
- [ ] Camera/microphone permissions requested before `joinChannel()`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Project builds without errors

---

## NEVER

- NEVER create `AgoraRtcEngineKit` in the Entry VC
- NEVER call `leaveChannel` or `destroy` in `viewDidDisappear` — use `willMove(toParent:)` with `parent == nil`
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `DispatchQueue.main.async { }`
- NEVER add a new scene to `Main.storyboard` — each case must have its own `.storyboard` file
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER call `joinChannel` before requesting camera/microphone permissions
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
