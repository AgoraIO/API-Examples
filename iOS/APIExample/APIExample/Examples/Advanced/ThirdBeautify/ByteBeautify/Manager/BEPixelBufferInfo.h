//
//  BEPixelBufferInfo.h
//  BeautifyExample
//
//  Created by zhaoyongqiang on 2022/8/19.
//  Copyright © 2022 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

//   {zh} / 数据格式     {en} /Data format
typedef NS_ENUM(NSInteger, BEFormatType) {
    //    {zh} 未知格式        {en} Unknown format
    BE_UNKNOW,
    // 8bit R G B A
    BE_RGBA,
    // 8bit B G R A
    BE_BGRA,
    // video range, 8bit Y1 Y2 Y3 Y4... U1 V1...
    BE_YUV420V,
    // full range, 8bit Y1 Y2 Y3 Y4... U1 V1...
    BE_YUV420F
};


@interface BEPixelBufferInfo : NSObject

@property (nonatomic, assign) BEFormatType format;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) int bytesPerRow;

@end

NS_ASSUME_NONNULL_END
