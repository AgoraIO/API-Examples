//
//  SimpleAudioFilterManager.m
//  audioFilter
//
//  Created by xianing on 2021/7/27.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

#import "SimpleAudioFilterManager.h"
//#include "SimpleAudioFilterProvider.h"
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

static NSString *kVendorName = @"Simple.AudioFilter";

@implementation SimpleAudioFilterManager

+ (id)sharedInstance {
    static SimpleAudioFilterManager * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[SimpleAudioFilterManager alloc] init];
    });
    return sharedInstance;
}

+ (NSString *)vendorName {
  return kVendorName;
}

//- (SimpleAudioExtensionObject *)mediaFilterExtension {
//  SimpleAudioExtensionObject *obj = [SimpleAudioExtensionObject new];
//  obj.vendorName = kVendorName;
//  
////  if (_bdProvider) {
////    obj.mediaFilterProvider = _bdProvider;
////  } else {
////    obj.mediaFilterProvider = nil;
////  }
//  
//  return obj;
//}

- (void)loadPlugin {
    
}

- (int)setParameter:(NSString *)parameter {
//  if (_bdProcessor) {
//    std::string p([parameter UTF8String]);
//    return _bdProcessor->setParameters(p);
//  }
//
  return 0;
}

@end
