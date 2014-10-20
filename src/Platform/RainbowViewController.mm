// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "RainbowViewController.h"

#import <CoreMotion/CoreMotion.h>
#import <objc/runtime.h>

#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "Input/Touch.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace
{
	const NSTimeInterval kAccelerometerUpdateInterval = 1.0 / 60.0;
	const NSUInteger kMaxTouches = 16;

	CGSize GetScreenSize(UIScreen *screen)
	{
		if (![screen respondsToSelector:@selector(nativeBounds)])
		{
			CGSize size = screen.bounds.size;
			const CGFloat scale = screen.scale;
			size.width *= scale;
			size.height *= scale;
			return size;
		}
		return screen.nativeBounds.size;
	}
}

@interface RainbowViewController ()
@property(readonly, nonatomic) NSUInteger supportedInterfaceOrientations;
@property(readonly, nonatomic) Director *director;
@property(readonly, nonatomic) CMMotionManager *motionManager;
@end

@implementation RainbowViewController {
	Touch _touches[kMaxTouches];
}

+ (void)load {
	static dispatch_once_t dispatchOnce;
	dispatch_once(&dispatchOnce, ^{
		if (![UIScreen instancesRespondToSelector:@selector(coordinateSpace)])
		{
			Class klass = [self class];
			method_exchangeImplementations(
			    class_getInstanceMethod(klass, @selector(convertTouches:)),
			    class_getInstanceMethod(klass, @selector(touchesFromSet:)));
		}
	});
}

- (instancetype)init
{
	if (self = [super init])
	{
		self.preferredFramesPerSecond = 60;
		Rainbow::Config config;
		_supportedInterfaceOrientations =
		    (config.is_portrait()) ? UIInterfaceOrientationMaskPortrait
		                           : UIInterfaceOrientationMaskLandscape;
		if (config.needs_accelerometer())
		{
			_motionManager = [[CMMotionManager alloc] init];
			self.motionManager.accelerometerUpdateInterval =
			    kAccelerometerUpdateInterval;
		}
	}
	return self;
}

- (void)dealloc
{
	delete self.director;
	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
}

- (EAGLContext *)context
{
	return reinterpret_cast<GLKView*>(self.view).context;
}

- (void)setContext:(EAGLContext *)context
{
	reinterpret_cast<GLKView*>(self.view).context = context;
}

- (Touch *)touches
{
	return _touches;
}

/// Returns an array of UITouches converted to the coordinate space of our
/// viewport.
/// \param touches  Set of touches to convert.
/// \return Array of touches. Must not be deleted.
- (Touch *)convertTouches:(NSSet *)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	UIView *view = self.view;
	UIScreen *screen = view.window.screen;
	id<UICoordinateSpace> space = screen.coordinateSpace;
	const CGFloat height = CGRectGetHeight(space.bounds);
	const CGFloat scale = screen.nativeScale;
	Touch *t = self.touches;
	for (UITouch *touch in touches)
	{
		CGPoint p = [view convertPoint:[touch locationInView:view]
		             toCoordinateSpace:space];
		t->hash = touch.hash;
		t->x = p.x * scale;
		t->y = (height - p.y) * scale;
		p = [view convertPoint:[touch previousLocationInView:view]
		     toCoordinateSpace:space];
		t->x0 = p.x * scale;
		t->y0 = (height - p.y) * scale;
		++t;
	}
	return self.touches;
}

#pragma mark - GLKViewControllerDelegate protocol

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

#pragma mark - GLKViewDelegate protocol

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	self.director->draw();
}

#pragma mark - UIResponder overrides

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().on_touch_began([self convertTouches:touches],
	                                      touches.count);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().on_touch_moved([self convertTouches:touches],
	                                      touches.count);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().on_touch_ended([self convertTouches:touches],
	                                      touches.count);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	self.director->input().on_touch_canceled();
}

#pragma mark - UIViewController overrides

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.view.multipleTouchEnabled = YES;
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (!self.context)
	{
		NSLog(@"[Rainbow] Failed to create ES context");
		return;
	}
	[EAGLContext setCurrentContext:self.context];

	[self.motionManager startAccelerometerUpdates];

	CGSize size = GetScreenSize([UIScreen mainScreen]);
	if (self.supportedInterfaceOrientations ==
	    UIInterfaceOrientationMaskLandscape)
	{
		// Swap screen width and height. The retrieved screen size is based on
		// the device in a portrait-up orientation. It does not change as the
		// device rotates.
		std::swap(size.width, size.height);
	}
	_director = new Director();
	self.director->init(Data::load_asset("main.lua"),
	                    Vec2i(size.width, size.height));
}

- (BOOL)shouldAutorotate
{
	return YES;
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

- (BOOL)prefersStatusBarHidden
{
	return YES;
}

#pragma mark - Deprecated in iOS 8.0

- (Touch *)touchesFromSet:(NSSet *)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	UIScreen *screen = self.view.window.screen;
	const CGSize size = GetScreenSize(screen);
	const CGFloat scale = screen.scale;
	Touch *t = self.touches;
	switch ([UIApplication sharedApplication].statusBarOrientation)
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = touch.hash;
				t->x = p.x * scale;
				t->y = size.height - p.y * scale;
				p = [touch previousLocationInView:nil];
				t->x0 = p.x * scale;
				t->y0 = size.height - p.y * scale;
				t->timestamp = touch.timestamp * 1000.0;
				++t;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = touch.hash;
				t->x = size.width - p.x * scale;
				t->y = p.y * scale;
				p = [touch previousLocationInView:nil];
				t->x0 = size.width - p.x * scale;
				t->y0 = p.y * scale;
				t->timestamp = touch.timestamp * 1000.0;
				++t;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = touch.hash;
				t->x = size.height - p.y * scale;
				t->y = size.width - p.x * scale;
				p = [touch previousLocationInView:nil];
				t->x0 = size.height - p.y * scale;
				t->y0 = size.width - p.x * scale;
				t->timestamp = touch.timestamp * 1000.0;
				++t;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch *touch in touches)
			{
				CGPoint p = [touch locationInView:nil];
				t->hash = touch.hash;
				t->x = p.y * scale;
				t->y = p.x * scale;
				p = [touch previousLocationInView:nil];
				t->x0 = p.y * scale;
				t->y0 = p.x * scale;
				t->timestamp = touch.timestamp * 1000.0;
				++t;
			}
			break;
		default:
			R_ASSERT(false, "Reached unreachable code");
			break;
	}
	return self.touches;
}

@end
