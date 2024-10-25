//  BEGLUtils.h
// EffectsARSDK


#ifndef BEGLUtils_h
#define BEGLUtils_h

#import <OpenGLES/EAGL.h>

@interface BEGLUtils : NSObject

+ (EAGLContext *)createContextWithDefaultAPI:(EAGLRenderingAPI)api;

+ (EAGLContext *)createContextWithDefaultAPI:(EAGLRenderingAPI)api sharegroup:(EAGLSharegroup *)sharegroup;

@end


#endif /* BEGLUtils_h */
