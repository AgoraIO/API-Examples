//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "FusionCDN.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import <AgoraRtcKit/AgoraRtcMediaPlayerProtocol.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

typedef NS_ENUM(NSInteger, StreamingMode) {
    StreamingModeAgoraChannel = 1,
    StreamingModeCDNUrl
};

@implementation CDNChannelInfo
@end

@interface FusionCDNEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (nonatomic, assign) StreamingMode mode;

@end

@implementation FusionCDNEntry

- (void)viewDidLoad {
    [super viewDidLoad];
    self.mode = StreamingModeAgoraChannel;
}

- (IBAction)setStreamingMode:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Streaming Mode".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    
    UIAlertAction *agoraChannel = [UIAlertAction actionWithTitle:@"Agora Channel".localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        self.textField.placeholder = @"Set Channel Name";
        self.mode = StreamingModeAgoraChannel;
        [sender setTitle:@"Agora Channel".localized forState:(UIControlStateNormal)];
    }];
    UIAlertAction *cdn = [UIAlertAction actionWithTitle:@"CDN URL".localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        self.textField.placeholder = @"Set CDN UR";
        self.mode = StreamingModeCDNUrl;
        [sender setTitle:@"CDN URL".localized forState:(UIControlStateNormal)];
    }];
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler: nil];
    [alertVC addAction:agoraChannel];
    [alertVC addAction:cdn];
    [alertVC addAction:cancel];
    
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)joinAsHost:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"FusionCDN" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"Host"];
    newViewController.configs = @{@"channelName": self.textField.text, @"mode": @(self.mode)};
    [self.navigationController pushViewController:newViewController animated:YES];
}
- (IBAction)joinAsAudience:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"FusionCDN" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"Audience"];
    newViewController.configs = @{@"channelName": self.textField.text, @"mode": @(self.mode)};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


CGFloat WIDTH = 480;
CGFloat HEIGHT = 640;

@interface FusionCDNHost ()<AgoraRtcEngineDelegate, AgoraDirectCdnStreamingEventDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UIButton *streamingButton;
@property (weak, nonatomic) IBOutlet UILabel *rtcSwitcherLabel;
@property (weak, nonatomic) IBOutlet UISwitch *rtcSwitcher;
@property (weak, nonatomic) IBOutlet UISlider *volumeSlider;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;

@property (nonatomic, copy) NSString *streamingUrl;
@property (nonatomic, assign) BOOL isCdnStreaming;
@property (nonatomic, assign) BOOL isRtcStreaming;
@property (nonatomic, strong) AgoraLiveTranscoding *transcoding;
@property (nonatomic, assign) NSUInteger localUid;
@property (nonatomic, strong) AgoraVideoEncoderConfiguration *videoConfig;

@end

@implementation FusionCDNHost

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
    
    self.transcoding = [AgoraLiveTranscoding defaultTranscoding];
    self.localUid = arc4random() % 9999999;
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView]];
    
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
    self.videoConfig = encoderConfig;
    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
    [self.agoraKit setDirectCdnStreamingVideoConfiguration:encoderConfig];
    [self.agoraKit setDirectCdnStreamingAudioConfiguration:(AgoraAudioProfileDefault)];
    
    self.transcoding.size = CGSizeMake(WIDTH, HEIGHT);
    self.transcoding.videoFramerate = 15;
    
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
    
    StreamingMode mode = ((NSNumber *)[self.configs objectForKey:@"mode"]).intValue;
    if (mode == StreamingModeAgoraChannel) {
        self.streamingUrl = [NSString stringWithFormat: @"rtmp://push.webdemo.agoraio.cn/lbhd/%@", channelName];
        [self.rtcSwitcher setEnabled:NO];
    } else {
        self.streamingUrl = channelName;
        [self.rtcSwitcher setHidden:YES];
        [self.rtcSwitcherLabel setHidden:YES];
    }
}

