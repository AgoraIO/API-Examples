//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "MediaPlayer.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"
#import "UIView+CSshortFrame.h"

@interface MediaPlayerEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation MediaPlayerEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"MediaPlayer" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"MediaPlayer"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface MediaPlayer ()<AgoraRtcEngineDelegate, UITextFieldDelegate, AgoraRtcMediaPlayerDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, weak) IBOutlet UITextField *mediaUrlField;
@property (nonatomic, weak) IBOutlet UIStackView *playerControlStack;
@property (nonatomic, weak) IBOutlet UISlider *playerProgressSlider;
@property (nonatomic, weak) IBOutlet UISlider *playoutVolume;
@property (nonatomic, weak) IBOutlet UISlider *publishVolume;
@property (nonatomic, weak) IBOutlet UILabel *playerDurationLabel;

@property (nonatomic, weak) id<AgoraRtcMediaPlayerProtocol>mediaPlayerKit;
@property (nonatomic, assign) CGFloat originY;
@property (nonatomic, assign) NSUInteger PLAYER_UID;
@property (nonatomic, assign) NSUInteger CAMERA_UID;
@end

@implementation MediaPlayer
- (NSUInteger)PLAYER_UID {
    if (_PLAYER_UID <= 0) {
        _PLAYER_UID = arc4random() % 99999;
    }
    return _PLAYER_UID;
}
- (NSUInteger)CAMERA_UID {
    if (_CAMERA_UID <= 0) {
        _CAMERA_UID = arc4random() % 99999;
    }
    return _CAMERA_UID;
}
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

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

- (void)keyboardWillAppear: (NSNotification *)noti {
    NSDictionary *keyboardInfo = [noti.userInfo objectForKey:UIKeyboardFrameEndUserInfoKey];
    CGFloat keyboardHeight = ((NSValue *)keyboardInfo).CGRectValue.size.height;
    if (self.originY == 0) {
        self.originY = self.view.centerY_CS;
    }
    CGRect rect = [self.mediaUrlField convertRect:self.mediaUrlField.bounds toView:self.view];
    CGFloat y = self.view.bounds.size.height - rect.origin.y - self.mediaUrlField.bounds.size.height - keyboardHeight;
    if (y < 0) {
        UIViewPropertyAnimator *animator = [[UIViewPropertyAnimator alloc] initWithDuration:0.25 curve:(UIViewAnimationCurveEaseOut) animations:^{
            self.view.centerY_CS = y + self.originY;
        }];
        [animator startAnimation];
    }
}
- (void)keyboardWillDisappear: (NSNotification *)noti {
    UIViewPropertyAnimator *animator = [[UIViewPropertyAnimator alloc] initWithDuration:0.25 curve:(UIViewAnimationCurveEaseOut) animations:^{
        self.view.centerY_CS = self.originY;
    }];
    [animator startAnimation];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mediaUrlField.delegate = self;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillAppear:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillDisappear:) name:UIKeyboardWillHideNotification object:nil];
    
    // layout render view
    [self.localView setPlaceholder:@"No Player Loaded".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
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
    
    // prepare media player
    self.mediaPlayerKit = [self.agoraKit createMediaPlayerWithDelegate:self];
    [self.mediaPlayerKit setView:self.localView.videoView];
    [self.mediaPlayerKit setRenderMode:(AgoraMediaPlayerRenderModeFit)];
    
    self.playoutVolume.minimumValue = 0;
    self.playoutVolume.maximumValue = 400;
    [self.playoutVolume setValue:[self.mediaPlayerKit getPlayoutVolume]];
    
    self.publishVolume.minimumValue = 0;
    self.publishVolume.maximumValue = 100;
    [self.mediaPlayerKit adjustPublishSignalVolume:50];
    [self.publishVolume setValue:50];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    videoCanvas.sourceType = AgoraVideoSourceTypeMediaPlayer;
    videoCanvas.mediaPlayerId = self.mediaPlayerKit.getMediaPlayerId;
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
    options.publishCustomAudioTrack = YES;
    options.publishCameraTrack = YES;
    options.publishMicrophoneTrack = YES;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:self.CAMERA_UID success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:self.CAMERA_UID mediaOptions:options joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
    [self.agoraKit muteRemoteAudioStream:self.PLAYER_UID mute:YES];
    
    
    AgoraRtcChannelMediaOptions *options1 = [[AgoraRtcChannelMediaOptions alloc] init];
    options1.autoSubscribeAudio = NO;
    options1.autoSubscribeVideo = NO;
    options1.clientRoleType = AgoraClientRoleBroadcaster;
    options1.enableAudioRecordingOrPlayout = NO;
    
    AgoraRtcConnection *connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.PLAYER_UID];
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:self.PLAYER_UID success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelExByToken:token connection:connection delegate:self mediaOptions:options1 joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
        [self doOpenMediaUrlWithSender: nil];
    }];
}

