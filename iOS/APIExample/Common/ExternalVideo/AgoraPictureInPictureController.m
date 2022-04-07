//
//  AgoraPictureInPictureController.m
//  APIExample
//
//  Created by 胡润辰 on 2022/4/1.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

#import "AgoraPictureInPictureController.h"

@interface AgoraPictureInPictureController () <AVPictureInPictureSampleBufferPlaybackDelegate>

@property (nonatomic, strong) AVPictureInPictureController *pipController;

@property (nonatomic, strong) AgoraSampleBufferRender *displayView;

@end

@implementation AgoraPictureInPictureController

- (instancetype)initWithDisplayView:(AgoraSampleBufferRender *)displayView {
    if (@available(iOS 15.0, *)) {
        if ([AVPictureInPictureController isPictureInPictureSupported]) {
            self = [super init];
            if (self) {
                _displayView = displayView;
                AVPictureInPictureControllerContentSource *pipControllerContentSource = [[AVPictureInPictureControllerContentSource alloc] initWithSampleBufferDisplayLayer:_displayView.displayLayer playbackDelegate:self];
                
                _pipController = [[AVPictureInPictureController alloc] initWithContentSource:pipControllerContentSource];
            }
            return self;
        }
    }
    return nil;
}


#pragma mark - <AVPictureInPictureSampleBufferPlaybackDelegate>

- (void)pictureInPictureController:(nonnull AVPictureInPictureController *)pictureInPictureController didTransitionToRenderSize:(CMVideoDimensions)newRenderSize {
    
}

- (void)pictureInPictureController:(nonnull AVPictureInPictureController *)pictureInPictureController setPlaying:(BOOL)playing {
    
}

- (void)pictureInPictureController:(nonnull AVPictureInPictureController *)pictureInPictureController skipByInterval:(CMTime)skipInterval completionHandler:(nonnull void (^)(void))completionHandler {
    
}

- (BOOL)pictureInPictureControllerIsPlaybackPaused:(nonnull AVPictureInPictureController *)pictureInPictureController {
    return NO;
}

- (CMTimeRange)pictureInPictureControllerTimeRangeForPlayback:(nonnull AVPictureInPictureController *)pictureInPictureController {
    return CMTimeRangeMake(kCMTimeZero, CMTimeMake(INT64_MAX, 1000));
}

@end
