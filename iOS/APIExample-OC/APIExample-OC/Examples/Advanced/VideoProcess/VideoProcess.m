//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "VideoProcess.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface VideoProcessEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation VideoProcessEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"VideoProcess" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"VideoProcess"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface VideoProcess ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UISwitch *beautySwitch;
@property (weak, nonatomic) IBOutlet UISwitch *colorEnhanceSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *virtualBgSwitch;
@property (weak, nonatomic) IBOutlet UISegmentedControl *virtualBgSegment;
@property (weak, nonatomic) IBOutlet UISlider *lightenSlider;
@property (weak, nonatomic) IBOutlet UISlider *rednessSlider;
@property (weak, nonatomic) IBOutlet UISlider *sharpnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *smoothSlider;
@property (weak, nonatomic) IBOutlet UISlider *strengthSlider;
@property (weak, nonatomic) IBOutlet UISlider *skinProtectSlider;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong)AgoraBeautyOptions *beautifyOption;
@property (nonatomic, assign)CGFloat skinProtect;
@property (nonatomic, assign)CGFloat strength;

@end

@implementation VideoProcess

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
    
    self.beautifyOption = [[AgoraBeautyOptions alloc] init];
    self.skinProtect = 0.5;
    self.strength = 0.5;
    
    self.lightenSlider.value = self.beautifyOption.lighteningLevel;
    self.rednessSlider.value = self.beautifyOption.rednessLevel;
    self.sharpnessSlider.value = self.beautifyOption.sharpnessLevel;
    self.smoothSlider.value = self.beautifyOption.smoothnessLevel;
    self.strengthSlider.value = self.strength;
    self.skinProtectSlider.value = self.skinProtect;
    
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    // enable filters
    [self.agoraKit enableExtensionWithVendor:@"agora_video_filters_clear_vision" extension:@"clear_vision" enabled:YES sourceType:(AgoraMediaSourceTypePrimaryCamera)];
    
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

- (IBAction)onChangeBeauty:(UISwitch *)sender {
    [self.agoraKit setBeautyEffectOptions:sender.isOn options:self.beautifyOption];
}
- (IBAction)onLightenSlider:(UISlider *)sender {
    self.beautifyOption.lighteningLevel = sender.value;
    [self.agoraKit setBeautyEffectOptions:self.beautySwitch.isOn options:self.beautifyOption];
}
- (IBAction)onRednessSlider:(UISlider *)sender {
    self.beautifyOption.rednessLevel = sender.value;
    [self.agoraKit setBeautyEffectOptions:self.beautySwitch.isOn options:self.beautifyOption];
}
- (IBAction)onSharpnessSlider:(UISlider *)sender {
    self.beautifyOption.sharpnessLevel = sender.value;
    [self.agoraKit setBeautyEffectOptions:self.beautySwitch.isOn options:self.beautifyOption];
}
- (IBAction)onSmoothSlider:(UISlider *)sender {
    self.beautifyOption.smoothnessLevel = sender.value;
    [self.agoraKit setBeautyEffectOptions:self.beautySwitch.isOn options:self.beautifyOption];
}
- (IBAction)onChangeLowLightEnhance:(UISwitch *)sender {
    AgoraLowlightEnhanceOptions *options = [[AgoraLowlightEnhanceOptions alloc] init];
    options.level = AgoraLowlightEnhanceLevelFast;
    options.mode = AgoraLowlightEnhanceModeAuto;
    [self.agoraKit setLowlightEnhanceOptions:sender.isOn options:options];
}
- (IBAction)onChangeVideoDenoise:(UISwitch *)sender {
    AgoraVideoDenoiserOptions *options = [[AgoraVideoDenoiserOptions alloc] init];
    options.level = AgoraVideoDenoiserLevelHighQuality;
    options.mode = AgoraVideoDenoiserModeManual;
    [self.agoraKit setVideoDenoiserOptions:sender.isOn options:options];
}
- (IBAction)onChangeColorEnhance:(UISwitch *)sender {
    AgoraColorEnhanceOptions *options = [[AgoraColorEnhanceOptions alloc] init];
    options.strengthLevel = self.strength;
    options.skinProtectLevel = self.skinProtect;
    [self.agoraKit setColorEnhanceOptions:sender.isOn options:options];
}
- (IBAction)onStrengthSlider:(UISlider *)sender {
    self.strength = sender.value;
    AgoraColorEnhanceOptions *options = [[AgoraColorEnhanceOptions alloc] init];
    options.strengthLevel = self.strength;
    options.skinProtectLevel = self.skinProtect;
    [self.agoraKit setColorEnhanceOptions:self.colorEnhanceSwitch.isOn options:options];
}
- (IBAction)onSkinProtectSlider:(UISlider *)sender {
    self.skinProtect = sender.value;
    AgoraColorEnhanceOptions *options = [[AgoraColorEnhanceOptions alloc] init];
    options.strengthLevel = self.strength;
    options.skinProtectLevel = self.skinProtect;
    [self.agoraKit setColorEnhanceOptions:self.colorEnhanceSwitch.isOn options:options];
}
- (IBAction)onChangeVirtualBgSwtich:(UISwitch *)sender {
    [self changeVirtualBackground];
}
- (IBAction)onChangeVirtualBgSegment:(UISegmentedControl *)sender {
    [self changeVirtualBackground];
}
- (void)changeVirtualBackground {
    AgoraVirtualBackgroundSource *source = [[AgoraVirtualBackgroundSource alloc] init];
    switch (self.virtualBgSegment.selectedSegmentIndex) {
        case 0:
        {
            NSString *imagePath = [[NSBundle mainBundle]pathForResource:@"agora-logo" ofType:@"png"];
            source.backgroundSourceType = AgoraVirtualBackgroundImg;
            source.source = imagePath;
        }
            break;
        case 1:
        {
            source.backgroundSourceType = AgoraVirtualBackgroundColor;
            source.color = 0xFFFFFF;
        }
            break;
        case 2:
        {
            source.backgroundSourceType = AgoraVirtualBackgroundBlur;
            source.blurDegree = AgoraBlurHigh;
        }
            break;
        case 3:
        {
            NSString *videoPath = [[NSBundle mainBundle]pathForResource:@"sample" ofType:@"mov"];
            source.backgroundSourceType = AgoraVirtualBackgroundVideo;
            source.source = videoPath;
        }
        default:
            break;
    }
    source.backgroundSourceType = self.virtualBgSwitch.isOn ? source.backgroundSourceType : AgoraVirtualBackgroundNone;
    [self.agoraKit enableVirtualBackground:self.virtualBgSwitch.isOn backData:source segData:[[AgoraSegmentationProperty alloc]init]];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    // close virtural
    [self.agoraKit enableVirtualBackground:NO backData:nil segData:nil];
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

@end
