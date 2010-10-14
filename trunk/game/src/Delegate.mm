/*
 Oolong Engine for the iPhone / iPod touch
 Copyright (c) 2007-2008 Wolfgang Engel  http://code.google.com/p/oolongengine/
 
 This software is provided 'as-is', without any express or implied warranty.
 In no event will the authors be held liable for any damages arising from the use of this software.
 Permission is granted to anyone to use this software for any purpose, 
 including commercial applications, and to alter it and redistribute it freely, 
 subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
 */

#import "GameSystem.h"
#import "Delegate.h"
#import "Accelerometer.h"
#import "TouchScreen.h"


#define kFPS			33.0

static CGameSystem *shell = NULL;
static Accel *accel = NULL;


@implementation AppController

- (void) update
{
    double vec[3];
    [accel GetAccelerometerVector: (double*)vec];
    shell->OnPointerMove(vec[1]*50, vec[2]*50);
    
    TouchScreenValues* pVals = GetValuesTouchScreen();
    if (pVals->CountTouchesBegan > 0)
    {
        float x = pVals->LocationXTouchesBegan/320.0f*480.0f;
        float y = pVals->LocationYTouchesBegan/480.0f*320.0f;
        shell->OnPointerDown(x,y);
    }
    else if (pVals->CountTouchesMoved > 0)
    {
        float x = pVals->LocationXTouchesMoved/320.0f*480.0f;
        float y = pVals->LocationYTouchesMoved/480.0f*320.0f;
        shell->OnPointerDown(x,y);
    }
    
	shell->Update(30);
    shell->Draw();
	
    glFlush();
	[_glView swapBuffers];
}


- (void) applicationDidFinishLaunching:(UIApplication*)application
{
	CGRect	rect = [[UIScreen mainScreen] bounds];
	
	// create a full-screen window
	_window = [[UIWindow alloc] initWithFrame:rect];
	
	// create the OpenGL view and add it to the window
	_glView = [[EAGLCameraView alloc] initWithFrame:rect pixelFormat:GL_RGB565_OES depthFormat:GL_DEPTH_COMPONENT16_OES preserveBackbuffer:NO];
	
	[_window addSubview:_glView];

	// show the window
	[_window makeKeyAndVisible];
    
    accel = [Accel alloc];
    [accel SetupAccelerometer: kFPS];
	
	shell = new CGameSystem();
	if(!shell)
		printf("InitApplication error\n");
	
	// create our rendering timer
	[NSTimer scheduledTimerWithTimeInterval:(1.0 / kFPS) target:self selector:@selector(update) userInfo:nil repeats:YES];
}


- (void) dealloc
{
    [accel release];
	delete shell;
    shell = NULL;

	[_glView release];
	[_window release];
	
	[super dealloc];
}

@end