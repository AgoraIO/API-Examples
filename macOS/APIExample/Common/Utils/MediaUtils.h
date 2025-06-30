//
//  MediaUtils.h
//  APIExample
//
//  Created by Arlin on 2022/4/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

//#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <CoreImage/CoreImage.h>

NS_ASSUME_NONNULL_BEGIN

@interface MediaUtils : NSObject

+ (CVPixelBufferRef)i420ToPixelBuffer:(void *)srcY srcU:(void *)srcU srcV:(void *)srcV width:(int)width height:(int)height;

+ (NSImage *)pixelBufferToImage:(CVPixelBufferRef)pixelBuffer withRotationDegrees:(CGFloat)angleInDegrees;

+ (NSData *)dataFromPixelBuffer:(CVPixelBufferRef)pixelBuffer;

+ (nullable NSImage *)i420ToImage:(nullable void *)srcY srcU:(nullable void *)srcU srcV:(nullable void *)srcV width:(int)width height:(int)height;

@end

NS_ASSUME_NONNULL_END
