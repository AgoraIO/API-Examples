//
//  AgoraYUVImageSourcePush.h
//  APIExample
//
//  Created by 张乾泽 on 2020/11/6.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AgoraYUVImageSourcePushDelegate <NSObject>

-(void)onVideoFrame:(NSData*)buffer size:(CGSize)size rotation:(int)rotation;

@end

@interface AgoraYUVImageSourcePush : NSObject
@property(nonatomic, weak)id<AgoraYUVImageSourcePushDelegate> delegate;

-(void)startSource;
-(void)stopSource;
@end

NS_ASSUME_NONNULL_END
