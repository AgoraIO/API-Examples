//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "JoinChannelVideoToken.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"

@interface JoinChannelVideoTokenEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UITextField *appIdTextField;
@property (weak, nonatomic) IBOutlet UITextField *tokenTextField;

@end

@implementation JoinChannelVideoTokenEntry

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)onClickTipsButton:(id)sender {
    [self showAlertWithTitle:@"Quick input APPID and Token methods".localized
                     message:@"I: the mobile phone and Mac log in to the same Apple account. After copying the Mac, it will automatically synchronize other terminals with the same account. The mobile phone can directly click the input box to paste.\n\n II: use https://cl1p.net/ online clipboard:\n\n1.Enter in a URL that starts with cl1p.net. Example cl1p.net/uqztgjnqcalmd\n\n2.Paste in anything you want.\n\n3.On another computer enter the same URL and get your stuff.".localized
               textAlignment:(NSTextAlignmentLeft)];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    
    if (self.appIdTextField.text.length <= 0) {
        [ToastView showWithText:@"please input AppId!".localized postion:ToastViewPostionCenter];
        return;
    }
    if (self.tokenTextField.text.length <= 0) {
        [ToastView showWithText:@"please input Token!".localized postion:ToastViewPostionCenter];
        return;
    }
    if (self.textField.text.length <= 0) {
        [ToastView showWithText:@"please input channel name!".localized postion:ToastViewPostionCenter];
        return;
    }
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"JoinChannelVideoToken" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"JoinChannelVideoToken"];
    newViewController.configs = @{@"channelName": self.textField.text,
                                  @"appId": self.appIdTextField.text,
                                  @"token": self.tokenTextField.text};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface JoinChannelVideoToken ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;

@end

@implementation JoinChannelVideoToken

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
    [self.localView setPlaceholder:@"Local Host".localized];
    [self.remoteView setPlaceholder:@"Remote Host".localized];
    [self.containerView layoutStream:@[self.localView, self.remoteView]];
    
    NSString *channelName = [[self.configs objectForKey:@"channelName"] stringByTrimmingCharactersInSet:
    [NSCharacterSet whitespaceCharacterSet]];
    NSString *appId = [[self.configs objectForKey:@"appId"] stringByTrimmingCharactersInSet:
                       [NSCharacterSet whitespaceCharacterSet]];
    NSString *token = [[self.configs objectForKey:@"token"] stringByTrimmingCharactersInSet:
                       [NSCharacterSet whitespaceCharacterSet]];
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = appId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
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
    
    int result = [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:options joinSuccess:nil];
    if (result != 0) {
        // Usually happens with invalid parameters
        // Error code description can be found at:
        // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
        // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
        NSLog(@"joinChannel call failed: %d, please check your params", result);
    }
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
}

@end
