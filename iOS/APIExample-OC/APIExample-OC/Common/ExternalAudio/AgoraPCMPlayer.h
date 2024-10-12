//
//  AgoraPCMPlayer.h
//  APIExample-OC
//
//  Created by wushengtao on 2024/9/26.
//

#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AgoraPCMPlayer : NSObject

@property (nonatomic, strong) AVAudioEngine *audioEngine;
@property (nonatomic, strong) AVAudioPlayerNode *playerNode;
@property (nonatomic) double sampleRate;
@property (nonatomic) AVAudioChannelCount channels;

- (instancetype)initWithSampleRate:(double)sampleRate channels:(AVAudioChannelCount)channels;
- (void)playPCMData:(UInt8 *)pcmData count:(NSUInteger)count;

@end

NS_ASSUME_NONNULL_END
