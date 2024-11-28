#import "macro.h"
#import "Config.h"

#define CHECK_RET_AND_RETURN(MSG, ret) \
if (ret != 0 && ret != -11 && ret != 1) {\
    const char *msg = bef_effect_ai_error_code_get(ret);\
    if (msg != NULL) {\
        NSLog(@"%s error: %d, %s", #MSG, ret, msg);\
        [[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                                    object:nil\
                                                  userInfo:@{@"data": [NSString stringWithCString:msg encoding:NSUTF8StringEncoding]}];\
    } else {\
        NSLog(@"%s error: %d", #MSG, ret);\
        [[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                                    object:nil\
                                                    userInfo:@{@"data": [NSString stringWithFormat:@"%s error: %d", #MSG, ret]}];\
    }\
    return ret;\
}

#define CHECK_RET_AND_RETURN_RESULT(MSG, ret, result) \
if (ret != 0 && ret != -11 && ret != 1) {\
    const char *msg = bef_effect_ai_error_code_get(ret);\
    if (msg != NULL) {\
        NSLog(@"%s error: %d, %s", #MSG, ret, msg);\
[[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                            object:nil\
                                          userInfo:@{@"data": [NSString stringWithCString:msg encoding:NSUTF8StringEncoding]}];\
    } else {\
        NSLog(@"%s error: %d", #MSG, ret);\
[[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                                object:nil\
                                                userInfo:@{@"data": [NSString stringWithFormat:@"%s error: %d", #MSG, ret]}];\
    }\
    return result;\
}

#define CHECK_RET_AND_DO(MSG, ret, DO) \
if (ret != 0 && ret != -11 && ret != 1) {\
    const char *msg = bef_effect_ai_error_code_get(ret);\
    if (msg != NULL) {\
        NSLog(@"%s error: %d, %s", #MSG, ret, msg);\
[[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                            object:nil\
                                          userInfo:@{@"data": [NSString stringWithCString:msg encoding:NSUTF8StringEncoding]}];\
    } else {\
        NSLog(@"%s error: %d", #MSG, ret);\
[[NSNotificationCenter defaultCenter] postNotificationName:@"kBESdkErrorNotification"\
                                                    object:nil\
                                                    userInfo:@{@"data": [NSString stringWithFormat:@"%s error: %d", #MSG, ret]}];\
    }\
    { DO; }\
}

#if DEBUG_LOG
#define BELog(fmt, ...) NSLog((fmt), ##__VA_ARGS__);
#else
#define BELog(fmt, ...)
#endif
