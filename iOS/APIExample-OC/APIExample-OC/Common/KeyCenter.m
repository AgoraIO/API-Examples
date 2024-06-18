//
//  KeyCenter.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "KeyCenter.h"

static NSString * const APPID = <#YOUR APPID#>;
static NSString * const Certificate = <#YOUR Certificate#>;

@implementation KeyCenter

+ (nullable NSString *)AppId {
    return APPID;
}

+ (nullable NSString *)Certificate {
    return Certificate;
}

@end
