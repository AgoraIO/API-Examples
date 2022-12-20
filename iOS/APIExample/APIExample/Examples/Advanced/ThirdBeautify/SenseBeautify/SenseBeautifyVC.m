//
//  FUBeautify.m
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/21.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "SenseBeautifyVC.h"
#import "EFMotionManager.h"
#import "VideoProcessingManager.h"
#import "EffectsProcess.h"
#import <AgoraRtcKit/AgoraRtcEngineKitEx.h>
#import "APIExample-Swift.h"

@interface SenseBeautifyVC () <AgoraRtcEngineDelegate, AgoraVideoFrameDelegate>

@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;
@property (weak, nonatomic) IBOutlet UIStackView *container;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UIView *remoteVideo;


@property (nonatomic, strong) VideoProcessingManager *videoProcessing;
@property (nonatomic, strong) AgoraRtcEngineKit *rtcEngineKit;

@end

@implementation SenseBeautifyVC

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
    NSString *licensePath = [[NSBundle mainBundle] pathForResource:@"SENSEME" ofType:@"lic"];
    BOOL isSuccess = [EffectsProcess authorizeWithLicensePath:licensePath];
    NSLog(@"isSuccess == %d", isSuccess);
    if (isSuccess) {
        [self initSDK];
    } else {
        [self.tipsLabel setHidden:NO];
        [self.container setHidden:YES];
        [self.tipsLabel setText:NSLocalizedString(@"license authorization failed, please check whether the license file is correct", nil)];
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
    
    self.rtcEngineKit = [AgoraRtcEngineKit sharedEngineWithAppId:KeyCenter.AppId delegate:self];
    // setup videoFrameDelegate
    [self.rtcEngineKit setVideoFrameDelegate:self];
    
    [self.rtcEngineKit enableVideo];
    [self.rtcEngineKit enableAudio];
    
    // add FaceUnity filter and add to process manager
    self.videoProcessing = [VideoProcessingManager new];
    
    // set up local video to render your local camera preview
    AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
    videoCanvas.uid = 0;
    // the view to be binded
    videoCanvas.view = self.localVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    videoCanvas.mirrorMode = AgoraVideoMirrorModeDisabled;
    [self.rtcEngineKit setupLocalVideo:videoCanvas];
    [self.rtcEngineKit startPreview];

    // set custom capturer as video source
    AgoraRtcChannelMediaOptions *option = [[AgoraRtcChannelMediaOptions alloc] init];
    option.clientRoleType = AgoraClientRoleBroadcaster;
    option.publishMicrophoneTrack = YES;
    option.publishCameraTrack = YES;
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

#pragma mark - VideoFrameDelegate
- (BOOL)onCaptureVideoFrame:(AgoraOutputVideoFrame *)videoFrame {
    CVPixelBufferRef pixelBuffer = [self.videoProcessing videoProcessHandler:videoFrame.pixelBuffer];
    videoFrame.pixelBuffer = pixelBuffer;
    return YES;
}

- (AgoraVideoFormat)getVideoFormatPreference{
    return AgoraVideoFormatCVPixelNV12;
}
- (AgoraVideoFrameProcessMode)getVideoFrameProcessMode{
    return AgoraVideoFrameProcessModeReadWrite;
}

- (BOOL)getMirrorApplied{
    return YES;
}

- (BOOL)getRotationApplied {
    return NO;
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
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    AgoraRtcVideoCanvas *videoCanvas = [AgoraRtcVideoCanvas new];
    videoCanvas.uid = uid;
    // the view to be binded
    videoCanvas.view = nil;
    [self.rtcEngineKit setupRemoteVideo:videoCanvas];
}

- (void)dealloc {
    [self.rtcEngineKit leaveChannel:nil];
    [self.rtcEngineKit stopPreview];
    [AgoraRtcEngineKit destroy];
}


@end
