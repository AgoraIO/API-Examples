//
//  BytesBeautyRender.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#define BytesMoudle "bef_effect_ai_api.h"

#import <Foundation/Foundation.h>

#if __has_include(BytesMoudle) && __has_include("BEImageUtils.h") && __has_include("BEFrameProcessor.h")
#import "BEImageUtils.h"
#import "BEFrameProcessor.h"
#endif
#import "BeautyAPI.h"

NS_ASSUME_NONNULL_BEGIN

@interface BytesBeautyRender : NSObject<BeautyRenderDelegate>

#if __has_include(BytesMoudle)
@property (nonatomic, strong) BEFrameProcessor *frameProcessor;
@property (nonatomic, strong) BEImageUtils *imageUtils;
#endif

@end

NS_ASSUME_NONNULL_END
