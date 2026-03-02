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

```cpp
// Initialization code example
agora::rtc::RtcEngineContext context;
context.appId = KeyCenter::GetAppId();
context.eventHandler = this;
m_agoraKit = agora::rtc::createAgoraRtcEngine();
m_agoraKit->initialize(context);
```

**Key Parameters:**
- `appId`: {parameter description}
- `eventHandler`: {parameter description}

### 2. Configure Parameters

```cpp
// Configuration code example
```

**Configurable Parameters:**
- `param1`: {parameter description and optional values}
- `param2`: {parameter description and optional values}

### 3. Execute Core Operation

```cpp
// Core operation code example
```

### 4. Handle Event Callbacks

```cpp
// Event handling code example
```

### 5. Clean Up Resources

```cpp
// Cleanup code example
m_agoraKit->leaveChannel();
m_agoraKit->release();
```

## Key Event Callbacks

### {callbackName1}
```cpp
virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
    // Callback description
}
```
{Description of when callback is triggered and its purpose}

### {callbackName2}
```cpp
virtual void onUserJoined(uid_t uid, int elapsed) {
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

- [Windows API Reference (English)](https://api-ref.agora.io/en/video-sdk/windows/4.x/API/toc_video_call.html)
- [Windows API Reference (Chinese)](https://doc.shengwang.cn/api-ref/rtc/windows/API/toc_video_call)
- [Error Code Description](https://doc.shengwang.cn/api-ref/rtc/windows/error-code)
- [{Specific Feature Documentation}]({link})

## Related Examples

- `{RelatedExample1}` - {Brief description}
- `{RelatedExample2}` - {Brief description}
- `{RelatedExample3}` - {Brief description}
