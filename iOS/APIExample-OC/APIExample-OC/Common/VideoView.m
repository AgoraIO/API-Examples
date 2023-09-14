//
//  VideoView.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "VideoView.h"

@implementation NSBundle (LoadView)

+ (UIView *)loadViewFormNib: (NSString *)name {
    UIView *view = [[NSBundle mainBundle] loadNibNamed:name owner:nil options:nil].firstObject;
    return view;
}

+ (UIView *)loadVideoViewFormType: (StreamType)type audioOnly: (BOOL)audioOnly {
    VideoView *view = (VideoView *)[NSBundle loadViewFormNib:@"VideoView"];
    view.audioOnly = audioOnly;
    view.type = type;
    return view;
}

@end

@interface VideoView ()
@property (weak, nonatomic) IBOutlet UILabel *placeholderLabel;
@property (weak, nonatomic) IBOutlet UILabel *infoLabel;
@property (weak, nonatomic) IBOutlet UILabel *statsLabel;

@end

@implementation VideoView

-(void)setPlaceholder: (NSString *)text {
    self.placeholderLabel.text = text;
}

- (void)setUid:(NSUInteger)uid {
    _uid = uid;
    if (uid <= 0) {
        self.infoLabel.text = @"";
        return;
    }
    self.infoLabel.text = [NSString stringWithFormat: @"uid: %lu",uid];
}

- (void)setInfo: (NSString *)text {
    self.infoLabel.text = text;
}

- (void)setStats:(NSString *)stats {
    self.statsLabel.text = stats;
}

@end

@interface SampleBufferDisplayView ()
@property (weak, nonatomic) IBOutlet UILabel *placeholderLabel;
@property (weak, nonatomic) IBOutlet UILabel *infoLabel;
@end

@implementation SampleBufferDisplayView

-(void)setPlaceholder: (NSString *)text {
    self.placeholderLabel.text = text;
}

- (void)setUid:(NSUInteger)uid {
    _uid = uid;
    if (uid <= 0) {
        self.infoLabel.text = @"";
        return;
    }
    self.infoLabel.text = [NSString stringWithFormat: @"uid: %lu",uid];
}

- (void)setInfo: (NSString *)text {
    self.infoLabel.text = text;
}

@end

@interface MetalVideoView ()
@property (weak, nonatomic) IBOutlet UILabel *placeholderLabel;
@property (weak, nonatomic) IBOutlet UILabel *infoLabel;
@end

@implementation MetalVideoView

-(void)setPlaceholder: (NSString *)text {
    self.placeholderLabel.text = text;
}

- (void)setUid:(NSUInteger)uid {
    _uid = uid;
    if (uid <= 0) {
        self.infoLabel.text = @"";
        return;
    }
    self.infoLabel.text = [NSString stringWithFormat: @"uid: %lu",uid];
}

- (void)setInfo: (NSString *)text {
    self.infoLabel.text = text;
}

@end
