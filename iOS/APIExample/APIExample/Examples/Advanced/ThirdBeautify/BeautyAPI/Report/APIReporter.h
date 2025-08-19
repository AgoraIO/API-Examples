//
//  APIReporter.h
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2024/4/17.
//

#import <Foundation/Foundation.h>
#import <AgoraRtcKit/AgoraRtcEngineKit.h>

typedef NS_ENUM(NSInteger, APIType) {
    APITypeKTV = 1,
    APITypeCall = 2,
    APITypeBeauty = 3,
    APITypeVideoLoader = 4,
    APITypePK = 5,
    APITypeVitualSpace = 6,
    APITypeScreenSpace = 7,
    APITypeAudioScenario = 8
};

typedef NS_ENUM(NSInteger, APIEventType) {
    APIEventTypeAPI = 0,
    APIEventTypeCost,
    APIEventTypeCustom
};

typedef NS_ENUM(NSInteger, APICostEvent) {
    APICostEventChannelUsage = 0,
    APICostEventFirstFrameActual,
    APICostEventFirstFramePerceived 
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
