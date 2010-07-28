#include <Opengl2.h>
#include <wx/wx.h>
#include "EditorCanvas.h"
#include <OrangeGrass.h>
#include <IOGGraphicsHelpers.h>
#include <IOGMath.h>


#define TIMER_ID    1000

BEGIN_EVENT_TABLE(CEditorCanvas, wxGLCanvas)
    EVT_TIMER(TIMER_ID, CEditorCanvas::OnTimer)
    EVT_SIZE(CEditorCanvas::OnSize)
    EVT_PAINT(CEditorCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(CEditorCanvas::OnEraseBackground)
    EVT_KEY_DOWN( CEditorCanvas::OnKeyDown )
    EVT_KEY_UP( CEditorCanvas::OnKeyUp )
    EVT_ENTER_WINDOW( CEditorCanvas::OnEnterWindow )
	EVT_TOOLCMD( wxID_ANY, CEditorCanvas::OnToolCmdEvent )
	EVT_LEVELLOAD( wxID_ANY, CEditorCanvas::OnLevelLoadEvent )
	EVT_ENTER_WINDOW( CEditorCanvas::OnMouseEnter )
	EVT_LEAVE_WINDOW( CEditorCanvas::OnMouseLeave )
	EVT_MOTION( CEditorCanvas::OnMouseMove )
	EVT_LEFT_DOWN( CEditorCanvas::OnLMBDown )
	EVT_LEFT_UP( CEditorCanvas::OnLMBUp )
	EVT_RIGHT_DOWN( CEditorCanvas::OnRMBDown )
	EVT_RIGHT_UP( CEditorCanvas::OnRMBUp )
	EVT_MOUSEWHEEL( CEditorCanvas::OnMouseWheel )
END_EVENT_TABLE()


MATRIX	m_mProjection;
MATRIX	m_mView;
IOGTerrain*	m_pCurTerrain = NULL;
bool	m_bIntersectionFound;
Vec3	m_vIntersection;
Vec3	m_vPatchGrid[512];
int		m_NumPatchVerts = 0;

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
CEditorCanvas::CEditorCanvas (  wxWindow *parent, 
                                wxWindowID id,
                                const wxPoint& pos, 
                                const wxSize& size, 
                                long style, 
                                const wxString& name) : wxGLCanvas(parent, (wxGLCanvas*)NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name),
                                                        m_timer(this, TIMER_ID)
{
    m_init = false;
	m_bLoaded = false;
	GetEventHandlersTable()->AddEventHandler(EVENTID_RESSWITCH, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_TOOLCMD, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_LEVELLOAD, this);
    m_timer.Start(100);
	m_fCameraDistance = 600.0f;

	m_bShowAABB = false;
	m_bMouseInWindow = true;
	m_bMouseMoved = false;
	m_bRedrawPatch = false;

	m_bIntersectionFound = false;
}


/// @brief Destructor.
CEditorCanvas::~CEditorCanvas()
{
}


/// @brief Render.
void CEditorCanvas::Render()
{
    wxPaintDC dc(this);
    dc.GetWindow()->GetHandle();

    if (!GetContext()) 
        return;

    SetCurrent();

    // Init OpenGL once, but after SetCurrent
    if (!m_init)
    {
        InitGL();
		GetResourceMgr()->Init("resources.xml");
		GetLevelManager()->Init("levels.xml");
		m_init = true;
    }

	if (!m_bLoaded)
	{
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();
		SwapBuffers();
		LoadNextResource();
	}

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_mProjection.f);
	
	glMatrixMode(GL_MODELVIEW);
	m_mView = GetCamera()->Update();
	glLoadMatrixf(m_mView.f);
	VECTOR4 vDiffuse;
	vDiffuse.x = 1.0f;
	vDiffuse.y = 1.0f;
	vDiffuse.z = 1.0f;
	vDiffuse.w = 1.0f;
	VECTOR4 vLightDirection;
	vLightDirection.x = 0.0f;
	vLightDirection.y = 1.0f;
	vLightDirection.z = 0.0f;
	vLightDirection.w = 0;
	glLightfv(GL_LIGHT0, GL_POSITION, (VERTTYPE*)&vLightDirection);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (VERTTYPE*)&vDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, (VERTTYPE*)&vDiffuse);
	glEnable(GL_NORMALIZE);

	if (m_pCurTerrain)
		m_pCurTerrain->Render(m_mView);

    RenderHelpers();

    glFlush();
    SwapBuffers();
}


