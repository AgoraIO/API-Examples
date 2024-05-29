//
//  LiveStreaming.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/14.
//

#import "LiveStreaming.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "APIExample_OC-swift.h"
#import "VideoView.h"
#import "KeyCenter.h"

@interface LiveStreamingEntry ()
@property (weak, nonatomic) IBOutlet UITextField *channelNameTextField;
@property (weak, nonatomic) IBOutlet UIButton *preloadButton;
@property (nonatomic, assign) BOOL isFirstFrame;
@property (nonatomic, assign) AgoraClientRole role;
@property (nonatomic, assign) UInt32 backgroundColor;

@end

@implementation LiveStreamingEntry

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.preloadButton setTitle:@"preload Channel".localized forState:(UIControlStateNormal)];
    [self.preloadButton setTitle:@"cancel preload".localized forState:(UIControlStateSelected)];
}

- (IBAction)onClickChoseBackgroundColor:(UIButton *)sender {
    [self.view endEditing:YES];
    PickerView *pickerView = [[PickerView alloc] init];
    NSDictionary *colors = @{
        @"Red".localized: @(0xff0d00ff),
        @"Blue".localized: @(0x0400ffff),
        @"Pink".localized: @(0xff006aff),
        @"Purple".localized: @(0xff00d9ff),
        @"Yellow".localized: @(0xeaff00ff)
    };
    pickerView.dataArray = colors.allKeys;
    pickerView.pickerViewSelectedValueClosure = ^(NSString * key) {
        [sender setTitle:key forState:(UIControlStateNormal)];
        NSNumber *number = [colors objectForKey:key];
        self.backgroundColor = number.unsignedIntValue;
    };
    [pickerView show];
}
- (IBAction)onClickFirstFrameSwitch:(UISwitch *)sender {
    if (sender.isOn) {
        UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"After this function is enabled, it cannot be disabled and takes effect only when both the primary and secondary ends are enabled".localized message:nil preferredStyle:(UIAlertControllerStyleAlert)];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:@"Sure".localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            self.isFirstFrame = YES;
        }];
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:^(UIAlertAction * _Nonnull action) {
            self.isFirstFrame = NO;
        }];
        [alertVC addAction:ok];
        [alertVC addAction:cancel];
//        [self presentViewController:alertVC animated:YES completion:nil];
        [self presentAlertViewController:alertVC];
    } else {
        self.isFirstFrame = NO;
    }
}

- (IBAction)onClickPoreloadChannel:(UIButton *)sender {
    if (self.channelNameTextField.text.length <= 0) {
        [ToastView showWithText:@"please input channel name!".localized postion:ToastViewPostionCenter];
        return;
    }
    [sender setSelected:!sender.isSelected];
}
- (IBAction)onClickJoinButton:(id)sender {
    [self.channelNameTextField resignFirstResponder];
    if (self.channelNameTextField.text.length <= 0) {
        return;
    }
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Pick Role".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    UIAlertAction *broadcaster = [UIAlertAction actionWithTitle:@"Broadcaster".localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        self.role = AgoraClientRoleBroadcaster;
        [self doJoin];
    }];
    UIAlertAction *audience = [UIAlertAction actionWithTitle:@"Audience".localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        self.role = AgoraClientRoleAudience;
        [self doJoin];
    }];
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:^(UIAlertAction * _Nonnull action) {
        self.isFirstFrame = NO;
    }];
    [alertVC addAction:broadcaster];
    [alertVC addAction:audience];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
    
}

- (void)doJoin {
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"LiveStreaming" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"LiveStreaming"];
    newViewController.configs = @{@"channelName": self.channelNameTextField.text,
                                  @"role": @(self.role),
                                  @"isFirstFrame": @(self.isFirstFrame),
                                  @"isPreloadChannel": @(self.preloadButton.isSelected),
                                  @"backgroundColor": @(self.backgroundColor)
    };
    [self.navigationController pushViewController:newViewController animated:YES];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    
    [self.view endEditing:YES];
}

@end


