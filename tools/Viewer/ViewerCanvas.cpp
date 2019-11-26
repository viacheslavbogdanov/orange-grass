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
#include <Opengl2.h>
#include <wx/wx.h>
#include "ViewerCanvas.h"
#include <OrangeGrass.h>
#include <IOGGraphicsHelpers.h>
#include <IOGMath.h>
#include "ViewerScene.h"


#define TIMER_ID    1000

BEGIN_EVENT_TABLE(CViewerCanvas, wxGLCanvas)
    EVT_TIMER(TIMER_ID, CViewerCanvas::OnTimer)
    EVT_SIZE(CViewerCanvas::OnSize)
    EVT_PAINT(CViewerCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(CViewerCanvas::OnEraseBackground)
    EVT_KEY_DOWN( CViewerCanvas::OnKeyDown )
    EVT_KEY_UP( CViewerCanvas::OnKeyUp )
	EVT_RESSWITCH( wxID_ANY, CViewerCanvas::OnResourceSwitch )
	EVT_MTLADJUST( wxID_ANY, CViewerCanvas::OnMaterialAdjust )
	EVT_TOOLCMD( wxID_ANY, CViewerCanvas::OnToolCmdEvent )
	EVT_ENTER_WINDOW( CViewerCanvas::OnMouseEnter )
	EVT_LEAVE_WINDOW( CViewerCanvas::OnMouseLeave )
	EVT_MOTION( CViewerCanvas::OnMouseMove )
	EVT_LEFT_DOWN( CViewerCanvas::OnLMBDown )
	EVT_LEFT_UP( CViewerCanvas::OnLMBUp )
	EVT_RIGHT_DOWN( CViewerCanvas::OnRMBDown )
	EVT_RIGHT_UP( CViewerCanvas::OnRMBUp )
	EVT_MOUSEWHEEL( CViewerCanvas::OnMouseWheel )
END_EVENT_TABLE()


CViewerScene*  g_pScene = NULL;

bool bRmb = false;
bool bLmb = false;
int  mouse_x = 0, mouse_y = 0;

/// @brief Constructor.
/// @param parent - parent window.
/// @param id - window ID.
/// @param pos - window position.
/// @param size - window size.
/// @param style - window style.
/// @param name - window name.
CViewerCanvas::CViewerCanvas (  wxWindow *parent, 
                                wxWindowID id,
                                const wxPoint& pos, 
                                const wxSize& size, 
                                long style, 
                                const wxString& name) : wxGLCanvas(parent, (wxGLCanvas*)NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
                                                        m_timer(this, TIMER_ID)
{
	g_pScene = new CViewerScene();
    m_init = false;
	m_bLoaded = false;
	GetEventHandlersTable()->AddEventHandler(EVENTID_RESSWITCH, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_TOOLCMD, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_MTLADJUST, this);
    m_timer.Start(33);
}


/// @brief Destructor.
CViewerCanvas::~CViewerCanvas()
{
    OG_SAFE_DELETE(g_pScene);
}


/// @brief Render.
void CViewerCanvas::Render()
{
    wxPaintDC dc(this);
    dc.GetWindow()->GetHandle();
    if (!GetContext()) 
        return;
    SetCurrent();
}


/// @brief Timer handler.
/// @param event - event structute.
void CViewerCanvas::OnTimer(wxTimerEvent& event)
{
    g_pScene->Init();
    g_pScene->Update(10);
    g_pScene->RenderScene();
	SwapBuffers();
}


/// @brief Paint handler.
/// @param event - event structute.
void CViewerCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}


/// @brief Resizing handler.
/// @param event - event structute.
void CViewerCanvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event);
    GetClientSize(&m_ResX, &m_ResY);
    if (GetContext())
    {
		g_pScene->SetViewport(m_ResX, m_ResY);
    }
}


/// @brief Key down handler.
/// @param event - event structute.
void CViewerCanvas::OnKeyDown( wxKeyEvent& event )
{
    event.Skip();
}


/// @brief Key up handler.
/// @param event - event structute.
void CViewerCanvas::OnKeyUp( wxKeyEvent& event )
{
    event.Skip();
}


/// @brief Resource switching event handler
/// @param event - event structute.
void CViewerCanvas::OnResourceSwitch ( CommonToolEvent<ResSwitchEventData>& event )
{
	const ResSwitchEventData& evtData = event.GetEventCustomData();
    switch (evtData.m_pItem->type)
	{
	case RESTYPE_MODEL:
        g_pScene->SetupModel(evtData.m_pItem);
		break;
	}
	Refresh();
}


/// @brief Tool command event handler
/// @param event - event structute.
void CViewerCanvas::OnToolCmdEvent ( CommonToolEvent<ToolCmdEventData>& event )
{
	const ToolCmdEventData& evtData = event.GetEventCustomData();
	switch (evtData.m_CmdType)
	{
	case CMD_COORDGRID:
		g_pScene->SetGridMode(evtData.m_bSwitcher);
		break;

	case CMD_AABB:
		g_pScene->SetAABBMode(evtData.m_bSwitcher);
		break;

	case CMD_ITEM_SAVE:
        g_pScene->SaveChanges();
		break;
	}
	Refresh ();
}


/// @brief Material adjust event handler
/// @param event - event structute.
void CViewerCanvas::OnMaterialAdjust ( CommonToolEvent<MtlAdjustEventData>& event )
{
	const MtlAdjustEventData& evtData = event.GetEventCustomData();
    g_pScene->AdjustMaterial(evtData.m_type, evtData.m_val);
}


/// @brief Mouse enter handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseEnter(wxMouseEvent& event)
{
	 SetFocus();
}


/// @brief Mouse leave handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseLeave(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = false;
	bRmb = false;
}


/// @brief Mouse move handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseMove(wxMouseEvent& event)
{
	if (bLmb)
	{
		int prev_x = mouse_x;
		int prev_y = mouse_y;

		mouse_x = event.GetX();
		mouse_y = event.GetY();

		if (event.ControlDown())
		{
			int delta_y = mouse_y - prev_y;
			float fV = (float)delta_y / (float)m_ResY;
			g_pScene->CameraRotateVer(fV * 2.0f);
		}
		else
		{
			int delta_x = mouse_x - prev_x;
			float fH = (float)delta_x / (float)m_ResX;
			g_pScene->CameraRotateHor(fH * 2.0f);
		}
	}

	mouse_x = event.GetX();
	mouse_y = event.GetY();
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CViewerCanvas::OnLMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = false;
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CViewerCanvas::OnLMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = true;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CViewerCanvas::OnRMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = false;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CViewerCanvas::OnRMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = true;
}


/// @brief Mouse wheel handler.
/// @param event - event structute.
void CViewerCanvas::OnMouseWheel(wxMouseEvent& event)
{
	int delta = event.GetWheelRotation();
	g_pScene->CameraZoom((float)delta / 10.0f);
	Refresh ();
}
