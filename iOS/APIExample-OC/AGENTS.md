# AGENTS.md — APIExample-OC

Objective-C version of the API demo. Mirrors cases from `APIExample/` but uses
Objective-C instead of Swift.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample-OC.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-OC/Common/KeyCenter.m`:
```objc
+ (NSString *)AppId {
    return @"YOUR_APP_ID";
}

+ (NSString *)Certificate {
    return nil;   // leave nil if App Certificate is not enabled
}
```

## Skills

| Task | Skill | Status |
|------|-------|--------|
| Find an existing example | `query-cases` | TODO |
| Add a new example | `add-new-case` | TODO |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/Main pattern details
