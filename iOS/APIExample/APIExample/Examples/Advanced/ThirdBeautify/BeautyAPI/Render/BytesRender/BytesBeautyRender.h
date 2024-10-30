//
//  BytesBeautyRender.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2023/6/30.
//

#define BytesMoudle <effect-sdk/bef_effect_ai_api.h>

#import <Foundation/Foundation.h>

#if __has_include("BEImageUtils.h") && __has_include("BEEffectManager.h")
#import "BEImageUtils.h"
#import "BEEffectManager.h"
#import "BEEffectResourceHelper.h"
#endif
#import "BeautyAPI.h"

NS_ASSUME_NONNULL_BEGIN

@interface BytesBeautyRender : NSObject<BeautyRenderDelegate>

#if __has_include("BEImageUtils.h") && __has_include("BEEffectManager.h")
@property (nonatomic, strong) BEEffectManager *effectManager;
@property (nonatomic, strong) BEImageUtils *imageUtils;
#endif
- (BOOL)checkLicense;
@end

NS_ASSUME_NONNULL_END
