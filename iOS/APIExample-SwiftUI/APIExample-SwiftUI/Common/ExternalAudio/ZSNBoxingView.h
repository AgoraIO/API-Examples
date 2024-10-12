//
//  ZSNBoxingView.h
//  DemoAPIOCZhaosn
//
//  Created by zhaosuning on 2022/12/1.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface ZSNBoxingView : UIView

@property (nonatomic, copy) void (^itemLevelCallback)(void);

@property (nonatomic) NSUInteger numberOfItems;

@property (nonatomic) UIColor * itemColor;

@property (nonatomic) CGFloat level;

@property (nonatomic) UILabel *timeLabel;

@property (nonatomic) NSString *text;

@property (nonatomic) CGFloat middleInterval;

- (void)start;
- (void)stop;

@end

NS_ASSUME_NONNULL_END