- (IBAction)setStreaming:(id)sender {
    if (self.isRtcStreaming) {
        [self stopRtcStreaming];
    } else if (self.isCdnStreaming) {
        [self stopRskStreaming];
        [self resetUI];
    } else {
        [self switchToRskStreaming];
    }
}
- (IBAction)setRtcStreaming:(UISwitch *)sender {
    self.isRtcStreaming = sender.isOn;
    if (self.isRtcStreaming) {
        [self stopRskStreaming];
    } else {
        [self stopRtcStreaming];
        [self switchToRskStreaming];
    }
}
- (IBAction)onChangeRecordingVolume:(UISlider *)sender {
    NSInteger value = (NSInteger)sender.value;
    [LogUtil log:[NSString stringWithFormat:@"adjustRecordingSignalVolume == %ld", value] level:(LogLevelDebug)];
    [self.agoraKit adjustRecordingSignalVolume:value];
}

- (void)stopRtcStreaming {
    self.isRtcStreaming = NO;
    [self.rtcSwitcher setOn:NO];
    [self resetUI];
    AgoraLeaveChannelOptions *options = [[AgoraLeaveChannelOptions alloc] init];
    options.stopMicrophoneRecording = NO;
    [self.agoraKit leaveChannel:options leaveChannelBlock:nil];
    [self.agoraKit stopRtmpStream:self.streamingUrl];
    [self.containerView layoutStream:@[self.localView]];
}

- (void)stopRskStreaming {
    self.isCdnStreaming = NO;
    [self.rtcSwitcher setEnabled:YES];
    [self.agoraKit stopDirectCdnStreaming];
}

- (void)resetUI {
    self.isRtcStreaming = NO;
    self.isCdnStreaming = NO;
    [self.rtcSwitcher setOn:NO];
    [self.rtcSwitcher setEnabled:NO];
    [self.streamingButton setTitle:@"Start Live Streaming" forState:(UIControlStateNormal)];
    [self.streamingButton setTitleColor:[UIColor blueColor] forState:(UIControlStateNormal)];
}

- (void)switchToRskStreaming {
    [self.agoraKit setDirectCdnStreamingVideoConfiguration:self.videoConfig];
    [self.agoraKit setDirectCdnStreamingAudioConfiguration:(AgoraAudioProfileDefault)];
    AgoraDirectCdnStreamingMediaOptions *options = [[AgoraDirectCdnStreamingMediaOptions alloc] init];
    options.publishCameraTrack = YES;
    options.publishMicrophoneTrack = YES;
    int ret = [self.agoraKit startDirectCdnStreaming:self publishUrl:self.streamingUrl mediaOptions:options];
    if (ret == 0) {
        [self.streamingButton setTitle:@"Streaming" forState:(UIControlStateNormal)];
        [self.streamingButton setTitleColor:[UIColor grayColor] forState:(UIControlStateNormal)];
        [self.agoraKit startPreview];
    } else {
        [self stopRskStreaming];
        [self resetUI];
        [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"startDirectCdnStreaming failed: %d", ret]];
    }
}

- (void)switchToRtcStreaming {
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
    options.publishCameraTrack = YES;
    options.publishMicrophoneTrack = YES;
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

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self stopRskStreaming];
    [self stopRtcStreaming];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