/// @brief Timer handler.
/// @param event - event structute.
void CEditorCanvas::OnTimer(wxTimerEvent& event)
{
	if (bRmb || bLmb || m_bMouseMoved)
	{
		//ToolSettings* pTool = GetToolSettings();
		//Vec3 vPick = GetPickRay (mouse_x, mouse_y);
		//Vec3 vPos = GetCamera()->GetPosition();
		//Vec3 vVec = vPick - vPos;
		//vVec.normalize();
		//if (m_pCurTerrain && pTool->GetEditMode() != EDITMODE_NO)
		//{
		//	m_bIntersectionFound = m_pCurTerrain->GetRayIntersection(vPos, vVec, &m_vIntersection);
		//	if (m_bIntersectionFound)
		//	{
		//		if (bRmb || bLmb)
		//		{
		//			switch (pTool->GetEditMode())
		//			{
		//			case EDITMODE_HEIGHT:
		//				EditHeightmap (bLmb ? 1 : -1);
		//				break;

		//			case EDITMODE_COLOR:
		//				EditColor (bLmb);
		//				break;
		//			}
		//		}
		//		int brushSize = 0;
		//		switch (pTool->GetEditMode())
		//		{
		//		case EDITMODE_HEIGHT:
		//			brushSize = pTool->GetHeightmapBrushSize();
		//			break;

		//		case EDITMODE_COLOR:
		//			brushSize = pTool->GetColorBrushSize();
		//			break;
		//		}
		//		m_NumPatchVerts = m_pCurTerrain->GetPatch(m_vIntersection, brushSize, &m_vPatchGrid[0]);
		//		m_bMouseMoved = false;
		//		m_bRedrawPatch = true;
		//		Refresh();
		//	}
		//}
	}
}


/// @brief Enter window handler.
/// @param event - event structute.
void CEditorCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
    SetFocus();
}


/// @brief Paint handler.
/// @param event - event structute.
void CEditorCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}


/// @brief Resizing handler.
/// @param event - event structute.
void CEditorCanvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event);
    GetClientSize(&m_ResX, &m_ResY);
    if (GetContext())
    {
        SetCurrent();
        glViewport(0, 0, m_ResX, m_ResY);
		MatrixPerspectiveFovRH(m_mProjection, 1.0f, float(m_ResX)/float(m_ResY), 4.0f, 4500.0f, true);
    }
}


