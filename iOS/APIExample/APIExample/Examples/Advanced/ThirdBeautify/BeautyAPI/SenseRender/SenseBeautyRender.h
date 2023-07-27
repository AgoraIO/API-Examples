//
//  SenseBeatuyRender.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#define Sensetime "st_mobile_common.h"

#import <Foundation/Foundation.h>
#if __has_include("VideoProcessingManager.h") && __has_include(Sensetime)
#import "VideoProcessingManager.h"
#endif
#import "BeautyAPI.h"

NS_ASSUME_NONNULL_BEGIN

@interface SenseBeautyRender : NSObject<BeautyRenderDelegate>
@property (nonatomic, assign) BOOL isSuccessLicense;
#if __has_include(Sensetime)
@property (nonatomic, strong) VideoProcessingManager *videoProcessing;
#endif


@end

NS_ASSUME_NONNULL_END
