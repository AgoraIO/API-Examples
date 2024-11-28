//  BEGLUtils.m
// EffectsARSDK


#import "BEGLUtils.h"

@implementation BEGLUtils

+ (EAGLContext *)createContextWithDefaultAPI:(EAGLRenderingAPI)api {
    while (api != 0) {
        EAGLContext *context = [[EAGLContext alloc] initWithAPI:api];
        if (context != nil) {
            return context;
        }
        NSLog(@"not support api %lu, use lower api %lu", (unsigned long)api, [self be_lowerAPI:api]);
        api = [self be_lowerAPI:api];
    }
    return nil;
}

+ (EAGLContext *)createContextWithDefaultAPI:(EAGLRenderingAPI)api sharegroup:(EAGLSharegroup *)sharegroup {
    while (api != 0) {
        EAGLContext *context = [[EAGLContext alloc] initWithAPI:api sharegroup:sharegroup];
        if (context != nil) {
            return context;
        }
        NSLog(@"not support api %lu, use lower api %lu", (unsigned long)api, [self be_lowerAPI:api]);
        api = [self be_lowerAPI:api];
    }
    return nil;
}

+ (EAGLRenderingAPI)be_lowerAPI:(EAGLRenderingAPI)api {
    switch (api) {
        case kEAGLRenderingAPIOpenGLES3:
            return kEAGLRenderingAPIOpenGLES2;
        case kEAGLRenderingAPIOpenGLES2:
            return kEAGLRenderingAPIOpenGLES1;
        case kEAGLRenderingAPIOpenGLES1:
            return 0;
    }
    return 0;
}

@end
