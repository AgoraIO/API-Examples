//
//  EffectMacro.h
//  SenseMeEffects
//
//  Created by sunjian on 2021/9/17.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#ifndef EffectMacro_h
#define EffectMacro_h

#import <AVFoundation/AVFoundation.h>

typedef NS_ENUM(NSUInteger, EffectsType){
    EffectsTypePreview,
    EffectsTypePhoto,
    EffectsTypeVideo,
};

typedef enum : NSUInteger {
    EFDetectConfigModeOther = 0,
    EFDetectConfigModeItsMe,
} EFDetectConfigMode;

#define EFFECTS_LOG 1
#ifdef DEBUG
#if EFFECTS_LOG
#define EFFECTSLog(format , ...) NSLog((format) , ##__VA_ARGS__);
#else
#define EFFECTSLog(format , ...)
#endif
#else
#define EFFECTSLog(format , ...)
#endif

#ifdef DEBUG
#if EFFECTS_LOG
#define NSLog(format , ...) NSLog((format) , ##__VA_ARGS__);
#endif
#endif

#if EFFECTS_LOG
#define EFFECTSTIMELOG(key) double key = CFAbsoluteTimeGetCurrent();
#define EFFECTSTIMEPRINT(key , dsc) printf("%s\t%.1f \n" , dsc , (CFAbsoluteTimeGetCurrent() - key) * 1000);
#else
#define EFFECTSTIMELOG(key)
#define EFFECTSTIMEPRINT(key , dsc)
#endif

#endif /* EffectMacro_h */
