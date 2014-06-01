// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "RainbowViewController.h"

#import <CoreMotion/CoreMotion.h>

#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "Input/Touch.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace
{
	const NSUInteger kMaxTouches = 32;
}

@interface RainbowViewController ()
@property (readonly, nonatomic) Director *director;
@property (readonly, nonatomic) CMMotionManager *motionManager;
@property (readonly, nonatomic) NSUInteger supportedInterfaceOrientations;
@end

@implementation RainbowViewController {
	Touch _touches[kMaxTouches];
}

- (void)dealloc
{
	delete self.director;
	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
}

- (EAGLContext *)context
{
	GLKView *view = (GLKView *)self.view;
	return view.context;
}

- (void)setContext:(EAGLContext *)context
{
	GLKView *view = (GLKView *)self.view;
	view.context = context;
}

- (Touch *)touches
{
	return _touches;
}

/// Converts an NSSet of touches to an array.
/// \note The dimension of the screen does not change with its orientation.
///       Since the iPad is in portrait mode by default, the resolution is
///       768x1024 in landscape mode as well.
/// \param touches  Set of touches to convert.
/// \return Array of touches. Must not be deleted.
- (Touch *)touchesArrayFromSet:(NSSet *)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	Touch *arrayRef = self.touches;
	UIScreen *screen = [UIScreen mainScreen];
	const CGSize size = screen.bounds.size;
	const CGFloat scale = screen.scale;
	switch ([UIApplication sharedApplication].statusBarOrientation)
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = p.x * scale;
				arrayRef->y = (size.height - p.y) * scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.x * scale;
				arrayRef->y0 = (size.height - p.y) * scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = (size.width - p.x) * scale;
				arrayRef->y = p.y * scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.width - p.x) * scale;
				arrayRef->y0 = p.y * scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = (size.height - p.y) * scale;
				arrayRef->y = (size.width - p.x) * scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.height - p.y) * scale;
				arrayRef->y0 = (size.width - p.x) * scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = p.y * scale;
				arrayRef->y = p.x * scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.y * scale;
				arrayRef->y0 = p.x * scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		default:
			R_ASSERT(false, "Reached unreachable code");
			break;
	}
	return self.touches;
}

#pragma mark - GLKViewControllerDelegate

- (void)update
{
	if (self.motionManager.accelerometerActive)
	{
		const CMAccelerometerData *data = self.motionManager.accelerometerData;
		self.director->input().accelerated(
		    data.acceleration.x, data.acceleration.y, data.acceleration.z,
		    data.timestamp);
	}
	self.director->update(self.timeSinceLastDraw * 1000);
}

#pragma mark - GLKViewDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	self.director->draw();
}

#pragma mark - UIResponder

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().touch_began([self touchesArrayFromSet:touches],
	                                   touches.count);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().touch_moved([self touchesArrayFromSet:touches],
	                                   touches.count);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().touch_ended([self touchesArrayFromSet:touches],
	                                   touches.count);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().touch_canceled();
}

#pragma mark - UIViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	// Set up OpenGL ES 2.0 context.
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (!self.context)
	{
		NSLog(@"[Rainbow] Failed to create ES context");
		return;
	}
	self.preferredFramesPerSecond = 60;
	[EAGLContext setCurrentContext:self.context];

	self.view.multipleTouchEnabled = YES;

	UIScreen *screen = [UIScreen mainScreen];
	CGSize size = screen.bounds.size;
	const CGFloat scale = screen.scale;
	size.width *= scale;
	size.height *= scale;

	Rainbow::Config config;
	if (config.is_portrait())
		_supportedInterfaceOrientations = UIInterfaceOrientationMaskPortrait;
	else
	{
		_supportedInterfaceOrientations = UIInterfaceOrientationMaskLandscape;
		// Swap screen width and height. See comments for touchesArrayFromSet:.
		std::swap(size.width, size.height);
	}
	if (config.needs_accelerometer())
	{
		// Enable accelerometer.
		_motionManager = [[CMMotionManager alloc] init];
		self.motionManager.accelerometerUpdateInterval = 1.0 / 60.0;
		[self.motionManager startAccelerometerUpdates];
	}

	// Prepare graphics and initialise the director.
	_director = new Director();
	self.director->init(Data::load_asset("main.lua"),
	                    Vec2i(size.width, size.height));
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	// TODO: Investigate whether propagating the warning to Director still has
	// any bad side effects.

	// Don't propagate the warning to Director. It will force Lua to collect
	// garbage and mess up iPad 1's OpenGL driver.
	//self.director->on_memory_warning();
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	[self.motionManager startAccelerometerUpdates];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[self.motionManager stopAccelerometerUpdates];
	[super viewDidDisappear:animated];
}

@end
