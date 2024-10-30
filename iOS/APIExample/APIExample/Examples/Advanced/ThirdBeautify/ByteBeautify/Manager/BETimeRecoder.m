//  BETimeRecoder.m
// EffectsARSDK


#import "BETimeRecoder.h"

static NSMutableDictionary<NSString *, NSNumber *> *be_startTime;

@interface BETimeRecoder ()

@end

@implementation BETimeRecoder

+ (void)initialize
{
    if (self == [BETimeRecoder class]) {
        be_startTime = [NSMutableDictionary dictionary];
    }
}

+ (void)record:(NSString *)tag {
    [be_startTime setObject:[NSNumber numberWithDouble:[NSDate date].timeIntervalSince1970] forKey:tag];
}

+ (void)stop:(NSString *)tag {
    NSNumber *start = [be_startTime objectForKey:tag];
    if (start == nil) {
        [self be_startNotFound:tag];
        return;
    }
    [be_startTime removeObjectForKey:tag];
    double s = [start doubleValue];
    double e = [NSDate date].timeIntervalSince1970;
    [self be_recordOnce:tag interval:e - s];
}

#pragma mark - private
+ (void)be_startNotFound:(NSString *)tag {
    NSLog(@"call record with tag %@ first", tag);
}

+ (void)be_recordOnce:(NSString *)tag interval:(double)interval {
    NSLog(@"TimeRecoder %@ %f", tag, interval * 1000);
}

@end
