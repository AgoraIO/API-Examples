//
//  AgoraSampleBufferRender.h
//  APIExample
//
//  Created by 胡润辰 on 2022/4/2.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AgoraSampleBufferRender : UIView

@property (nonatomic, readonly) AVSampleBufferDisplayLayer *displayLayer;

- (void)reset;

- (void)renderVideoData:(AgoraVideoDataFrame *_Nonnull)videoData;

@end

NS_ASSUME_NONNULL_END
