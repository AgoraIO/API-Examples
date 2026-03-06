# AGENTS.md — APIExample-OC

Objective-C variant of the API demo. Mirrors cases from `APIExample/` using Objective-C instead of Swift.

## Build Commands

```bash
pod install
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

To obtain an App ID, see [README.md](README.md#obtain-an-app-id).

## Skills

| Task | Skill | When to use |
|------|-------|-------------|
| Add or modify a case | `.agent/skills/upsert-case/` | Need to create a new API demo or update an existing one |
| Code review | `.agent/skills/review-case/` | Review case code for lifecycle, thread safety, and OC convention compliance |
| Find an existing case | `.agent/skills/query-cases/` | Locate which file demonstrates a specific API or feature |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration, Entry/Main pattern, engine lifecycle
