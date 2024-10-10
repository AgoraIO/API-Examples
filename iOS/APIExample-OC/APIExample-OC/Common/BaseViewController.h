//
//  BaseViewController.h
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface BaseViewController : UIViewController

@property(nonatomic, strong) NSDictionary *configs;

- (void)showAlertWithTitle: (NSString *)title;

- (void)showAlertWithTitle: (NSString *)title
                   message: (NSString *)message;

- (void)showAlertWithTitle: (NSString *)title
                   message: (NSString *)message
             textAlignment: (NSTextAlignment)alignment;

- (void)presentAlertViewController:(UIAlertController*)alertVC;
@end

// 布局模块
@interface UIView (VideoContainerLayout)
- (void)layoutStream:(NSArray <UIView *>*)views;
@end


// 多语言
@interface NSString (Localized)

- (NSString *)localized;

@end

// 封装日志模块
typedef NS_ENUM(NSInteger, LogLevel) {
    LogLevelInfo,
    LogLevelError,
    LogLevelDebug
};
@interface LogUtil : NSObject

+ (void)log:(NSString *)message;

+ (void)log:(NSString *)message level:(LogLevel)level;

@end

NS_ASSUME_NONNULL_END