@interface LiveStreaming ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *backgroundVideoContainer;
@property (weak, nonatomic) IBOutlet UIView *foregroundVideoContainer;
@property (weak, nonatomic) IBOutlet UISegmentedControl *codingSegment;
@property (weak, nonatomic) IBOutlet UIView *bFrameContainer;
@property (weak, nonatomic) IBOutlet UIView *clientRoleToggleView;
@property (weak, nonatomic) IBOutlet UISwitch *clientRoleToggle;
@property (weak, nonatomic) IBOutlet UIView *watarMarkContainer;
@property (weak, nonatomic) IBOutlet UIView *dualStreamContainer;
@property (weak, nonatomic) IBOutlet UILabel *dualStreamTipsLabel;
@property (weak, nonatomic) IBOutlet UIButton *takeSnapshot;
@property (weak, nonatomic) IBOutlet UIView *ultraLowLatencyToggleView;
@property (weak, nonatomic) IBOutlet UISwitch *ultraLowLatencyToggle;

@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, assign) NSUInteger remoteUid;
@property (nonatomic, assign) AgoraClientRole role;
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;
@property (nonatomic, assign) BOOL isLocalVideoForeground;
@property (nonatomic, assign) BOOL isUltraLowLatencyOn;

@end

@implementation LiveStreaming
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

- (void)setRemoteUid:(NSUInteger)remoteUid {
    _remoteUid = remoteUid;
    [self.foregroundVideoContainer setHidden:!(self.role == AgoraClientRoleBroadcaster && remoteUid != 0)];
}

- (void)setRole:(AgoraClientRole)role {
    _role = role;
    [self.foregroundVideoContainer setHidden:!(self.role == AgoraClientRoleBroadcaster && self.remoteUid != 0)];
    [self.ultraLowLatencyToggle setEnabled:self.role == AgoraClientRoleAudience];
    [self.watarMarkContainer setHidden:self.role == AgoraClientRoleAudience];
    [self.bFrameContainer setHidden:self.role == AgoraClientRoleAudience];
    [self.codingSegment setHidden:self.role == AgoraClientRoleAudience];
}

- (void)setIsLocalVideoForeground:(BOOL)isLocalVideoForeground {
    _isLocalVideoForeground = isLocalVideoForeground;
    if (isLocalVideoForeground) {
        [self.localView setPlaceholder:@"Local Host".localized];
        [self.remoteView setPlaceholder:@"Remote Host".localized];
    } else {
        [self.localView setPlaceholder:@"Remote Host".localized];
        [self.remoteView setPlaceholder:@"Local Host".localized];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];

    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapForegroundVideo)];
    [self.foregroundVideoContainer addGestureRecognizer:tap];
    
    [self.foregroundVideoContainer addSubview:self.localView];
    [self.backgroundVideoContainer addSubview:self.remoteView];
    self.localView.translatesAutoresizingMaskIntoConstraints = NO;
    self.remoteView.translatesAutoresizingMaskIntoConstraints = NO;
    [[self.localView.leadingAnchor constraintEqualToAnchor:self.foregroundVideoContainer.leadingAnchor]setActive:YES];
    [[self.localView.topAnchor constraintEqualToAnchor:self.foregroundVideoContainer.topAnchor]setActive:YES];
    [[self.localView.trailingAnchor constraintEqualToAnchor:self.foregroundVideoContainer.trailingAnchor]setActive:YES];
    [[self.localView.bottomAnchor constraintEqualToAnchor:self.foregroundVideoContainer.bottomAnchor]setActive:YES];
    
    [[self.remoteView.leadingAnchor constraintEqualToAnchor:self.backgroundVideoContainer.leadingAnchor]setActive:YES];
    [[self.remoteView.topAnchor constraintEqualToAnchor:self.backgroundVideoContainer.topAnchor]setActive:YES];
    [[self.remoteView.trailingAnchor constraintEqualToAnchor:self.backgroundVideoContainer.trailingAnchor]setActive:YES];
    [[self.remoteView.bottomAnchor constraintEqualToAnchor:self.backgroundVideoContainer.bottomAnchor]setActive:YES];
    
    // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    BOOL isFirstFrame = [((NSNumber *)[self.configs objectForKey:@"isFirstFrame"])boolValue];
    if (isFirstFrame) {
        [self.agoraKit enableInstantMediaRendering];
        [self.agoraKit startMediaRenderingTracing];
    }
    // get channel name from configs
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    self.role = [((NSNumber *)[self.configs objectForKey:@"role"])integerValue];
    // for audience put local video in foreground
    self.isLocalVideoForeground = self.role == AgoraClientRoleAudience;
    // if inital role is broadcaster, do not show audience options
    [self.clientRoleToggleView setHidden:self.role == AgoraClientRoleBroadcaster];
    [self.ultraLowLatencyToggleView setHidden:self.role == AgoraClientRoleBroadcaster];
    
    // make this room live broadcasting room
    [self updateClientRole: self.role];
    
    // enable video module and set up video encoding configs
    [self.agoraKit enableVideo];
    
    // Set audio route to speaker
    [self.agoraKit setDefaultAudioRouteToSpeakerphone:YES];
    
    // start joining channel
    // 1. Users can only see each other after they join the
    // same channel successfully using the same app id.
    // 2. If app certificate is turned on at dashboard, token is needed
    // when joining channel. The channel name and uid used to calculate
    // the token has to match the ones used for channel join
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.autoSubscribeAudio = YES;
    options.autoSubscribeVideo = YES;
    options.publishCameraTrack = self.role == AgoraClientRoleBroadcaster;
    options.publishMicrophoneTrack = self.role == AgoraClientRoleBroadcaster;
    options.clientRoleType = self.role;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
        BOOL preloadChannel = [((NSNumber *)[self.configs objectForKey:@"isPreloadChannel"])boolValue];
        if (preloadChannel) {
            [self.agoraKit preloadChannelByToken:token channelId:channelName uid:0];
        }
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:options joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

