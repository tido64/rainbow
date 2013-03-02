// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#import "RainbowViewController.h"

#import <CoreMotion/CoreMotion.h>

#include "Config.h"
#include "Director.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"
#include "Input/Touch.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

@interface RainbowViewController () {
@private
	ConFuoco::Mixer mixer;
}
@property (strong, nonatomic) EAGLContext *context;
@property (assign, nonatomic) Director *director;
@property (strong, nonatomic) CMMotionManager *motionManager;
@property (assign, nonatomic) CGFloat scale;

- (Touch *)touchesArrayFromSet:(NSSet *)touches;
@end

@implementation RainbowViewController

/// Convert an NSSet of touches to an array.
/// \note The dimension of the screen does not change with its orientation.
///       Since the iPad is in portrait mode by default, the resolution is
///       768x1024 in landscape mode as well.
/// \param touches  Set of touches to convert.
/// \return Array of touches.
- (Touch *)touchesArrayFromSet:(NSSet *)touches
{
	Touch *array = new Touch[touches.count];
	Touch *arrayRef = array;

	const CGSize size = [[UIScreen mainScreen] bounds].size;
	switch ([[UIApplication sharedApplication] statusBarOrientation])
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = [touch hash];
				arrayRef->x = p.x * self.scale;
				arrayRef->y = (size.height - p.y) * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.x * self.scale;
				arrayRef->y0 = (size.height - p.y) * self.scale;
				arrayRef->timestamp = [touch timestamp] * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = [touch hash];
				arrayRef->x = (size.width - p.x) * self.scale;
				arrayRef->y = p.y * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.width - p.x) * self.scale;
				arrayRef->y0 = p.y * self.scale;
				arrayRef->timestamp = [touch timestamp] * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = [touch hash];
				arrayRef->x = (size.height - p.y) * self.scale;
				arrayRef->y = (size.width - p.x) * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.height - p.y) * self.scale;
				arrayRef->y0 = (size.width - p.x) * self.scale;
				arrayRef->timestamp = [touch timestamp] * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = [touch hash];
				arrayRef->x = p.y * self.scale;
				arrayRef->y = p.x * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.y * self.scale;
				arrayRef->y0 = p.x * self.scale;
				arrayRef->timestamp = [touch timestamp] * 1000.0;
				++arrayRef;
			}
			break;
		default:
			R_ASSERT(false, "Reached unreachable code");
			break;
	}

	return array;
}

#pragma mark - GLKViewControllerDelegate

- (void)update
{
	if (self.motionManager.accelerometerActive)
	{
		const CMAccelerometerData *data = self.motionManager.accelerometerData;
		Input::Instance->accelerated(
				data.acceleration.x, data.acceleration.y, data.acceleration.z,
				data.timestamp);
	}
	self.director->update(self.timeSinceLastDraw * 1000);
}

#pragma mark - GLKViewDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	glClear(GL_COLOR_BUFFER_BIT);
	self.director->draw();
}

#pragma mark - UIResponder

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self touchesArrayFromSet:touches];
	Input::Instance->touch_began(t, touches.count);
	delete[] t;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self touchesArrayFromSet:touches];
	Input::Instance->touch_moved(t, touches.count);
	delete[] t;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self touchesArrayFromSet:touches];
	Input::Instance->touch_ended(t, touches.count);
	delete[] t;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	Input::Instance->touch_canceled();
}

#pragma mark - UIViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.motionManager = [[CMMotionManager alloc] init];

	Rainbow::Config config;
	if (config.needs_accelerometer())
	{
		// Enable accelerometer.
		self.motionManager.accelerometerUpdateInterval = 1.0 / 60.0;
		[self.motionManager startAccelerometerUpdates];
	}
	[[self view] setMultipleTouchEnabled:YES];

	// Set up OpenGL ES 2.0 context.
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (!self.context)
	{
		NSLog(@"[Rainbow] Failed to create ES context");
		return;
	}
	((GLKView *)self.view).context = self.context;
	[self setPreferredFramesPerSecond:60];
	[EAGLContext setCurrentContext:self.context];

	// Prepare graphics and initialise the director.
	Renderer::init();
	self.director = new Director();

	// Swap screen width and height. See comments for touchesArrayFromSet:.
	CGSize size = [[UIScreen mainScreen] bounds].size;
	if (UIInterfaceOrientationIsLandscape([[UIApplication sharedApplication] statusBarOrientation]))
	{
		CGFloat tmp = size.width;
		size.width = size.height;
		size.height = tmp;
	}
	self.scale = [[UIScreen mainScreen] scale];
	size.width *= self.scale;
	size.height *= self.scale;

	// Set screen resolution.
	Renderer::resize(size.width, size.height);

	// Load and initialise script.
	self.director->init(Data("main.lua"), size.width, size.height);
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	// Don't propagate the warning to Director. It will force Lua to collect
	// garbage and mess up iPad 1's OpenGL driver.
	//self.director->on_memory_warning();
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];

	if (self.motionManager)
		[self.motionManager startAccelerometerUpdates];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];

	if (self.motionManager)
		[self.motionManager stopAccelerometerUpdates];
}

- (NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskLandscape;
}

// TODO: Deprecated in iOS 6.0

- (void)viewDidUnload
{
	delete self.director;
	Renderer::release();

	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
	self.context = nil;

	[super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	BOOL supported = NO;
	switch (interfaceOrientation) {
		case UIInterfaceOrientationLandscapeLeft:
		case UIInterfaceOrientationLandscapeRight:
			supported = YES;
		//case UIInterfaceOrientationPortrait:
		//case UIInterfaceOrientationPortraitUpsideDown:
		default:
			break;
	}
	return supported;
}

@end
