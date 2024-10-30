//
//  APIReporter.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2024/4/17.
//

#import <Foundation/Foundation.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

typedef NS_ENUM(NSInteger, APIType) {
    APITypeKTV = 1,  //K歌
    APITypeCall = 2,  //呼叫
    APITypeBeauty = 3,  //美颜
    APITypeVideoLoader = 4, //秒开/秒切
    APITypePK = 5, //团战
    APITypeVitualSpace = 6,
    APITypeScreenSpace = 7, //屏幕共享
    APITypeAudioScenario = 8  //音频scenario
};

typedef NS_ENUM(NSInteger, APIEventType) {
    APIEventTypeAPI = 0, //api事件
    APIEventTypeCost,    //耗时事件
    APIEventTypeCustom   //自定义事件
};

typedef NS_ENUM(NSInteger, APICostEvent) {
    APICostEventChannelUsage = 0, //频道使用耗时
    APICostEventFirstFrameActual,  //首帧实际耗时
    APICostEventFirstFramePerceived //首帧感官耗时
};

NS_ASSUME_NONNULL_BEGIN

@interface APIReporter : NSObject

- (instancetype)initWithType:(APIType)type version:(NSString *)version engine:(AgoraRtcEngineKit *)engine;
- (void)reportFuncEventWithName:(NSString *)name value:(NSDictionary<NSString *, id> *)value ext:(NSDictionary<NSString *, id> *)ext;
- (void)startDurationEventWithName:(NSString *)name;
- (void)endDurationEventWithName:(NSString *)name ext:(NSDictionary<NSString *, id> *)ext;
- (void)reportCostEventWithName:(APICostEvent)name cost:(NSInteger)cost ext:(NSDictionary<NSString *, id> *)ext;
- (void)reportCustomEventWithName:(NSString *)name ext:(NSDictionary<NSString *, id> *)ext;
- (void)writeLogWithContent:(NSString *)content level:(AgoraLogLevel)level;
- (void)cleanCache;

@end

NS_ASSUME_NONNULL_END
