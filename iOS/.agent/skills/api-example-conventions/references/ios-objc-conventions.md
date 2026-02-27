# iOS Objective-C Platform API Example Conventions

This document describes the structure conventions for examples in the `APIExample-OC/` project.

## Directory Structure

- **Basic Examples:** `APIExample-OC/APIExample-OC/Examples/Basic/<ExampleName>/`
- **Advanced Examples:** `APIExample-OC/APIExample-OC/Examples/Advanced/<ExampleName>/`

## File Composition

Each example typically contains:
- Header file: `<ExampleName>.h`
- Implementation file: `<ExampleName>.m`
- Storyboard: `Base.lproj/<ExampleName>.storyboard`

## Objective-C Specific Conventions

### Entry and Main Pattern

Similar to Swift version, but using Objective-C syntax:

**Entry Class:**
```objc
@interface <ExampleName>Entry : UIViewController
@end
```

**Main Class:**
```objc
@interface <ExampleName>Main : BaseViewController
@end
```

### Delegate Methods

Implement `AgoraRtcEngineDelegate` protocol:

```objc
@interface <ExampleName>Main () <AgoraRtcEngineDelegate>
@end
```

### Property Declaration

```objc
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;
@property (nonatomic, weak) IBOutlet UIView *localVideoView;
```

## Menu Registration

Register example entry in `ViewController.m`.

## Detailed Conventions

(To be supplemented with complete Objective-C platform conventions)
