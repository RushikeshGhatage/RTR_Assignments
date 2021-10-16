//
//  MyView.h
//  Window
//
//  Created by user173485 on 7/5/21.
//  Copyright © 2021 AstroMediComp. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GLESView : UIView <UIGestureRecognizerDelegate>
-(void)startAnimation;
-(void)stopAnimation;
@end

