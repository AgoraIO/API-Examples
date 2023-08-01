//
//  JoinChannelVideo.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "StreamEncryption.h"
#import "KeyCenter.h"
#import <AgoraRtcKit/AgoraRtcKit.h>
#import "VideoView.h"
#import "APIExample_OC-swift.h"
#import "AgoraCustomEncryption.h"

@interface StreamEncryptionEntry ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UITextField *encryptSecretField;
@property (weak, nonatomic) IBOutlet UIButton *encryptModeBtn;
@property (nonatomic, assign) AgoraEncryptionMode mode;
@property (nonatomic, assign) BOOL useCustom;

@end

@implementation StreamEncryptionEntry

- (void)viewDidLoad {
    [super viewDidLoad];
    self.mode = AgoraEncryptionModeAES128GCM2;
    [self.encryptModeBtn setTitle:@"AES128GCM" forState:(UIControlStateNormal)];
}

- (IBAction)setEncryptionMode:(UIButton *)sender {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:@"Set Encryption Mode".localized message:nil preferredStyle:(UIAlertControllerStyleActionSheet)];
    NSDictionary *params = @{
        @"AES128GCM": @(AgoraEncryptionModeAES128GCM2),
        @"AES256GCM": @(AgoraEncryptionModeAES256GCM2)
    };
    NSArray *allKeys = params.allKeys;
    for (int i = 0; i < allKeys.count; i++) {
        NSString *title = allKeys[i];
        UIAlertAction *ok = [UIAlertAction actionWithTitle:title.localized style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            AgoraEncryptionMode mode = ((NSNumber *)params[title]).integerValue;
            self.mode = mode;
            self.useCustom = NO;
            [sender setTitle:title forState:(UIControlStateNormal)];
        }];
        [alertVC addAction:ok];
    }
    UIAlertAction *custom = [UIAlertAction actionWithTitle:@"Custom" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        self.useCustom = YES;
        [self.encryptModeBtn setTitle:@"Custom" forState:(UIControlStateNormal)];
    }];
    
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel".localized style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:custom];
    [alertVC addAction:cancel];
    [self presentViewController:alertVC animated:YES completion:nil];
}

- (IBAction)onClickJoinButton:(id)sender {
    [self.textField resignFirstResponder];
    [self.encryptSecretField resignFirstResponder];
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"StreamEncryption" bundle:nil];
    BaseViewController *newViewController = [storyBoard instantiateViewControllerWithIdentifier:@"StreamEncryption"];
    newViewController.configs = @{@"channelName": self.textField.text,
                                  @"mode": @(self.mode),
                                  @"secret": self.encryptSecretField.text,
                                  @"userCustom": @(self.useCustom)};
    [self.navigationController pushViewController:newViewController animated:YES];
}

@end


@interface StreamEncryption ()<AgoraRtcEngineDelegate>
@property (weak, nonatomic) IBOutlet UIView *containerView;
@property (nonatomic, strong)VideoView *localView;
@property (nonatomic, strong)VideoView *remoteView;
@property (nonatomic, strong)AgoraRtcEngineKit *agoraKit;

@end

@implementation StreamEncryption

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
    
    // set up agora instance when view loaded
    AgoraRtcEngineConfig *config = [[AgoraRtcEngineConfig alloc] init];
    config.appId = KeyCenter.AppId;
    config.channelProfile = AgoraChannelProfileLiveBroadcasting;
    
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithConfig:config delegate:self];
    
    NSString *channelName = [self.configs objectForKey:@"channelName"];
    NSString *secret = [self.configs objectForKey:@"secret"];
    AgoraEncryptionMode mode = ((NSNumber *)[self.configs objectForKey:@"mode"]).integerValue;
    BOOL useCustom = ((NSNumber *)[self.configs objectForKey:@"useCustom"]).boolValue;
    // make myself a broadcaster
    [self.agoraKit setClientRole:(AgoraClientRoleBroadcaster)];
    // enable video module and set up video encoding configs
    [self.agoraKit enableAudio];
    [self.agoraKit enableVideo];
    
    if (useCustom) {
        // your own custom algorithm encryption
        [AgoraCustomEncryption registerPacketProcessing:self.agoraKit];
        
    } else {
        AgoraEncryptionConfig *config = [[AgoraEncryptionConfig alloc] init];
        config.encryptionMode = mode;
        config.encryptionKey = secret;
        config.encryptionKdfSalt = [@"EncryptionKdfSaltInBase64Strings" dataUsingEncoding:(kCFStringEncodingUTF8)];
       int ret = [self.agoraKit enableEncryption:YES encryptionConfig:config];
        if (ret != 0) {
            // for errors please take a look at:
            // CN https://docs.agora.io/cn/live-streaming-premium-legacy/API%20Reference/oc/Classes/AgoraRtcChannel.html?platform=iOS#//api/name/enableEncryption:encryptionConfig:
            // EN https://docs.agora.io/en/video-calling/develop/media-stream-encryption#implement--media-stream-encryption
            [self showAlertWithTitle:@"Error" message:[NSString stringWithFormat:@"enableEncryption call failed: %d, please check your params", ret]];
        }
    }
    
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
            // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
    [self.agoraKit enableEncryption:NO encryptionConfig:[AgoraEncryptionConfig new]];
    BOOL useCustom = ((NSNumber *)[self.configs objectForKey:@"useCustom"]).boolValue;
    if (useCustom) {
        [AgoraCustomEncryption deregisterPacketProcessing:self.agoraKit];
    }
    [AgoraRtcEngineKit destroy];
}

/// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
/// to let user know something wrong is happening
/// Error code description can be found at:
/// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
/// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
