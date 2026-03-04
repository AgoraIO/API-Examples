# AGENTS.md — APIExample-Audio

Audio-only demo project. Uses `AgoraAudio_iOS` — the video module is not available.
Use this project only when the PRD is explicitly audio-only.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample-Audio.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-Audio/Common/KeyCenter.swift`:
```swift
static let AppId: String = "YOUR_APP_ID"
static let Certificate: String? = nil   // leave nil if App Certificate is not enabled
```

## Skills

| Task | Skill | Status |
|------|-------|--------|
| Find an existing example | `query-cases` | TODO |
| Add a new example | `add-new-case` | TODO |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/Main pattern details
