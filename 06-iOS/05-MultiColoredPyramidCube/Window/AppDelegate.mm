//
//  AppDelegate.m
//  Window
//
//  Created by user173485 on 7/5/21.
//  Copyright © 2021 AstroMediComp. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "GLESView.h"

@implementation AppDelegate
{
    @private
    UIWindow *window;
    ViewController *viewController;
    GLESView *view;
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    CGRect win_rect = [[UIScreen mainScreen]bounds];
    
    window = [[UIWindow alloc]initWithFrame:win_rect];
    
    viewController = [[ViewController alloc]init];
    
    [window setRootViewController:viewController];
    
    view = [[GLESView alloc]initWithFrame:win_rect];
    
    [viewController setView:view];
    [view release];
    
    [window makeKeyAndVisible];
    
    [view startAnimation];
    return(YES);
}

-(void)applicationWillResignActive:(UIApplication *)application
{
    //Code
    [view stopAnimation];
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
    //Code
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
    //Code
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
    //Code
    [view startAnimation];
}

-(void)applicationWillTerminate:(UIApplication *)application
{
    //Code
    [view stopAnimation];
}

-(void)dealloc
{
    //Code
    [view release];
    
    [viewController release];
    
    [window release];
    
    [super dealloc];
}

@end