- (void)onDirectCdnStreamingStateChanged:(AgoraDirectCdnStreamingState)state reason:(AgoraDirectCdnStreamingReason)reason message:(NSString *)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        switch (state) {
            case AgoraDirectCdnStreamingStateRunning:
                [self.streamingButton setTitle:@"Stop Streaming" forState:(UIControlStateNormal)];
                [self.streamingButton setTitleColor:[UIColor redColor] forState:(UIControlStateNormal)];
                self.isCdnStreaming = YES;
                [self.rtcSwitcher setEnabled:YES];
                break;
            
            case AgoraDirectCdnStreamingStateStopped:
                if (self.isRtcStreaming) {
                    // switch to rtc streaming when direct cdn streaming completely stopped
                    [self switchToRtcStreaming];
                } else {
                    [self.streamingButton setTitle:@"Start Live Streaming" forState:(UIControlStateNormal)];
                    [self.streamingButton setTitleColor:[UIColor blueColor] forState:(UIControlStateNormal)];
                    self.isCdnStreaming = NO;
                }
                break;
                
            case AgoraDirectCdnStreamingStateFailed:
                [self showAlertWithTitle:@"Error" message:@"Start Streaming failed, please go back to previous page and check the settings."];
            default:
                [LogUtil log:[NSString stringWithFormat:@"onDirectCdnStreamingStateChanged: %ld, %ld %@", state, reason, message] level:(LogLevelDebug)];
                break;
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
    AgoraLiveTranscodingUser *user = [[AgoraLiveTranscodingUser alloc] init];
    user.rect = CGRectMake(0, 0, WIDTH, HEIGHT);
    user.uid = uid;
    [self.transcoding addUser:user];
    // agoraKit.updateRtmpTranscoding(transcoding)
    if (self.streamingUrl.length > 0) {
        [self.agoraKit startRtmpStreamWithTranscoding:self.streamingUrl transcoding:self.transcoding];
    }
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine streamUnpublishedWithUrl:(NSString *)url {
    [self switchToRtcStreaming];
    [self.containerView layoutStream:@[self.localView]];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine rtmpStreamingChangedToState:(NSString *)url state:(AgoraRtmpStreamingState)state reason:(AgoraRtmpStreamingReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"On rtmpStreamingChangedToState, state: %ld reason: %ld", state, reason] level:(LogLevelDebug)];
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
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
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
    [self.containerView layoutStream:@[self.localView]];
}

@end

@interface FusionCDNAudience ()<AgoraRtcEngineDelegate, AgoraRtcMediaPlayerDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UILabel *rtcSwitcherLabel;
@property (weak, nonatomic) IBOutlet UISwitch *rtcSwitcher;
@property (weak, nonatomic) IBOutlet UISlider *volumeSlider;
@property (weak, nonatomic) IBOutlet UILabel *cdnSelectorLabel;
@property (weak, nonatomic) IBOutlet UIButton *cdnSelector;
@property (weak, nonatomic) IBOutlet UILabel *volumeSliderLabel;

@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, weak)id<AgoraRtcMediaPlayerProtocol>medoaPlayerKit;
@property (nonatomic, copy) NSString *streamingUrl;
@property (nonatomic, assign)UInt32 channelNumber;
@property (nonatomic, assign) BOOL isRtcStreaming;

@end

@implementation FusionCDNAudience

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
    
    // layout render view
    [self.localView setPlaceholder:@"Player".localized];
    [self.containerView layoutStream:@[self.localView]];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleAudience)];
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
    self.medoaPlayerKit = [self.agoraKit createMediaPlayerWithDelegate:self];
    [self.medoaPlayerKit setView:self.localView.videoView];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localView.videoView;
    videoCanvas.sourceType = AgoraVideoSourceTypeMediaPlayer;
    videoCanvas.mediaPlayerId = [self.medoaPlayerKit getMediaPlayerId];
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupLocalVideo:videoCanvas];
    // you have to call startPreview to see local video
    [self.agoraKit startPreview];
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    
    StreamingMode mode = ((NSNumber *)[self.configs objectForKey:@"mode"]).intValue;
    if (mode == StreamingModeAgoraChannel) {
        self.streamingUrl = [NSString stringWithFormat:@"rtmp://pull.webdemo.agoraio.cn/lbhd/%@", channelName];
        [self.rtcSwitcher setEnabled:NO];
        [self.medoaPlayerKit open:self.streamingUrl startPos:0];
    } else {
        self.streamingUrl = channelName;
        [self.rtcSwitcher setHidden:YES];
        [self.rtcSwitcherLabel setHidden:YES];
        [self.medoaPlayerKit open:self.streamingUrl startPos:0];
    }
}

