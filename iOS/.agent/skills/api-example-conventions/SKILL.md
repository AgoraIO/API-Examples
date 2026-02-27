---
name: api-example-conventions
description: Structure conventions for iOS APIExample projects, including UIKit, SwiftUI, Objective-C, and Audio implementations
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 2.0.0
  platform: iOS
---

# iOS API Example Conventions

This SKILL contains structure conventions for different APIExample implementations on iOS platform. Load the corresponding convention document on demand based on your target project type.

## iOS Project Types

This repository contains the following iOS example projects:

### 1. APIExample (Standard UIKit)

**Directory:** `APIExample/`

**Core Rules:**
- **Example Directory:** `APIExample/APIExample/Examples/[Basic|Advanced]/<ExampleName>/`
- **Files:**
  - `<ExampleName>.swift` - Entry and Main classes
  - `Base.lproj/<ExampleName>.storyboard`
- **Classes:**
  - Entry: `<ExampleName>Entry` inherits from `UIViewController`
  - Main: `<ExampleName>Main` inherits from `BaseViewController`
- **Storyboard IDs:**
  - Entry scene: `EntryViewController`
  - Main scene: `<ExampleName>`
- **Menu Registration:** Add `MenuItem` to the `menus` array in `APIExample/APIExample/ViewController.swift`

**Detailed Conventions:** Read `references/ios-uikit-conventions.md`

### 2. APIExample-SwiftUI

**Directory:** `APIExample-SwiftUI/`

**Core Rules:**
- Uses SwiftUI framework
- Uses `@StateObject` and `@ObservableObject` for state management
- Uses `UIViewRepresentable` to wrap video rendering views

**Detailed Conventions:** Read `references/ios-swiftui-conventions.md`

### 3. APIExample-OC (Objective-C)

**Directory:** `APIExample-OC/`

**Core Rules:**
- Uses Objective-C language
- Similar Entry/Main pattern as UIKit
- Uses `.h` and `.m` files

**Detailed Conventions:** Read `references/ios-objc-conventions.md`

### 4. APIExample-Audio

**Directory:** `APIExample-Audio/`

**Core Rules:**
- Focuses on audio-related features
- Simplified UI implementation
- Audio-specific API calls

**Detailed Conventions:** Read `references/ios-audio-conventions.md`

## Common Conventions

Conventions shared across all iOS projects:

1. **Example Naming:** Use PascalCase (e.g., `JoinChannelVideo`)
2. **Categories:** Basic (basic features) and Advanced (advanced features)
3. **SKILL Documentation:** Each example may include `SKILL.md` providing API usage guide
4. **Code Style:** Follow official Swift/Objective-C coding standards
5. **Configuration Management:** Use `KeyCenter` to manage App ID and Token
6. **Global Settings:** Use `GlobalSettings.shared` to manage common configurations

## Load on Demand

When you need to work with specific example types:

1. **UIKit Standard Examples:** Read `references/ios-uikit-conventions.md`
2. **SwiftUI Examples:** Read `references/ios-swiftui-conventions.md`
3. **Objective-C Examples:** Read `references/ios-objc-conventions.md`
4. **Audio-Focused Examples:** Read `references/ios-audio-conventions.md`

