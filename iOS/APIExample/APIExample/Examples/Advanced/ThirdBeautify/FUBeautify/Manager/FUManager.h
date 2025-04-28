//
//  FUManager.h
//  FULiveDemo
//
//  Created by 刘洋 on 2017/8/18.
//  Copyright © 2017年 刘洋. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@protocol VideoFilterDelegate <NSObject>

- (CVPixelBufferRef)processFrame:(CVPixelBufferRef)frame;

@end

@protocol FUManagerProtocol <NSObject>

/// Used to detect if there is an AI face and human shape
- (void)faceUnityManagerCheckAI;

@end

@interface FUManager : NSObject <VideoFilterDelegate>

@property(nonatomic, weak) id<FUManagerProtocol> delegate;

+ (FUManager *)shareManager;

/// Destroy all items
- (void)destoryItems;

/// Update beauty blur effect (set different blur effects based on face detection confidence)
- (void)updateBeautyBlurEffect;

- (void)setBuauty:(BOOL)isSelected;
- (void)setMakeup:(BOOL)isSelected;
- (void)setSticker:(BOOL)isSelected;
- (void)setFilter:(BOOL)isSelected;

@end
