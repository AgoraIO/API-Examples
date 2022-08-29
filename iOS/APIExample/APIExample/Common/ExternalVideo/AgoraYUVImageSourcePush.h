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

-(void)onVideoFrame:(CVPixelBufferRef)buffer
               size:(CGSize)size
          trackId: (NSUInteger)trackId
           rotation:(int)rotation;

@end

@interface AgoraYUVImageSourcePush : NSObject
- (instancetype)initWithSize: (CGSize)size fileName: (NSString *)fileName frameRate: (int)frameRate;
@property(nonatomic, assign)UInt32 trackId;
@property(nonatomic, weak)id<AgoraYUVImageSourcePushDelegate> delegate;

-(void)startSource;
-(void)stopSource;
@end


NS_ASSUME_NONNULL_END
