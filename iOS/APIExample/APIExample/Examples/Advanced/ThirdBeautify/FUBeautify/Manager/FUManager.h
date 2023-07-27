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

//用于检测是否有ai人脸和人形
- (void)faceUnityManagerCheckAI;

@end

@interface FUManager : NSObject <VideoFilterDelegate>

@property (nonatomic, weak) id<FUManagerProtocol>delegate;

+ (FUManager *)shareManager;

/// 销毁全部道具
- (void)destoryItems;

/// 更新美颜磨皮效果（根据人脸检测置信度设置不同磨皮效果）
- (void)updateBeautyBlurEffect;

- (void)setBuauty: (BOOL)isSelected;
- (void)setMakeup: (BOOL)isSelected;
- (void)setSticker: (BOOL)isSelected;
- (void)setFilter: (BOOL)isSelected;

@end
