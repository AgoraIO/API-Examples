// Copyright (C) 2019 Beijing Bytedance Network Technology Co., Ltd.

#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/glext.h>

@interface BERenderHelper : NSObject

/// transfor texture to buffer
/// @param texture texture
/// @param buffer buffer
/// @param rWidth width of buffer
/// @param rHeight height of buffer
- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight;

/// transfor texture to buffer
/// @param texture texture
/// @param buffer buffer
/// @param rWidth width of buffer
/// @param rHeight height of buffer
/// @param format pixel format, such as GL_RGBA,GL_BGRA...
- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format;

/// transfor texture to buffer
/// @param texture texture
/// @param buffer buffer
/// @param rWidth width of buffer
/// @param rHeight height of buffer
/// @param format pixel format, such as GL_RGBA,GL_BGRA...
/// @param rotation rotation of buffer, 0: 0˚, 1: 90˚, 2: 180˚, 3: 270˚
- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format rotation:(int)rotation;
+ (int) compileShader:(NSString *)shaderString withType:(GLenum)shaderType;
@end

