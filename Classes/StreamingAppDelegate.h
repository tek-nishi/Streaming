//
//  StreamingAppDelegate.h
//  Streaming
//
//  Created by nishi on 10/07/29.
//  Copyright TARGET ENTERTAINMENT INC. 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class StreamingViewController;

@interface StreamingAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    StreamingViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet StreamingViewController *viewController;

@end

