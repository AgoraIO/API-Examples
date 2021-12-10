//
//  SimpleFilterManager.m
//
//  Created by xianing on 2021/7/27.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

#import "SimpleFilterManager.h"
#import <AgoraRtcKit/AgoraRtcEngineKit.h>
#import <AgoraRtcKit/AgoraExtensionProviderEntry.h>
#import "ExtensionProvider.hpp"

using namespace agora::extension;

// Register extension provider
// No need to use quotation marks for input parameter of PROVIDER_NAME
REGISTER_AGORA_EXTENSION_PROVIDER(Agora, agora::extension::ExtensionProvider, agora::rtc::IExtensionProvider);

static NSString *kVendorName = @"Agora";

bool enable = false;

@implementation SimpleFilterManager

+ (id)sharedInstance {
    static SimpleFilterManager * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[SimpleFilterManager alloc] init];
    });
    return sharedInstance;
}

+ (NSString *)vendorName {
  return kVendorName;
}

- (NSString * _Nonnull)vendor {
    return kVendorName;
}

@end
