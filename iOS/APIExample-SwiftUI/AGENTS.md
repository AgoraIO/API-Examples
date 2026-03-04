# AGENTS.md — APIExample-SwiftUI

SwiftUI version of the API demo. Mirrors cases from `APIExample/` but uses
SwiftUI views instead of UIKit + Storyboards.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample-SwiftUI.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-SwiftUI/Common/KeyCenter.swift`:
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

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/RTC pattern details
