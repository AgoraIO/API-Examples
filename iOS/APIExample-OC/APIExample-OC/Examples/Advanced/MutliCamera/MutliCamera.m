//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "MutliCamera.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface MutliCameraEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;

@end

@implementation MutliCameraEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"MutliCamera" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"MutliCamera"];
    newViewController.configs = @{@"channelName": self.textField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface MutliCamera ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (weak, nonatomic) IBOutlet UIButton *cameraButton;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;
@property (nonatomic, assign) NSUInteger uid;
@property (nonatomic, assign) NSUInteger mutliCameraUid;
@property (nonatomic, assign) BOOL isOpenCamera;

@end

@implementation MutliCamera

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
    self.uid = arc4random() % 9999;
    self.mutliCameraUid = arc4random() % 99999999;
    
    [self.cameraButton setTitle:@"Open Rear Camera".localized forState:(UIControlStateNormal)];
    // layout render view
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"第二路摄像头".localized];
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
    
    // open Multi Camera
    AgoraCameraCapturerConfiguration *capturerConfig = [[AgoraCameraCapturerConfiguration alloc] init];
    capturerConfig.cameraDirection = AgoraCameraDirectionRear;
    [self.agoraKit enableMultiCamera:YES config:capturerConfig];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = self.uid;
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
    
    [[NetworkManager shared] generateTokenWithChannelName:channelName uid:self.uid success:^(NSString * _Nullable token) {
        int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:self.uid mediaOptions:options joinSuccess:nil];
        if (result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
            // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            NSLog(@"joinChannel call failed: %d, please check your params", result);
        }
    }];
}

- (IBAction)onClickSecondCameraButton:(UIButton *)sender {
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    self.isOpenCamera = !self.isOpenCamera;
    NSString *title = self.isOpenCamera ? @"Close Rear Camera".localized : @"Open Rear Camera".localized;
    [sender setTitle:title forState:(UIControlStateNormal)];
    AgoraRtcConnection * connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.mutliCameraUid];
    if (self.isOpenCamera) {
        AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
        videoCanvas.uid = self.mutliCameraUid;
        videoCanvas.view = self.remoteView.videoView;
        videoCanvas.renderMode = AgoraVideoRenderModeHidden;
        videoCanvas.sourceType = AgoraVideoSourceTypeCameraSecondary;
        videoCanvas.mirrorMode = AgoraVideoMirrorModeDisabled;
        [self.agoraKit setupLocalVideo:videoCanvas];
        
        AgoraCameraCapturerConfiguration *cameraConfig = [[AgoraCameraCapturerConfiguration alloc] init];
        cameraConfig.cameraDirection = AgoraCameraDirectionRear;
        cameraConfig.dimensions = self.remoteView.videoView.frame.size;
        [self.agoraKit enableMultiCamera:YES config:cameraConfig];
        [self.agoraKit startCameraCapture:(AgoraVideoSourceTypeCameraSecondary) config:cameraConfig];
        
        AgoraRtcChannelMediaOptions *options = [[AgoraRtcChannelMediaOptions alloc] init];
        options.publishScreenCaptureVideo = YES;
        [[NetworkManager shared] generateTokenWithChannelName:channelName uid:self.mutliCameraUid success:^(NSString * _Nullable token) {
            [self.agoraKit joinChannelExByToken:token connection:connection delegate:self mediaOptions:options joinSuccess:nil];
            [self.agoraKit muteRemoteAudioStream:self.mutliCameraUid mute:YES];
            [self.agoraKit muteRemoteVideoStream:self.mutliCameraUid mute:YES];
        }];
    } else {
        AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
        videoCanvas.uid = self.mutliCameraUid;
        videoCanvas.view = nil;
        videoCanvas.renderMode = AgoraVideoRenderModeHidden;
        videoCanvas.sourceType = AgoraVideoSourceTypeCameraSecondary;
        [self.agoraKit setupLocalVideo:videoCanvas];
        [self.agoraKit stopCameraCapture:(AgoraVideoSourceTypeScreenSecondary)];
        [self.agoraKit leaveChannelEx:connection leaveChannelBlock:nil];
    }
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self.agoraKit disableAudio];
    [self.agoraKit disableVideo];
    [self.agoraKit stopPreview];
    [self.agoraKit leaveChannel:nil];
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        if (self.isOpenCamera) {
            [self.agoraKit stopCameraCapture:(AgoraVideoSourceTypeScreenSecondary)];
            NSString *channelName = [self.configs objectForKey:@"channelName"];
            AgoraRtcConnection * connection = [[AgoraRtcConnection alloc] initWithChannelId:channelName localUid:self.mutliCameraUid];
            [self.agoraKit leaveChannelEx:connection leaveChannelBlock:nil];
        }
        [AgoraRtcEngineKit destroy];
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
}

/// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
/// @param uid uid of remote joined user
/// @param reason reason why this user left, note this event may be triggered when the remote user
/// become an audience in live broadcasting profile
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    [LogUtil log:[NSString stringWithFormat:@"remote user left: %lu", uid] level:(LogLevelDebug)];
}

@end
