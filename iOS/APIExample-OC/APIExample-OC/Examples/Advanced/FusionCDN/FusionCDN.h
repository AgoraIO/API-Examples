//
//  JoinChannelVideo.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "BaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface CDNChannelInfo : NSObject
@property(nonatomic, copy) NSString *channelName;
@property(nonatomic, assign)NSInteger index;
@end

@interface FusionCDNEntry : BaseViewController

@end

@interface FusionCDNHost: BaseViewController

@end

@interface FusionCDNAudience: BaseViewController

@end

NS_ASSUME_NONNULL_END