- (IBAction)setCDNChannel:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Select CDN Channel".localized message:nil preferredStyle:(UIAlertControllerStyleAlert)];
    for (int i = 0; i < self.channelNumber; i++) {
        NSString *title = [NSString stringWithFormat:@"%@%d",@"CDN Channel".localized,i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            [self.cdnSelector setTitle:title forState:(UIControlStateNormal)];
            [self.medoaPlayerKit switchAgoraCDNLineByIndex:i];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:cancel];
//    [self presentViewController:alertVC animated:YES completion:nil];
    [self presentAlertViewController:alertVC];
}
- (IBAction)setRtcStreaming:(UISwitch *)sender {
    self.isRtcStreaming = sender.isOn;
    if (sender.isOn) {
        NSString *channelName = [self.configs objectForKey:@"channelName"];
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
        
        [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
            int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:options joinSuccess:nil];
            if (result != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                NSLog(@"joinChannel call failed: %d, please check your params", result);
            } else {
                AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
                videoCanvas.uid = 0;
                // the view to be binded
                videoCanvas.view = self.localView.videoView;
                videoCanvas.renderMode = AgoraVideoRenderModeHidden;
                [self.agoraKit setupLocalVideo:videoCanvas];
                [self.agoraKit startPreview];
                [self.cdnSelector setEnabled:NO];
                [self.volumeSlider setHidden:NO];
                [self.volumeSliderLabel setHidden:NO];
            }
        }];
    } else {
        AgoraLeaveChannelOptions *leaveChannelOption = [[AgoraLeaveChannelOptions alloc] init];
        leaveChannelOption.stopMicrophoneRecording = NO;
        [self.agoraKit leaveChannel:leaveChannelOption leaveChannelBlock:nil];
        [self.cdnSelector setEnabled:YES];
        [self.volumeSlider setHidden:YES];
        [self.volumeSliderLabel setHidden:YES];
        AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
        videoCanvas.uid = 0;
        // the view to be binded
        videoCanvas.view = self.localView.videoView;
        videoCanvas.sourceType = AgoraVideoSourceTypeMediaPlayer;
        videoCanvas.mediaPlayerId = [self.medoaPlayerKit getMediaPlayerId];
        videoCanvas.renderMode = AgoraVideoRenderModeHidden;
        [self.agoraKit setupLocalVideo:videoCanvas];
        [self.containerView layoutStream:@[self.localView]];
    }
}
- (IBAction)onChangeRecordingVolume:(UISlider *)sender {
    [self.agoraKit adjustRecordingSignalVolume:sender.value];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
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
    // Only one remote video view is available for this
    // tutorial. Here we check if there exists a surface
    // view tagged as this uid.
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc]init];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = self.remoteView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
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
    [self.containerView layoutStream:@[self.localView]];
    self.remoteView.uid = 0;

    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

- (void)AgoraRtcMediaPlayer:(id<AgoraRtcMediaPlayerProtocol>)playerKit didChangedToState:(AgoraMediaPlayerState)state reason:(AgoraMediaPlayerReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"player rtc channel publish helper state changed to: %ld error: %ld", state, reason] level:(LogLevelDebug)];
    dispatch_async(dispatch_get_main_queue(), ^{
        switch (state) {
            case AgoraMediaPlayerStateFailed:
                [self showAlertWithTitle:[NSString stringWithFormat:@"media player error: %ld", reason]];
                break;
            
            case AgoraMediaPlayerStateOpenCompleted:
                [self.medoaPlayerKit play];
                StreamingMode mode = ((NSNumber *)[self.configs objectForKey:@"mode"]).intValue;
                if (mode == StreamingModeAgoraChannel) {
                    int num = [self.medoaPlayerKit getAgoraCDNLineCount];
                    if (num > 0) {
                        self.channelNumber = num;
                        [self.cdnSelectorLabel setHidden:NO];
                        [self.cdnSelector setHidden:NO];
                        [self.cdnSelector setTitle:[NSString stringWithFormat:@"%@%d",@"CDN Channel".localized, 1] forState:(UIControlStateNormal)];
                    }
                    [self.rtcSwitcher setEnabled:YES];
                }
            
            default:
                break;
        }
    });
}

- (void)AgoraRtcMediaPlayer:(id<AgoraRtcMediaPlayerProtocol>)playerKit didOccurEvent:(AgoraMediaPlayerEvent)eventCode elapsedTime:(NSInteger)elapsedTime message:(NSString *)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        switch (eventCode) {
            case AgoraMediaPlayerEventSwitchError:
                [self showAlertWithTitle:[NSString stringWithFormat:@"switch cdn channel error!: %@", message]];
                break;
               
            case AgoraMediaPlayerEventSwitchComplete:
                [self showAlertWithTitle:@"switch cdn channel complete!"];
                break;
                
            default:
                break;
        }
    });
}
@end
