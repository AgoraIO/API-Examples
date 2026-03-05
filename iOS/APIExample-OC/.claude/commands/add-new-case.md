# add-new-case

Create a new example in the APIExample-OC project following the Entry/Main pattern in Objective-C.

## When to Use

- User wants to add a new API demonstration in Objective-C
- User needs to create a sample for a specific Agora feature
- Porting an example from another platform to Objective-C

## Prerequisites

Before creating a new case:
1. Run `query-cases` to verify the example doesn't already exist
2. Confirm the target category (Basic or Advanced)
3. Determine the example name (PascalCase, e.g., `MyNewFeature`)

## Instructions

### Step 1: Create the Example Folder

Create folder at `APIExample-OC/Examples/[Basic|Advanced]/<ExampleName>/`

### Step 2: Create the Header File

Create `<ExampleName>.h`:

```objc
#import <UIKit/UIKit.h>
#import "BaseViewController.h"

@interface <ExampleName>Entry : UIViewController
@end

@interface <ExampleName>Main : BaseViewController
@end
```

### Step 3: Create the Implementation File

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
    
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"<ExampleName>" bundle:nil];
    <ExampleName>Main *mainVC = [storyboard instantiateViewControllerWithIdentifier:@"<ExampleName>"];
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
    [self setupAgoraKit];
}

- (void)setupAgoraKit {
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = [KeyCenter AppId];
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = self.configs[@"channelName"];
    
    AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
    option.clientRoleType = AgoraClientRoleBroadcaster;
    option.publishCameraTrack = YES;
    option.publishMicrophoneTrack = YES;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName success:^(NSString *token) {
        [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:option];
    }];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

#pragma mark - AgoraRtcEngineDelegate

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    NSLog(@"Joined channel: %@", channel);
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    NSLog(@"Error: %ld", (long)errorCode);
}

@end
```

### Step 4: Create the Storyboard

Create `<ExampleName>.storyboard` with two scenes:
- Entry scene: Storyboard ID = `EntryViewController`, Class = `<ExampleName>Entry`
- Main scene: Storyboard ID = `<ExampleName>`, Class = `<ExampleName>Main`

### Step 5: Register the Example

Add to `+[MenuSection menus]` in `APIExample-OC/ViewController.m`:

```objc
[[MenuItem alloc] initWithName:@"<Display Name>".localized storyboard:@"<ExampleName>" controller:@""]
```

## Architecture Rules

- Entry class inherits `UIViewController`
- Main class inherits `BaseViewController`
- Main class owns the `AgoraRtcEngineKit` lifecycle
- Always call `leaveChannel:` then `[AgoraRtcEngineKit destroy]` when leaving
- Dispatch UI updates to main thread from delegate callbacks
- Request camera/microphone permissions before `joinChannelByToken:`

## Verification Checklist

- [ ] Example folder created in correct category
- [ ] Header and implementation files created
- [ ] Storyboard has correct scene IDs
- [ ] MenuItem added to ViewController.m
- [ ] Engine lifecycle properly managed (create/destroy)
- [ ] Permissions requested before joining
- [ ] Build succeeds without errors
