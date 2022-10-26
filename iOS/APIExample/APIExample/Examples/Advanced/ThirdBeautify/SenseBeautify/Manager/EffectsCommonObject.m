//
//  EffectsTracker.m
//  SenseMeEffects
//
//  Created by sunjian on 2021/7/19.
//  Copyright © 2021 SenseTime. All rights reserved.
//

#import "EffectsCommonObject.h"

@interface EffectsCommonObject ()
{
#if __has_include("st_mobile_common.h")
    st_handle_t _hTrackerHandle;
    st_rect_t _rect;
#endif
    BOOL _bTracking;
}
@end

@implementation EffectsCommonObject
- (void)dealloc{
#if __has_include("st_mobile_common.h")
    if (_hTrackerHandle) {
        st_mobile_object_tracker_destroy(_hTrackerHandle);
        _hTrackerHandle = NULL;
    }
#endif
}

- (instancetype)init{
    if ((self = [super init])) {
        [self createHandler];
    }
    return self;
}

- (void)createHandler{
#if __has_include("st_mobile_common.h")
    st_result_t ret = st_mobile_object_tracker_create(&_hTrackerHandle);
    if (ret != ST_OK) {
        NSLog(@"st_mobile_object_tracker_create error %d", ret);
    }
#endif
}


#if __has_include("st_mobile_common.h")
- (void)setObjectRect:(st_rect_t)rect{
    _rect = rect;
    _bTracking = YES;
}
- (st_result_t)detectObjectWithPixelbuffer:(CVPixelBufferRef)pixelBuffer
                                      rect:(st_rect_t*)rect
                                     score:(float*)score
{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    unsigned char* pixelData = (unsigned char*)CVPixelBufferGetBaseAddress(pixelBuffer);
    int iBytesPerRow = (int)CVPixelBufferGetBytesPerRow(pixelBuffer);
    int iWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int iHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    st_result_t state = ST_OK;
    if (_bTracking) {
        [self setObjectWithBuffer:pixelData
                      pixelFormat:ST_PIX_FMT_BGRA8888
                            width:iWidth
                           height:iHeight
                           stride:iBytesPerRow
                             rect:&_rect];
        _bTracking = NO;
    }
    [self detectObjectWithBuffer:pixelData
                     pixelFormat:ST_PIX_FMT_BGRA8888
                           width:iWidth
                          height:iHeight 
                          stride:iBytesPerRow
                            rect:rect
                           score:score];
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return state;
}


- (st_result_t)setObjectWithBuffer:(unsigned char *)buffer
                       pixelFormat:(st_pixel_format)pixelFormat
                             width:(int)width
                            height:(int)height
                            stride:(int)stride
                              rect:(st_rect_t*)rect
{
    st_result_t iRet = st_mobile_object_tracker_set_target(_hTrackerHandle,
                                                           buffer,
                                                           pixelFormat,
                                                           width,
                                                           height,
                                                           stride,
                                                           rect);
    return iRet;
}

- (st_result_t)detectObjectWithBuffer:(unsigned char *)buffer
                          pixelFormat:(st_pixel_format)pixelFormat
                                width:(int)width
                               height:(int)height
                               stride:(int)stride
                                 rect:(st_rect_t *)rect
                                score:(nonnull float *)result_score
{
    if (_bTracking) {
        [self setObjectWithBuffer:buffer
                      pixelFormat:pixelFormat
                            width:width
                           height:height
                           stride:stride
                             rect:&_rect];
        _bTracking = NO;
    }
    EFFECTSTIMELOG(key)
    st_result_t iRet = st_mobile_object_tracker_track(_hTrackerHandle,
                                                      buffer,
                                                      pixelFormat,
                                                      width,
                                                      height,
                                                      stride,
                                                      rect,
                                                      result_score);
    EFFECTSTIMEPRINT(key, "st_mobile_object_tracker_track");
    return iRet;
}
#endif
@end
