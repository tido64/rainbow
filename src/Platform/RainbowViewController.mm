// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "RainbowViewController.h"

#import <CoreMotion/CoreMotion.h>
#import <objc/runtime.h>

#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "Input/Pointer.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace
{
	constexpr NSTimeInterval kAccelerometerUpdateInterval = 1.0 / 60.0;
	constexpr NSUInteger kMaxTouches = 16;

	CGSize GetScreenSize(UIScreen* screen)
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
@end

@implementation RainbowViewController
{
	Director* _director;
	CMMotionManager* _motionManager;
	Pointer _pointers[kMaxTouches];
}

+ (void)load
{
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
		rainbow::Config config;
		_supportedInterfaceOrientations =
		    (config.is_portrait() ? UIInterfaceOrientationMaskPortrait
		                          : UIInterfaceOrientationMaskLandscape);
		if (config.needs_accelerometer())
		{
			_motionManager = [[CMMotionManager alloc] init];
			_motionManager.accelerometerUpdateInterval =
			    kAccelerometerUpdateInterval;
		}
	}
	return self;
}

- (void)dealloc
{
	delete _director;
	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
}

- (EAGLContext*)context
{
	return reinterpret_cast<GLKView*>(self.view).context;
}

- (void)setContext:(EAGLContext*)context
{
	reinterpret_cast<GLKView*>(self.view).context = context;
}

- (Pointer*)pointers { return _pointers; }

/// Returns an array of UITouches converted to the coordinate space of our
/// viewport.
/// \param touches  Set of touches to convert.
/// \return Array of Pointers. Must not be deleted.
- (Pointer*)convertTouches:(NSSet*)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	UIView* view = self.view;
	UIScreen* screen = view.window.screen;
	id<UICoordinateSpace> space = screen.coordinateSpace;
	const CGFloat height = CGRectGetHeight(space.bounds);
	const CGFloat scale = screen.nativeScale;
	Pointer* ptr = self.pointers;
	for (UITouch* touch in touches)
	{
		const CGPoint p = [view convertPoint:[touch locationInView:view]
		                   toCoordinateSpace:space];
		ptr->hash = touch.hash;
		ptr->x = p.x * scale;
		ptr->y = (height - p.y) * scale;
		++ptr;
	}
	return self.pointers;
}

#pragma mark - GLKViewControllerDelegate protocol

- (void)update
{
	if (_motionManager.accelerometerActive)
	{
		const CMAccelerometerData* data = _motionManager.accelerometerData;
		_director->input().accelerated(data.acceleration.x,
		                               data.acceleration.y,
		                               data.acceleration.z,
		                               data.timestamp);
	}
	_director->update(self.timeSinceLastDraw * 1000);
}

#pragma mark - GLKViewDelegate protocol

- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
	_director->draw();
}

#pragma mark - GLKViewController overrides

- (NSInteger)preferredFramesPerSecond { return 60; }

#pragma mark - UIResponder overrides

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	_director->input().on_pointer_began(touches.count,
	                                    [self convertTouches:touches]);
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	_director->input().on_pointer_moved(touches.count,
	                                    [self convertTouches:touches]);
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	_director->input().on_pointer_ended(touches.count,
	                                    [self convertTouches:touches]);
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	_director->input().on_pointer_canceled();
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

	[_motionManager startAccelerometerUpdates];

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
	_director->init(Vec2i(size.width, size.height));
}

- (BOOL)shouldAutorotate { return YES; }

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
	_director->on_memory_warning();
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	[_motionManager startAccelerometerUpdates];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[_motionManager stopAccelerometerUpdates];
	[super viewDidDisappear:animated];
}

- (BOOL)prefersStatusBarHidden { return YES; }

#pragma mark - Deprecated in iOS 8.0

- (Pointer*)touchesFromSet:(NSSet*)touches
{
	R_ASSERT(touches.count <= kMaxTouches, "Unsupported number of touches");

	UIScreen* screen = self.view.window.screen;
	const CGSize size = GetScreenSize(screen);
	const CGFloat scale = screen.scale;
	Pointer* ptr = self.pointers;
	switch ([UIApplication sharedApplication].statusBarOrientation)
	{
		case UIInterfaceOrientationPortrait:
			for (UITouch* touch in touches)
			{
				const CGPoint p = [touch locationInView:nil];
				ptr->hash = touch.hash;
				ptr->x = p.x * scale;
				ptr->y = size.height - p.y * scale;
				ptr->timestamp = touch.timestamp * 1000.0;
				++ptr;
			}
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			for (UITouch* touch in touches)
			{
				const CGPoint p = [touch locationInView:nil];
				ptr->hash = touch.hash;
				ptr->x = size.width - p.x * scale;
				ptr->y = p.y * scale;
				ptr->timestamp = touch.timestamp * 1000.0;
				++ptr;
			}
			break;
		case UIInterfaceOrientationLandscapeLeft:
			for (UITouch* touch in touches)
			{
				const CGPoint p = [touch locationInView:nil];
				ptr->hash = touch.hash;
				ptr->x = size.height - p.y * scale;
				ptr->y = size.width - p.x * scale;
				ptr->timestamp = touch.timestamp * 1000.0;
				++ptr;
			}
			break;
		case UIInterfaceOrientationLandscapeRight:
			for (UITouch* touch in touches)
			{
				const CGPoint p = [touch locationInView:nil];
				ptr->hash = touch.hash;
				ptr->x = p.y * scale;
				ptr->y = p.x * scale;
				ptr->timestamp = touch.timestamp * 1000.0;
				++ptr;
			}
			break;
		default:
			R_ASSERT(false, "Reached unreachable code.");
			UNREACHABLE();
			break;
	}
	return self.pointers;
}

@end
