//
//  StreamingViewController.h
//  Streaming
//
//  Created by nishi on 10/07/29.
//  Copyright TARGET ENTERTAINMENT INC. 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioStreamer.h"

@interface StreamingViewController : UIViewController {
	IBOutlet UIImageView *rotate;

	AudioStreamer *streamer;
	BOOL loopFlag;

	BOOL inited;
	CGPoint vect[10];
}
- (IBAction)loopSwtich:(id)sender;

-(void)nextRotate1;
-(void)nextRotate2;

- (IBAction)play:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)pause:(id)sender;

@end

