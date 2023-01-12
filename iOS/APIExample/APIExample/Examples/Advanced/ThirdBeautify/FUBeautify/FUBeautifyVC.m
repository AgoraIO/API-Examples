//
//  FUBeautify.m
//  APIExample
//
//  Created by zhaoyongqiang on 2022/10/21.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "FUBeautifyVC.h"
#import "FUManager.h"
#import <AgoraRtcKit/AgoraRtcEngineKitEx.h>
#import "APIExample-Swift.h"

@interface FUBeautifyVC () <AgoraRtcEngineDelegate, AgoraVideoFrameDelegate>

@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;
@property (weak, nonatomic) IBOutlet UIStackView *container;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UIView *remoteVideo;


@property (nonatomic, strong) FUManager *videoFilter;
@property (nonatomic, strong) AgoraRtcEngineKit *rtcEngineKit;

@end

@implementation FUBeautifyVC

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self initSDK];
}

- (void) initSDK {
#if __has_include(<FURenderKit/FURenderKit.h>)
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
    self.videoFilter = [FUManager shareManager];
    
    // add Sticker
    [self.videoFilter setSticker:@"fashi"];
    
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
                                          uid:0
                                 mediaOptions:option
                                  joinSuccess:^(NSString * _Nonnull channel, NSUInteger uid, NSInteger elapsed) {
            NSLog(@"join channel success uid: %lu", uid);
        }];
    }];
}

#pragma mark - VideoFrameDelegate
- (BOOL)onCaptureVideoFrame:(AgoraOutputVideoFrame *)videoFrame {
    CVPixelBufferRef pixelBuffer = [self.videoFilter processFrame:videoFrame.pixelBuffer];
    videoFrame.pixelBuffer = pixelBuffer;
    return YES;
}

- (AgoraVideoFormat)getVideoFormatPreference{
    return AgoraVideoFormatCVPixelBGRA;
}
- (AgoraVideoFrameProcessMode)getVideoFrameProcessMode{
    return AgoraVideoFrameProcessModeReadWrite;
}

- (BOOL)getMirrorApplied{
    return NO;
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
