/*
 * OrangeGrass
 * Copyright (C) 2019 Vyacheslav Bogdanov.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/lgpl-3.0-standalone.html>.
 */
 
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <cstdio>
#include <cstdlib>
#include <string>

#include <OpenGL2.h>
#include <common.h>
#include <Game.h>

static const int MAX_FPS = 30;

IOGGameSystem*  pGameSystem = nullptr;

uint64_t TimeStampNsec()
{
    timespec timestamp;
    static int timer_type = CLOCK_MONOTONIC_RAW;
    while (0 != clock_gettime(timer_type, &timestamp))
    {
        assert(timer_type == CLOCK_MONOTONIC_RAW);
        timer_type = CLOCK_MONOTONIC;
    }
    return ((uint64_t)timestamp.tv_sec) * ((uint64_t)1000000000) + (uint64_t)(timestamp.tv_nsec);
};

uint64_t TimeStampUsec()
{
    return TimeStampNsec() / 1000;
}

void uSleep(uint64_t in_uSecSleep)
{
    timespec pause;
    pause.tv_sec = in_uSecSleep / (uint64_t)1000000;
    pause.tv_nsec = (in_uSecSleep - pause.tv_sec * 1000000) * 1000;
    nanosleep(&pause, nullptr);
}

struct GLWindow
{
    Display *            display;
    int                  screen;
    Window               window;
    GLXContext           context;
    XSetWindowAttributes attr;
    int                  x, y;
    unsigned int         width, height;
};

GLWindow  GLWin;
uint64_t g_renderStartUsec;
uint64_t g_frameStartUsec;

static int attrListDbl[] =
{
    GLX_RGBA,
    GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_DEPTH_SIZE, 16,
    None
};

void createWindow()
{
    GetGlobalVars()->SetIVar("view_width", 480);
    GetGlobalVars()->SetIVar("view_height", 800);

    GetAppSettings()->Init("settings.xml");
    GetAppSettings()->InitScreenMode();

    int ScrWidth = GetGlobalVars()->GetIVar("view_width");
    int ScrHeight = GetGlobalVars()->GetIVar("view_height");

    Atom         wmDelete;
    Window       winDummy;
    unsigned int borderDummy;

    GLWin.display = XOpenDisplay(nullptr);
    if (!GLWin.display)
    {
        OG_LOG_ERROR("XOpenDisplay() failed");
        exit(1);
    }

    GLWin.screen = XDefaultScreen(GLWin.display);

    XVisualInfo *vi = glXChooseVisual(GLWin.display, GLWin.screen, attrListDbl);
    if (vi == nullptr)
    {
        OG_LOG_ERROR("glXChooseVisual() failed");
        exit(1);
    }

    GLWin.context = glXCreateContext(GLWin.display, vi, nullptr, GL_TRUE);
    if (!GLWin.context)
    {
        OG_LOG_ERROR("glXCreateContext() failed");
        exit(1);
    }

    GLWin.attr.colormap = XCreateColormap(GLWin.display, XRootWindow(GLWin.display, vi->screen), vi->visual, AllocNone);
    GLWin.attr.border_pixel = 0;
    GLWin.attr.event_mask   = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask
                            | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
                            | PropertyChangeMask | VisibilityChangeMask;
    GLWin.window = XCreateWindow(GLWin.display,
                           RootWindow(GLWin.display, vi->screen),
                           0,
                           0,
                           ScrWidth,
                           ScrHeight,
                           0,
                           vi->depth,
                           InputOutput,
                           vi->visual,
                           CWBorderPixel | CWColormap | CWEventMask,
                           &GLWin.attr);
    wmDelete = XInternAtom(GLWin.display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(GLWin.display, GLWin.window, &wmDelete, 1);
    XClassHint *hint = XAllocClassHint();
    //hint->res_name   = (char *)ICON_NAME;
    //hint->res_class  = (char *)ICON_NAME;
    XSetClassHint(GLWin.display, GLWin.window, hint);
    XFree(hint);
    XSizeHints hints = {PMinSize, 0, 0, 0, 0, ScrWidth, ScrHeight};
    XSetWMNormalHints(GLWin.display, GLWin.window, &hints);
    XMapRaised(GLWin.display, GLWin.window);

    glXMakeCurrent(GLWin.display, GLWin.window, GLWin.context);

    glViewport(0, 0, ScrWidth, ScrHeight);
    glDisable(GL_CULL_FACE);
}

void renderGL()
{
    uint64_t now      = TimeStampUsec();
    float deltaSecs   = (float)(now - g_renderStartUsec) / 1000000.f;
    g_renderStartUsec = now;

    pGameSystem->Update(33);
    pGameSystem->Draw();

    glXSwapBuffers(GLWin.display, GLWin.window);

    static const uint64_t usPerFrame = 1000000 / MAX_FPS;
    uint64_t frameDuration = TimeStampUsec() - g_frameStartUsec;

    if (usPerFrame > frameDuration)
    {
        uSleep(usPerFrame - frameDuration);
    }

    g_frameStartUsec = TimeStampUsec();
}

int main(int argc, char **argv)
{
    StartOrangeGrass("./assets/", false);
    StartGameCore();

    createWindow();

    XStoreName(GLWin.display, GLWin.window, "Sky Crasher");

    g_renderStartUsec = TimeStampUsec();

    pGameSystem = GetGame();

    bool visible = false;
    bool done = false;
    while (!done)
    {
        XEvent event;
        // If the window is not visible, block until the next event comes.
        if (!visible)
        {
            XPeekEvent(GLWin.display, &event);
        }

        while (XPending(GLWin.display) > 0)
        {
            XNextEvent(GLWin.display, &event);
            switch (event.type)
            {
                case VisibilityNotify:
                    if (event.xvisibility.state == VisibilityFullyObscured)
                        visible = false;
                    if (event.xvisibility.state == VisibilityUnobscured
                     || event.xvisibility.state == VisibilityPartiallyObscured)
                        visible = true;
                    break;
                case ClientMessage:
                    if (strcmp(XGetAtomName(GLWin.display, event.xclient.message_type), "WM_PROTOCOLS") == 0)
                    {
                        done = true;
                    }
                    break;
                default:
                    break;
            }
        }
        if (visible)
            renderGL();
    }

    FinishGameCore();
    FinishOrangeGrass();

    glXDestroyContext(GLWin.display, GLWin.context);
    XCloseDisplay(GLWin.display);
    return 0;
}
