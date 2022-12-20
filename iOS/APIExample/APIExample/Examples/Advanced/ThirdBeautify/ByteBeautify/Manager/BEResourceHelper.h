//
//  BEResourceHelper.h
//  BytedEffects
//
//  Created by QunZhang on 2019/10/22.
//  Copyright © 2019 ailab. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol BEResourceHelperDelegate <NSObject>

@optional

/// path of license dir
- (NSString *)licenseDirPath;

/// path of composer node
- (NSString *)composerNodeDirPath;

/// path of filter dir
- (NSString *)filterDirPath;

/// path of sticker dir
- (NSString *)stickerDirPath;

/// path of composer
- (NSString *)composerDirPath;

/// path of model dir
- (NSString *)modelDirPath;

/// license name
- (NSString *)licenseName;

@end

@interface BEResourceHelper : NSObject

@property (nonatomic, weak) id<BEResourceHelperDelegate> delegate;

- (NSString *)licensePath;
- (NSString *)composerNodePath:(NSString *)nodeName;
- (NSString *)filterPath:(NSString *)filterName;
- (NSString *)stickerPath:(NSString *)stickerName;
- (NSString *)modelPath:(NSString *)modelName;
- (NSString *)composerPath;
- (NSString *)modelDirPath;

@end
