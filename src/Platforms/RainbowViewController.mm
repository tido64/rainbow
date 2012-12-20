// Copyright 2012 Bifrost Entertainment. All rights reserved.
// \author Tommy Nguyen

#import "RainbowViewController.h"

#include "Config.h"
#include "Director.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

@interface RainbowViewController()
{
	CGFloat scale;
	Director *director;  ///< Rainbow director.
	ConFuoco::Mixer mixer;
}

@property (strong, nonatomic) EAGLContext *context;

@end

@implementation RainbowViewController
@synthesize context = _context;

/// Convert an NSSet of touches to an array.
/// \note The dimension of the screen does not change with its orientation.
///       Since the iPad is in portrait mode by default, the resolution is
///       768x1024 in portrait mode as well.
/// \param touches  Set of touches to convert.
/// \return Array of touches.
- (Touch *)getTouches:(NSSet *)touches
{
	Touch *t_arr = new Touch[touches.count];
	Touch *t = t_arr;

	const CGSize size = [[UIScreen mainScreen] bounds].size;
	switch ([[UIApplication sharedApplication] statusBarOrientation])
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = [touch hash];
				t->x = p.x * self->scale;
				t->y = (size.height - p.y) * self->scale;
				p = [touch previousLocationInView:nil];
				t->x0 = p.x * self->scale;
				t->y0 = (size.height - p.y) * self->scale;
				++t;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = [touch hash];
				t->x = (size.width - p.x) * self->scale;
				t->y = p.y * self->scale;
				p = [touch previousLocationInView:nil];
				t->x0 = (size.width - p.x) * self->scale;
				t->y0 = p.y * self->scale;
				++t;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = [touch hash];
				t->x = (size.height - p.y) * self->scale;
				t->y = (size.width - p.x) * self->scale;
				p = [touch previousLocationInView:nil];
				t->x0 = (size.height - p.y) * self->scale;
				t->y0 = (size.width - p.x) * self->scale;
				++t;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = [touch hash];
				t->x = p.y * self->scale;
				t->y = p.x * self->scale;
				p = [touch previousLocationInView:nil];
				t->x0 = p.y * self->scale;
				t->y0 = p.x * self->scale;
				++t;
			}
			break;
		default:
			break;
	}

	return t_arr;
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	// Don't propagate the warning to Director. It will force Lua to collect
	// garbage and mess up iPad 1's OpenGL driver.
	//self->director->on_memory_warning();
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	Config config;
	if (config.needs_accelerometer())
	{
		// Enable accelerometer.
		[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / self.preferredFramesPerSecond)];
		[[UIAccelerometer sharedAccelerometer] setDelegate:self];
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
	self->director = new Director();

	// Swap screen width and height. See comments for getTouches().
	CGSize size = [[UIScreen mainScreen] bounds].size;
	if (UIInterfaceOrientationIsLandscape([[UIApplication sharedApplication] statusBarOrientation]))
	{
		CGFloat tmp = size.width;
		size.width = size.height;
		size.height = tmp;
	}
	self->scale = [[UIScreen mainScreen] scale];
	size.width *= self->scale;
	size.height *= self->scale;

	// Set screen resolution.
	Renderer::resize(size.width, size.height);
	self->director->set_video(size.width, size.height);

	// Load and initialise script.
	self->director->init(Data("main.lua"));
}

- (void)viewDidUnload
{
	[[UIAccelerometer sharedAccelerometer] setDelegate:nil];

	delete self->director;
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

#pragma mark - GLKViewDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	glClear(GL_COLOR_BUFFER_BIT);
	self->director->draw();
}

#pragma mark - GLKViewControllerDelegate

- (void)update
{
	self->director->update(self.timeSinceLastDraw * 1000);
}

#pragma mark - UIAccelerometerDelegate

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	Input::Instance->accelerated(
		acceleration.x, acceleration.y, acceleration.z, acceleration.timestamp);
}

#pragma mark - UIResponder

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self getTouches:touches];
	Input::Instance->touch_began(t, touches.count);
	delete[] t;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	Input::Instance->touch_canceled();
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self getTouches:touches];
	Input::Instance->touch_ended(t, touches.count);
	delete[] t;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	Touch *t = [self getTouches:touches];
	Input::Instance->touch_moved(t, touches.count);
	delete[] t;
}

@end
