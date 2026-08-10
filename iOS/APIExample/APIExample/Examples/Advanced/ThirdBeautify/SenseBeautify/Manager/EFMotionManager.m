//
//  EFMotionManager.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/15.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import "EFMotionManager.h"
#import <CoreMotion/CoreMotion.h>

@interface EFMotionManager ()
{
    BOOL _begin;
}
@end

@implementation EFMotionManager

+ (instancetype)sharedInstance{
    static EFMotionManager *manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[EFMotionManager alloc] init];
    });
    return manager;
}

#pragma mark - getter/setter
- (CMMotionManager *)motionManager{
    if (!_motionManager) {
        _motionManager = [[CMMotionManager alloc] init];
        _motionManager.accelerometerUpdateInterval = 0.5;
        _motionManager.deviceMotionUpdateInterval = 1 / 25.0;
    }
    return _motionManager;
}

- (void)start{
    if(_begin) return;
    _begin = YES;
    if ([self.motionManager isAccelerometerAvailable]) {
        [self.motionManager startAccelerometerUpdates];
    }
    if ([self.motionManager isDeviceMotionAvailable]) {
        [self.motionManager startDeviceMotionUpdates];
    }
}

- (void)stop{
    _begin = NO;
    [self.motionManager stopAccelerometerUpdates];
    [self.motionManager stopDeviceMotionUpdates];
}

@end
