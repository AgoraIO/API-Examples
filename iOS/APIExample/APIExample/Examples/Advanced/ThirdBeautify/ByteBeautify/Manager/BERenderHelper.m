// Copyright (C) 2019 Beijing Bytedance Network Technology Co., Ltd.
#import "BERenderHelper.h"

#define TTF_STRINGIZE(x) #x
#define TTF_STRINGIZE2(x) TTF_STRINGIZE(x)
#define TTF_SHADER_STRING(text) @ TTF_STRINGIZE2(text)

static NSString *const CAMREA_RESIZE_VERTEX = TTF_SHADER_STRING
(
attribute vec4 position;
attribute vec4 inputTextureCoordinate;
varying vec2 textureCoordinate;
void main(){
    textureCoordinate = inputTextureCoordinate.xy;
    gl_Position = position;
}
);

static NSString *const CAMREA_RESIZE_FRAGMENT = TTF_SHADER_STRING
(
 precision mediump float;
 varying highp vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 void main()
 {
     gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
 }
);

@interface  BERenderHelper (){
    GLuint _resizeProgram;
    GLuint _resizeLocation;
    GLuint _resizeInputImageTexture;
    GLuint _resizeTextureCoordinate;
    GLuint _resizeTexture;
    
    //为了resize buffer
    GLuint _frameBuffer;
}

@end

static float TEXTURE_RORATION_0[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,};
static float TEXTURE_ROTATED_90[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,};
static float TEXTURE_ROTATED_180[] = {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,};
static float TEXTURE_ROTATED_270[] = {1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,};
static float CUBE[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,};

@implementation BERenderHelper

- (instancetype) init
{
    self = [super init];
    if (self) {
        [self loadResizeShader];
        glGenFramebuffers(1, &_frameBuffer);
        glGenTextures(1, &_resizeTexture);
    }
    
    return self;
}

-(void)dealloc{
    glDeleteFramebuffers(1, &_frameBuffer);
    glDeleteTextures(1, &_resizeTexture);
}

+ (int) compileShader:(NSString *)shaderString withType:(GLenum)shaderType {
    GLuint shaderHandle = glCreateShader(shaderType);
    const char * shaderStringUTF8 = [shaderString UTF8String];
    
    int shaderStringLength = (int) [shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength);
    glCompileShader(shaderHandle);
    GLint success;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    
    if (success == GL_FALSE){
        NSLog(@"BErenderHelper compiler shader error: %s", shaderStringUTF8);
        return 0;
    }
    return shaderHandle;
}

/*
 * load resize shader
 */
- (void) loadResizeShader{
    GLuint vertexShader = [BERenderHelper compileShader:CAMREA_RESIZE_VERTEX withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [BERenderHelper compileShader:CAMREA_RESIZE_FRAGMENT withType:GL_FRAGMENT_SHADER];
    
    _resizeProgram = glCreateProgram();
    glAttachShader(_resizeProgram, vertexShader);
    glAttachShader(_resizeProgram, fragmentShader);
    glLinkProgram(_resizeProgram);
    
    GLint linkSuccess;
    glGetProgramiv(_resizeProgram, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE){
        NSLog(@"BERenderHelper link shader error");
    }
    
    glUseProgram(_resizeProgram);
    _resizeLocation = glGetAttribLocation(_resizeProgram, "position");
    _resizeTextureCoordinate = glGetAttribLocation(_resizeProgram, "inputTextureCoordinate");
    _resizeInputImageTexture = glGetUniformLocation(_resizeProgram, "inputImageTexture");
    
    if (vertexShader)
        glDeleteShader(vertexShader);
    
    if (fragmentShader)
        glDeleteShader(fragmentShader);
}

/*
 * transfer a image th buffer
 */
- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight {
    [self textureToImage:texture withBuffer:buffer Width:rWidth height:rHeight format:GL_RGBA];
}

- (void) textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format {
    [self textureToImage:texture withBuffer:buffer Width:rWidth height:rHeight format:format rotation:0];
}

- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char *)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format rotation:(int)rotation {
    glBindTexture(GL_TEXTURE_2D, _resizeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rWidth, rHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _resizeTexture, 0);
    
    glUseProgram(_resizeProgram);
    glVertexAttribPointer(_resizeLocation, 2, GL_FLOAT, false, 0, CUBE);
    glEnableVertexAttribArray(_resizeLocation);
    float *rota = TEXTURE_RORATION_0;
    if (rotation == 1) {
        rota = TEXTURE_ROTATED_90;
    } else if (rotation == 2) {
        rota = TEXTURE_ROTATED_180;
    } else if (rotation == 3) {
        rota = TEXTURE_ROTATED_270;
    }
    glVertexAttribPointer(_resizeTextureCoordinate, 2, GL_FLOAT, false, 0, rota);
    glEnableVertexAttribArray(_resizeTextureCoordinate);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(_resizeInputImageTexture, 0);
    glViewport(0, 0, rWidth, rHeight);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(_resizeLocation);
    glDisableVertexAttribArray(_resizeTextureCoordinate);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glReadPixels(0, 0, rWidth, rHeight, format, GL_UNSIGNED_BYTE, buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    [self checkGLError];
}

- (void)checkGLError {
    int error = glGetError();
    if (error != GL_NO_ERROR) {
        NSLog(@"checkGLError %d", error);
        @throw [NSException exceptionWithName:@"GLError" reason:@"error " userInfo:nil];
    }
}

@end
