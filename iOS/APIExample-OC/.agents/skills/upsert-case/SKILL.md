---
name: upsert-case
description: >
  Add a new API demo case or modify an existing one in the APIExample-OC (Objective-C + UIKit) project.
  Covers folder creation, Entry/Main OC files, storyboard, MenuItem registration, Xcode target
  membership, and Case Index update.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# upsert-case — APIExample-OC

## When to Use

- **Add**: the feature has no existing case in `Examples/Basic/` or `Examples/Advanced/`
- **Modify**: the case already exists — follow the Modify Existing Case flow below

Before adding, search the Case Index in `ARCHITECTURE.md` to confirm the case does not already exist.

## Files to Touch

| Scenario | Files |
|----------|-------|
| Add new case | New folder + `.h/.m` files + `.storyboard`, `ViewController.m` (MenuItem), `APIExample-OC.xcodeproj/project.pbxproj` (target membership), `ARCHITECTURE.md` (Case Index) |
| Modify existing case | Existing `.h/.m` files, optionally `.storyboard`, `ARCHITECTURE.md` (Case Index); update the project file only for new or moved build inputs |

---

## Modify Existing Case

1. Locate the existing .h/.m implementation and change the actual runtime behavior first.
2. Update storyboard outlets, actions, and controller identifiers when the behavior change needs it.
3. Adjust registration in `ViewController.m` only when the menu or navigation wiring changes.
4. Update Xcode target membership for new or moved build inputs, then synchronize the Case Index.
5. Build and review the changed behavior, lifecycle, and registration. Reusing an existing folder does not replace implementation work.

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
#import "APIExample_OC-swift.h"

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
@property (nonatomic, strong, nullable) AgoraRtcEngineKit *agoraKit;
@property (nonatomic) NSUInteger tokenRequestID;
@end

@implementation <ExampleName>Main
- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupRTC];
}

- (void)setupRTC {
    NSAssert([NSThread isMainThread], @"RTC lifecycle must run on main");
    if (self.agoraKit != nil) return;
    AgoraRtcEngineConfig *config = [AgoraRtcEngineConfig new];
    config.appId = [KeyCenter AppId];
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    // Configure media, then call requestJoin:requestPermission: with the actual permission flow.
}

- (void)requestJoin:(NSString *)channelName
 requestPermission:(void (^)(void (^)(BOOL)))requestPermission {
    NSAssert([NSThread isMainThread], @"RTC lifecycle must run on main");
    AgoraRtcEngineKit *engine = self.agoraKit;
    if (engine == nil || channelName.length == 0) return;
    NSString *channel = [channelName copy];
    NSUInteger uid = 0;
    NSUInteger requestID = ++self.tokenRequestID;
    __weak typeof(self) weakSelf = self;
    __weak AgoraRtcEngineKit *weakEngine = engine;
    requestPermission(^(BOOL granted) {
        dispatch_async(dispatch_get_main_queue(), ^{
            typeof(self) self = weakSelf;
            AgoraRtcEngineKit *engine = weakEngine;
            if (!self || !engine || self.tokenRequestID != requestID || self.agoraKit != engine) return;
            if (!granted) {
                NSLog(@"Permission denied");
                return;
            }
            [[NetworkManager shared] generateTokenWithChannelName:channel uid:uid success:^(NSString * _Nullable token) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    typeof(self) self = weakSelf;
                    AgoraRtcEngineKit *engine = weakEngine;
                    if (!self || !engine || self.tokenRequestID != requestID || self.agoraKit != engine) return;
                    if ([KeyCenter Certificate].length > 0 && token.length == 0) {
                        NSLog(@"Token request failed");
                        return;
                    }
                    AgoraRtcChannelMediaOptions *option = [AgoraRtcChannelMediaOptions new];
                    option.clientRoleType = AgoraClientRoleBroadcaster;
                    option.publishMicrophoneTrack = YES;
                    // Configure video publication/canvases only after camera permission.
                    int result = [engine joinChannelByToken:token channelId:channel
                                                       uid:uid mediaOptions:option joinSuccess:nil];
                    if (result != 0) NSLog(@"joinChannel failed: %d", result);
                });
            }];
        });
    });
}

