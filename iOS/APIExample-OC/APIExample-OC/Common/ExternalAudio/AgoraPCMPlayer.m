//
//  AgoraPCMPlayer.m
//  APIExample-OC
//
//  Created by wushengtao on 2024/9/26.
//

#import "AgoraPCMPlayer.h"
@implementation AgoraPCMPlayer

- (instancetype)initWithSampleRate:(double)sampleRate channels:(AVAudioChannelCount)channels {
    self = [super init];
    if (self) {
        _sampleRate = sampleRate;
        _channels = channels;
        
        _audioEngine = [[AVAudioEngine alloc] init];
        _playerNode = [[AVAudioPlayerNode alloc] init];
        
        [_audioEngine attachNode:_playerNode];
        
        // 使用 AVAudioFormat 的 init 方法
        AVAudioFormat *format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:sampleRate channels:channels];
        [_audioEngine connect:_playerNode to:_audioEngine.mainMixerNode format:format];
        
        NSError *error = nil;
        // 使用 start 方法的无参数版本
        [_audioEngine startAndReturnError:&error];
        if (error) {
            NSLog(@"Audio Engine failed to start: %@", error);
        }
    }
    return self;
}

- (void)playPCMData:(UInt8 *)pcmData count:(NSUInteger)count {
    AVAudioFormat *format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:self.sampleRate channels:self.channels];
    AVAudioPCMBuffer *audioBuffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:(AVAudioFrameCount)(count / 4)]; // 16-bit stereo, 4 bytes per frame
    
    if (!audioBuffer.floatChannelData) {
        return;
    }
    
    audioBuffer.frameLength = (AVAudioFrameCount)(count / 4);
    
    for (NSUInteger frame = 0; frame < audioBuffer.frameLength; frame++) {
        int16_t leftSample = pcmData[frame * 4] | (pcmData[frame * 4 + 1] << 8);
        int16_t rightSample = pcmData[frame * 4 + 2] | (pcmData[frame * 4 + 3] << 8);
        
        audioBuffer.floatChannelData[0][frame] = (float)leftSample / INT16_MAX;
        audioBuffer.floatChannelData[1][frame] = (float)rightSample / INT16_MAX;
    }
    
    [_playerNode scheduleBuffer:audioBuffer completionHandler:nil];
    [_playerNode play];
}

@end
