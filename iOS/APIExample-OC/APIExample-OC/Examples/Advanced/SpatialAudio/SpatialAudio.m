//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "SpatialAudio.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface SpatialAudioEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation SpatialAudioEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"SpatialAudio" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"SpatialAudio"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface SpatialAudio ()<AgoraRtcEngineDelegate, AgoraRtcMediaPlayerDelegate>
@property (weak, nonatomic) IBOutlet UILabel *infoLabel;
@property (weak, nonatomic) IBOutlet UIImageView *selfPostionView;
@property (weak, nonatomic) IBOutlet UIView *voiceContainerView1;
@property (weak, nonatomic) IBOutlet UIView *voiceContainerView2;
@property (weak, nonatomic) IBOutlet UIButton *voiceButton1;
@property (weak, nonatomic) IBOutlet UIButton *voiceButton2;
@property (weak, nonatomic) IBOutlet UIButton *remoteUserButton1;
@property (weak, nonatomic) IBOutlet UIButton *remoteUserButton2;

@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong) SpatialAudioActionSheet *actionView1;
@property (nonatomic, strong) SpatialAudioActionSheet *actionView2;
@property (nonatomic, weak) id<AgoraRtcMediaPlayerProtocol>mediaPlayer1;
@property (nonatomic, weak) id<AgoraRtcMediaPlayerProtocol>mediaPlayer2;
@property (nonatomic, strong) AgoraLocalSpatialAudioKit *localSpatial;

@property (nonatomic, assign) CGFloat currentAngle;
@property (nonatomic, assign) CGFloat currentDistance;
@property (nonatomic, assign) CGFloat maxDistance;
@property (nonatomic, assign) simd_float3 forward;
@property (nonatomic, assign) simd_float3 right;
@property (nonatomic, assign) simd_float3 up;

@end

@implementation SpatialAudio
- (SpatialAudioActionSheet *)actionView1 {
    if (_actionView1 == nil) {
        _actionView1 = [[SpatialAudioActionSheet alloc] init];
    }
    return _actionView1;
}
- (SpatialAudioActionSheet *)actionView2 {
    if (_actionView2 == nil) {
        _actionView2 = [[SpatialAudioActionSheet alloc] init];
    }
    return _actionView2;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.currentAngle = 0;
    self.currentDistance = 0;
    self.maxDistance = 10;
    self.forward = simd_make_float3(1.0, 0.0, 0.0);
    self.right = simd_make_float3(0.0, 1.0, 0.0);
    self.up = simd_make_float3(0.0, 0.0, 1.0);
    
    self.infoLabel.text = @"Please move the red icon to experience the 3D audio effect".localized;
    [self.voiceButton1 setTitle:@"" forState:(UIControlStateNormal)];
    [self.voiceButton2 setTitle:@"" forState:(UIControlStateNormal)];
    [self.voiceButton1 setImage:[UIImage imageNamed:@"spatial_sound2"] forState:(UIControlStateNormal)];
    [self.voiceButton2 setImage:[UIImage imageNamed:@"spatial_sound2"] forState:(UIControlStateNormal)];
    [self.voiceContainerView1 setHidden:YES];
    [self.voiceContainerView2 setHidden:YES];
    [self.remoteUserButton1 setHidden:YES];
    [self.remoteUserButton2 setHidden:YES];
    
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGestureRecognizer:)];
    [self.selfPostionView addGestureRecognizer:panGesture];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleBroadcaster)];
    // enable video module and set up video encoding configs
    [self.agoraKit enableAudio];
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    AgoraLocalSpatialAudioConfig * localSpatialConfig = [[AgoraLocalSpatialAudioConfig alloc] init];
    localSpatialConfig.rtcEngine = self.agoraKit;
    self.localSpatial = [AgoraLocalSpatialAudioKit sharedLocalSpatialAudioWithConfig:localSpatialConfig];
    [self.localSpatial setAudioRecvRange:[UIScreen mainScreen].bounds.size.height];
    [self.localSpatial setMaxAudioRecvCount:2];
    [self.localSpatial setDistanceUnit:1];
}