- (IBAction)doOpenMediaUrlWithSender:(UIButton *)sender {
    [self.mediaUrlField resignFirstResponder];
    [self.mediaPlayerKit open:self.mediaUrlField.text startPos:0];
}
- (IBAction)doPlayWithSender:(UIButton *)sender {
    [self.mediaPlayerKit play];
}
- (IBAction)doStopWithSender:(UIButton *)sender {
    [self.mediaPlayerKit stop];
}
- (IBAction)doPauseWithSender:(UIButton *)sender {
    [self.mediaPlayerKit pause];
}
- (IBAction)doPublishWithSender:(UIButton *)sender {
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.publishMediaPlayerVideoTrack = YES;
    options.publishMediaPlayerAudioTrack = YES;
    options.publishMediaPlayerId = self.mediaPlayerKit.getMediaPlayerId;
    options.publishCameraTrack = NO;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    AgoraRtcConnection *connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.PLAYER_UID];
    [self.agoraKit updateChannelExWithMediaOptions:options connection:connection];
}
- (IBAction)dloStopPublish:(UIButton *)sender {
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.publishMediaPlayerVideoTrack = NO;
    options.publishMediaPlayerAudioTrack = NO;
    options.publishMediaPlayerId = self.mediaPlayerKit.getMediaPlayerId;
    options.publishCameraTrack = NO;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    AgoraRtcConnection *connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.PLAYER_UID];
    [self.agoraKit updateChannelExWithMediaOptions:options connection:connection];
}
- (IBAction)doAdjustPlayoutVolumeWithSender:(UISlider *)sender {
    [self.mediaPlayerKit adjustPlayoutVolume:sender.value];
}
- (IBAction)doAdjustPublishVolumeWithSender:(UISlider *)sender {
    [self.mediaPlayerKit adjustPublishSignalVolume:sender.value];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit leaveChannel:nil];
    [self.agoraKit destroyMediaPlayer:self.mediaPlayerKit];
    [AgoraRtcEngineKit destroy];
}

- (void)AgoraRtcMediaPlayer:(id<AgoraRtcMediaPlayerProtocol>)playerKit didChangedToState:(AgoraMediaPlayerState)state reason:(AgoraMediaPlayerReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"player rtc channel publish helper state changed to: %ld error: %ld", state, reason]];
    __weak MediaPlayer *weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        switch (state) {
            case AgoraMediaPlayerStateFailed:
                [weakSelf showAlertWithTitle:[NSString stringWithFormat:@"media player error: %ld", reason]];
                break;
            
            case AgoraMediaPlayerStateOpenCompleted:
            {
                NSInteger duation = playerKit.getDuration;
                [weakSelf.playerControlStack setHidden:NO];
                weakSelf.playerDurationLabel.text = [NSString stringWithFormat:@"%02ld : %02ld", duation / 60000, duation % 60000 / 1000];
                [weakSelf.playerProgressSlider setValue:0 animated:YES];
            }
                break;
                
            case AgoraMediaPlayerStateStopped:
            {
                [weakSelf.playerControlStack setHidden:YES];
                [weakSelf.playerProgressSlider setValue:0 animated:YES];
                weakSelf.playerDurationLabel.text = @"00 : 00";
                NSString *channelName = [self.configs objectForKey:@"channelName"];
                AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
                option.publishMediaPlayerVideoTrack = NO;
                option.publishMediaPlayerAudioTrack = NO;
                AgoraRtcConnection *connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.PLAYER_UID];
                [weakSelf.agoraKit updateChannelExWithMediaOptions:option connection:connection];
            }
            default:
                break;
        }
    });
}

- (void)AgoraRtcMediaPlayer:(id<AgoraRtcMediaPlayerProtocol>)playerKit didChangedToPosition:(NSInteger)position {
    NSInteger duration = self.mediaPlayerKit.getDuration;
    float progress = 0;
    long left = 0;
    if (duration > 0) {
        progress = position / duration;
        left = (self.mediaPlayerKit.getDuration - position) / 1000;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        self.playerDurationLabel.text = [NSString stringWithFormat:@"%02ld : %02ld", left/60, left % 60];
        if (self.playerProgressSlider.isTouchInside) {
            [self.playerProgressSlider setValue:progress animated:YES];
        }
    });
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
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = self.remoteView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    self.remoteView.uid = uid;
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
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

@end
