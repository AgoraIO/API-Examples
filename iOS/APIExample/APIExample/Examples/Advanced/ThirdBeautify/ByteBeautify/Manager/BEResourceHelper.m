//
//  BEResourceHelper.m
//  BytedEffects
//
//  Created by QunZhang on 2019/10/22.
//  Copyright © 2019 ailab. All rights reserved.
//

#import "BEResourceHelper.h"
#import "BELicenseHelper.h"
#import "BundleUtil.h"
#import "Core.h"

static NSString *LICENSE_PATH = @"LicenseBag";
static NSString *COMPOSER_PATH = @"ComposeMakeup";
static NSString *FILTER_PATH = @"FilterResource";
static NSString *STICKER_PATH = @"StickerResource";
static NSString *MODEL_PATH = @"ModelResource";

static NSString *BUNDLE = @"bundle";

@interface BEResourceHelper () {
    NSString            *_licensePrefix;
    NSString            *_composerPrefix;
    NSString            *_filterPrefix;
    NSString            *_stickerPrefix;
}

@end

@implementation BEResourceHelper

- (NSString *)licensePath {
//    const char *license = [BELicenseHelper shareInstance].licensePath;
//    return [[NSString alloc]initWithCString:license encoding:(NSUTF8StringEncoding)];
    NSString *licenseName;
    if ([self.delegate respondsToSelector:@selector(licenseName)]) {
        licenseName = [self.delegate licenseName];
    } else {
        licenseName = [[NSString alloc] initWithCString:LICENSE_NAME encoding:NSUTF8StringEncoding];
    }
    if ([self.delegate respondsToSelector:@selector(licenseDirPath)]) {
        return [[self.delegate licenseDirPath] stringByAppendingString:licenseName];
    }
    if (!_licensePrefix) {
        _licensePrefix = [[NSBundle mainBundle] pathForResource:LICENSE_PATH ofType:BUNDLE];
    }
    return [_licensePrefix stringByAppendingString:licenseName];
}

- (NSString *)composerNodePath:(NSString *)nodeName {
    if ([self.delegate respondsToSelector:@selector(composerNodeDirPath)]) {
        return [self.delegate composerNodeDirPath];
    }
    if (!_composerPrefix) {
        NSBundle *budle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
        _composerPrefix = [[budle pathForResource:COMPOSER_PATH ofType:BUNDLE] stringByAppendingString:@"/ComposeMakeup/"];
    }
    if ([nodeName containsString:_composerPrefix]) {
        return nodeName;
    }
    return [_composerPrefix stringByAppendingString:nodeName];
}

- (NSString *)filterPath:(NSString *)filterName {
    if ([self.delegate respondsToSelector:@selector(filterDirPath)]) {
        return [[self.delegate filterDirPath] stringByAppendingString:filterName];
    }
    if (!_filterPrefix) {
        NSBundle *budle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
        _filterPrefix = [[budle pathForResource:FILTER_PATH ofType:BUNDLE] stringByAppendingFormat:@"/Filter/"];
    }
    return [_filterPrefix stringByAppendingString:filterName];
}

- (NSString *)stickerPath:(NSString *)stickerName {
    if ([self.delegate respondsToSelector:@selector(stickerDirPath)]) {
        return [[self.delegate stickerDirPath] stringByAppendingString:stickerName];
    }
    if (!_stickerPrefix) {
        NSBundle *budle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
        _stickerPrefix = [[budle pathForResource:STICKER_PATH ofType:BUNDLE] stringByAppendingString:@"/stickers/"];
    }
    return [_stickerPrefix stringByAppendingString:stickerName];
}

- (NSString *)modelPath:(NSString *)modelName {
    return [[self modelDirPath] stringByAppendingString:modelName];
}

- (NSString *)composerPath {
    if ([self.delegate respondsToSelector:@selector(composerDirPath)]) {
        return [self.delegate composerDirPath];
    }
    if (!_composerPrefix) {
        NSBundle *budle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
        _composerPrefix = [[budle pathForResource:COMPOSER_PATH ofType:BUNDLE] stringByAppendingString:@"/ComposeMakeup/"];
    }
    return [_composerPrefix stringByAppendingString:@"/composer"];
}

- (NSString *)modelDirPath {
    if ([self.delegate respondsToSelector:@selector(modelDirPath)]) {
        return [self.delegate modelDirPath];
    }
    NSBundle *budle = [BundleUtil bundleWithBundleName:@"ByteEffectLib" podName:@"bytedEffect"];
    return [budle pathForResource:MODEL_PATH ofType:BUNDLE];
}

@end
