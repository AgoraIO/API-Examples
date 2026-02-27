---
name: {example-name-kebab-case}-guide
description: Guide for implementing {Feature Name} in business scenarios, including initialization, configuration, and event handling
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  example: {ExampleName}
  category: {Basic|Advanced}
  apis: [{API1}, {API2}, {API3}]
---

# {Feature Name} Implementation Guide

## Feature Description

This example demonstrates how to use Agora RTC SDK to implement {feature description}, including:
- {Feature point 1}
- {Feature point 2}
- {Feature point 3}

## Core API Call Flow

### 1. Initialize SDK

```swift
// Initialization code example
let config = AgoraRtcEngineConfig()
config.appId = KeyCenter.AppId
config.areaCode = GlobalSettings.shared.area
agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
```

**Key Parameters:**
- `appId`: {parameter description}
- `areaCode`: {parameter description}

### 2. Configure Parameters

```swift
// Configuration code example
```

**Configurable Parameters:**
- `param1`: {parameter description and optional values}
- `param2`: {parameter description and optional values}

### 3. Execute Core Operation

```swift
// Core operation code example
```

### 4. Handle Event Callbacks

```swift
// Event handling code example
```

### 5. Clean Up Resources

```swift
// Cleanup code example
agoraKit.leaveChannel { stats in
    print("left channel")
}
```

## Key Event Callbacks

### {callbackName1}
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, {callback signature}) {
    // Callback description
}
```
{Description of when callback is triggered and its purpose}

### {callbackName2}
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, {callback signature}) {
    // Callback description
}
```
{Description of when callback is triggered and its purpose}

## Common Questions

### Q: {Question description}?
A: {Solution and code example}

### Q: {Question description}?
A: {Solution and code example}

### Q: {Question description}?
A: {Solution and code example}

## Reference Documentation

- [iOS API Reference (English)](https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit)
- [iOS API Reference (Chinese)](https://doc.shengwang.cn/api-ref/rtc/ios/API/toc_video_call)
- [Error Code Description](https://doc.shengwang.cn/api-ref/rtc/ios/error-code)
- [{Specific Feature Documentation}]({link})

## Related Examples

- `{RelatedExample1}` - {Brief description}
- `{RelatedExample2}` - {Brief description}
- `{RelatedExample3}` - {Brief description}
