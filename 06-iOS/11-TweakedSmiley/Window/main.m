//
//  main.m
//  Window
//
//  Created by user173485 on 7/5/21.
//  Copyright © 2021 AstroMediComp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[])
{
    //Code
    NSString *appDelegateClassName;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    appDelegateClassName = NSStringFromClass([AppDelegate class]);
    int ret =  UIApplicationMain(argc, argv, nil, appDelegateClassName);
    
    [pool release];
    return(ret);
}
