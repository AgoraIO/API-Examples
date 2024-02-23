//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "JoinChannelVideoRecorder.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface JoinChannelVideoRecorderEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation JoinChannelVideoRecorderEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"JoinChannelVideoRecorder" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"JoinChannelVideoRecorder"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface JoinChannelVideoRecorder ()<AgoraRtcEngineDelegate, AgoraMediaRecorderDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)UIButton *localRecorderButton;
@property (nonatomic, strong)UIButton *remoteRecorderButton;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong)AgoraMediaRecorder *localRecorder;
@property (nonatomic, strong)AgoraMediaRecorder *remoteRecorder;
@property (nonatomic, assign)NSUInteger localUid;
@property (nonatomic, assign)NSUInteger remoteUid;

@end

@implementation JoinChannelVideoRecorder

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
- (UIButton *)localRecorderButton {
    if (_localRecorderButton == nil) {
        _localRecorderButton = [[UIButton alloc]init];
        _localRecorderButton.translatesAutoresizingMaskIntoConstraints = NO;
        [_localRecorderButton setTitle:@"Recording".localized forState:(UIControlStateNormal)];
        [_localRecorderButton setTitle:@"Stop Recording".localized forState:(UIControlStateSelected)];
        [_localRecorderButton setTitleColor:[UIColor redColor] forState:(UIControlStateNormal)];
        [_localRecorderButton setTitleColor:[UIColor redColor] forState:(UIControlStateSelected)];
        _localRecorderButton.titleLabel.font = [UIFont systemFontOfSize:14];
        _localRecorderButton.layer.borderColor = [UIColor redColor].CGColor;
        _localRecorderButton.layer.borderWidth = 1;
        _localRecorderButton.layer.cornerRadius = 3;
        [_localRecorderButton addTarget:self action:@selector(onTapLocalRecorderButton:) forControlEvents:(UIControlEventTouchUpInside)];
    }
    return _localRecorderButton;
}
- (UIButton *)remoteRecorderButton {
    if (_remoteRecorderButton == nil) {
        _remoteRecorderButton = [[UIButton alloc]init];
        _remoteRecorderButton.translatesAutoresizingMaskIntoConstraints = NO;
        [_remoteRecorderButton setTitle:@"Recording".localized forState:(UIControlStateNormal)];
        [_remoteRecorderButton setTitle:@"Stop Recording".localized forState:(UIControlStateSelected)];
        [_remoteRecorderButton setTitleColor:[UIColor redColor] forState:(UIControlStateNormal)];
        [_remoteRecorderButton setTitleColor:[UIColor redColor] forState:(UIControlStateSelected)];
        _remoteRecorderButton.titleLabel.font = [UIFont systemFontOfSize:14];
        _remoteRecorderButton.layer.borderColor = [UIColor redColor].CGColor;
        _remoteRecorderButton.layer.borderWidth = 1;
        _remoteRecorderButton.layer.cornerRadius = 3;
        [_remoteRecorderButton setHidden:YES];
        [_remoteRecorderButton addTarget:self action:@selector(onTapRemoteRecorderButton:) forControlEvents:(UIControlEventTouchUpInside)];
    }
    return _remoteRecorderButton;
}
- (AgoraMediaRecorder *)localRecorder {
    if (_localRecorder == nil) {
        AgoraRecorderStreamInfo *streamInfo = [[AgoraRecorderStreamInfo alloc] init];
        streamInfo.channelId = self.title;
        streamInfo.uid = self.localUid;
        _localRecorder = [self.agoraKit createMediaRecorder:streamInfo];
        [_localRecorder setMediaRecorderDelegate:self];
    }
    return _localRecorder;
}
- (AgoraMediaRecorder *)remoteRecorder {
    if (_remoteRecorder == nil) {
        AgoraRecorderStreamInfo *streamInfo = [[AgoraRecorderStreamInfo alloc] init];
        streamInfo.channelId = self.title;
        streamInfo.uid = self.remoteUid;
        _remoteRecorder = [self.agoraKit createMediaRecorder:streamInfo];
        [_remoteRecorder setMediaRecorderDelegate:self];
    }
    return _remoteRecorder;
}
- (NSString *)storagePath {
    NSString *path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    return path;
}
- (NSUInteger)localUid {
    if (_localUid <= 0) {
        _localUid = arc4random() % 9999999;
    }
    return _localUid;
}
- (void)setRemoteUid:(NSUInteger)remoteUid {
    _remoteUid = remoteUid;
    [self.remoteRecorderButton setHidden:remoteUid == 0];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    [self.localView addSubview:self.localRecorderButton];
    [self.remoteView addSubview:self.remoteRecorderButton];
    [[self.localRecorderButton.bottomAnchor constraintEqualToAnchor:self.localView.bottomAnchor constant:-10] setActive:YES];
    [[self.localRecorderButton.trailingAnchor constraintEqualToAnchor:self.localView.trailingAnchor constant:-10] setActive:YES];
    [[self.localRecorderButton.widthAnchor constraintEqualToConstant:70] setActive:YES];
    [[self.localRecorderButton.heightAnchor constraintEqualToConstant:30] setActive:YES];
    
    [[self.remoteRecorderButton.bottomAnchor constraintEqualToAnchor:self.remoteView.bottomAnchor constant:-10] setActive:YES];
    [[self.remoteRecorderButton.trailingAnchor constraintEqualToAnchor:self.remoteView.trailingAnchor constant:-10] setActive:YES];
    [[self.remoteRecorderButton.widthAnchor constraintEqualToConstant:70] setActive:YES];
    [[self.remoteRecorderButton.heightAnchor constraintEqualToConstant:30] setActive:YES];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    self.title = channelName;
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
    videoCanvas.view = self.localView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupLocalVideo:videoCanvas];
    // you have to call startPreview to see local video
    [self.agoraKit startPreview];
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    // start joining channel
    // 1. Users can only see each other after they join the
    // same channel successfully using the same app id.
    // 2. If app certificate is turned on at dashboard, token is needed
    // when joining channel. The channel name and uid used to calculate
    // the token has to match the ones used for channel join
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.autoSubscribeAudio = YES;
    options.autoSubscribeVideo = YES;
    options.publishCameraTrack = YES;
    options.publishMicrophoneTrack = YES;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:self.localUid success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:self.localUid mediaOptions:options joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
}