- (void)panGestureRecognizer: (UIPanGestureRecognizer *)gesture {
    CGPoint move = [gesture translationInView:self.view];
    CGPoint objectCenter = gesture.view.center;
    objectCenter = CGPointMake(objectCenter.x + move.x, objectCenter.y + move.y);
    self.selfPostionView.center = objectCenter;
    [gesture setTranslation:CGPointZero inView:self.view];
    if (gesture.state == UIGestureRecognizerStateEnded) {
        [self updatePosition];
    }
}

- (IBAction)onTapVoice1Button:(id)sender {
    [self updateMediaPlayerParams:self.mediaPlayer1 actionView:self.actionView1];
}
- (IBAction)onTapVoice2Button:(id)sender {
    [self updateMediaPlayerParams:self.mediaPlayer2 actionView:self.actionView2];
}
- (IBAction)onTapRemoteUser1Button:(id)sender {
}
- (IBAction)onTapRemoteUser2Button:(id)sender {
}
- (IBAction)onTapMicSwitch:(UISwitch *)sender {
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.publishMicrophoneTrack = sender.isOn;
    [self.agoraKit updateChannelWithMediaOptions:options];
}
- (IBAction)onTapAudioAttenuationSwitch:(UISwitch *)sender {
    [self.voiceContainerView1 setHidden:!sender.isOn];
    if (sender.isOn) {
        AgoraSpatialAudioZone *audioZone = [[AgoraSpatialAudioZone alloc] init];
        audioZone.forwardLength = self.voiceContainerView1.frame.size.width;
        audioZone.rightLength = self.voiceContainerView1.frame.size.height;
        audioZone.upLength = self.maxDistance;
        audioZone.zoneSetId = 1;
        audioZone.audioAttenuation = 1;
        audioZone.forward = self.forward;
        audioZone.right = self.right;
        audioZone.up = self.up;
        audioZone.position = [self getViewCenterPostion:self.voiceContainerView1];
        [self.localSpatial setZones:@[audioZone]];
    } else {
        [self.localSpatial setZones:nil];
    }
    simd_float3 pos = [self getViewCenterPostion:self.selfPostionView];
    [self.localSpatial updateSelfPosition:pos
                              axisForward:self.forward
                                axisRight:self.right
                                   axisUp:self.up];
}


- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    self.mediaPlayer1 = [self.agoraKit createMediaPlayerWithDelegate:self];
    [self.mediaPlayer1 setLoopCount:-1];
    [self.mediaPlayer1 open:@"https://webdemo.agora.io/audiomixing.mp3" startPos:0];
    [self.localSpatial updatePlayerPositionInfo:self.mediaPlayer1.getMediaPlayerId positionInfo:[self getPlayerPostion:self.voiceButton1]];
    [self.localSpatial setPlayerAttenuation:0.2 playerId:self.mediaPlayer1.getMediaPlayerId forceSet:NO];
    
    self.mediaPlayer2 = [self.agoraKit createMediaPlayerWithDelegate:self];
    [self.mediaPlayer2 setLoopCount:-1];
    [self.mediaPlayer2 open:@"https://webdemo.agora.io/dang.mp3" startPos:0];
    [self.localSpatial updatePlayerPositionInfo:self.mediaPlayer2.getMediaPlayerId positionInfo:[self getPlayerPostion:self.voiceButton2]];
    [self.localSpatial setPlayerAttenuation:0.2 playerId:self.mediaPlayer2.getMediaPlayerId forceSet:NO];
    
    [self updatePosition];
    
    [self joinChannel];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit leaveChannel:nil];
    [AgoraLocalSpatialAudioKit destroy];
    [AgoraRtcEngineKit destroy];
}

