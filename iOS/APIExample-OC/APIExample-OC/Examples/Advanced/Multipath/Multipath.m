#import "Multipath.h"
#import "BaseViewController.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"
#import "KeyCenter.h"
#import "Util.h"

@interface MultipathEntry ()

@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UISegmentedControl *modeSegment;
@property (weak, nonatomic) IBOutlet UISegmentedControl *roleSegment;

@end

@implementation MultipathEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"Multipath" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"Multipath"];
    newViewController.title = self.textField.text;
    newViewController.configs = @{
        @"channelName": self.textField.text,
        @"role_index": @(self.roleSegment.selectedSegmentIndex),
        @"mode_index": @(self.modeSegment.selectedSegmentIndex)
    };
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end

@interface MultipathViewController ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UILabel *modeLabel;
@property (nonatomic, strong) VideoView *localView;
@property (nonatomic, strong) VideoView *remoteView;
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;
@property (nonatomic, copy) NSString *channelName;
@property (nonatomic, assign) BOOL isJoined;
@property (nonatomic, strong) AgoraRtcChannelMediaOptions *channelMediaOption;
@end

@implementation MultipathViewController

- (VideoView *)localView {
    if (_localView == nil) {
        _localView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeLocal audioOnly:NO];
    }
    return _localView;
}

- (VideoView *)remoteView {
    if (_remoteView == nil) {
        _remoteView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeRemote audioOnly:NO];
    }
    return _remoteView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    NSInteger roleIndex = [[self.configs objectForKey:@"role_index"] integerValue];
    NSInteger modeIndex = [[self.configs objectForKey:@"mode_index"] integerValue];
    self.channelName = channelName;
    self.modeLabel.text = (modeIndex == 0) ? @"dynamic" : @"duplicate";
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:[NSString stringWithFormat:@"%@\n%@", channelName, @"Remote Host".localized]];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    // set up agora instance when view loaded
    self.channelMediaOption = [[AgoraRtcChannelMediaOptions alloc] init];
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    // make myself a broadcaster
    [self.agoraKit setClientRole:(roleIndex == 0) ? AgoraClientRoleBroadcaster : AgoraClientRoleAudience];
    
    if (roleIndex == 0) {
        // enable video module and set up video encoding configs
        [self.agoraKit enableAudio];
        [self.agoraKit enableVideo];
        
        AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(640, 360)
                                                                                                   frameRate:(AgoraVideoFrameRateFps15)
                                                                                                     bitrate:AgoraVideoBitrateStandard
                                                                                             orientationMode:(AgoraVideoOutputOrientationModeAdaptative)
                                                                                                  mirrorMode:(AgoraVideoMirrorModeAuto)];
        [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
        
        // set up local video to render your local camera preview
        AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
        videoCanvas.uid = 0;
        videoCanvas.view = self.localView.videoView;
        videoCanvas.renderMode = AgoraVideoRenderModeHidden;
        [self.agoraKit setupLocalVideo:videoCanvas];
        [self.agoraKit startPreview];
    }
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    // start joining channel
    self.channelMediaOption.autoSubscribeAudio = YES;
    self.channelMediaOption.autoSubscribeVideo = YES;
    self.channelMediaOption.publishCameraTrack = (roleIndex == 0);
    self.channelMediaOption.publishMicrophoneTrack = (roleIndex == 0);
    self.channelMediaOption.clientRoleType = (roleIndex == 0) ? AgoraClientRoleBroadcaster : AgoraClientRoleAudience;
    self.channelMediaOption.enableMultipath = YES;
    self.channelMediaOption.uplinkMultipathMode = (modeIndex == 0) ? AgoraMultipathModeDynamic : AgoraMultipathModeDuplicate;
    self.channelMediaOption.downlinkMultipathMode = (modeIndex == 0) ? AgoraMultipathModeDynamic : AgoraMultipathModeDuplicate;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:self.channelMediaOption joinSuccess:nil];
        if (result != 0) {
            [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"Join channel failed: %d, please check your params", result]];
        }
    }];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    if (self.isJoined) {
        [self.agoraKit stopPreview];
        [self.agoraKit leaveChannel:nil];
    }
    [AgoraRtcEngineKit destroy];
}

- (IBAction)onClickMultipathSwitch:(UISwitch *)sender {
    self.channelMediaOption.enableMultipath = sender.isOn;
    [self.agoraKit updateChannelWithMediaOptions:self.channelMediaOption];
}

#pragma mark - AgoraRtcEngineDelegate

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurWarning:(AgoraWarningCode)warningCode {
    [LogUtil log:[NSString stringWithFormat:@"Warning: %ld", warningCode] level:LogLevelInfo];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    [LogUtil log:[NSString stringWithFormat:@"Error %ld occur",errorCode] level:(LogLevelError)];
    [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"Error occurred: %ld", errorCode]];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    self.isJoined = YES;
    [LogUtil log:[NSString stringWithFormat:@"Join %@ with uid %lu elapsed %ldms", channel, uid, elapsed] level:(LogLevelDebug)];
    self.localView.uid = uid;
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"remote user join: %lu %ldms", uid, elapsed] level:(LogLevelDebug)];
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    videoCanvas.view = self.remoteView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = uid;
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    videoCanvas.view = nil;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = 0;
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu reason %ld", uid, reason] level:LogLevelDebug];
}

- (void)rtcEngine:(AgoraRtcEngineKit* _Nonnull)engine connectionChangedToState:(AgoraConnectionState)state reason:(AgoraConnectionChangedReason)reason {

    [LogUtil log:[NSString stringWithFormat:@"Connection state changed: %ld %ld", state, reason] level:LogLevelInfo];
}

@end 