- (void)updateClientRole: (AgoraClientRole)role {
    self.role = role;
    if (role == AgoraClientRoleBroadcaster) {
        [self becomeBroadcaster];
    } else {
        [self becomeAudience];
    }
    AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
    option.publishCameraTrack = role == AgoraClientRoleBroadcaster;
    option.publishMicrophoneTrack = role == AgoraClientRoleBroadcaster;
    [self.agoraKit updateChannelWithMediaOptions:option];
}

- (void)becomeBroadcaster {
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleBroadcaster)];
    // enable video module and set up video encoding configs
    [self.agoraKit enableAudio];
    [self.agoraKit enableVideo];
    
    AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(960, 540)
                                                                                               frameRate:(AgoraVideoFrameRateFps15)
                                                                                                 bitrate:15
                                                                                         orientationMode:(AgoraVideoOutputOrientationModeFixedPortrait)
                                                                                              mirrorMode:(AgoraVideoMirrorModeAuto)];
    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = [self localVideoCanvas];
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupLocalVideo:videoCanvas];
    // you have to call startPreview to see local video
    [self.agoraKit startPreview];
}

- (void)becomeAudience {
    // unbind view
    [self.agoraKit setupLocalVideo:nil];
    // You have to provide client role options if set to audience
    AgoraClientRoleOptions *options = [[AgoraClientRoleOptions alloc]init];
    options.audienceLatencyLevel = self.isUltraLowLatencyOn ? AgoraAudienceLatencyLevelUltraLowLatency : AgoraAudienceLatencyLevelLowLatency;
    [self.agoraKit setClientRole:(AgoraClientRoleAudience) options:options];
}

- (UIView *)localVideoCanvas {
    return self.isLocalVideoForeground ? self.localView.videoView : self.remoteView.videoView;
}

- (UIView *)remoteVideoCanvas {
    return self.isLocalVideoForeground ? self.remoteView.videoView : self.localView.videoView;
}

- (void)onTapForegroundVideo {
    self.isLocalVideoForeground = !self.isLocalVideoForeground;
    AgoraRtcVideoCanvas *localVideoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    localVideoCanvas.uid = 0;
    localVideoCanvas.renderMode = AgoraVideoRenderModeHidden;
    localVideoCanvas.view = [self localVideoCanvas];
    
    AgoraRtcVideoCanvas *remoteVideoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    remoteVideoCanvas.renderMode = AgoraVideoRenderModeHidden;
    remoteVideoCanvas.view = [self remoteVideoCanvas];
    
    [self.agoraKit setupLocalVideo:localVideoCanvas];
    if (self.remoteUid > 0) {
        remoteVideoCanvas.uid = self.remoteUid;
        [self.agoraKit setupRemoteVideo:remoteVideoCanvas];
    }
}

- (IBAction)onTapCodingSegment:(UISegmentedControl *)sender {
    AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] init];
    AgoraAdvancedVideoOptions *advancedOptions = [[AgoraAdvancedVideoOptions alloc] init];
    switch (sender.selectedSegmentIndex) {
        case 0:
            advancedOptions.encodingPreference = AgoraEncodingPreferAuto;
            break;
            
        case 1:
            advancedOptions.encodingPreference = AgoraEncodingPrefersoftware;
            break;
            
        case 2:
            advancedOptions.encodingPreference = AgoraEncodingPreferhardware;
            break;
        default: break;
    }
    encoderConfig.advancedVideoOptions = advancedOptions;
    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
}

