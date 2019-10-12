// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "Platform/iOS/RainbowViewController.h"

#import <optional>

#import <CoreMotion/CoreMotion.h>

#include "Config.h"
#include "Director.h"
#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"
#include "Input/Pointer.h"

using rainbow::Bundle;
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
    std::optional<Director> _director;
    CMMotionManager* _motionManager;
    Pointer _pointers[kMaxTouches];
    Bundle _bundle;
}

- (instancetype)initWithNibName:(NSString*)nibNameOrNil
                         bundle:(NSBundle*)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        [self initialize];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder*)coder
{
    if (self = [super initWithCoder:coder])
    {
        [self initialize];
    }
    return self;
}

- (void)dealloc
{
    _director.reset();
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
        *ptr = Pointer{rainbow::narrow_cast<uint32_t>(touch.hash),
                       rainbow::truncate<int32_t>(p.x * scale),
                       rainbow::truncate<int32_t>((height - p.y) * scale),
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
        _director->input().accelerated(  //
            data.acceleration.x,
            data.acceleration.y,
            data.acceleration.z,
            data.timestamp);
    }
    _director->update(self.timeSinceLastDraw * 1000);
}

#pragma mark - GLKViewDelegate protocol

- (void)glkView:(GLKView*)__unused view drawInRect:(CGRect)__unused rect
{
    _director->draw();
}

#pragma mark - GLKViewController overrides

- (NSInteger)preferredFramesPerSecond
{
    return 60;
}

#pragma mark - UIResponder overrides

- (void)touchesBegan:(NSSet*)touches withEvent:(nullable UIEvent*)__unused event
{
    _director->input().on_pointer_began(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
}

- (void)touchesMoved:(NSSet*)touches withEvent:(nullable UIEvent*)__unused event
{
    _director->input().on_pointer_moved(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
}

- (void)touchesEnded:(NSSet*)touches withEvent:(nullable UIEvent*)__unused event
{
    _director->input().on_pointer_ended(
        ArrayView<Pointer>{[self convertTouches:touches], touches.count});
}

- (void)touchesCancelled:(NSSet*)__unused touches
               withEvent:(nullable UIEvent*)__unused event
{
    _director->input().on_pointer_canceled();
}

#ifdef USE_HEIMDALL
- (void)motionEnded:(UIEventSubtype)motion
          withEvent:(nullable UIEvent*)__unused event
{
    if (motion == UIEventSubtypeMotionShake)
        [self showDiagnosticTools];
}
#endif

#pragma mark - UIViewController overrides

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (self.context == nil)
    {
        NSLog(@"Failed to create ES context");
        return;
    }

    [EAGLContext setCurrentContext:self.context];

    const CGSize bounds = self.view.bounds.size;
    const CGFloat scale = self.view.contentScaleFactor;
    CGSize size = CGSizeMake(bounds.width * scale, bounds.height * scale);

    if (self.supportedInterfaceOrientations ==
        UIInterfaceOrientationMaskLandscape)
    {
        // Swap screen width and height. The retrieved screen size is based on
        // the device in a portrait-up orientation. It does not change as the
        // device rotates.
        std::swap(size.width, size.height);
    }

    _director.emplace();
    _director->init(Vec2i(size.width, size.height));

#ifdef USE_HEIMDALL
    UIKeyCommand* showDiagnosticTools =
        [UIKeyCommand keyCommandWithInput:@"d"
                            modifierFlags:UIKeyModifierCommand
                                   action:@selector(showDiagnosticTools)];
    [self addKeyCommand:showDiagnosticTools];
#endif
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

#pragma mark - Private

- (void)initialize
{
    rainbow::filesystem::initialize(_bundle, _bundle.exec_path(), false);

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

#ifdef USE_HEIMDALL
- (void)showDiagnosticTools
{
    _director->show_diagnostic_tools();
}
#endif

@end
