//
//  UIColor+CSRGB.m
//
//  Created by CavanSu on 17/3/3.
//  Copyright © 2017 CavanSu. All rights reserved.
//

#import "UIColor+CSRGB.h"

@implementation UIColor (CSRGB)
+ (UIColor*)Red:(CGFloat)red Green:(CGFloat)green Blue:(CGFloat)blue {
    UIColor *color = [UIColor colorWithRed:red / 255 green:green / 255 blue:blue / 255 alpha: 1];
    return color;
}
@end
