//
//  EFMotionManager.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/15.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>

NS_ASSUME_NONNULL_BEGIN

@interface EFMotionManager : NSObject

@property (nonatomic, strong) CMMotionManager *motionManager;

+ (instancetype)sharedInstance;

- (void)start;

- (void)stop;

@end

NS_ASSUME_NONNULL_END