- (void)onTapLocalRecorderButton: (UIButton *)sender {
    [sender setSelected:!sender.isSelected];
    NSString *path = [NSString stringWithFormat:@"%@/%lu.mp4",[self storagePath],self.localUid];
    if (sender.isSelected) {
        AgoraMediaRecorderConfiguration *config = [[AgoraMediaRecorderConfiguration alloc] init];
        config.storagePath = path;
        config.containerFormat = AgoraMediaRecorderContainerFormatMP4;
        config.maxDurationMs = 10 * 1000;
        [self.localRecorder startRecording:config];
    } else {
        [self.localRecorder stopRecording];
        [ToastView showWithText:path postion:ToastViewPostionCenter];
    }
}
- (void)onTapRemoteRecorderButton: (UIButton *)sender {
    [sender setSelected:!sender.isSelected];
    NSString *path = [NSString stringWithFormat:@"%@/%lu.mp4",[self storagePath],self.remoteUid];
    if (sender.isSelected) {
        AgoraMediaRecorderConfiguration *config = [[AgoraMediaRecorderConfiguration alloc] init];
        config.storagePath = path;
        config.containerFormat = AgoraMediaRecorderContainerFormatMP4;
        config.maxDurationMs = 10 * 1000;
        [self.remoteRecorder startRecording:config];
    } else {
        [self.remoteRecorder stopRecording];
        [ToastView showWithText:path postion:ToastViewPostionCenter];
    }
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.localRecorder stopRecording];
    [self.remoteRecorder stopRecording];
    [self.agoraKit destroyMediaRecorder:self.localRecorder];
    [self.agoraKit destroyMediaRecorder:self.remoteRecorder];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

#pragma MARK: MediaRecorderDelegate
- (void)mediaRecorder:(AgoraMediaRecorder *)recorder informationDidUpdated:(NSString *)channelId uid:(NSUInteger)uid info:(AgoraMediaRecorderInfo *)info {
    NSLog(@"uid == %lu info == %@", uid, info.description);
}

- (void)mediaRecorder:(AgoraMediaRecorder * _Nonnull)recorder stateDidChanged:(NSString * _Nonnull)channelId uid:(NSUInteger)uid state:(AgoraMediaRecorderState)state reason:(AgoraMediaRecorderReasonCode)reason { 
    [LogUtil log: [NSString stringWithFormat:@"stateDidChanged uid == %lu state == %ld reason == %ld", uid, state, reason]];
}


- (void)mediaRecorder:(AgoraMediaRecorder *)recorder stateDidChanged:(NSString *)channelId uid:(NSUInteger)uid state:(AgoraMediaRecorderState)state {
    NSLog(@"uid == %lu state == %ld", uid, state);
}

#pragma MARK: RTCDelegate
/// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
/// to let user know something wrong is happening
/// Error code description can be found at:
/// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
/// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
/// @param errorCode error code of the problem
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    NSLog(@"Error %ld occur",errorCode);
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString *)channel withUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    NSLog(@"Join %@ with uid %lu elapsed %ldms", channel, uid, elapsed);
    self.localView.uid = uid;
}

/// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param elapsed time elapse since current sdk instance join the channel in ms
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    NSLog(@"remote user join: %lu %ldms", uid, elapsed);
    // Only one remote video view is available for this
    // tutorial. Here we check if there exists a surface
    // view tagged as this uid.
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = self.remoteView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
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
    self.remoteUid = 0;
}

@end
