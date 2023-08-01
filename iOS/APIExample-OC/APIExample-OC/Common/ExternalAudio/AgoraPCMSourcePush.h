//
//  AgoraPCMSourcePush.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/21.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol AgoraPcmSourceDelegate <NSObject>

- (void)onAudioFrame: (void *)data;

@end

@interface AgoraPCMSourcePush : NSObject

@property (nonatomic, weak) id<AgoraPcmSourceDelegate>delegate;

- (instancetype)initWithDelegate: (id<AgoraPcmSourceDelegate>)delegate
                        filePath: (NSString *)filePath
                      sampleRate: (NSInteger)sampleRate
                channelsPerFrame:(NSInteger)channelsPerFrame
                   bitPerSamples: (NSInteger)bitPerSamples
                         samples: (NSInteger)samples;

- (void)start;

- (void)stop;

@end

NS_ASSUME_NONNULL_END
