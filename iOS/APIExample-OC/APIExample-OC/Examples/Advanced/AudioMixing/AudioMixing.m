//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "AudioMixing.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface AudioMixingEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UIButton *scenarioBtn;
@property (weak, nonatomic) IBOutlet UIButton *profileBtn;

@property (nonatomic, assign) AgoraAudioProfile profile;
@property (nonatomic, assign) AgoraAudioScenario scenario;

@end

@implementation AudioMixingEntry

- (void)viewDidLoad {
    [super viewDidLoad];
    self.profile = AgoraAudioProfileDefault;
    self.scenario = AgoraAudioScenarioDefault;
    [self.profileBtn setTitle:@"Default".localized forState:(UIControlStateNormal)];
    [self.scenarioBtn setTitle:@"Default".localized forState:(UIControlStateNormal)];
}

- (IBAction)setAudioScenario:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Audio Scenario".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Default": @(AgoraAudioScenarioDefault),
        @"Game Streaming": @(AgoraAudioScenarioGameStreaming)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            AgoraAudioScenario mode = ((NSNumber *)params[title]).integerValue;
            self.scenario = mode;
            [sender setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)setAudioProfile:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Audio Profile".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"Default": @(AgoraAudioProfileDefault),
        @"Music Standard": @(AgoraAudioProfileMusicStandard),
        @"Music Standard Stereo": @(AgoraAudioProfileMusicStandardStereo),
        @"Music High Quality": @(AgoraAudioProfileMusicHighQuality),
        @"Music High Quality Stereo": @(AgoraAudioProfileMusicHighQualityStereo),
        @"Speech Standard": @(AgoraAudioProfileSpeechStandard),
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            AgoraAudioProfile profile = ((NSNumber *)params[title]).integerValue;
            self.profile = profile;
            [sender setTitle:title.localized forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"AudioMixing" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"AudioMixing"];
    newViewController.configs = @{@"channelName": self.textField.text,
                                  @"audioProfile": @(self.profile),
                                  @"audioScenario": @(self.scenario)};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface AudioMixing ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;

@property (weak, nonatomic) IBOutlet UISlider *audioMixingVolumeSlider;
@property (weak, nonatomic) IBOutlet UISlider *audioMixingPlaybackVolumeSlider;
@property (weak, nonatomic) IBOutlet UISlider *audioMixingPublishVolumeSlider;
@property (weak, nonatomic) IBOutlet UIProgressView *audioMixingProgressView;
@property (weak, nonatomic) IBOutlet UILabel *audioMixingDuration;
@property (weak, nonatomic) IBOutlet UISlider *audioEffectVolumeSlider;

@property (nonatomic, strong) NSTimer *timer;

@end

@implementation AudioMixing

- (VideoView *)localView {
    if (_localView == nil) {
        _localView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeLocal audioOnly:YES];
    }
    return _localView;
}

- (VideoView *)remoteView {
    if (_remoteView == nil) {
        _remoteView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeRemote audioOnly:YES];
    }
    return _remoteView;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    AgoraAudioProfile audioProfile = ((NSNumber *)[self.configs objectForKey:@"audioProfile"]).integerValue;
    AgoraAudioScenario audioScenario = ((NSNumber *)[self.configs objectForKey:@"audioScenario"]).integerValue;
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    config.audioScenario = audioScenario;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleBroadcaster)];
    // enable video module and set up video encoding configs
    [self.agoraKit enableAudio];
    
    // update slider values
    [self.audioMixingPlaybackVolumeSlider setValue:self.agoraKit.getAudioMixingPlayoutVolume animated:YES];
    [self.audioMixingPublishVolumeSlider setValue:self.agoraKit.getAudioMixingPublishVolume animated:YES];
    [self.audioEffectVolumeSlider setValue:self.agoraKit.getEffectsVolume animated:YES];
    
    // set audio profile/audio scenario
    [self.agoraKit setAudioProfile:audioProfile];
    
    // enable volume indicator
    [self.agoraKit enableAudioVolumeIndication:200 smooth:3 reportVad:NO];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupLocalVideo:videoCanvas];
    
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
    options.autoSubscribeVideo = NO;
    options.publishMicrophoneTrack = YES;
    options.publishCameraTrack = NO;
    options.clientRoleType = AgoraClientRoleBroadcaster;
    
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

