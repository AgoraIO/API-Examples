# iOS Platform API Example Conventions

This document describes the structure conventions for examples in the iOS APIExample project. Please follow these conventions when adding or modifying any examples under `APIExample/APIExample/Examples/`.

## Directory Structure

- **Basic Examples:** `APIExample/APIExample/Examples/Basic/<ExampleName>/`
- **Advanced Examples:** `APIExample/APIExample/Examples/Advanced/<ExampleName>/`

The folder name is the **example identifier** (e.g., `VideoProcess`, `JoinChannelVideo`), using PascalCase without spaces.

The folder typically contains:

- One or more Swift files (e.g., `VideoProcess.swift`, `JoinChannelVideo.swift`), defining Entry and Main view controllers.
- One storyboard: `Base.lproj/<ExampleName>.storyboard` (same name as identifier).

## Swift: Entry and Main

Each example contains two view controller roles:

### Entry (Entry Page)

- **Class Name:** `<ExampleName>Entry` (e.g., `VideoProcessEntry`, `JoinChannelVideoEntry`).
- **Base Class:** `UIViewController`.
- **Responsibilities:**
  - Provide channel name input field and "Join" button (or equivalent controls).
  - Optional: Collect resolution, frame rate, orientation, etc. (refer to `JoinChannelVideoEntry`).
  - In the "Join" button callback:
    - Read channel name (and any additional required parameters).
    - Load **same-named storyboard**: `UIStoryboard(name: identifier, bundle: nil)`, where `identifier = "<ExampleName>"`.
    - Instantiate Main VC: `storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController`.
    - Set `newViewController.configs`, at least including `"channelName"`, plus other keys needed by Main (e.g., `"resolution"`, `"fps"`, `"orientation"`).
    - Push Main VC: `navigationController?.pushViewController(newViewController, animated: true)`.

### Main (Example Main Logic)

- **Class Name:** `<ExampleName>Main`, or a single class name consistent with Main's identifier in storyboard (if no Main suffix, same as identifier).
- **Base Class:** `BaseViewController`.
- **Responsibilities:**
  - Read channel name etc. from `configs` (e.g., `configs["channelName"] as? String`) and options passed from Entry.
  - Create and configure `AgoraRtcEngineKit` (e.g., `KeyCenter.AppId`, `GlobalSettings.shared.area`), call `Util.configPrivatization(agoraKit:)` when needed.
  - Implement specific Agora API calls (join channel, toggle audio/video, etc.).
  - Conform to `AgoraRtcEngineDelegate` and other required protocols.

Reference: `JoinChannelVideo.swift` (Entry + Main), `VideoProcess.swift` (Entry + Main).

## Storyboard

- **File Path:** `APIExample/Examples/<Basic|Advanced>/<ExampleName>/Base.lproj/<ExampleName>.storyboard`.
- **Two Scenes:**

  1. **Entry Scene**
     - **Storyboard Identifier:** `EntryViewController`.
     - **Custom Class:** `<ExampleName>Entry`, Module: `APIExample`.
     - Connect channel input field and join button to Entry class (outlets and actions).

  2. **Main Scene**
     - **Storyboard Identifier:** `<ExampleName>` (consistent with folder name, e.g., `VideoProcess`, `JoinChannelVideo`).
     - **Custom Class:** `<ExampleName>Main` (or actual Main class name used in code), Module: `APIExample`.
     - Layout example main interface and connect outlets/actions to this class.

- When entering from menu, **Entry is shown first**; after user clicks join, Main is instantiated and pushed in code.

## Menu Registration

- **File:** `APIExample/ViewController.swift`.
- **Structure:** `menus` is an array of `MenuSection`, each section has `name` (e.g., `"Basic"`, `"Anvanced"`) and `rows: [MenuItem]`.

When adding an example:

- Append a `MenuItem` to the `rows` of the corresponding section.
- **Field Description:**
  - `name`: Display name in list (can add `.localized` if project uses internationalization).
  - `storyboard`: Storyboard file name (**without** extension), consistent with example identifier, e.g., `"VideoProcess"`, `"JoinChannelVideo"`.
  - `entry`: Usually `"EntryViewController"` (default).
  - `controller`: Main VC's identifier or empty string `""`; since Entry pushes Main in this project, this is often `""` or consistent with Main's storyboard identifier.

Example:

```swift
MenuItem(name: "Video Process".localized, storyboard: "VideoProcess", controller: "VideoProcess")
```

If an example's Entry class or identifier is different (e.g., KtvCopyrightMusic), set `entry` and `controller` accordingly so the main `ViewController` loads the correct initial VC from that storyboard.

## Common Dependencies

- **App ID:** `KeyCenter.AppId`.
- **Region / Role / Resolution / Frame Rate / Orientation:** `GlobalSettings.shared` (e.g., `GlobalSettings.shared.area`, `getSetting(key: "resolution")`, etc.).
- **Logging:** `LogUtils`, call `agoraKit.setLogFile(LogUtils.sdkLogPath())` when needed.
- **Privatization:** After creating engine, call `Util.configPrivatization(agoraKit: agoraKit)`.
- **Video Layout:** Use `AGEVideoLayout`, `AGEVideoContainer` / `Bundle.loadVideoView` to display local/remote views.

Please keep naming and structure consistent with existing examples (e.g., `JoinChannelVideo`, `VideoProcess`) to maintain unified project style.
