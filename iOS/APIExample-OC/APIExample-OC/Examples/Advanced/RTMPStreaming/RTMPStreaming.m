//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "RTMPStreaming.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface RTMPStreamingEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UILabel *noteLabel;

@end

@implementation RTMPStreamingEntry

- (void)viewDidLoad {
    [super viewDidLoad];
    self.noteLabel.text = @"Ensure that you enable the RTMP Converter service at Agora Dashboard before using this function.".localized;
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"RTMPStreaming" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"RTMPStreaming"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface RTMPStreaming ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (weak, nonatomic) IBOutlet UIButton *publishButton;
@property (weak, nonatomic) IBOutlet UITextField *rtmpTextField;
@property (weak, nonatomic) IBOutlet UISwitch *transcodingSwitch;
@property (weak, nonatomic) IBOutlet UILabel *transcodingLabel;

@property (nonatomic, assign) BOOL isJoined;
@property (nonatomic, assign) BOOL isPublished;
@property (nonatomic, assign) NSUInteger remoteUid;
@property (nonatomic, copy) NSString *rtmpURL;
@property (nonatomic, strong) AgoraLiveTranscoding *transcoding;

@end

static CGFloat CANVAS_WIDTH = 640;
static CGFloat CANVAS_HEIGHT = 480;

@implementation RTMPStreaming

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

- (void)setIsJoined:(BOOL)isJoined {
    _isJoined = isJoined;
    [self.rtmpTextField setHidden:!isJoined];
    [self.publishButton setHidden:!isJoined];
    [self.transcodingLabel setHidden:!isJoined];
    [self.transcodingSwitch setHidden:!isJoined];
}

- (void)setIsPublished:(BOOL)isPublished {
    _isPublished = isPublished;
    [self.rtmpTextField setEnabled:!isPublished];
    [self.transcodingSwitch setEnabled:!isPublished];
    [self.publishButton setTitle:isPublished ? @"stop".localized : @"push".localized forState:(UIControlStateNormal)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    self.transcoding = [AgoraLiveTranscoding defaultTranscoding];
    
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
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

- (IBAction)onClickPublishButton:(id)sender {
    if (self.rtmpTextField.text.length <= 0) { return; }
    NSString *rtmpURL = self.rtmpTextField.text;
    if (self.isPublished) {
        // stop rtmp streaming
        [self.agoraKit stopRtmpStream:rtmpURL];
    } else {
        // resign rtmp text field
        [self.rtmpTextField resignFirstResponder];
        
        // check whether we need to enable transcoding
        BOOL transcodingEnabled = self.transcodingSwitch.isOn;
        if (transcodingEnabled) {
            // we will use transcoding to composite multiple hosts' video
            // therefore we have to create a livetranscoding object and call before addPublishStreamUrl
            self.transcoding.size = CGSizeMake(CANVAS_WIDTH, CANVAS_HEIGHT);
            [self.agoraKit startRtmpStreamWithTranscoding:rtmpURL transcoding:self.transcoding];
            
            if (self.remoteUid > 0) {
                // add new user onto the canvas
                AgoraLiveTranscodingUser *user = [[AgoraLiveTranscodingUser alloc] init];
                user.rect = CGRectMake(CANVAS_WIDTH * 0.5, 0, CANVAS_WIDTH * 0.5, CANVAS_HEIGHT);
                user.uid = self.remoteUid;
                [self.transcoding addUser:user];
                // remember you need to call setLiveTranscoding again if you changed the layout
                [self.agoraKit updateRtmpTranscoding:self.transcoding];
            }
        } else {
            [self.agoraKit startRtmpStreamWithoutTranscoding:rtmpURL];
        }
        self.rtmpURL = rtmpURL;
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
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
    self.isJoined = YES;
    [LogUtil log:[NSString stringWithFormat:@"Join %@ with uid %lu elapsed %ldms", channel, uid, elapsed] level:(LogLevelDebug)];
    self.localView.uid = uid;
    
    AgoraLiveTranscodingUser *user = [[AgoraLiveTranscodingUser alloc]init];
    user.rect = CGRectMake(0, 0, CANVAS_WIDTH * 0.5, CANVAS_HEIGHT);
    user.uid = uid;
    [self.transcoding addUser:user];
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
    
    // remove preivous user from the canvas
    if (self.remoteUid > 0) {
        [self.transcoding removeUser:self.remoteUid];
    }
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
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
    
    // check whether we have enabled transcoding
    if (self.transcodingSwitch.isOn) {
        // remove user from canvas if current cohost left channel
        if (self.remoteUid > 0) {
            [self.transcoding removeUser:self.remoteUid];
        }
        self.remoteUid = 0;
        [self.agoraKit updateRtmpTranscoding:self.transcoding];
    }
}

-(void)rtcEngine:(AgoraRtcEngineKit *)engine rtmpStreamingChangedToState:(NSString *)url state:(AgoraRtmpStreamingState)state reason:(AgoraRtmpStreamingReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"streamStateChanged: %@ state %ld reason %ld", url, state, reason] level:(LogLevelDebug)];
    if (state == AgoraRtmpStreamingStateRunning) {
        [self showAlertWithTitle:@"Notice" message:@"RTMP Publish Success"];
        self.isPublished = YES;
    } else if (state == AgoraRtmpStreamingStateFailure) {
        
    } else if (state == AgoraRtmpStreamingStateIdle) {
        [self showAlertWithTitle:@"Notice" message:@"RTMP Publish Stopped"];
        self.isPublished = NO;
    }
}

- (void)rtcEngineTranscodingUpdated:(AgoraRtcEngineKit *)engine {
    [LogUtil log:@"live transcoding updated" level:(LogLevelDebug)];
}

@end
