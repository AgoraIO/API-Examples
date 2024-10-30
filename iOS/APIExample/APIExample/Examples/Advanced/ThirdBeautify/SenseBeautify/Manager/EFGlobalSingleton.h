//
//  EFGlobalSingleton.h
//  SenseMeEffects
//
//  Created by 马浩萌 on 2021/11/30.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import <Foundation/Foundation.h>

static NSString * const EFGlobalSingletonMaleKey = @"EFGlobalSingletonMaleKey";

@interface EFGlobalSingleton : NSObject

@property (nonatomic, assign) int efTouchTriggerAction; // 点击屏幕触发事件保存
@property (nonatomic, assign) BOOL efHasSegmentCapability; // 标识是否有皮肤分割capability
@property (nonatomic, assign) BOOL isMale; // 标识当前用户性别（不同默认参数）
@property (nonatomic, assign) BOOL needDelay; // 是否是tryon试鞋/试表需要开启未来帧
@property (nonatomic, assign) BOOL isTryonShoes; // 是否是tryon试鞋(光脚提示)
@property (nonatomic, assign) BOOL isPortraitOnly; // 是否是gan image（只支持横屏）

+(instancetype)sharedInstance;
-(instancetype)init NS_UNAVAILABLE;
+(instancetype)new NS_UNAVAILABLE;
+(instancetype)alloc NS_UNAVAILABLE;

@end

