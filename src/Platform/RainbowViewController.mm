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

namespace
{
	const size_t kMaxTouches = 32;
}

@interface RainbowViewController () {
@private
	ConFuoco::Mixer _mixer;
	Touch _touchesArray[kMaxTouches];
}
@property (strong, nonatomic) EAGLContext *context;
@property (readonly, nonatomic) Director *director;
@property (strong, nonatomic) CMMotionManager *motionManager;
@property (readonly, nonatomic) CGFloat scale;

/// Convert an NSSet of touches to an array.
/// \note The dimension of the screen does not change with its orientation.
///       Since the iPad is in portrait mode by default, the resolution is
///       768x1024 in landscape mode as well.
/// \param touches  Set of touches to convert.
/// \return Array of touches. Must not be deleted.
- (Touch *)touchesArrayFromSet:(NSSet *)touches;
@end

@implementation RainbowViewController

- (void)dealloc
{
	delete self.director;
	Renderer::release();

	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
}

- (Touch *)touchesArrayFromSet:(NSSet *)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	Touch *arrayRef = _touchesArray;
	const CGSize size = [[UIScreen mainScreen] bounds].size;
	switch ([[UIApplication sharedApplication] statusBarOrientation])
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = p.x * self.scale;
				arrayRef->y = (size.height - p.y) * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.x * self.scale;
				arrayRef->y0 = (size.height - p.y) * self.scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = (size.width - p.x) * self.scale;
				arrayRef->y = p.y * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.width - p.x) * self.scale;
				arrayRef->y0 = p.y * self.scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = (size.height - p.y) * self.scale;
				arrayRef->y = (size.width - p.x) * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = (size.height - p.y) * self.scale;
				arrayRef->y0 = (size.width - p.x) * self.scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				arrayRef->hash = touch.hash;
				arrayRef->x = p.y * self.scale;
				arrayRef->y = p.x * self.scale;
				p = [touch previousLocationInView:nil];
				arrayRef->x0 = p.y * self.scale;
				arrayRef->y0 = p.x * self.scale;
				arrayRef->timestamp = touch.timestamp * 1000.0;
				++arrayRef;
			}
			break;
		default:
			R_ASSERT(false, "Reached unreachable code");
			break;
	}

	return _touchesArray;
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
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self touchesArrayFromSet:touches];
	Input::Instance->touch_moved(t, touches.count);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self touchesArrayFromSet:touches];
	Input::Instance->touch_ended(t, touches.count);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	Input::Instance->touch_canceled();
}

#pragma mark - UIViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	Rainbow::Config config;
	if (config.needs_accelerometer())
	{
		// Enable accelerometer.
		self.motionManager = [[CMMotionManager alloc] init];
		self.motionManager.accelerometerUpdateInterval = 1.0 / 60.0;
		[self.motionManager startAccelerometerUpdates];
	}

	self.view.multipleTouchEnabled = YES;

	// Set up OpenGL ES 2.0 context.
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (!self.context)
	{
		NSLog(@"[Rainbow] Failed to create ES context");
		return;
	}
	((GLKView *)self.view).context = self.context;
	self.preferredFramesPerSecond = 60;
	[EAGLContext setCurrentContext:self.context];

	// Prepare graphics and initialise the director.
	Renderer::init();
	_director = new Director();

	// Swap screen width and height. See comments for touchesArrayFromSet:.
	CGSize size = [[UIScreen mainScreen] bounds].size;
	if (UIInterfaceOrientationIsLandscape([[UIApplication sharedApplication] statusBarOrientation]))
		Rainbow::swap(size.width, size.height);
	_scale = [[UIScreen mainScreen] scale];
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
	if (self.motionManager)
		[self.motionManager stopAccelerometerUpdates];

	[super viewDidDisappear:animated];
}

- (NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskLandscape;
}

// TODO: Deprecated in iOS 6.0

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
