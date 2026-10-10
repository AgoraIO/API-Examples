//
//  AppDelegate.m
//  APIExample
//
//  Created by zhaoyongqiang on 2023/7/11.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (@available(iOS 13.0, *)) {
        // UIKit creates the window using the scene storyboard configuration.
    } else {
        self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
        self.window.rootViewController = [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateInitialViewController];
        [self.window makeKeyAndVisible];
    }
    return YES;
}

@end
