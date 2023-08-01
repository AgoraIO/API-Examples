//
//  KeyCenter.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "KeyCenter.h"

static NSString * const APPID = @"aab8b8f5a8cd4469a63042fcfafe7063";
static NSString * const Certificate = nil;

@implementation KeyCenter

+ (nullable NSString *)AppId {
    return APPID;
}

+ (nullable NSString *)Certificate {
    return Certificate;
}

@end
