//
//  UIView+CSshortFrame.m
//
//  Created by CavanSu on 23/02/2017.
//  Copyright © 2017 CavanSu. All rights reserved.
//

#import "UIView+CSshortFrame.h"

@implementation UIView (CSshortFrame)
-(void)setX_CS:(CGFloat)x_CS {
    CGRect frame = self.frame;
    frame.origin.x = x_CS;
    self.frame = frame;
}

-(CGFloat)x_CS {
    return self.frame.origin.x;
}

-(void)setY_CS:(CGFloat)y_CS {
    CGRect frame = self.frame;
    frame.origin.y = y_CS;
    self.frame = frame;
}

-(CGFloat)y_CS {
    return self.frame.origin.y;
}

-(void)setWidth_CS:(CGFloat)width_CS {
    CGRect frame = self.frame;
    frame.size.width = width_CS;
    self.frame = frame;
}

-(CGFloat)width_CS {
    return self.frame.size.width;
}

-(void)setHeight_CS:(CGFloat)height_CS {
    CGRect frame = self.frame;
    frame.size.height = height_CS;
    self.frame = frame;
}

-(CGFloat)height_CS {
    return self.frame.size.height;
}

-(void)setSize_CS:(CGSize)size_CS {
    CGRect frame = self.frame;
    frame.size = size_CS;
    self.frame = frame;
}

-(CGSize)size_CS {
    return self.frame.size;
}

-(void)setOrigin_CS:(CGPoint)origin_CS {
    CGRect frame = self.frame;
    frame.origin = origin_CS;
    self.frame = frame;
}

-(CGPoint)origin_CS {
    return self.frame.origin;
}

-(void)setCenterX_CS:(CGFloat)centerX_CS {
    CGPoint center = self.center;
    center.x = centerX_CS;
    self.center = center;
}

-(CGFloat)centerX_CS {
    return self.center.x;
}

-(void)setCenterY_CS:(CGFloat)centerY_CS {
    CGPoint center = self.center;
    center.y = centerY_CS;
    self.center = center;
}

-(CGFloat)centerY_CS {
    return self.center.y;
}
@end