- (IBAction)onChangeAudioMixingVolume:(UISlider *)sender {
    [self.agoraKit adjustAudioMixingVolume:sender.value];
}
- (IBAction)onChangeAudioMixingPlaybackVolume:(UISlider *)sender {
    [self.agoraKit adjustAudioMixingPlayoutVolume:sender.value];
}
- (IBAction)onChangeAudioMixingPublishVolume:(UISlider *)sender {
    [self.agoraKit adjustAudioMixingPublishVolume:sender.value];
}
- (IBAction)onChangeAudioEffectVolume:(UISlider *)sender {
    [self.agoraKit setEffectsVolume:sender.value];
}
- (IBAction)onStartAudioMixing:(UIButton *)sender {
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"audiomixing" ofType:@"mp3"];
    int result = [self.agoraKit startAudioMixing:filePath loopback:NO cycle:-1];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"stopAudioMixing call failed: %d, please check your params",result]];
    }
}
- (IBAction)onStopAudioMixing:(UIButton *)sender {
    int result = [self.agoraKit stopAudioMixing];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"stopAudioMixing call failed: %d, please check your params",result]];
    } else {
        [self stopProgressTimer];
        [self updateTotalDuration:YES];
    }
}
- (IBAction)onPauseAudioMixing:(UIButton *)sender {
    int result = [self.agoraKit pauseAudioMixing];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"pauseAudioMixing call failed: %d, please check your params",result]];
    } else {
        [self stopProgressTimer];
    }
}
- (IBAction)onResumeAudioMixing:(UIButton *)sender {
    int result = [self.agoraKit resumeAudioMixing];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"resumeAudioMixing call failed: %d, please check your params",result]];
    } else {
        [self startProgressTimer];
    }
}

- (void)startProgressTimer {
    if (self.timer == nil) {
        __weak AudioMixing *weakSelf = self;
        self.timer = [NSTimer scheduledTimerWithTimeInterval:0.5 repeats:YES block:^(NSTimer * _Nonnull timer) {
            float progress = weakSelf.agoraKit.getAudioMixingCurrentPosition / weakSelf.agoraKit.getAudioMixingDuration;
            [weakSelf.audioMixingProgressView setProgress:progress animated:YES];
        }];
    }
}
- (void)stopProgressTimer {
    if (self.timer != nil) {
        [self.timer invalidate];
        self.timer = nil;
    }
}
- (void)updateTotalDuration: (BOOL)reset {
    if (reset) {
        self.audioMixingDuration.text = @"00 : 00";
    } else {
        int duration = self.agoraKit.getAudioMixingDuration;
        int seconds = duration / 1000;
        self.audioMixingDuration.text = [NSString stringWithFormat:@"%02d : %02d", seconds / 60, seconds % 60];
    }
}
- (IBAction)onPlayEffect:(UIButton *)sender {
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"audioeffect" ofType:@"mp3"];
    int result = [self.agoraKit playEffect:1 filePath:filePath loopCount:-1 pitch:1 pan:0 gain:100 publish:YES];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"playEffect call failed: %d, please check your params", result]];
    }
}
- (IBAction)onStopEffect:(UIButton *)sender {
    int result = [self.agoraKit stopEffect:1];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"stopEffect call failed: %d, please check your params", result]];
    }
}
- (IBAction)onPauseEffect:(UIButton *)sender {
    int result = [self.agoraKit pauseEffect:1];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"pauseEffect call failed: %d, please check your params", result]];
    }
}
- (IBAction)onResumeEffect:(UIButton *)sender {
    int result = [self.agoraKit resumeEffect:1];
    if (result != 0) {
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"resumeEffect call failed: %d, please check your params", result]];
    }
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
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
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
    self.remoteView.uid = 0;
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine audioMixingStateChanged:(AgoraAudioMixingStateType)state reasonCode:(AgoraAudioMixingReasonCode)reasonCode {
    [LogUtil log:[NSString stringWithFormat:@"audioMixingStateChanged: %ld code: %ld", state, reasonCode] level:(LogLevelDebug)];
    if (state == AgoraAudioMixingStateTypePlaying) {
        [self startProgressTimer];
        [self updateTotalDuration:NO];
    }
}

@end
