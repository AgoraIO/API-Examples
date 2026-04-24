# AGENTS.md — macOS

## Project Context

This is the Swift + Cocoa implementation of Agora RTC SDK examples for macOS. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules.

## Build Commands

```bash
# Install dependencies
pod install

# Build in Xcode
xcodebuild -workspace APIExample.xcworkspace -scheme APIExample -configuration Release

# Or open in Xcode and build manually
open APIExample.xcworkspace
```

## App ID Configuration

Configure your Agora App ID in `APIExample/Common/KeyCenter.swift`:

```swift
struct KeyCenter {
    static let AppId: String = "<#YOUR_APP_ID#>"
    
    // Token is optional for testing, but required for production
    static func Token(channelName: String) -> String {
        return "<#YOUR_TOKEN#>"
    }
}
```

## Architecture Red Lines

**Do NOT:**
- Introduce UIKit or SwiftUI — use Cocoa (AppKit) only
- Use Combine or async/await patterns unless already present in the file being modified
- Create examples outside the `APIExample/Examples/[Basic|Advanced]/` directory structure
- Forget to call `leaveChannel()` and `destroy()` when closing an example
- Update UI from background threads — always dispatch to main thread
- Share engine instances between examples — each example manages its own lifecycle

## Rules

### Follow the Architecture

All work must conform to the rules defined in `ARCHITECTURE.md`:
- Every example is a self-contained class implementing `AgoraRtcEngineDelegate`
- Each example manages its own Agora engine lifecycle
- Configuration is passed via initialization or property injection
- All examples are registered in `APIExample/ViewController.swift`

### Follow the Existing Language and Framework

- Language is Swift — do not introduce Objective-C files
- UI framework is Cocoa (AppKit) — do not introduce UIKit or SwiftUI
- State management uses instance variables and delegate callbacks — do not introduce Combine or async/await patterns unless they already exist in the file being modified
- Match the code style, naming, and patterns of existing examples

### Use Project-Level SKILLs

For broader tasks, use the skills in `.agent/skills/`:

| Task | Skill | When to use |
|------|-------|-------------|
| Add or modify an example | `.agent/skills/upsert-case/` | Need to create a new API demo or update an existing one |
| Code review | `.agent/skills/review-case/` | Review example code for lifecycle, thread safety, and convention compliance |