- (void)joinChannel {
    // start joining channel
    // 1. Users can only see each other after they join the
    // same channel successfully using the same app id.
    // 2. If app certificate is turned on at dashboard, token is needed
    // when joining channel. The channel name and uid used to calculate
    // the token has to match the ones used for channel join
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.autoSubscribeAudio = YES;
    options.autoSubscribeVideo = NO;
    options.publishMicrophoneTrack = YES;
    options.publishCameraTrack = NO;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
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

- (void)updateMediaPlayerParams: (id<AgoraRtcMediaPlayerProtocol>)mediaPlayer actionView: (SpatialAudioActionSheet *)actionView {
    actionView.onTapMuteSwitchClosure = ^(BOOL isPause) {
        isPause ? [mediaPlayer pause] : [mediaPlayer resume];
    };
    actionView.onTapVoiceBlurSwitchClosure = ^(BOOL isOn) {
        AgoraSpatialAudioParams *params = [[AgoraSpatialAudioParams alloc] init];
        params.enable_blur = isOn;
        [mediaPlayer setSpatialAudioParams:params];
    };
    actionView.onTapAirborneSwitchClosure = ^(BOOL isOn) {
        AgoraSpatialAudioParams *params = [[AgoraSpatialAudioParams alloc] init];
        params.enable_air_absorb = isOn;
        [mediaPlayer setSpatialAudioParams:params];
    };
    __weak SpatialAudio *weakSelf = self;
    actionView.onTapAttenuationSilderClosure = ^(float value) {
        [weakSelf.localSpatial setPlayerAttenuation:value playerId:mediaPlayer.getMediaPlayerId forceSet:NO];
    };
    [actionView show];
}

- (void)updatePosition {
    simd_float3 pos = [self getViewCenterPostion:self.selfPostionView];
    [self.localSpatial updateSelfPosition:pos
                              axisForward:self.forward
                                axisRight:self.right
                                   axisUp:self.up];
}

- (AgoraRemoteVoicePositionInfo *)getPlayerPostion: (UIView *)view {
    simd_float3 postion = [self getViewCenterPostion:view];
    AgoraRemoteVoicePositionInfo *postionInfo = [[AgoraRemoteVoicePositionInfo alloc] init];
    postionInfo.position = postion;
    postionInfo.forward = self.forward;
    return postionInfo;
}
- (simd_float3)getViewCenterPostion: (UIView *)view {
    return simd_make_float3(view.center.x, view.center.y, 0.0);
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
}

/// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param elapsed time elapse since current sdk instance join the channel in ms
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [LogUtil log:[NSString stringWithFormat:@"remote user join: %lu %ldms", uid, elapsed] level:(LogLevelDebug)];
    
    if (self.remoteUserButton1.tag <= 0) {
        [self.remoteUserButton1 setTitle:[NSString stringWithFormat:@"%lu",uid] forState:(UIControlStateNormal)];
        self.remoteUserButton1.tag = uid;
        [self.remoteUserButton1 setHidden:NO];
        [self.localSpatial updateRemotePosition:uid positionInfo:[self getPlayerPostion:self.remoteUserButton1]];
    } else if (self.remoteUserButton2.tag <= 0) {
        [self.remoteUserButton2 setTitle:[NSString stringWithFormat:@"%lu",uid] forState:(UIControlStateNormal)];
        self.remoteUserButton2.tag = uid;
        [self.remoteUserButton2 setHidden:NO];
        [self.localSpatial updateRemotePosition:uid positionInfo:[self getPlayerPostion:self.remoteUserButton2]];
    }
}

/// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param reason reason why this user left, note this event may be triggered when the remote user
/// become an audience in live broadcasting profile
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    // to unlink your view from sdk, so that your view reference will be released
    // note the video will stay at its last frame, to completely remove it
    // you will need to remove the EAGL sublayer from your binded view
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
    if (self.remoteUserButton1.tag == uid) {
        [self.remoteUserButton1 setHidden:YES];
        self.remoteUserButton1.tag = 0;
    } else if (self.remoteUserButton2.tag == uid) {
        [self.remoteUserButton2 setHidden:YES];
        self.remoteUserButton2.tag = 0;
    }
    [self.localSpatial removeRemotePosition:uid];
}

- (void)AgoraRtcMediaPlayer:(id<AgoraRtcMediaPlayerProtocol>)playerKit didChangedToState:(AgoraMediaPlayerState)state reason:(AgoraMediaPlayerReason)reason {
    if (state == AgoraMediaPlayerStateOpenCompleted || state == AgoraMediaPlayerStatePlayBackAllLoopsCompleted || state == AgoraMediaPlayerStatePlayBackCompleted) {
        [playerKit play];
    }
}

@end
