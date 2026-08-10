//
//  FUBeautify.m
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/21.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "SenseBeautifyVC.h"
#import <AgoraRtcKit/AgoraRtcEngineKitEx.h>
#import "APIExample-Swift.h"
#import "BeautyAPI.h"
#import "SenseBeautyRender.h"

@interface SenseBeautifyVC () <AgoraRtcEngineDelegate>

@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;
@property (weak, nonatomic) IBOutlet UIView *container;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UIView *remoteVideo;

@property (nonatomic, strong) AgoraRtcEngineKit *rtcEngineKit;
@property (nonatomic, strong) BeautyAPI *beautyAPI;
@property (nonatomic, strong) SenseBeautyRender *senseRender;
@property (nonatomic, assign) BOOL didCleanup;

@end

@implementation SenseBeautifyVC
- (BeautyAPI *)beautyAPI {
    if (_beautyAPI == nil) {
        _beautyAPI = [[BeautyAPI alloc] init];
        [_beautyAPI enable:YES];
    }
    return _beautyAPI;
}

- (SenseBeautyRender *)senseRender {
    if (_senseRender == nil) {
        _senseRender = [[SenseBeautyRender alloc] init];
    }
    return _senseRender;
}

- (void)viewDidLoad {
    [super viewDidLoad];
#if __has_include("st_mobile_common.h")
    [self setupSenseArService];
#else
    [self.tipsLabel setHidden:NO];
    [self.container setHidden:YES];
#endif
}

- (void)setupSenseArService {
    if (self.senseRender.isSuccessLicense) {
        [self initSDK];
    } else {
        [self.tipsLabel setHidden:NO];
        [self.container setHidden:YES];
        [self.tipsLabel setText:NSLocalizedString(@"license authorization failed, please check whether the license file is correct", nil)];
        [self performSelector:@selector(setupSenseArService) withObject:nil afterDelay:1];
    }
}

- (void) initSDK {
#if __has_include("st_mobile_common.h")
    [self.tipsLabel setHidden:YES];
    [self.container setHidden:NO];
#else
    [self.tipsLabel setHidden:NO];
    [self.container setHidden:YES];
#endif

    AgoraRtcEngineConfig *rtcConfig = [[AgoraRtcEngineConfig alloc] init];
    rtcConfig.appId = KeyCenter.AppId;
    rtcConfig.channelProfile = AgoraChannelProfileLiveBroadcasting;
    self.rtcEngineKit = [AgoraRtcEngineKit sharedEngineWithConfig:rtcConfig delegate:self];

    [self.rtcEngineKit enableVideo];
    [self.rtcEngineKit enableAudio];

    AgoraVideoEncoderConfiguration *encodeConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(480, 640)
                                                                                              frameRate:(AgoraVideoFrameRateFps15)
                                                                                                bitrate:AgoraVideoBitrateStandard
                                                                                        orientationMode:(AgoraVideoOutputOrientationModeFixedPortrait)
                                                                                             mirrorMode:(AgoraVideoMirrorModeDisabled)];
    [self.rtcEngineKit setVideoEncoderConfiguration:encodeConfig];

    BeautyConfig *config = [[BeautyConfig alloc] init];
    config.rtcEngine = self.rtcEngineKit;
    config.captureMode = CaptureModeAgora;
    config.beautyRender = self.senseRender;
    [self.beautyAPI initialize:config];

    [self.beautyAPI setupLocalVideo:self.localVideo renderMode:AgoraVideoRenderModeHidden];
    [self.rtcEngineKit startPreview];

    // set custom capturer as video source
    AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
    option.clientRoleType = AgoraClientRoleBroadcaster;
    option.channelProfile = AgoraChannelProfileLiveBroadcasting;
    option.publishMicrophoneTrack = YES;
    option.publishCameraTrack = YES;
    option.autoSubscribeAudio = YES;
    option.autoSubscribeVideo = YES;
    __weak typeof(self) weakSelf = self;
    [[NetworkManager shared] generateTokenWithChannelName:self.title uid:0 success:^(NSString * _Nullable token) {
        __strong typeof(weakSelf) strongSelf = weakSelf;
        if (strongSelf == nil || strongSelf.didCleanup) {
            return;
        }
        [strongSelf.rtcEngineKit joinChannelByToken:token
                                    channelId:strongSelf.title
                                          uid: 0
                                 mediaOptions:option
                                  joinSuccess:^(NSString * _Nonnull channel, NSUInteger uid, NSInteger elapsed) {
            NSLog(@"join channel success uid: %lu", uid);
        }];
    }];
}

- (IBAction)onTapSwitchCameraButton:(id)sender {
    [self.beautyAPI switchCamera];
}
- (IBAction)onTapBeautyButton:(UIButton *)sender {
    [sender setSelected:!sender.isSelected];
    if (sender.isSelected) {
        [self.beautyAPI setBeautyPreset:(BeautyPresetModeDefault)];
    } else {
        [self.beautyAPI.beautyRender reset];
    }
}
- (IBAction)onTapMakeupButton:(UIButton *)sender {
    [sender setSelected:!sender.isSelected];
    [self.beautyAPI.beautyRender setMakeup:sender.isSelected];
}
- (IBAction)onTapStickerButton:(UIButton *)sender {
    [sender setSelected:!sender.isSelected];
    [self.beautyAPI.beautyRender setSticker:sender.isSelected];
}


#pragma mark - RtcEngineDelegate
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    dispatch_async(dispatch_get_main_queue(), ^{
        AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
        videoCanvas.uid = uid;
        videoCanvas.view = self.remoteVideo;
        videoCanvas.renderMode = AgoraVideoRenderModeHidden;
        videoCanvas.mirrorMode = AgoraVideoMirrorModeDisabled;
        [self.rtcEngineKit setupRemoteVideo:videoCanvas];
        [self.remoteVideo setHidden:NO];
    });
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    dispatch_async(dispatch_get_main_queue(), ^{
        AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
        videoCanvas.uid = uid;
        videoCanvas.view = nil;
        [self.rtcEngineKit setupRemoteVideo:videoCanvas];
        [self.remoteVideo setHidden:YES];
    });
}

- (void)willMoveToParentViewController:(UIViewController *)parent {
    [super willMoveToParentViewController:parent];
    if (parent == nil) {
        [self cleanup];
    }
}

- (void)cleanup {
    if (self.didCleanup) {
        return;
    }
    self.didCleanup = YES;
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(setupSenseArService) object:nil];
    if (_beautyAPI != nil) {
        [_beautyAPI destroy];
        _beautyAPI = nil;
    } else if (_senseRender != nil) {
        [_senseRender destroy];
    }
    _senseRender = nil;
    if (_rtcEngineKit != nil) {
        [_rtcEngineKit stopPreview];
        [_rtcEngineKit leaveChannel:nil];
        [AgoraRtcEngineKit destroy];
        _rtcEngineKit = nil;
    }
}

- (void)dealloc {
    [self cleanup];
}


@end
