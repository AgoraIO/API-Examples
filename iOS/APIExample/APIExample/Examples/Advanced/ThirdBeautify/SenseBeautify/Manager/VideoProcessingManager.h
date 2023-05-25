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

- (void)setBuauty: (BOOL)isSelected;
- (void)setMakeup: (BOOL)isSelected;
- (void)setSticker: (BOOL)isSelected;
- (void)setFilter: (BOOL)isSelected;

- (CVPixelBufferRef)videoProcessHandler:(CVPixelBufferRef)pixelBuffer;

@end

NS_ASSUME_NONNULL_END
