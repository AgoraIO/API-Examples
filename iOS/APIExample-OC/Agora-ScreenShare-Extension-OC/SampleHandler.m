//
//  SampleHandler.m
//  Agora-ScreenShare-Extension-OC
//
//  Created by zhaoyongqiang on 2023/7/27.
//


#import "SampleHandler.h"

@interface SampleHandler ()<AgoraReplayKitExtDelegate>

@end

@implementation SampleHandler

- (void)broadcastStartedWithSetupInfo:(NSDictionary<NSString *,NSObject *> *)setupInfo {
    // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    [[AgoraReplayKitExt shareInstance] start:self];
}

- (void)broadcastPaused {
    // User has requested to pause the broadcast. Samples will stop being delivered.
    NSLog(@"broadcastPaused");
    [[AgoraReplayKitExt shareInstance] pause];
}

- (void)broadcastResumed {
    // User has requested to resume the broadcast. Samples delivery will resume.
    NSLog(@"broadcastResumed");
    [[AgoraReplayKitExt shareInstance] resume];
}

- (void)broadcastFinished {
    // User has requested to finish the broadcast.
    NSLog(@"broadcastFinished");
    [[AgoraReplayKitExt shareInstance] stop];
}

- (void)processSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType {
    [[AgoraReplayKitExt shareInstance] pushSampleBuffer:sampleBuffer withType:sampleBufferType];
}

- (void)broadcastFinished:(AgoraReplayKitExt * _Nonnull)broadcast reason:(AgoraReplayKitExtReason)reason {
    switch (reason) {
        case AgoraReplayKitExtReasonInitiativeStop:
        {
            //                NSDictionary *userInfo = @{NSLocalizedDescriptionKey : @"Host app stop srceen capture"};
            //                NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:0 userInfo:userInfo];
            //                [self finishBroadcastWithError:error];
            NSLog(@"AgoraReplayKitExtReasonInitiativeStop");
        }
            break;
        case AgoraReplayKitExtReasonConnectFail:
        {
            //                NSDictionary *userInfo = @{NSLocalizedDescriptionKey : @"Connect host app fail need startScreenCapture in host app"};
            //                NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:0 userInfo:userInfo];
            //                [self finishBroadcastWithError:error];
            NSLog(@"AgoraReplayKitExReasonConnectFail");
        }
            break;
        case AgoraReplayKitExtReasonDisconnect:
        {
            //                NSDictionary *userInfo = @{NSLocalizedDescriptionKey : @"disconnect with host app"};
            //                NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:0 userInfo:userInfo];
            //                [self finishBroadcastWithError:error];
            NSLog(@"AgoraReplayKitExReasonDisconnect");
        }
            break;
        default:
            break;
            
    }
}
- (void)beginRequestWithExtensionContext:(nonnull NSExtensionContext *)context {
    
}

@end
