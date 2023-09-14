//
//  VideoView.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import <UIKit/UIKit.h>
#import "AgoraSampleBufferRender.h"
#import "APIExample_OC-swift.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger,StreamType)
{
    StreamTypeLocal,
    StreamTypeRemote
};

@interface NSBundle (LoadView)
+ (UIView *)loadViewFormNib: (NSString *)name;
+ (UIView *)loadVideoViewFormType: (StreamType)type audioOnly: (BOOL)audioOnly;
@end

@interface VideoView : UIView

@property (weak, nonatomic) IBOutlet UIView *videoView;

@property (nonatomic, assign)BOOL audioOnly;
@property (nonatomic, assign)NSUInteger uid;
@property (nonatomic, assign)StreamType type;

- (void)setPlaceholder: (NSString *)text;

- (void)setInfo: (NSString *)text;

- (void)setStats:(NSString *)stats;

@end

@interface SampleBufferDisplayView : UIView
@property (nonatomic, assign)NSUInteger uid;
@property (weak, nonatomic) IBOutlet AgoraSampleBufferRender *videoView;

- (void)setPlaceholder: (NSString *)text;

- (void)setInfo: (NSString *)text;

@end

@interface MetalVideoView : UIView
@property (nonatomic, assign)NSUInteger uid;
@property (weak, nonatomic) IBOutlet AgoraMetalRender *videoView;

- (void)setPlaceholder: (NSString *)text;

- (void)setInfo: (NSString *)text;

@end

NS_ASSUME_NONNULL_END