- (IBAction)onTapBFrameSwitch:(UISwitch *)sender {
    AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] init];
    AgoraAdvancedVideoOptions *advancedOptions = [[AgoraAdvancedVideoOptions alloc] init];
    advancedOptions.compressionPreference = sender.isOn ? AgoraCompressionQuality : AgoraCompressionLowLatency;
    encoderConfig.advancedVideoOptions = advancedOptions;
    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
}

- (IBAction)onToggleClientRole:(UISwitch *)sender {
    AgoraClientRole role = sender.isOn ? AgoraClientRoleBroadcaster : AgoraClientRoleAudience;
    [self updateClientRole:role];
}

- (IBAction)onTapWatermarkSwitch:(UISwitch *)sender {
    if (sender.isOn) {
        NSString *path = [[NSBundle mainBundle] pathForResource:@"agora-logo" ofType:@"png"];
        NSURL *url = [NSURL URLWithString:path];
        WatermarkOptions *waterMark = [[WatermarkOptions alloc] init];
        waterMark.visibleInPreview = YES;
        waterMark.positionInPortraitMode = CGRectMake(10, 80, 60, 60);
        waterMark.positionInLandscapeMode = CGRectMake(10, 80, 60, 60);
        [self.agoraKit addVideoWatermark:url options:waterMark];
    } else {
        [self.agoraKit clearVideoWatermarks];
    }
}

- (IBAction)onTapDualStreamSwitch:(UISwitch *)sender {
    [self.agoraKit setDualStreamMode:sender.isOn ? AgoraEnableSimulcastStream : AgoraDisableSimulcastStream];
    self.dualStreamTipsLabel.text = sender.isOn ? @"已开启" : @"默认: 大流";
}

- (IBAction)onTakeSnapshot:(UIButton *)sender {
    if (self.remoteUid <= 0) {
        [self showAlertWithTitle:@"remote user has not joined, and cannot take a screenshot".localized];
        return;
    }
    NSString *path = [NSTemporaryDirectory() stringByAppendingString:@"1.png"];
    [self.agoraKit takeSnapshot:self.remoteUid filePath:path];
    [self showAlertWithTitle:@"Screenshot successful".localized message:path];
}

- (IBAction)onToggleUltraLowLatency:(UISwitch *)sender {
    if (self.role == AgoraClientRoleAudience) {
        self.isUltraLowLatencyOn = sender.isOn;
        [self updateClientRole:AgoraClientRoleAudience];
    }
}

/// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
/// to let user know something wrong is happening
/// Error code description can be found at:
/// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
/// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
/// @param errorCode error code of the problem
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    [LogUtil log:[NSString stringWithFormat:@"Error %ld occur",errorCode] level:(LogLevelError)];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"Join %@ with uid %lu elapsed %ldms", channel, uid, elapsed] level:(LogLevelDebug)];
    self.localView.uid = uid;
}

/// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param elapsed time elapse since current sdk instance join the channel in ms
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"remote user join: %lu %ldms", uid, elapsed] level:(LogLevelDebug)];
    // Only one remote video view is available for this
    // tutorial. Here we check if there exists a surface
    // view tagged as this uid.
    UInt32 backgroundColor = [((NSNumber *)[self.configs objectForKey:@"backgroundColor"])unsignedIntValue];
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = [self remoteVideoCanvas];
    videoCanvas.renderMode = AgoraVideoRenderModeFit;
    videoCanvas.backgroundColor = backgroundColor;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = uid;
    self.remoteUid = uid;
}

/// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param reason reason why this user left, note this event may be triggered when the remote user
/// become an audience in live broadcasting profile
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    // to unlink your view from sdk, so that your view reference will be released
    // note the video will stay at its last frame, to completely remove it
    // you will need to remove the EAGL sublayer from your binded view
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = nil;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = 0;
    if (self.remoteUid == uid) {
        self.remoteUid = 0;
    }
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine videoRenderingTracingResultOfUid:(NSUInteger)uid currentEvent:(AgoraMediaTraceEvent)currentEvent tracingInfo:(AgoraVideoRenderingTracingInfo *)tracingInfo {
    NSString *stats = [NSString stringWithFormat:@"firstFrameTime: %ld \n join2Success: %ld",tracingInfo.elapsedTime, tracingInfo.join2JoinSuccess];
    [self.remoteView setStats:stats];
}

@end
