//
//  APIReporter.m
//  BeautyAPi
//
//  Created by zhaoyongqiang on 2024/4/17.
//

#import "APIReporter.h"

@interface APIReporter ()

@property (nonatomic, strong) AgoraRtcEngineKit *engine;
@property (nonatomic, copy) NSString *category;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSNumber *> *durationEventStartMap;
@property (nonatomic, copy) NSString *messsageId;

@end

@implementation APIReporter

- (NSString *)messsageId {
    return @"agora:scenarioAPI";
}

- (instancetype)initWithType:(APIType)type version:(NSString *)version engine:(AgoraRtcEngineKit *)engine {
    self = [super init];
    if (self) {
        _category = [NSString stringWithFormat:@"%ld_iOS%@", (long)type, version];
        _engine = engine; 
        [self configParameters];
    }
    return self;
}

- (void)reportFuncEventWithName:(NSString *)name value:(NSDictionary<NSString *, id> *)value ext:(NSDictionary<NSString *, id> *)ext { 
    NSString *content = [NSString stringWithFormat:@"[APIReporter]reportFuncEvent: %@ value: %@ ext: %@", name, value, ext];
    [self debugApiPrint:content];
    
    NSDictionary *eventMap = @{ @"type": @(APIEventTypeAPI), @"desc": name }; 
    NSDictionary *labelMap = @{ @"apiValue": value, @"ts": @([self getCurrentTs]), @"ext": ext};

    NSString *event = [self convertToJSONString:eventMap]; 
    NSString *label = [self convertToJSONString:labelMap];

    [self.engine sendCustomReportMessage:self.messsageId category:self.category event:event label:label value:0];
}

- (void)startDurationEventWithName:(NSString *)name { 
    self.durationEventStartMap[name] = @([self getCurrentTs]);
}

- (void)endDurationEventWithName:(NSString *)name ext:(NSDictionary<NSString *, id> *)ext {
    NSNumber *beginTs = self.durationEventStartMap[name];
    if (!beginTs) {
        return;
    }
    [self.durationEventStartMap removeObjectForKey:name];
    
    NSInteger ts = [self getCurrentTs]; 
    NSInteger cost = ts - beginTs.integerValue;

    [self reportCostEventWithTs:ts name:name cost:cost ext:ext];
}

- (void)reportCostEventWithName:(APICostEvent)name cost:(NSInteger)cost ext:(NSDictionary<NSString *, id> *)ext { [self.durationEventStartMap removeObjectForKey: [self getCostEventWithName:name]];
    
    [self reportCostEventWithTs:[self getCurrentTs] name:[self getCostEventWithName:name] cost:cost ext:ext];
}

- (void)reportCustomEventWithName:(NSString *)name ext:(NSDictionary<NSString *, id> *)ext {
    NSString *content = [NSString stringWithFormat:@"[APIReporter]reportCustomEvent: %@ ext: %@", name, ext];
    [self debugApiPrint:content];
    
    NSDictionary *eventMap = @{ @"type": @(APIEventTypeCustom), @"desc": name };
    NSDictionary *labelMap = @{ @"ts": @([self getCurrentTs]), @"ext": ext };

    NSString *event = [self convertToJSONString:eventMap]; 
    NSString *label = [self convertToJSONString:labelMap];

    [self.engine sendCustomReportMessage:self.messsageId category:self.category event:event label:label value:0];
}

- (NSString *)getCostEventWithName:(APICostEvent)name {
    switch (name) {
        case APICostEventChannelUsage: return @"channelUsage";
        case APICostEventFirstFrameActual: return @"firstFrameActual";
        case APICostEventFirstFramePerceived: return @"firstFramePerceived";
    }
}

- (void)writeLogWithContent:(NSString *)content level:(AgoraLogLevel)level {
    [self.engine writeLog:level content:content];
}

- (void)cleanCache {
    [self.durationEventStartMap removeAllObjects];
}

#pragma mark - Private Methods
- (void)reportCostEventWithTs:(NSInteger)ts name:(NSString *)name cost:(NSInteger)cost ext:(NSDictionary<NSString *, id> *)ext {
    NSString *content = [NSString stringWithFormat:@"[APIReporter]reportCostEvent: %@ cost: %ld ms ext: %@", name, (long)cost, ext];
    [self debugApiPrint:content];
    [self writeLogWithContent:content level:AgoraLogLevelInfo];

    NSDictionary *eventMap = @{ @"type": @(APIEventTypeCost), @"desc": name }; 
    NSDictionary *labelMap = @{ @"ts": @(ts), @"ext": ext };

    NSString *event = [self convertToJSONString:eventMap];
    NSString *label = [self convertToJSONString:labelMap];

    [self.engine sendCustomReportMessage:self.messsageId category:self.category event:event label:label value:cost];
}

- (void)configParameters { 
    [self.engine setParameters:@"{\"rtc.direct_send_custom_event\": true}"];
    [self.engine setParameters:@"{\"rtc.log_external_input\": true}"];
}

- (NSInteger)getCurrentTs { 
    return (NSInteger)([[NSDate date] timeIntervalSince1970] * 1000.0);
}

- (NSString *)convertToJSONString:(NSDictionary *)dictionary {
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dictionary options:0 error:&error];
    if (!jsonData) {
        [self writeLogWithContent:[NSString stringWithFormat:@"[APIReporter]convert to json fail: %@ dictionary: %@", error, dictionary] level:AgoraLogLevelWarn];
        return nil;
    }
    NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    return jsonString;
}

- (void)debugApiPrint:(NSString *)message {
#if DEBUG
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
    NSString *timeString = [formatter stringFromDate:[NSDate date]];
    NSLog(@"%@ %@", timeString, message);
#endif
}

@end
