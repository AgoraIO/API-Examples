//
//  AgoraYUVImageSourcePush.m
//  APIExample
//
//  Created by 张乾泽 on 2020/11/6.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

#import "AgoraYUVImageSourcePush.h"

@interface AgoraYUVImageSourcePush ()
@property(nonatomic, strong) NSData* yuvData;
@property(nonatomic, strong) NSTimer* timer;
@end

@implementation AgoraYUVImageSourcePush

-(id)init
{
    if(self = [super init]) {
        // initialize yuv data
        NSString *yuvPath = [[NSBundle mainBundle] pathForResource:@"frames0" ofType:@"yuv"];
        self.yuvData = [NSData dataWithContentsOfFile:yuvPath options:NSDataReadingUncached error:NULL];
    }
    return self;
}

-(void)startSource
{
    if(self.timer) {
        return;
    }
    __weak AgoraYUVImageSourcePush * weakSelf = self;
    self.timer = [NSTimer scheduledTimerWithTimeInterval:0.5 repeats:YES block:^(NSTimer * _Nonnull timer) {
        if(weakSelf.delegate) {
            [weakSelf.delegate onVideoFrame:weakSelf.yuvData size:CGSizeMake(176, 144) rotation:0];
        }
    }];
}

-(void)stopSource
{
    if(self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
}

@end
