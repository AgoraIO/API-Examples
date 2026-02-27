# iOS SwiftUI Platform API Example Conventions

This document describes the structure conventions for examples in the `APIExample-SwiftUI/` project.

## Directory Structure

- **Basic Examples:** `APIExample-SwiftUI/APIExample-SwiftUI/Examples/Basic/<ExampleName>/`
- **Advanced Examples:** `APIExample-SwiftUI/APIExample-SwiftUI/Examples/Advanced/<ExampleName>/`

## File Composition

Each example typically contains:
- SwiftUI View file
- ViewModel (using `@ObservableObject`)
- Video rendering view wrapper (`UIViewRepresentable`)

## SwiftUI-Specific Conventions

### ViewModel Pattern

```swift
class <ExampleName>ViewModel: ObservableObject {
    private var agoraKit: AgoraRtcEngineKit?
    @Published var isJoined = false
    @Published var remoteUsers: [UInt] = []
    
    func initializeEngine() { /* ... */ }
    func joinChannel() { /* ... */ }
    func leaveChannel() { /* ... */ }
}
```

### View Structure

```swift
struct <ExampleName>View: View {
    @StateObject private var viewModel = <ExampleName>ViewModel()
    
    var body: some View {
        // UI implementation
    }
}
```

### Video Rendering Wrapper

Use `UIViewRepresentable` to wrap UIKit's video rendering view:

```swift
struct VideoViewRepresentable: UIViewRepresentable {
    let uid: UInt
    let agoraKit: AgoraRtcEngineKit?
    
    func makeUIView(context: Context) -> UIView { /* ... */ }
    func updateUIView(_ uiView: UIView, context: Context) { /* ... */ }
}
```

## Menu Registration

Register example entry in SwiftUI's navigation structure.

## Detailed Conventions

(To be supplemented with complete SwiftUI platform conventions)
