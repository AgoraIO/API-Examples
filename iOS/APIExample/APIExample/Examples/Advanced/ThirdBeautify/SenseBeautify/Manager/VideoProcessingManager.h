//
//  VideoProcessingManager.h
//  BeautifyExample
//
//  Created by zhaoyongqiang on 2022/6/28.
//  Copyright © 2022 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "EffectsProcess.h"

NS_ASSUME_NONNULL_BEGIN

@interface VideoProcessingManager : NSObject

@property (nonatomic, strong) EffectsProcess *effectsProcess;

- (void)setEffectType: (uint32_t)type value:(float)value;

- (CVPixelBufferRef)videoProcessHandler:(CVPixelBufferRef)pixelBuffer;

- (void)setStickerWithPath: (NSString *)stickerPath callBack:(void (^)(int))callback;

- (void)addStylePath: (NSString *)stylePath groupId: (int)groudId strength: (CGFloat)strength callBack:(void (^)(int))callback;

- (void)removeStickerId: (int)stickerId;

- (void)cleareStickers;

@end

NS_ASSUME_NONNULL_END
