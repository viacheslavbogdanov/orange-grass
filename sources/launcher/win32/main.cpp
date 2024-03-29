/*
 * OrangeGrass
 * Copyright (C) 2009 Vyacheslav Bogdanov.
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
#include "main.h"
#include "common.h"
#include "Game.h"

#define TIMER_ID	1
#define TIMER_RATE	30


IOGGameSystem*  pGameSystem = NULL;
int				ScrWidth, ScrHeight;
std::vector<std::string> CmdParams;


/*!***************************************************************************
 @Function			GetResourcePathASCII
 @Output            _pOutPath output path string
 @Input				_PathLength max. path length
 @Description		Returns the full path to resources
 ****************************************************************************/
void GetResourcePathASCII(char* _pOutPath, int _PathLength);


/// Application initialization.
void Initialize ()
{
    GLuint PixelFormat;
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 16;
    pfd.cDepthBits = 16;
    shDC = GetDC( shWnd );
    PixelFormat = ChoosePixelFormat( shDC, &pfd );
    SetPixelFormat( shDC, PixelFormat, &pfd);
    shRC = wglCreateContext( shDC );
    wglMakeCurrent( shDC, shRC );
    glewInit();

    glViewport(0, 0, ScrWidth, ScrHeight);
    glDisable(GL_CULL_FACE);

    pGameSystem = GetGame();
}


/// Application shutdown.
void Shutdown()
{
    PostQuitMessage(0);
    //wglDeleteContext( shRC );
}


/// Application window procedure.
LRESULT CALLBACK WndProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_DESTROY:
			Shutdown();
     		break;

		case WM_KEYDOWN:
            if (wParam == VK_LEFT)
            {
                sMouseX -= 3;
            }
            else if (wParam == VK_RIGHT)
            {
                sMouseX += 3;
            }
            pGameSystem->OnPointerMove(sMouseX, sMouseY);
            //pGameSystem->OnKeyDown(0);
			break;

		case WM_KEYUP:
            //pGameSystem->OnKeyUp(0);
			break;

		case WM_LBUTTONUP:
        {
            bTouch = false;
            sTouchX = (SHORT)LOWORD(lParam);
            sTouchY = (SHORT)HIWORD(lParam);
            pGameSystem->OnPointerUp(sTouchX, sTouchY);
        }
        break;

		case WM_LBUTTONDOWN:
        {
            bTouch = true;
            sTouchX = (SHORT)LOWORD(lParam);
            sTouchY = (SHORT)HIWORD(lParam);
            pGameSystem->OnPointerDown(sTouchX, sTouchY);
            //int X = (SHORT)LOWORD(lParam);
            //int Y = (SHORT)HIWORD(lParam);
            //pGameSystem->OnPointerDown(X, Y);
        }
        break;

		case WM_MOUSEMOVE:
		{
            //sMouseX = (SHORT)LOWORD(lParam);
            //sMouseY = (SHORT)HIWORD(lParam);
            //pGameSystem->OnPointerMove(sMouseX, sMouseY);
		}
		break;

		default:
			return DefWindowProc ( hWnd, message, wParam, lParam );
   }
   return 0;
}


/// timer callback function
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (idEvent == TIMER_ID)
	{
        if (pGameSystem->GetControllerState() != SYSSTATE_EXIT)
        {
            //pGameSystem->OnPointerMove(sMouseX, sMouseY);
            //if (bTouch)
            //{
            //    pGameSystem->OnPointerDown(sTouchX, sTouchY);
            //}
            pGameSystem->Update(33);
            pGameSystem->Draw();
            SwapBuffers(shDC);
        }
        else
        {
            Shutdown();
        }
	}
}


/// Application window initialization.
BOOL InitInstance ( HINSTANCE hInstance, int nCmdShow )
{
	shInstance = hInstance;
	shWnd = FindWindow ( L"SkyCrasher.MainWindow", L"SkyCrasher" );

	if ( shWnd )
	{
		SetForegroundWindow ((HWND)(((__int64)shWnd) | 0x01));    
		return FALSE;
	} 

	GetGlobalVars()->SetIVar("view_width", 480);
	GetGlobalVars()->SetIVar("view_height", 800);
	GetAppSettings()->Init("settings.xml");

	if (CmdParams.size() == 2)
	{
		GetGlobalVars()->SetSVar("profile", CmdParams[0]);
		if (CmdParams[1].compare("l") == 0)
		{
			GetGlobalVars()->SetIVar("landscape", 1);
		}
		else
		{
			GetGlobalVars()->SetIVar("landscape", 0);
		}
	}

	GetAppSettings()->InitScreenMode();

	ScrWidth = GetGlobalVars()->GetIVar("view_width");
	ScrHeight = GetGlobalVars()->GetIVar("view_height");

	WNDCLASS	wc;
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= L"SkyCrasher.MainWindow";
	RegisterClass ( &wc );
	
    int wndSizeX = ScrWidth + (GetSystemMetrics(SM_CXBORDER) * 2);
    int wndSizeY = ScrHeight + GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER);

	shWnd = CreateWindow (	L"SkyCrasher.MainWindow", L"SkyCrasher", WS_SYSMENU|WS_OVERLAPPED,
							(GetSystemMetrics(SM_CXSCREEN)-wndSizeX)/2, 
							(GetSystemMetrics(SM_CYSCREEN)-wndSizeY)/2, 
							wndSizeX, wndSizeY, NULL, NULL, hInstance, NULL);
	if ( !shWnd )
		return FALSE;

    if (SetTimer(shWnd, TIMER_ID,  TIMER_RATE,  (TIMERPROC)TimerProc)!=TIMER_ID ) 
        return FALSE;

	ShowWindow(shWnd, nCmdShow);	
	UpdateWindow(shWnd);

	Initialize ();

	return TRUE;
}


/// main function
int WINAPI WinMain( HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpszCmdLine,
					int nCmdShow )
{
    char path[OG_MAX_PATH];
    GetResourcePathASCII(path, OG_MAX_PATH);
    std::string strPath = std::string(path) + std::string("./assets/");
    //std::string strPath = std::string(path) + std::string("/orange-grass.apk");
    StartOrangeGrass(strPath, false);
    StartGameCore();

	char* pch;
	pch = strtok (lpszCmdLine, " -");
	while (pch != NULL)
	{
		CmdParams.push_back(std::string(pch));
		pch = strtok (NULL, " -");
	}

	if ( !InitInstance ( hInstance, nCmdShow ) ) 
		return FALSE;

	MSG msg;
    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            WaitMessage();
        }
    }

    FinishGameCore();
	FinishOrangeGrass();

	return (int)msg.wParam;
}


/*!***************************************************************************
 @Function			GetResourcePathASCII
 @Output            _pOutPath output path string
 @Input				_PathLength max. path length
 @Description		Returns the full path to resources
 ****************************************************************************/
void GetResourcePathASCII(char* _pOutPath, int _PathLength)
{
    {
        wchar_t* pPath = new wchar_t [ _PathLength ];
        GetModuleFileName ( NULL, pPath, _PathLength );
        WideCharToMultiByte( CP_ACP, 0, pPath, -1, _pOutPath, _PathLength, "", false );
    }
    int pos = (int)strlen( _pOutPath );
    while ( --pos )
    {
        if ( _pOutPath [ pos ] == '\\') 
        {
            _pOutPath [ pos ] = '\0';
            break;
        }
        else
            _pOutPath [ pos + 1 ] = ' ';
    }
}
