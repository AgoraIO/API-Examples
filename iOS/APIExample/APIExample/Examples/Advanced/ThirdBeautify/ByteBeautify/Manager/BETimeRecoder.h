//  BETimeRecoder.h
// EffectsARSDK


#import <Foundation/Foundation.h>

#if TIME_LOG
#define RECORD_TIME(NAME) double _##NAME = [NSDate date].timeIntervalSince1970;
#else
#define RECORD_TIME(NAME)
#endif

#if TIME_LOG
#define STOP_TIME(NAME) NSLog(@"TimeRecoder %s %f", #NAME, ([NSDate date].timeIntervalSince1970 - _##NAME) * 1000);
#else
#define STOP_TIME(NAME)
#endif

@interface BETimeRecoder : NSObject

//   {zh} / @brief 开始记录耗时     {en} /@Brief start recording time 
//   {zh} / @param tag 标签     {en} /@param tag 
+ (void)record:(NSString *)tag;

//   {zh} / @brief 停止记录耗时     {en} /@Briefing Stop Recording Time-consuming 
//   {zh} / @param tag 标签     {en} /@param tag 
+ (void)stop:(NSString *)tag;

+ (void)be_recordOnce:(NSString *)tag interval:(double)interval;

@end
