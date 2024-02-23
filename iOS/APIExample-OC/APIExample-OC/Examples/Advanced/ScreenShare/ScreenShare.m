//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "ScreenShare.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"
#import <ReplayKit/ReplayKit.h>

@interface ScreenShareEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation ScreenShareEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"ScreenShare" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"ScreenShare"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface ScreenShare ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UIButton *fpsButton;
@property (weak, nonatomic) IBOutlet UIView *broadcasterPickerContainer;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong)PickerView *pickerView;
@property (nonatomic, strong)PickerView *scenarioPickerView;
@property (nonatomic, strong)AgoraScreenCaptureParameters2 *screenParams;
@property (nonatomic, strong)AgoraRtcChannelMediaOptions *option;
@property (nonatomic, strong)NSArray *fpsDataSources;
@property (nonatomic, strong)NSDictionary *scenarioTypeDataSources;
@property (nonatomic, strong)RPSystemBroadcastPickerView *systemBroadcastPicker;

@end

@implementation ScreenShare
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
- (NSArray *)fpsDataSources {
    if (_fpsDataSources == nil) {
        _fpsDataSources = @[@"15", @"30", @"60"];
    }
    return _fpsDataSources;
}
- (NSDictionary *)scenarioTypeDataSources {
    if (_scenarioTypeDataSources == nil) {
        _scenarioTypeDataSources = @{@"document": @"1",
                                     @"gaming": @"1",
                                     @"video": @"3",
                                     @"RDC": @"4"};
    }
    return _scenarioTypeDataSources;
}
- (PickerView *)pickerView {
    if (_pickerView == nil) {
        _pickerView = [[PickerView alloc] init];
        _pickerView.dataArray = self.fpsDataSources;
    }
    return _pickerView;
}
- (PickerView *)scenarioPickerView {
    if (_scenarioPickerView == nil) {
        _scenarioPickerView = [[PickerView alloc] init];
        _scenarioPickerView.dataArray = self.scenarioTypeDataSources.allKeys;
    }
    return _scenarioPickerView;
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
- (AgoraRtcChannelMediaOptions *)option {
    if (_option == nil) {
        _option = [[AgoraRtcChannelMediaOptions alloc] init];
        _option.clientRoleType = AgoraClientRoleBroadcaster;
        _option.publishCameraTrack = YES;
        _option.publishMicrophoneTrack = YES;
    }
    return _option;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
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
        
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:0 success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:self.option joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
}

- (void)prepareSystemBroadcaster {
    CGRect frame = CGRectMake(0, 0, 60, 60);
    self.systemBroadcastPicker = [[RPSystemBroadcastPickerView alloc]initWithFrame:frame];
    self.systemBroadcastPicker.showsMicrophoneButton = NO;
    self.systemBroadcastPicker.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin;
    NSString *bundleId = [NSBundle mainBundle].bundleIdentifier;
    self.systemBroadcastPicker.preferredExtension = [NSString stringWithFormat:@"%@.Agora-ScreenShare-Extension-OC",bundleId];
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

- (IBAction)clickCaptureAudio:(UISwitch *)sender {
    self.screenParams.captureAudio = sender.isOn;
}
- (IBAction)clickCaptureVideo:(UISwitch *)sender {
    self.screenParams.captureVideo = sender.isOn;
}
- (IBAction)captureSignalVolumeSlider:(UISlider *)sender {
    self.screenParams.audioParams.captureSignalVolume = sender.value * 100;
}
- (IBAction)clickFpsButton:(UIButton *)sender {
    __weak ScreenShare *weakself = self;
    self.pickerView.pickerViewSelectedValueClosure = ^(NSString * _Nonnull value) {
        weakself.screenParams.videoParams.frameRate = [value integerValue];
        [sender setTitle:[NSString stringWithFormat:@"%@fps",value] forState:(UIControlStateNormal)];
    };
    [self.pickerView show];
}
- (IBAction)clickScreenScenarioType:(UIButton *)sender {
    __weak ScreenShare *weakself = self;
    self.scenarioPickerView.pickerViewSelectedValueClosure = ^(NSString * _Nonnull key) {
        NSString *value = [weakself.scenarioTypeDataSources objectForKey:key];
        AgoraScreenScenarioType scenarioType = value.integerValue;
        [weakself.agoraKit setScreenCaptureScenario:scenarioType];
        [sender setTitle:[NSString stringWithFormat:@"%@",key] forState:(UIControlStateNormal)];
    };
    [self.scenarioPickerView show];
}
- (IBAction)stopScreenCapture:(id)sender {
    [self.agoraKit stopScreenCapture];
    self.option.publishScreenCaptureAudio = NO;
    self.option.publishScreenCaptureVideo = NO;
    self.option.publishCameraTrack = YES;
    [self.agoraKit updateChannelWithMediaOptions:self.option];
}
- (IBAction)startScreenCapture:(id)sender {
    [self.agoraKit startScreenCapture:self.screenParams];
    [self prepareSystemBroadcaster];
    for (UIView *view in self.systemBroadcastPicker.subviews) {
        if ([view isKindOfClass:[UIButton class]]) {
            [((UIButton *)view) sendActionsForControlEvents:(UIControlEventAllEvents)];
            break;
        }
    }
}
- (IBAction)updateScreenCapture:(id)sender {
    [self.agoraKit updateScreenCapture:self.screenParams];
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

- (void)rtcEngine:(AgoraRtcEngineKit *)engine localVideoStateChangedOfState:(AgoraVideoLocalState)state reason:(AgoraLocalVideoStreamReason)reason sourceType:(AgoraVideoSourceType)sourceType {
    if (state == AgoraVideoLocalStateCapturing && sourceType == AgoraVideoSourceTypeScreen) {
        self.option.publishScreenCaptureAudio = YES;
        self.option.publishScreenCaptureVideo = YES;
        self.option.publishCameraTrack = NO;
        [self.agoraKit updateChannelWithMediaOptions:self.option];
    }
}

@end
