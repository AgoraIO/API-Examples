//
//  EFGlobalSingleton.h
//  SenseMeEffects
//
//  Created by 马浩萌 on 2021/11/30.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import <Foundation/Foundation.h>

static NSString *const EFGlobalSingletonMaleKey = @"EFGlobalSingletonMaleKey";

@interface EFGlobalSingleton : NSObject

@property(nonatomic, assign) int efTouchTriggerAction;    // Click screen trigger event save
@property(nonatomic, assign) BOOL efHasSegmentCapability; // Identify if there is skin segmentation capability
@property(nonatomic, assign) BOOL isMale;                 // Identify current user gender (different default parameters)
@property(nonatomic, assign) BOOL needDelay;              // Whether tryon shoes/watches needs to enable future frames
@property(nonatomic, assign) BOOL isTryonShoes;           // Whether tryon shoes (shoe tip)
@property(nonatomic, assign) BOOL isPortraitOnly;         // Whether gan image (only supports landscape)

+ (instancetype)sharedInstance;
- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
+ (instancetype)alloc NS_UNAVAILABLE;

@end
