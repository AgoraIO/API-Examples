---
name: upsert-case
description: >
  Add a new API demo case or modify an existing one in the APIExample-OC (Objective-C + UIKit) project.
  Covers folder creation, Entry/Main OC files, storyboard, MenuItem registration, and Case Index update.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-OC

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — skip Steps 1–3, go directly to Step 4+

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `.h/.m` files + `.storyboard`, `ViewController.m` (MenuItem), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `.h/.m` files, optionally `.storyboard`, `ARCHITECTURE.md` (Case Index) |

---

## Step 1 — Create the Example Folder

```
APIExample-OC/Examples/[Basic|Advanced]/<ExampleName>/
```

## Step 2 — Create the Header File

Create `<ExampleName>.h`:

```objc
#import "BaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface <ExampleName>Entry : UIViewController
@end

@interface <ExampleName>Main : BaseViewController
@end

NS_ASSUME_NONNULL_END
```

## Step 3 — Create the Implementation File

Create `<ExampleName>.m`:

```objc
#import "<ExampleName>.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "KeyCenter.h"
#import "NetworkManager.h"

@interface <ExampleName>Entry ()
@property (weak, nonatomic) IBOutlet UITextField *channelTextField;
@end

@implementation <ExampleName>Entry
- (IBAction)onJoinPressed:(UIButton *)sender {
    NSString *channelName = self.channelTextField.text;
    if (channelName.length == 0) return;
    UIStoryboard *sb = [UIStoryboard storyboardWithName:@"<ExampleName>" bundle:nil];
    <ExampleName>Main *mainVC = [sb instantiateViewControllerWithIdentifier:@"<ExampleName>"];
    mainVC.configs = @{@"channelName": channelName};
    [self.navigationController pushViewController:mainVC animated:YES];
}
@end

@interface <ExampleName>Main () <AgoraRtcEngineDelegate>
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;
@end

@implementation <ExampleName>Main
- (void)viewDidLoad {
    [super viewDidLoad];
    NSString *channelName = self.configs[@"channelName"];
    AgoraRtcEngineConfig *config = [AgoraRtcEngineConfig new];
    config.appId = [KeyCenter AppId];
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    // configure engine, request permissions, then join
    [[NetworkManager shared] generateTokenWithChannelName:channelName success:^(NSString *token) {
        AgoraRtcChannelMediaOptions *option = [AgoraRtcChannelMediaOptions new];
        [self.agoraKit joinChannelByToken:token channelId:channelName
                                      uid:0 mediaOptions:option joinSuccess:nil];
    }];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    if (self.isMovingFromParentViewController) {
        [self.agoraKit leaveChannel:nil];
        [AgoraRtcEngineKit destroy];
    }
}
@end

@implementation <ExampleName>Main (AgoraRtcEngineDelegate)
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel
            withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    NSLog(@"Joined: %@ uid: %lu", channel, (unsigned long)uid);
}
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    NSLog(@"Error: %ld", (long)errorCode);
}
@end
```

## Step 4 — Create the Storyboard

Create `APIExample-OC/<ExampleName>.storyboard` with two scenes:

| Scene | Storyboard ID | Class |
|-------|--------------|-------|
| Entry | `EntryViewController` | `<ExampleName>Entry` |
| Main  | `<ExampleName>` | `<ExampleName>Main` |

## Step 5 — Register the MenuItem

Add to `+[MenuSection menus]` in `ViewController.m`:

```objc
[[MenuItem alloc] initWithName:NSLocalizedString(@"<Display Name>", nil)
                    storyboard:@"<ExampleName>"
                    controller:@""]
```

## Step 6 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.m` | `keyApi1:`, `keyApi2:` | One-line description |
```

---

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] Both `.h` and `.m` files created with Entry and Main classes
- [ ] Main inherits `BaseViewController` and conforms to `AgoraRtcEngineDelegate`
- [ ] Storyboard has correct scene IDs
- [ ] MenuItem added to `ViewController.m`
- [ ] `leaveChannel:` + `[AgoraRtcEngineKit destroy]` called when leaving
- [ ] UI updates inside delegate callbacks dispatched via `dispatch_async(dispatch_get_main_queue(), ^{ })`
- [ ] `__weak typeof(self) weakSelf = self` used in blocks that capture `self`
- [ ] Camera/microphone permissions requested before `joinChannelByToken:`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Project builds without errors

---

## NEVER

- NEVER create `AgoraRtcEngineKit` in the Entry VC
- NEVER use `__unsafe_unretained` for delegate references — use `__weak`
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `dispatch_async(dispatch_get_main_queue(), ^{ })`
- NEVER add a new scene to `Main.storyboard` — each case must have its own `.storyboard` file
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER call `joinChannelByToken:` before requesting camera/microphone permissions
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
