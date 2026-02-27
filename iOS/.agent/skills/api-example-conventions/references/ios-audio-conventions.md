# iOS Audio Platform API Example Conventions

This document describes the structure conventions for examples in the `APIExample-Audio/` project.

## Directory Structure

- **Basic Examples:** `APIExample-Audio/APIExample-Audio/Examples/Basic/<ExampleName>/`
- **Advanced Examples:** `APIExample-Audio/APIExample-Audio/Examples/Advanced/<ExampleName>/`

## File Composition

Similar to standard UIKit version, but focused on audio features:
- Entry and Main classes
- Simplified UI (no video rendering views)
- Audio-specific configuration and controls

## Audio-Specific Conventions

### Audio-Focused Features

- Audio capture and playback
- Audio effects (reverb, voice changer, etc.)
- Audio mixing
- Custom audio source
- Audio routing control

### Simplified UI

Since no video is involved, UI is more concise:
- Channel control buttons
- Audio status indicators
- Audio effect parameter adjustments

### API Call Focus

```swift
// Enable audio
agoraKit.enableAudio()

// Audio configuration
agoraKit.setAudioProfile(.musicHighQuality)
agoraKit.setAudioScenario(.gameStreaming)

// Audio effects
agoraKit.setLocalVoicePitch(1.0)
agoraKit.setLocalVoiceEqualization(.band31, withGain: 0)
```

## Menu Registration

Register example entry in `ViewController.swift`.

## Detailed Conventions

(To be supplemented with complete Audio platform conventions)
