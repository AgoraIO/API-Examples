---
name: migrate-api-to-project
description: Migrate API examples across different iOS projects, supporting cross-language, cross-framework, and cross-architecture migration
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  platform: iOS
dependencies:
  - api-example-conventions
---

# iOS Platform API Migration Guide

## Prerequisites

Activate `api-example-conventions` to understand the target project's structure conventions.

## Migration Workflow

### Step 1: Identify Source and Target

| Item | Source | Target |
|------|--------|--------|
| Project | APIExample / Audio / SwiftUI / OC | Target Project |
| Language | Swift / Objective-C | Swift / Objective-C |
| UI Framework | UIKit / SwiftUI | UIKit / SwiftUI |
| Architecture | Entry/Main | MVC / MVVM / Other |

### Step 2: Create Target Structure per Conventions

Create basic structure (directories, files, menu registration) according to `api-example-conventions` based on target project type.

### Step 3: Architecture Adaptation

If target architecture differs from source, adjust code organization first.

**Entry/Main → MVVM:**
```swift
// Source: Entry/Main pattern
class JoinChannelVideoMain: BaseViewController {
    var configs: [String: Any] = [:]
    private var agoraKit: AgoraRtcEngineKit?
}

// Target: MVVM pattern
struct VideoCallConfig {
    let channelName: String
    let token: String?
}

class VideoCallViewModel: ObservableObject {
    private var agoraKit: AgoraRtcEngineKit?
    let config: VideoCallConfig
    @Published var isJoined = false
}
```

### Step 4: Code Format Conversion

Convert code based on target project's language and UI framework.

#### Language Conversion (Swift → Objective-C)

**Swift:**
```swift
private var agoraKit: AgoraRtcEngineKit?

func joinChannel() {
    agoraKit?.joinChannel(byToken: nil, channelId: "test", uid: 0, mediaOptions: option)
}
```

**Objective-C:**
```objc
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;

- (void)joinChannel {
    [self.agoraKit joinChannelByToken:nil channelId:@"test" uid:0 mediaOptions:option joinSuccess:nil];
}
```

#### UI Framework Conversion (UIKit → SwiftUI)

**UIKit:**
```swift
class VideoCallMain: BaseViewController {
    @IBOutlet weak var localVideoView: UIView!
    private var agoraKit: AgoraRtcEngineKit?
}
```

**SwiftUI:**
```swift
class VideoCallViewModel: ObservableObject {
    private var agoraKit: AgoraRtcEngineKit?
    @Published var isJoined = false
}

struct VideoCallView: View {
    @StateObject private var viewModel = VideoCallViewModel()
    
    var body: some View {
        VideoViewRepresentable(uid: 0, agoraKit: viewModel.agoraKit)
    }
}

struct VideoViewRepresentable: UIViewRepresentable {
    let uid: UInt
    let agoraKit: AgoraRtcEngineKit?
    
    func makeUIView(context: Context) -> UIView { /* ... */ }
    func updateUIView(_ uiView: UIView, context: Context) { /* ... */ }
}
```

### Step 5: Adapt Project Differences

**Configuration Management:**
- Source: `KeyCenter.AppId`
- Target: Use target project's configuration approach

**Base Class Handling:**
- Inherit from target project's base class
- Or inline implement required methods

**Dependencies:**
- Ensure Agora SDK is integrated
- Handle third-party library differences

**Permission Configuration:**
- Ensure `Info.plist` contains necessary permissions

### Step 6: Test and Verify

- [ ] No compilation errors
- [ ] Functionality works correctly
- [ ] UI displays properly
- [ ] No memory leaks

## Conversion Quick Reference

### Language Differences

| Swift | Objective-C |
|-------|-------------|
| `var agoraKit: AgoraRtcEngineKit?` | `@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;` |
| `agoraKit?.method()` | `[self.agoraKit method]` |
| `{ }` closure | `^{ }` Block |
| Optional type `?` | Pointer can be `nil` |

### UI Framework Differences

| UIKit | SwiftUI |
|-------|---------|
| `UIViewController` | `View` + `ViewModel` |
| `@IBOutlet` | `@State` / `@Binding` |
| `UIView` | `UIViewRepresentable` |
| Delegate | `@Published` / Combine |

### Architecture Differences

| Entry/Main | MVVM |
|------------|------|
| `configs: [String: Any]` | `Config` struct |
| ViewController contains business logic | ViewModel contains business logic |
| Direct UI manipulation | Update UI via `@Published` |

## Migration Checklist

- [ ] Created target structure per conventions
- [ ] Architecture adapted (if needed)
- [ ] Code format converted
- [ ] Project differences handled
- [ ] Permission configuration added
- [ ] Test verification passed

## References

- **conventions:** Check target project's structure conventions
- **Example SKILL:** Check source example's API usage guide
