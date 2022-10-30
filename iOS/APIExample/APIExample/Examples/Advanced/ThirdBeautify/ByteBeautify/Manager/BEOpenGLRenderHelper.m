//
//  BEOpenGLRenderHelper.m
//  Core
//
//  Created by qun on 2021/6/29.
//

#import "BEOpenGLRenderHelper.h"

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

static float TEXTURE_FLIPPED[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,};
static float TEXTURE_RORATION_0[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,};
static float TEXTURE_ROTATED_90[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,};
static float TEXTURE_ROTATED_180[] = {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,};
static float TEXTURE_ROTATED_270[] = {1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,};
static float CUBE[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,};

@interface BEOpenGLProgram : NSObject {
    GLuint          _program;
    GLuint          _position;
    GLuint          _color;
}

- (instancetype)initWithVertex:(NSString *)vertex fragment:(NSString *)fragment;
- (void)destroy;

@end

@implementation BEOpenGLProgram

- (instancetype)initWithVertex:(NSString *)vertex fragment:(NSString *)fragment {
    if (self = [super init]) {
        GLuint vertexShader = [self compileShader:vertex withType:GL_VERTEX_SHADER];
        GLuint fragmentShader = [self compileShader:fragment withType:GL_FRAGMENT_SHADER];
        
        _program = glCreateProgram();
        glAttachShader(_program, vertexShader);
        glAttachShader(_program, fragmentShader);
        glLinkProgram(_program);
        
        GLint linkSuccess;
        glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess == GL_FALSE){
            NSLog(@"BERenderHelper link shader error");
        }
        
        if (vertexShader) {
            glDeleteShader(vertexShader);
        }
        
        if (fragmentShader) {
            glDeleteShader(fragmentShader);
        }
        
        glUseProgram(_program);
        _position = glGetAttribLocation(_program, "position");
        _color = glGetUniformLocation(_program, "color");
    }
    return self;
}

- (void)destroy {
    glDeleteProgram(_program);
}

- (GLuint)compileShader:(NSString *)shaderString withType:(GLenum)shaderType {
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

@end

@interface BEResizeTextureProgram : BEOpenGLProgram {
    GLuint              _textureCoordinate;
    GLuint              _inputTexture;
    
    GLuint              _frameBuffer;
    GLuint              _resizedTexture;
}

- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char *)buffer withWidth:(int)width height:(int)height withFormat:(GLenum)format withRotation:(int)rotation;

- (void)drawTexture:(GLuint)texture;

@end

@implementation BEResizeTextureProgram

- (instancetype)initWithVertex:(NSString *)vertex fragment:(NSString *)fragment {
    if (self = [super initWithVertex:vertex fragment:fragment]) {
        glUseProgram(_program);
        
        _textureCoordinate = glGetAttribLocation(_program, "inputTextureCoordinate");
        _inputTexture = glGetUniformLocation(_program, "inputImageTexture");
        
        glGenFramebuffers(1, &_frameBuffer);
        glGenTextures(1, &_resizedTexture);
    }
    return self;
}

- (void)drawTexture:(GLuint)texture {
    glUseProgram(_program);
    glVertexAttribPointer(_position, 2, GL_FLOAT, false, 0, CUBE);
    glEnableVertexAttribArray(_position);
    glVertexAttribPointer(_textureCoordinate, 2, GL_FLOAT, false, 0, TEXTURE_RORATION_0);
    glEnableVertexAttribArray(_textureCoordinate);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(_inputTexture, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(_position);
    glDisableVertexAttribArray(_textureCoordinate);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUseProgram(0);
}

- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char *)buffer withWidth:(int)width height:(int)height withFormat:(GLenum)format withRotation:(int)rotation {
    glBindTexture(GL_TEXTURE_2D, _resizedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _resizedTexture, 0);
    
    glUseProgram(_program);
    glVertexAttribPointer(_position, 2, GL_FLOAT, false, 0, CUBE);
    glEnableVertexAttribArray(_position);
    glVertexAttribPointer(_textureCoordinate, 2, GL_FLOAT, false, 0, [self rota:rotation]);
    glEnableVertexAttribArray(_textureCoordinate);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(_inputTexture, 0);
    glViewport(0, 0, width, height);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(_position);
    glDisableVertexAttribArray(_textureCoordinate);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

- (float *)rota:(int)rotation {
    switch (rotation) {
        case 1:
            return TEXTURE_ROTATED_90;
        case 2:
            return TEXTURE_ROTATED_180;
        case 3:
            return TEXTURE_ROTATED_270;
        default:
            return TEXTURE_RORATION_0;
    }
}

- (void)destroy {
    [super destroy];
    glDeleteFramebuffers(1, &_frameBuffer);
    glDeleteTextures(1, &_resizedTexture);
}

@end

@interface BEOpenGLRenderHelper () {
    BEResizeTextureProgram      *_resizeTextureProgram;
    
    int viewWidth;
    int viewHeight;
    
    float _ratio;
}

@end

@implementation BEOpenGLRenderHelper

- (instancetype)init
{
    self = [super init];
    if (self) {
            [self loadResizeShader];
            
            viewWidth = 720;
            viewWidth = 1080;
            _ratio = 0.0;
    }
    return self;
}

- (void)dealloc
{
    [_resizeTextureProgram destroy];
}

- (void) loadResizeShader{
    _resizeTextureProgram = [[BEResizeTextureProgram alloc] initWithVertex:CAMREA_RESIZE_VERTEX fragment:CAMREA_RESIZE_FRAGMENT];
    [self checkGLError];
}

- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight {
    [self textureToImage:texture withBuffer:buffer Width:rWidth height:rHeight format:GL_RGBA];
}

- (void) textureToImage:(GLuint)texture withBuffer:(unsigned char*)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format {
    [self textureToImage:texture withBuffer:buffer Width:rWidth height:rHeight format:format rotation:0];
}

- (void)textureToImage:(GLuint)texture withBuffer:(unsigned char *)buffer Width:(int)rWidth height:(int)rHeight format:(GLenum)format rotation:(int)rotation {
    glViewport(0, 0, viewWidth, viewHeight);
    [_resizeTextureProgram textureToImage:texture withBuffer:buffer withWidth:rWidth height:rHeight withFormat:format withRotation:rotation];
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
