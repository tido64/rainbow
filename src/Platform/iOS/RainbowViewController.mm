// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "Platform/iOS/RainbowViewController.h"

#import <CoreMotion/CoreMotion.h>

#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "Input/Pointer.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

using rainbow::Pointer;
using rainbow::Vec2i;

namespace
{
    constexpr NSTimeInterval kAccelerometerUpdateInterval = 1.0 / 60.0;
    constexpr NSUInteger kMaxTouches = 16;
}

@interface RainbowViewController ()
@property(readonly, nonatomic) NSUInteger supportedInterfaceOrientations;
@end

@implementation RainbowViewController
{
    std::unique_ptr<Director> _director;
    CMMotionManager* _motionManager;
    Pointer _pointers[kMaxTouches];
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
    _director.release();
    if (EAGLContext.currentContext == self.context)
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

- (Pointer*)pointers
{
    return _pointers;
}

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
        *ptr = Pointer{static_cast<uint32_t>(touch.hash),
                       static_cast<int32_t>(p.x * scale),
                       static_cast<int32_t>((height - p.y) * scale),
                       0};
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

- (NSInteger)preferredFramesPerSecond
{
    return 60;
}

#pragma mark - UIResponder overrides

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    _director->input().on_pointer_began(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    _director->input().on_pointer_moved(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    _director->input().on_pointer_ended(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
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
    if (self.context == nil)
    {
        NSLog(@"[Rainbow] Failed to create ES context");
        return;
    }

    [EAGLContext setCurrentContext:self.context];

    CGSize size = UIScreen.mainScreen.nativeBounds.size;
    if (self.supportedInterfaceOrientations ==
        UIInterfaceOrientationMaskLandscape)
    {
        // Swap screen width and height. The retrieved screen size is based on
        // the device in a portrait-up orientation. It does not change as the
        // device rotates.
        std::swap(size.width, size.height);
    }
    _director = std::make_unique<Director>();
    _director->init(Vec2i(size.width, size.height));
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

- (BOOL)shouldAutorotate
{
    return YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    _director->on_memory_warning();
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

@end
