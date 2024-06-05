//
//  BaseViewController.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "BaseViewController.h"
#import <objc/runtime.h>

@interface BaseViewController ()

@end

@implementation BaseViewController


- (void)viewDidLoad {
}

- (void)showAlertWithTitle: (NSString *)title {
    [self showAlertWithTitle:title message:@"" textAlignment:(NSTextAlignmentCenter)];
}

- (void)showAlertWithTitle: (NSString *)title
                   message: (NSString *)message {
    [self showAlertWithTitle:title message:message textAlignment:(NSTextAlignmentCenter)];
}

- (void)showAlertWithTitle: (NSString *)title
                   message: ( NSString *)message
             textAlignment: (NSTextAlignment)alignment {
    UIAlertController *alertVC = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:(UIAlertControllerStyleAlert)];
    
    UIAlertAction *ok = [UIAlertAction actionWithTitle:@"OK" style:(UIAlertActionStyleCancel) handler:nil];
    [alertVC addAction:ok];
    UILabel *messageLabel = [alertVC.view valueForKeyPath:@"_messageLabel"];
    if (messageLabel) {
        messageLabel.textAlignment = alignment;
    }
    [self presentViewController:alertVC animated:YES completion:nil];
}

- (void)presentAlertViewController:(UIAlertController*)alertVC {
    
    // 判断设备类型
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
        // iPad 上需要提供位置信息
        alertVC.popoverPresentationController.sourceView = self.view; // 设置源视图
        alertVC.popoverPresentationController.sourceRect = CGRectMake(self.view.bounds.size.width / 2.0, self.view.bounds.size.height / 2.0, 1.0, 1.0); // 设置源矩形
    }
    [self presentViewController:alertVC animated:YES completion:nil];
}

@end


@interface UIView (VideoContainerLayout)<UICollectionViewDelegate, UICollectionViewDataSource, UICollectionViewDelegateFlowLayout>

@property(nonatomic, weak)UICollectionView *collectionView;
@property(nonatomic, weak)NSArray <UIView *> *views;

@end

@implementation UIView (VideoContainerLayout)

- (UICollectionView *)collectionView {
    return objc_getAssociatedObject(self, _cmd);
}
- (void)setCollectionView:(UICollectionView *)collectionView {
    objc_setAssociatedObject(self, @selector(collectionView), collectionView, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray<UIView *> *)views {
    return objc_getAssociatedObject(self, _cmd);
}
- (void)setViews:(NSArray<UIView *> *)views {
    objc_setAssociatedObject(self, @selector(views), views, OBJC_ASSOCIATION_RETAIN);
}

- (void)layoutStream:(NSArray <UIView *>*)views {
    for (UIView *view in self.subviews) {
        [view removeFromSuperview];
    }
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc]init];
    NSInteger count = views.count > 2 ? 2 : views.count;
    flowLayout.itemSize = CGSizeMake(self.bounds.size.width / count, self.bounds.size.height);
    flowLayout.estimatedItemSize = CGSizeZero;
    flowLayout.minimumLineSpacing = 0;
    flowLayout.minimumInteritemSpacing = 0;
    flowLayout.scrollDirection = UICollectionViewScrollDirectionVertical;
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame: CGRectZero  collectionViewLayout:flowLayout];
    collectionView.delegate = self;
    collectionView.dataSource = self;
    collectionView.translatesAutoresizingMaskIntoConstraints = NO;
    [collectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"cell"];
    self.collectionView = collectionView;
    [self addSubview:collectionView];
    [[collectionView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor] setActive:YES];
    [[collectionView.topAnchor constraintEqualToAnchor:self.topAnchor] setActive:YES];
    [[collectionView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor] setActive:YES];
    [[collectionView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor] setActive:YES];
    self.views = views;
    [collectionView reloadData];
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.views.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"cell" forIndexPath:indexPath];
    UIView *view = self.views[indexPath.item];
    [cell.contentView addSubview:view];
    view.translatesAutoresizingMaskIntoConstraints = NO;
    [[view.leadingAnchor constraintEqualToAnchor:cell.contentView.leadingAnchor] setActive:YES];
    [[view.topAnchor constraintEqualToAnchor:cell.contentView.topAnchor] setActive:YES];
    [[view.trailingAnchor constraintEqualToAnchor:cell.contentView.trailingAnchor] setActive:YES];
    [[view.bottomAnchor constraintEqualToAnchor:cell.contentView.bottomAnchor] setActive:YES];
    return cell;
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    NSInteger count = self.views.count > 2 ? 2 : self.views.count;
    return CGSizeMake(self.bounds.size.width / count, self.bounds.size.height);
}

@end


@implementation NSString (Localized)

- (NSString *)localized {
    return NSLocalizedString(self, nil);
}

@end


@implementation LogUtil
+ (NSString *)getCurrentTime {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    return [formatter stringFromDate:[NSDate date]];
}

+ (NSString *)getLogPrefixForLevel:(LogLevel)level {
    switch (level) {
        case LogLevelInfo:
            return @"[INFO]";
        case LogLevelError:
            return @"[ERROR]";
        case LogLevelDebug:
            return @"[DEBUG]";
        default:
            return @"";
    }
}

+ (void)log:(NSString *)message {
    [self log:message level:(LogLevelDebug)];
}

+ (void)log:(NSString *)message level:(LogLevel)level {
    NSString *logString = [NSString stringWithFormat:@"%@ %@ %@\n",
                           [self getCurrentTime],
                           [self getLogPrefixForLevel:level],
                           message];
#if DEBUG
    NSLog(@"%@", logString);
#endif
    // 写入文件
    NSString *logFile = [NSString stringWithFormat:@"%@/APIExample_log.txt", NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject];
    [self checkLogFileSizeWithPath: logFile];
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:logFile];
    if (fileHandle) {
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            [fileHandle seekToEndOfFile];
            [fileHandle writeData:[logString dataUsingEncoding:NSUTF8StringEncoding]];
            [fileHandle closeFile];
        });
    } else {
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            [logString writeToFile:logFile atomically:YES encoding:NSUTF8StringEncoding error:nil];
        });
    }
}

+ (void)checkLogFileSizeWithPath: (NSString *)filePath {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;
    NSDictionary *fileAttributes = [fileManager attributesOfItemAtPath:filePath error:&error];
    if (fileAttributes) {
        NSNumber *fileSizeNumber = [fileAttributes objectForKey:NSFileSize];
        long long fileSize = [fileSizeNumber longLongValue];
        if (fileSize > 1024 * 1024 * 2) { // 文件大于2M
            [fileManager removeItemAtPath:filePath error:&error];
        }
    }
}

@end
