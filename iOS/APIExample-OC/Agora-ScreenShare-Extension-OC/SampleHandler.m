//
//  SampleHandler.m
//  Agora-ScreenShare-Extension-OC
//
//  Created by zhaoyongqiang on 2023/7/27.
//


#import "SampleHandler.h"

@interface SampleHandler ()

@end

@implementation SampleHandler

- (void)broadcastStartedWithSetupInfo:(NSDictionary<NSString *,NSObject *> *)setupInfo {
    // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    [super broadcastStartedWithSetupInfo:setupInfo];
}

- (void)broadcastPaused {
    // User has requested to pause the broadcast. Samples will stop being delivered.
    NSLog(@"broadcastPaused");
    [super broadcastPaused];
}

- (void)broadcastResumed {
    // User has requested to resume the broadcast. Samples delivery will resume.
    NSLog(@"broadcastResumed");
    [super broadcastResumed];
}

- (void)broadcastFinished {
    // User has requested to finish the broadcast.
    NSLog(@"broadcastFinished");
    [super broadcastFinished];
}

- (void)processSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType {
    [super processSampleBuffer:sampleBuffer withType:sampleBufferType];
}

@end
