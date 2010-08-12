//
//  StreamingViewController.m
//  Streaming
//
//  Created by nishi on 10/07/29.
//  Copyright TARGET ENTERTAINMENT INC. 2010. All rights reserved.
//

#import "StreamingViewController.h"


static float RandF(float max)
{
	float val = arc4random() / 10000.0f;				// 適当に割ってみる
	return fmodf(val, max);
}

@implementation StreamingViewController



/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


- (void)onTick:(NSTimer *)aTimer
{
	if(!inited)
	{
		inited = YES;
		for(int i = 0; i < 10; i++)
		{
			UIImageView *view = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"ball.png"]];
			view.center = CGPointMake(100.0 + RandF(160.0), 100.0 + RandF(160.0));
			float ang = RandF(M_PI * 2);
			vect[i].x = sinf(ang) * 2.0;
			vect[i].y = cosf(ang) * 2.0;
			view.tag = i + 10;
			[self.view addSubview:view];
			[self.view sendSubviewToBack:view];
			[view release];
		}
	}
	else
	{
		NSArray *array = [self.view subviews];
		for(UIView *view in array)
		{
			int tag = view.tag;
			if(tag >= 10)
			{
				CGPoint center = view.center;
				center.x += vect[tag - 10].x;
				center.y += vect[tag - 10].y;
				if((center.x < 15.0) || (center.x > 320.0 - 15.0)) vect[tag - 10].x = -vect[tag - 10].x;
				if((center.y < 15.0) || (center.y > 460.0 - 15.0)) vect[tag - 10].y = -vect[tag - 10].y;
				view.center = center;
			}
		}
	}
}


-(void)setRotateAnimation:(float)start end:(float)end sel:(SEL)aSel
{
	rotate.transform = CGAffineTransformMakeRotation(start);
	[UIView beginAnimations:nil context:UIGraphicsGetCurrentContext()];
	[UIView setAnimationCurve:UIViewAnimationCurveLinear];
	[UIView setAnimationDuration:3.0];
	[UIView setAnimationDelegate:self];
	[UIView setAnimationDidStopSelector:aSel];
	rotate.transform = CGAffineTransformMakeRotation(end);
	[UIView commitAnimations];
	
}

-(void)nextRotate1
{
	[self setRotateAnimation:-M_PI + 0.001 end:0.0 sel:@selector(nextRotate2)];
}

-(void)nextRotate2
{
	[self setRotateAnimation:0.0 end:M_PI - 0.001 sel:@selector(nextRotate1)];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	[self nextRotate2];

	NSTimer *timer = [NSTimer timerWithTimeInterval:(1.0 / 60.0) target:self selector:@selector(onTick:) userInfo:nil repeats:YES];
	[[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)destroyStreamer
{
	if (streamer)
	{
		[streamer stop:YES];
		[streamer release];
		streamer = nil;
	}
}

- (void)dealloc {
	[self destroyStreamer];

	[rotate release];
    [super dealloc];
}


- (void)createStreamer:(NSString *)file
{
	[self destroyStreamer];

	streamer = [[AudioStreamer alloc] initWithFile:[[NSBundle mainBundle] pathForResource:file ofType:nil]];
}


//
// UI
// 
- (IBAction)loopSwtich:(id)sender
{
	loopFlag = [sender isOn];
}


- (IBAction)play:(id)sender
{
	NSString *file[] = {
		@"sample1.mp3",
		@"sample2.mp3",
		@"sample2.m4a",
	};

	int tag = [sender tag];
	[self createStreamer:file[tag]];
	[streamer start];
	[streamer looping:loopFlag];
}

- (IBAction)stop:(id)sender
{
	[streamer stop:NO];
}

- (IBAction)pause:(id)sender
{
	[streamer pause];
}

@end
