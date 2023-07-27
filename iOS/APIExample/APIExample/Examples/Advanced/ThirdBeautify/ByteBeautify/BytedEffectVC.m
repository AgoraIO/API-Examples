//
//  FUBeautify.m
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/21.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "BytedEffectVC.h"
#import <AgoraRtcKit/AgoraRtcEngineKitEx.h>
#import "APIExample-Swift.h"
#import "BeautyAPI.h"
#import "BytesBeautyRender.h"

@interface BytedEffectVC () <AgoraRtcEngineDelegate>

@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;
@property (weak, nonatomic) IBOutlet UIView *container;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UIView *remoteVideo;

@property (nonatomic, strong) AgoraRtcEngineKit *rtcEngineKit;
@property (nonatomic, strong) BeautyAPI *beautyAPI;
@property (nonatomic, strong) BytesBeautyRender *bytesRender;

@end

@implementation BytedEffectVC
- (BeautyAPI *)beautyAPI {
    if (_beautyAPI == nil) {
        _beautyAPI = [[BeautyAPI alloc] init];
        [_beautyAPI enable:YES];
    }
    return _beautyAPI;
}

- (BytesBeautyRender *)bytesRender {
    if (_bytesRender == nil) {
        _bytesRender = [[BytesBeautyRender alloc] init];
    }
    return _bytesRender;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self initSDK];
}

- (void) initSDK {
#if __has_include("bef_effect_ai_api.h")
    [self.tipsLabel setHidden:YES];
    [self.container setHidden:NO];
#else
    [self.tipsLabel setHidden:NO];
    [self.container setHidden:YES];
#endif
    
    self.rtcEngineKit = [AgoraRtcEngineKit sharedEngineWithAppId:KeyCenter.AppId delegate:self];
    
    [self.rtcEngineKit enableVideo];
    [self.rtcEngineKit enableAudio];

    AgoraVideoEncoderConfiguration *encodeConfig = [[AgoraVideoEncoderConfiguration alloc] initWithSize:CGSizeMake(480, 640)
                                                                                              frameRate:(AgoraVideoFrameRateFps15)
                                                                                                bitrate:15
                                                                                        orientationMode:(AgoraVideoOutputOrientationModeFixedPortrait)
                                                                                             mirrorMode:(AgoraVideoMirrorModeDisabled)];
    [self.rtcEngineKit setVideoEncoderConfiguration:encodeConfig];
    
    BeautyConfig *config = [[BeautyConfig alloc] init];
    config.rtcEngine = self.rtcEngineKit;
    config.captureMode = CaptureModeAgora;
    config.beautyRender = self.bytesRender;
    [self.beautyAPI initialize:config];
    
    [self.beautyAPI setupLocalVideo:self.localVideo renderMode:AgoraVideoRenderModeHidden];
    [self.rtcEngineKit startPreview];
    
    // set custom capturer as video source
    AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
    option.clientRoleType = AgoraClientRoleBroadcaster;
    option.publishMicrophoneTrack = YES;
    option.publishCameraTrack = YES;
    option.autoSubscribeAudio = YES;
    option.autoSubscribeVideo = YES;
    [[NetworkManager shared] generateTokenWithChannelName:self.title uid:0 success:^(NSString * _Nullable token) {
        [self.rtcEngineKit joinChannelByToken:token
                                    channelId:self.title
                                          uid: 0
                                 mediaOptions:option
                                  joinSuccess:^(NSString * _Nonnull channel, NSUInteger uid, NSInteger elapsed) {
            NSLog(@"join channel success uid: %lu", uid);
        }];
    }];
}

- (IBAction)onTapSwitchCameraButton:(id)sender {
    [self.rtcEngineKit switchCamera];
    self.beautyAPI.isFrontCamera = !self.beautyAPI.isFrontCamera;
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
    AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = self.remoteVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    videoCanvas.mirrorMode = AgoraVideoMirrorModeDisabled;
    [self.rtcEngineKit setupRemoteVideo:videoCanvas];
    [self.remoteVideo setHidden:NO];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = nil;
    [self.rtcEngineKit setupRemoteVideo:videoCanvas];
    [self.remoteVideo setHidden:YES];
}

- (void)dealloc {
    [self.rtcEngineKit leaveChannel:nil];
    [self.rtcEngineKit stopPreview];
    [AgoraRtcEngineKit destroy];
    [self.beautyAPI destory];
}


@end
