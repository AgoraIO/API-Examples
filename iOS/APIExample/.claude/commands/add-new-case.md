# add-new-case

Create a new example in the APIExample project following the Entry/Main pattern.

## When to Use

- User wants to add a new API demonstration
- User needs to create a sample for a specific Agora feature
- Porting an example from another platform

## Prerequisites

Before creating a new case:
1. Run `query-cases` to verify the example doesn't already exist
2. Confirm the target category (Basic or Advanced)
3. Determine the example name (PascalCase, e.g., `MyNewFeature`)

## Instructions

### Step 1: Create the Example Folder

Create folder at `APIExample/Examples/[Basic|Advanced]/<ExampleName>/`

### Step 2: Create the Swift File

Create `<ExampleName>.swift` with both Entry and Main classes:

```swift
import UIKit
import AgoraRtcKit

class <ExampleName>Entry: UIViewController {
    @IBOutlet weak var channelTextField: UITextField!
    
    @IBAction func onJoinPressed(_ sender: UIButton) {
        guard let channelName = channelTextField.text, !channelName.isEmpty else { return }
        let storyboard = UIStoryboard(name: "<ExampleName>", bundle: nil)
        guard let mainVC = storyboard.instantiateViewController(withIdentifier: "<ExampleName>") as? <ExampleName>Main else { return }
        mainVC.configs = ["channelName": channelName]
        navigationController?.pushViewController(mainVC, animated: true)
    }
}

class <ExampleName>Main: BaseViewController {
    var agoraKit: AgoraRtcEngineKit?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupAgoraKit()
    }
    
    private func setupAgoraKit() {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        guard let channelName = configs["channelName"] as? String else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.clientRoleType = .broadcaster
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        
        NetworkManager.shared.generateToken(channelName: channelName) { [weak self] token in
            self?.agoraKit?.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
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
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Joined channel: \(channel)", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error: \(errorCode.rawValue)", level: .error)
    }
}
```

### Step 3: Create the Storyboard

Create `Base.lproj/<ExampleName>.storyboard` with two scenes:
- Entry scene: Storyboard ID = `EntryViewController`, Class = `<ExampleName>Entry`
- Main scene: Storyboard ID = `<ExampleName>`, Class = `<ExampleName>Main`

### Step 4: Register the Example

Add to `menus` array in `APIExample/ViewController.swift`:

```swift
MenuItem(name: "<Display Name>".localized, storyboard: "<ExampleName>", controller: "<ExampleName>")
```

## Architecture Rules

- Entry class inherits `UIViewController`
- Main class inherits `BaseViewController`
- Main class owns the `AgoraRtcEngineKit` lifecycle
- Always call `leaveChannel()` then `AgoraRtcEngineKit.destroy()` when leaving
- Dispatch UI updates to main thread from delegate callbacks
- Request camera/microphone permissions before `joinChannel()`

## Verification Checklist

- [ ] Example folder created in correct category
- [ ] Swift file contains both Entry and Main classes
- [ ] Storyboard has correct scene IDs
- [ ] MenuItem added to ViewController.swift
- [ ] Engine lifecycle properly managed (create/destroy)
- [ ] Permissions requested before joining
- [ ] Build succeeds without errors
