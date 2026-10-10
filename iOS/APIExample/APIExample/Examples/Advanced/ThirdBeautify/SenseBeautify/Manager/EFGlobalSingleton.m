//
//  EFGlobalSingleton.m
//  SenseMeEffects
//
//  Created by 马浩萌 on 2021/11/30.
//  Copyright © 2021 SoftSugar. All rights reserved.
//

#import "EFGlobalSingleton.h"

@implementation EFGlobalSingleton

+(instancetype)sharedInstance {
    static EFGlobalSingleton * _shared = nil;
    static dispatch_once_t sharedOnceToken;
    dispatch_once(&sharedOnceToken, ^{
        _shared = [[self alloc] init];
    });
    return _shared;
}

-(BOOL)isMale {
    NSNumber *isMaleNumberValue = (NSNumber *)[[NSUserDefaults standardUserDefaults] objectForKey:EFGlobalSingletonMaleKey];
    return isMaleNumberValue.boolValue;
}

-(void)setIsMale:(BOOL)isMale {
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithBool:isMale] forKey:EFGlobalSingletonMaleKey];
}

@end
