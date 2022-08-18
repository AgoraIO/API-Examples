//
//  MediaUtils.h
//  APIExample
//
//  Created by Arlin on 2022/4/12.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MediaUtils : NSObject

+ (nullable UIImage *)i420ToImage:(nullable void *)srcY srcU:(nullable void *)srcU srcV:(nullable void *)srcV width:(int)width height:(int)height;
+ (nullable UIImage *)pixelBufferToImage:(CVPixelBufferRef)pixelBuffer;

@end

NS_ASSUME_NONNULL_END
