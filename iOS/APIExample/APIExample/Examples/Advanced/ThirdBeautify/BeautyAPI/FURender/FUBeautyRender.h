//
//  FUBeautyRender.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#define FURenderMoudle <FURenderKit/FURenderKit.h>

#import <Foundation/Foundation.h>
#if __has_include("FUManager.h") && __has_include(<FURenderKit/FURenderKit.h>)
#import "FUManager.h"
#import <FURenderKit/FURenderKit.h>
#endif

#import "BeautyAPI.h"

NS_ASSUME_NONNULL_BEGIN

@interface FUBeautyRender : NSObject <BeautyRenderDelegate>

#if __has_include(FURenderMoudle)
@property (nonatomic, strong) FUManager *fuManager;
#endif

@end

NS_ASSUME_NONNULL_END
