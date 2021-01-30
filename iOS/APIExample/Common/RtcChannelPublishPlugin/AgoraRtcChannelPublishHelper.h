//
//  AgoraRtcChannelPublishHelper.h
//  player_demo_apple
//
//  Created by zhanxiaochao on 2020/1/13.
//  Copyright © 2020 agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AgoraMediaPlayer/AgoraMediaPlayerKit.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>
NS_ASSUME_NONNULL_BEGIN
@protocol AgoraRtcChannelPublishHelperDelegate <NSObject>

@optional

/// Description of state of Mediaplayer's state
/// @param playerKit AgoraMediaPlayer
/// @param state AgoraMediaPlayerState
/// @param reason AgoraMediaPlayerStateReason
/// @param error AgoraMediaPlayerError
- (void)AgoraRtcChannelPublishHelperDelegate:(AgoraMediaPlayer *_Nonnull)playerKit
       didChangedToState:(AgoraMediaPlayerState)state
                   error:(AgoraMediaPlayerError)error;

/// callback of position
/// @param playerKit AgoraMediaPlayer
/// @param position position
- (void)AgoraRtcChannelPublishHelperDelegate:(AgoraMediaPlayer *_Nonnull)playerKit
    didChangedToPosition:(NSInteger)position;

/// callback of seek state
/// @param playerkit AgoraMediaPlayer
/// @param state Description of seek state
- (void)AgoraRtcChannelPublishHelperDelegate:(AgoraMediaPlayer *_Nonnull)playerKit
          didOccureEvent:(AgoraMediaPlayerEvent)state;

/// callback of SEI
/// @param playerkit AgoraMediaPlayer
/// @param data SEI's data
- (void)AgoraRtcChannelPublishHelperDelegate:(AgoraMediaPlayer *_Nonnull)playerKit
          didReceiveData:(NSString *)data
                  length:(NSInteger)length;

@end

@interface AgoraRtcChannelPublishHelper : NSObject

+(instancetype)shareInstance;
// 连接 MediaPlayer 到主版本 RTC SDK
- (void)registerRtcChannelPublishHelperDelegate:(id<AgoraRtcChannelPublishHelperDelegate>)delegate;
- (void)attachPlayerToRtc:(AgoraMediaPlayer *)playerKit RtcEngine:(AgoraRtcEngineKit *)rtcEngine enableVideoSource:(bool)enable;
- (void)enableOnlyLocalAudioPlay:(bool)isEnable;
// 启动/停止推送音频流到频道
- (void)publishAudio;
- (void)unpublishAudio;
// 启动/停止推送视频流到频道
- (void)publishVideo;
- (void)unpublishVideo;
// 调节推送到频道内音频流的音量
- (void)adjustPublishSignalVolume:(int)volume;
// 调节播放视频的声音
- (void)adjustPlayoutSignalVolume:(int)volume;
// 断开 MediaPlayer 和 RTC SDK 的关联
- (void)detachPlayerFromRtc;

@end

NS_ASSUME_NONNULL_END
