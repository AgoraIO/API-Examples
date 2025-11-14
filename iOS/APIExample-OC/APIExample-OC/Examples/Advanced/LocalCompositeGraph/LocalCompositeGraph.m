//
//  LocalCompositeGraph.m
//  APIExample-OC
//
//  Created by qinhui on 2025/11/14.
//

#import "LocalCompositeGraph.h"
#import "VideoView.h"
#import "AgoraRtcKit/AgoraRtcKit.h"
#import <ReplayKit/ReplayKit.h>
#import "KeyCenter.h"

@interface LocalCompositeGraphEntry ()

@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation LocalCompositeGraphEntry
- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"LocalCompositeGraph" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"LocalCompositeGraph"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
}

@end

@interface LocalCompositeGraph ()<AgoraRtcEngineDelegate>

@property (nonatomic, strong) VideoView *localView;
@property (weak, nonatomic) IBOutlet UIView *container;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong)AgoraScreenCaptureParameters2 *screenParams;
@property (nonatomic, strong)RPSystemBroadcastPickerView *systemBroadcastPicker;
@property (nonatomic, strong)AgoraRtcChannelMediaOptions *option;

@end

@implementation LocalCompositeGraph

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.container layoutStream:@[self.localView]];
    
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
    
//    AgoraVideoEncoderConfiguration *encoderConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(960, 540)
//                                                                                               frameRate:(AgoraVideoFrameRateFps15)
//                                                                                                 bitrate:AgoraVideoBitrateStandard
//                                                                                         orientationMode:(AgoraVideoOutputOrientationModeFixedPortrait)
//                                                                                              mirrorMode:(AgoraVideoMirrorModeAuto)];
//    [self.agoraKit setVideoEncoderConfiguration:encoderConfig];
    
    // Set audio route to speaker
    [self.agoraKit setEnableSpeakerphone:YES];
    
    [self startScreenCapture];
    
    AgoraCameraCapturerConfiguration *cameraCaptureConfig = [[AgoraCameraCapturerConfiguration alloc] init];
    cameraCaptureConfig.dimensions = CGSizeMake(100, 100);
    [self.agoraKit startCameraCapture:AgoraVideoSourceTypeCamera config:cameraCaptureConfig];
        
    __weak typeof(self) weakSelf = self;
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        int result = [strongSelf.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:strongSelf.option joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
        [strongSelf startVideoTranscoder];
    }];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localView.videoView;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    videoCanvas.sourceType = AgoraVideoSourceTypeTransCoded;
    [self.agoraKit setupLocalVideo:videoCanvas];
    // you have to call startPreview to see local video
    [self.agoraKit startPreview];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit stopScreenCapture];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    [AgoraRtcEngineKit destroy];
}

- (VideoView *)localView {
    if (_localView == nil) {
        _localView = (VideoView *)[NSBundle loadVideoViewFormType:StreamTypeLocal audioOnly:NO];
    }
    return _localView;
}

- (AgoraScreenCaptureParameters2 *)screenParams {
    if (_screenParams == nil) {
        _screenParams = [[AgoraScreenCaptureParameters2 alloc] init];
        _screenParams.captureAudio = YES;
        _screenParams.captureVideo = YES;
        AgoraScreenAudioParameters *audioParams = [[AgoraScreenAudioParameters alloc] init];
        audioParams.captureSignalVolume = 50;
        _screenParams.audioParams = audioParams;
        AgoraScreenVideoParameters *videoParams = [[AgoraScreenVideoParameters alloc] init];
        videoParams.dimensions = [self screenShareVideoDimension];
        videoParams.frameRate = AgoraVideoFrameRateFps15;
        videoParams.bitrate = AgoraVideoBitrateStandard;
    }
    return _screenParams;
}

- (void)prepareSystemBroadcaster {
    CGRect frame = CGRectMake(0, 0, 60, 60);
    self.systemBroadcastPicker = [[RPSystemBroadcastPickerView alloc]initWithFrame:frame];
    self.systemBroadcastPicker.showsMicrophoneButton = NO;
    self.systemBroadcastPicker.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin;
    NSString *bundleId = [NSBundle mainBundle].bundleIdentifier;
    self.systemBroadcastPicker.preferredExtension = [NSString stringWithFormat:@"%@.Agora-ScreenShare-Extension-OC",bundleId];
}

- (void)startScreenCapture {
    [self.agoraKit startScreenCapture:self.screenParams];
    [self prepareSystemBroadcaster];
    for (UIView *view in self.systemBroadcastPicker.subviews) {
        if ([view isKindOfClass:[UIButton class]]) {
            [((UIButton *)view) sendActionsForControlEvents:(UIControlEventAllEvents)];
            break;
        }
    }
    
    self.option.publishScreenCaptureVideo = true;
    self.option.publishScreenCaptureAudio = true;
    [self.agoraKit updateChannelWithMediaOptions:self.option];
}

- (void)startVideoTranscoder {
    AgoraTranscodingVideoStream *videoStream = [[AgoraTranscodingVideoStream alloc] init];
    videoStream.rect = CGRectMake(0, 60, 100, 100);
    videoStream.sourceType = AgoraVideoSourceTypeCamera;
    
    AgoraTranscodingVideoStream *screenStream = [[AgoraTranscodingVideoStream alloc] init];
    CGSize screenStreamSize = [self screenShareVideoDimension];
    screenStream.rect = CGRectMake(0, 0, screenStreamSize.width, screenStreamSize.height);
    screenStream.sourceType = AgoraVideoSourceTypeScreen;
    
    AgoraLocalTranscoderConfiguration *config = [[AgoraLocalTranscoderConfiguration alloc] init];
    config.videoOutputConfiguration.dimensions = screenStreamSize;
    config.videoInputStreams = @[screenStream, videoStream];
    [self.agoraKit startLocalVideoTranscoder:config];
}

- (AgoraRtcChannelMediaOptions *)option {
    if (_option == nil) {
        _option = [[AgoraRtcChannelMediaOptions alloc] init];
        _option.clientRoleType = AgoraClientRoleBroadcaster;
        _option.publishCameraTrack = YES;
        _option.publishMicrophoneTrack = YES;
        _option.publishTranscodedVideoTrack = YES;
    }
    return _option;
}

- (CGSize)screenShareVideoDimension {
    CGSize screenSize = UIScreen.mainScreen.bounds.size;
    CGSize boundingSize = CGSizeMake(540, 960);
    CGFloat mW = boundingSize.width / screenSize.width;
    CGFloat mH = boundingSize.height / screenSize.height;
    if (mH < mW) {
        boundingSize.width = boundingSize.height / screenSize.height * screenSize.width;
    } else if (mW < mH) {
        boundingSize.height = boundingSize.width / screenSize.width * screenSize.height;
    }
    return boundingSize;
}

#pragma mark -- AgoraRtcEngineDelegate
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
}

/// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param reason reason why this user left, note this event may be triggered when the remote user
/// become an audience in live broadcasting profile
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

@end