- (void)leaveChannel {
    NSAssert([NSThread isMainThread], @"RTC lifecycle must run on main");
    self.tokenRequestID += 1;
    [self.agoraKit leaveChannel:nil];
}

- (void)onDestroy {
    [self leaveChannel]; // Invalidate even while permission/Token/join is pending.
    if (self.agoraKit == nil) return;
    // Stop case-owned capture, players, timers and observers here.
    [AgoraRtcEngineKit destroy];
    self.agoraKit = nil;
}

- (void)willMoveToParentViewController:(UIViewController *)parent {
    [super willMoveToParentViewController:parent];
    if (parent == nil) [self onDestroy];
}
@end

@implementation <ExampleName>Main (AgoraRtcEngineDelegate)
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel
            withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        if (weakSelf.agoraKit != engine) return;
        NSLog(@"Joined: %@ uid: %lu", channel, (unsigned long)uid);
    });
}
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        if (weakSelf.agoraKit != engine) return;
        NSLog(@"Error: %ld", (long)errorCode);
    });
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

## Step 6 — Add Files to the Xcode Target

This project uses explicit Xcode groups and build phases. For a new case, update
`APIExample-OC.xcodeproj/project.pbxproj` so the `.m` file belongs to the `APIExample-OC`
target's Sources build phase, the header has a file reference, and the storyboard belongs
to the target's Resources build phase. Add new localized or media resources to Resources
as well. Existing-file edits do not require a project-file change unless a build input was
added or moved.

## Step 7 — Update the Case Index

Add a row to the `## Case Index` table in `ARCHITECTURE.md`:

```markdown
| <ExampleName> | `Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.m` | `keyApi1:`, `keyApi2:` | One-line description |
```

---

This is a skeleton: call `requestJoin:requestPermission:` from setup or the Join action,
passing the channel from `configs` and the case's actual permission request. The permission
completion reports granted/denied; never use an unconditional grant in a real case. Keep
setup, join, leave and destroy on main. A user Leave action must call `leaveChannel`.
`NetworkManager` is Swift: import the generated `APIExample_OC-swift.h`; Swift default
arguments do not remove `uid:` from the Objective-C selector.

## Verification Checklist

- [ ] Folder created under correct category (Basic / Advanced)
- [ ] Both `.h` and `.m` files created with Entry and Main classes
- [ ] Main inherits `BaseViewController` and conforms to `AgoraRtcEngineDelegate`
- [ ] Storyboard has correct scene IDs
- [ ] MenuItem added to `ViewController.m`
- [ ] New Objective-C files are referenced and the `.m` file is in the `APIExample-OC` target's Sources build phase
- [ ] New storyboards, localized files, and media assets are in the target's Resources build phase
- [ ] `leaveChannel:` + `[AgoraRtcEngineKit destroy]` called when leaving
- [ ] UI updates inside delegate callbacks dispatched via `dispatch_async(dispatch_get_main_queue(), ^{ })`
- [ ] `__weak typeof(self) weakSelf = self` used in blocks that capture `self`
- [ ] Camera/microphone permissions requested before `joinChannelByToken:`
- [ ] Case Index row added/updated in `ARCHITECTURE.md`
- [ ] Permission/Token pending → leave/destroy → delayed callback does not join
- [ ] Repeat cleanup, reopen and out-of-order responses preserve only the current request
- [ ] Required Token failures and nonzero join returns are handled without logging credentials
- [ ] Project builds without errors

---

## NEVER

- NEVER create `AgoraRtcEngineKit` in the Entry VC
- NEVER use `__unsafe_unretained` for delegate references — use `__weak`
- NEVER update UI directly inside `AgoraRtcEngineDelegate` callbacks — always `dispatch_async(dispatch_get_main_queue(), ^{ })`
- NEVER add a new scene to `Main.storyboard` — each case must have its own `.storyboard` file
- NEVER add a source or resource file without adding it to the `APIExample-OC` target
- NEVER share an `AgoraRtcEngineKit` instance between cases
- NEVER call `joinChannelByToken:` before requesting camera/microphone permissions
- NEVER skip updating the Case Index in `ARCHITECTURE.md`
