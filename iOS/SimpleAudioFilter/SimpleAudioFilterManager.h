//
//  SimpleAudioFilterManager.h
//  audioFilter
//
//  Created by xianing on 2021/7/27.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import <AgoraRtcKit/AgoraMediaFilterExtensionDelegate.h>

NS_ASSUME_NONNULL_BEGIN

//@class SimpleAudioExtensionObject;

@interface SimpleAudioFilterManager : NSObject
+ (instancetype)sharedInstance;

+ (NSString * __nonnull)vendorName;
//- (SimpleAudioExtensionObject * __nonnull)mediaFilterExtension;
- (void)loadPlugin;
- (int)setParameter:(NSString * __nullable)parameter;
@end

NS_ASSUME_NONNULL_END

//NS_ASSUME_NONNULL_BEGIN
//
//@interface SimpleAudioExtensionObject : NSObject <AgoraMediaFilterExtensionDelegate>
//@property (copy, nonatomic) NSString * __nonnull vendorName;
//@property (assign, nonatomic) id<AgoraExtProviderDelegate> __nullable mediaFilterProvider;
//
//@end
//
//NS_ASSUME_NONNULL_END