/// @brief Initialize renderer.
void CEditorCanvas::InitGL()
{
	glewInit();

    SetupCamera ();

	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


/// @brief Load next resource bulk.
/// @return false if finished loading.
bool CEditorCanvas::LoadNextResource()
{
	while (GetResourceMgr()->LoadNext())
	{
		IOGResourceInfo resInfo;
		GetResourceMgr()->GetLoadProgress(resInfo);

		CommonToolEvent<ResLoadEventData> cmd(EVENTID_RESLOAD);
		cmd.SetEventCustomData(ResLoadEventData(wxT(resInfo.m_pResource), wxT(resInfo.m_pResourceGroup), wxT(resInfo.m_pResourceIcon)));
        GetEventHandlersTable()->FireEvent(EVENTID_RESLOAD, &cmd);
	}
	m_bLoaded = true;

	return true;
}


/// @brief Level load event handler
/// @param event - event structute.
void CEditorCanvas::OnLevelLoadEvent ( CommonToolEvent<LevelLoadEventData>& event )
{
	m_pCurTerrain = GetLevelManager()->GetTerrain(0);
	this->Refresh();
}


/// @brief Key down handler.
/// @param event - event structute.
void CEditorCanvas::OnKeyDown( wxKeyEvent& event )
{
    switch (event.GetKeyCode())
    {
    case WXK_UP:
		GetCamera()->Strafe(5.5f, Vec3(0, 0, -1));
		mouse_x = event.GetX();
		mouse_y = event.GetY();
		m_bMouseMoved = true;
		this->Refresh();
        break;

	case WXK_DOWN:
		GetCamera()->Strafe(5.5f, Vec3(0, 0, 1));
		mouse_x = event.GetX();
		mouse_y = event.GetY();
		m_bMouseMoved = true;
		this->Refresh();
        break;

	case WXK_LEFT:
		GetCamera()->Strafe(5.5f, Vec3(-1, 0, 0));
		mouse_x = event.GetX();
		mouse_y = event.GetY();
		m_bMouseMoved = true;
		this->Refresh();
        break;

	case WXK_RIGHT:
		GetCamera()->Strafe(5.5f, Vec3(1, 0, 0));
		mouse_x = event.GetX();
		mouse_y = event.GetY();
		m_bMouseMoved = true;
		this->Refresh();
        break;
    }
    event.Skip();
}


/// @brief Key up handler.
/// @param event - event structute.
void CEditorCanvas::OnKeyUp( wxKeyEvent& event )
{
    event.Skip();
}


/// @brief Tool command event handler
/// @param event - event structute.
void CEditorCanvas::OnToolCmdEvent ( CommonToolEvent<ToolCmdEventData>& event )
{
	const ToolCmdEventData& evtData = event.GetEventCustomData();
	switch (evtData.m_CmdType)
	{
	case CMD_AABB:
		m_bShowAABB = evtData.m_bSwitcher;
		break;
	}
	Refresh ();
}


/// @brief Setup camera.
void CEditorCanvas::SetupCamera()
{
	Vec3 vTarget (0, 0, 0);
	Vec3 vDir (0, 1.0f, 0.4f);
	vDir = vDir.normalize();
	Vec3 vUp = vDir.cross (Vec3(0, 1, 0));

	GetCamera()->Setup (vDir * m_fCameraDistance, vTarget, vUp);
}


/// @brief Render scene helpers.
void CEditorCanvas::RenderHelpers()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_mView.f);
    glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	if (m_bShowAABB)
	{
	}

	if (m_NumPatchVerts > 0 && m_bRedrawPatch)
	{
		DrawPatchGrid (m_NumPatchVerts, &m_vPatchGrid[0]);
		m_bRedrawPatch = false;
	}

    glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}


/// @brief Mouse enter handler.
/// @param event - event structute.
void CEditorCanvas::OnMouseEnter(wxMouseEvent& event)
{
	m_bMouseInWindow = true;
}


/// @brief Mouse leave handler.
/// @param event - event structute.
void CEditorCanvas::OnMouseLeave(wxMouseEvent& event)
{
	m_bMouseInWindow = false;
}


/// @brief Mouse move handler.
/// @param event - event structute.
void CEditorCanvas::OnMouseMove(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	m_bMouseMoved = true;
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CEditorCanvas::OnLMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = false;
}


/// @brief Mouse Left button up handler.
/// @param event - event structute.
void CEditorCanvas::OnLMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bLmb = true;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CEditorCanvas::OnRMBUp(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = false;
}


/// @brief Mouse Right button up handler.
/// @param event - event structute.
void CEditorCanvas::OnRMBDown(wxMouseEvent& event)
{
	mouse_x = event.GetX();
	mouse_y = event.GetY();
	bRmb = true;
}


/// @brief Mouse wheel handler.
/// @param event - event structute.
void CEditorCanvas::OnMouseWheel(wxMouseEvent& event)
{
	int delta = event.GetWheelRotation();
	GetCamera()->Move ((float)delta / 10.0f);
	Refresh ();
}


/// @brief Edit heightmap.
/// @param _Dir - editing direction.
void CEditorCanvas::EditHeightmap(int _Dir)
{
	//ToolSettings* pTool = GetToolSettings();
	//float fAdd = pTool->GetHeightmapAddValue() * _Dir;
	//m_pCurTerrain->AddHeight(m_vIntersection, pTool->GetHeightmapBrushSize(), 
	//	fAdd, pTool->GetHeightmapSmoothing());
}


/// @brief Edit terrain color.
/// @param _bPrimary - primary or secondary color.
void CEditorCanvas::EditColor(bool _bPrimary)
{
	//ToolSettings* pTool = GetToolSettings();
	//const wxColor& curColor = _bPrimary ? pTool->GetPrimaryColor() : pTool->GetSecondaryColor();
	//Vec3 vColor ((float)curColor.Red()/255.0f, (float)curColor.Green()/255.0f, (float)curColor.Blue()/255.0f);
	//m_pCurTerrain->ChangeColor(m_vIntersection, pTool->GetColorBrushSize(), vColor);
}
