//
//  AgoraPCMSourcePush.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/21.
//

#import "AgoraPCMSourcePush.h"

typedef NS_ENUM(NSInteger, AgoraPCMSourcePushState) {
    AgoraPCMSourcePushStatePlay = 0,
    AgoraPCMSourcePushStateStop = 1
};

@interface AgoraPCMSourcePush ()

@property(nonatomic, copy) NSString *filePath;
@property(nonatomic, assign) NSInteger sampleRate;
@property(nonatomic, assign) NSInteger bitPerSamples;
@property(nonatomic, assign) NSInteger samples;
@property(nonatomic, assign) NSInteger channelsPerFrame;
@property(nonatomic, assign) AgoraPCMSourcePushState state;
@property(nonatomic, strong) dispatch_queue_t captureQueue;

@end

@implementation AgoraPCMSourcePush

- (nonnull instancetype)initWithDelegate:(nonnull id<AgoraPcmSourceDelegate>)delegate
                                filePath:(nonnull NSString *)filePath
                              sampleRate:(NSInteger)sampleRate
                        channelsPerFrame:(NSInteger)channelsPerFrame
                           bitPerSamples:(NSInteger)bitPerSamples
                                 samples:(NSInteger)samples {
    if (self == [super init]) {
        self.state = AgoraPCMSourcePushStateStop;
        self.delegate = delegate;
        self.filePath = filePath;
        self.channelsPerFrame = channelsPerFrame;
        self.bitPerSamples = bitPerSamples;
        self.samples = samples;
        self.sampleRate = sampleRate;
        self.captureQueue = dispatch_queue_create("MyCaptureQueue", NULL);
    }
    return self;
}

- (void)start {
    if (self.state == AgoraPCMSourcePushStateStop) {
        self.state = AgoraPCMSourcePushStatePlay;
        [self play];
    }
}

- (void)stop {
    if (self.state == AgoraPCMSourcePushStatePlay) {
        self.state = AgoraPCMSourcePushStateStop;
    }
}

- (void)play {
    dispatch_async(self.captureQueue, ^{
        NSInputStream *input = [[NSInputStream alloc] initWithFileAtPath:self.filePath];
        [input open];
        
        NSInteger bufferSize = self.samples * self.bitPerSamples / 8 * self.channelsPerFrame;
        NSTimeInterval audioIntervalPreCall = (NSTimeInterval)self.samples / (NSTimeInterval)self.sampleRate;
        uint8_t *buffer = malloc(bufferSize * sizeof(uint8_t));
        NSInteger index = 0;
        double firstPush = 0;
        
        while (input.hasBytesAvailable && self.state == AgoraPCMSourcePushStatePlay) {
            if (firstPush == 0) {
                firstPush = CFAbsoluteTimeGetCurrent();
            }
            [input read:buffer maxLength:bufferSize];
            if ([self.delegate respondsToSelector:@selector(onAudioFrame:)]) {
                [self.delegate onAudioFrame:buffer];
            }
            index += 1;
            NSInteger nextPush = firstPush + index * audioIntervalPreCall;
            double sleep = nextPush - CFAbsoluteTimeGetCurrent();
            [NSThread sleepForTimeInterval:sleep];
        }
        if (input) {
            free(buffer);
            [input close];
        }
    });
}

@end
