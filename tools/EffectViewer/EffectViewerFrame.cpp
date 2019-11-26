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
#include <wx/wx.h>
#include <wx/gdicmn.h>
#include "EffectViewerFrame.h"
#include <ToolFramework.h>
#include "sample.xpm"
#include "OrangeGrass.h"


#define ID_DEF_ABOUT		10000
#define ID_DEF_COORDGRID	10001
#define ID_DEF_AABB			10002

const int ID_TOOLBAR = 500;

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;


BEGIN_EVENT_TABLE(CEffectViewerFrame, wxFrame)
    EVT_MENU (wxID_EXIT,                CEffectViewerFrame::OnExit)
    EVT_MENU (ID_DEF_ABOUT,             CEffectViewerFrame::OnAboutDlg)
    EVT_MENU (ID_DEF_COORDGRID,         CEffectViewerFrame::OnCoordGrid)
    EVT_MENU (ID_DEF_AABB,              CEffectViewerFrame::OnBounds)
    EVT_RESLOAD( wxID_ANY,              CEffectViewerFrame::OnLoadResource )
    EVT_EFFECTLOAD(wxID_ANY,            CEffectViewerFrame::OnLoadEffect )
END_EVENT_TABLE()


/// @brief Constructor.
/// @param parent - parent window.
/// @param title - window title.
/// @param pos - window position.
/// @param size - window size.
/// @param style - window style.
CEffectViewerFrame::CEffectViewerFrame( wxWindow *parent, 
						   wxWindowID id,
						   const wxString & title,
						   const wxPoint & pos,
						   const wxSize & size,
						   long style)
{
	Create(parent, id, title, pos, size, style);
}


/// Create frame
bool CEffectViewerFrame::Create(wxWindow * parent, 
						  wxWindowID id,
						  const wxString & title,
						  const wxPoint & pos,
						  const wxSize & size,
						  long style)
{
    int w, h;
    wxDisplaySize(&w, &h);
    wxSize appSize = wxSize(w*0.7, h*0.7);

	wxFrame::Create(parent, id, title, pos, appSize, style);

	SetIcon(wxIcon(sample_xpm));

	wxMenuBar* pMenuBar = new wxMenuBar ();
	wxMenu* pWindowMenu = new wxMenu ();
	wxMenu* pHelpMenu = new wxMenu ();
	pWindowMenu->Append(wxID_EXIT, _T("&Close"));
	wxMenuItem* pCoordItem = pWindowMenu->Append(ID_DEF_COORDGRID, _T("&Show coord. grid"), _T("Show coord. grid"), wxITEM_CHECK);
	pCoordItem->Check(false);
	pWindowMenu->Append(ID_DEF_AABB, _T("Show &Bounds"), _T("Show bounds"), wxITEM_CHECK);
	pHelpMenu->Append(ID_DEF_ABOUT, _T("&About"));
	pMenuBar->Append(pWindowMenu, _T("&EffectViewer"));
	pMenuBar->Append(pHelpMenu, _T("&Help"));
	SetMenuBar(pMenuBar);

	SetToolBar(NULL);

	style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL | wxTB_BOTTOM | wxTB_RIGHT | wxTB_HORZ_LAYOUT);
	style |= wxTB_TOP;
	style |= wxTB_NO_TOOLTIPS;
	m_pToolBar = CreateToolBar(style, ID_TOOLBAR);
	PopulateToolbar(m_pToolBar);

	m_pCanvas = new CEffectViewerCanvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	wxSize treeSize = wxSize(appSize.x * 0.2, appSize.y);
	m_pTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, treeSize);
	m_pTree->AddRoot(_T("Resources"));
	m_pTree->Expand(m_pTree->GetRootItem());
	m_pTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CEffectViewerFrame::OnResourceSwitch ), NULL, this );

    GetEventHandlersTable()->AddEventHandler(EVENTID_RESLOAD, this);
	GetEventHandlersTable()->AddEventHandler(EVENTID_EFFECTLOAD, this);

	m_pSettingsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(appSize.x * 0.3, appSize.y));

	m_Manager.SetManagedWindow(this);
	m_Manager.AddPane(m_pCanvas, wxAuiPaneInfo().CenterPane());
	m_Manager.AddPane(m_pTree, wxAuiPaneInfo().Left().Layer(1).CloseButton(false).Caption(wxT("Tools")));
	m_Manager.AddPane(m_pSettingsPanel, wxAuiPaneInfo().Right().Layer(1).CloseButton(false).Caption(wxT("Settings")));
	m_Manager.Update();

	return true;
}


/// Destructor
CEffectViewerFrame::~CEffectViewerFrame()
{
	m_Manager.UnInit();
}


/// @brief App exit handler.
/// @param event - event structute.
void CEffectViewerFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
	Close(true);
}


/// @brief About dialog handler.
/// @param event - event structute.
void CEffectViewerFrame::OnAboutDlg( wxCommandEvent& WXUNUSED(event) )
{
}


/// @brief Coord. grid switch handler.
/// @param event - event structute.
void CEffectViewerFrame::OnCoordGrid(wxCommandEvent& event)
{
	CommonToolEvent<ToolCmdEventData> cmd(EVENTID_TOOLCMD);
	ToolCmdEventData cmdData (CMD_COORDGRID, event.IsChecked ());
	cmd.SetEventCustomData(cmdData);
	GetEventHandlersTable()->FireEvent(EVENTID_TOOLCMD, &cmd);
}


/// @brief Bounds switch handler.
/// @param event - event structute.
void CEffectViewerFrame::OnBounds(wxCommandEvent& event)
{
	CommonToolEvent<ToolCmdEventData> cmd(EVENTID_TOOLCMD);
	ToolCmdEventData cmdData (CMD_AABB, event.IsChecked ());
	cmd.SetEventCustomData(cmdData);
	GetEventHandlersTable()->FireEvent(EVENTID_TOOLCMD, &cmd);
}


/// @brief Populate the toolbar.
/// @param toolBar - toolbar.
void CEffectViewerFrame::PopulateToolbar(wxToolBarBase* toolBar)
{
	enum
	{
		Tool_open,
		Tool_help,
		Tool_Max
	};

	wxBitmap toolBarBitmaps[Tool_Max];

	toolBarBitmaps[Tool_open] = wxBitmap(wxT("Resources\\open.bmp"), wxBITMAP_TYPE_BMP);
	toolBarBitmaps[Tool_help] = wxBitmap(wxT("Resources\\help.bmp"), wxBITMAP_TYPE_BMP);

	int w = toolBarBitmaps[Tool_open].GetWidth();
	int h = toolBarBitmaps[Tool_open].GetHeight();

	toolBar->SetToolBitmapSize(wxSize(w, h));

	toolBar->AddTool(wxID_OPEN, _T("Open"),
		toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
		_T("Open file"), _T("This is help for open file tool"));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_HELP, _T("Help"), toolBarBitmaps[Tool_help], _T("Help button"), wxITEM_CHECK);
	toolBar->Realize();
}


/// @brief Adding resource group
void CEffectViewerFrame::AddResourceGroup ( ResourceType _type, const wxString& _name )
{
	ResourceGroup groupItem(_type, _name);
	groupItem.item = m_pTree->AppendItem(m_pTree->GetRootItem(), groupItem.name, -1, -1, 0);
	m_ResourceGroups.push_back(groupItem);
}


/// @brief Resource loading event handler
void CEffectViewerFrame::OnLoadResource ( CommonToolEvent<ResLoadEventData>& event )
{
	const ResLoadEventData& evtData = event.GetEventCustomData();
	wxString resourceText = evtData.m_Resource;

	m_pTree->AppendItem(m_pTree->GetRootItem(), resourceText, -1, -1, new ResourceItem(RESTYPE_MODEL, resourceText, m_pTree->GetRootItem()) );
	m_pTree->Expand(m_pTree->GetRootItem());
}


/// @brief Resource switching event handler
void CEffectViewerFrame::OnResourceSwitch ( wxTreeEvent& event )
{
	if ( event.GetItem() == m_pTree->GetRootItem() )
		return;

	ResourceItem* pData = (ResourceItem *)m_pTree->GetItemData(event.GetItem());
	if(pData)
	{
		CommonToolEvent<ResSwitchEventData> cmd(EVENTID_RESSWITCH);
		cmd.SetEventCustomData(ResSwitchEventData(pData));
		GetEventHandlersTable()->FireEvent(EVENTID_RESSWITCH, &cmd);
	}
}


/// @brief Effect loading event handler
void CEffectViewerFrame::OnLoadEffect ( CommonToolEvent<EffectLoadEventData>& event )
{
	const EffectLoadEventData& evtData = event.GetEventCustomData();
    IOGEffect* pEffect = (IOGEffect*)evtData.m_pData;
    if (pEffect)
    {
        m_pSettingsPanel->DestroyChildren();
	    wxSize panelSize = m_pSettingsPanel->GetSize();

	    wxTreeCtrl* pTree = new wxTreeCtrl(m_pSettingsPanel, wxID_ANY, wxDefaultPosition, wxSize(panelSize.x, panelSize.y - 100));
	    pTree->AddRoot(_T("Emitters"));

        TEmittersList& emitters = pEffect->GetEmitters();
        TEmittersList::iterator iter = emitters.begin();
        for (; iter != emitters.end(); ++iter)
        {
        	wxTreeItemId itemid = pTree->AppendItem(pTree->GetRootItem(), (*iter)->GetAlias(), -1, -1, 0);
			TStringParamList& strparams = (*iter)->GetStringParams();
			TStringParamList::iterator strit = strparams.begin();
			for (; strit != strparams.end(); ++strit)
			{
				EmitterParamItem* pItem = new EmitterParamItem(EMPRMTYPE_STRING, strit->second);
				pTree->AppendItem(itemid, strit->first, -1, -1, pItem);
			}

			TIntParamList& intparams = (*iter)->GetIntParams();
			TIntParamList::iterator intit = intparams.begin();
			for (; intit != intparams.end(); ++intit)
			{
				EmitterParamItem* pItem = new EmitterParamItem(EMPRMTYPE_INT, intit->second);
				pTree->AppendItem(itemid, intit->first, -1, -1, pItem);
			}

			TFloatParamList& fltparams = (*iter)->GetFloatParams();
			TFloatParamList::iterator fltit = fltparams.begin();
			for (; fltit != fltparams.end(); ++fltit)
			{
				EmitterParamItem* pItem = new EmitterParamItem(EMPRMTYPE_FLOAT, fltit->second);
				pTree->AppendItem(itemid, fltit->first, -1, -1, pItem);
			}

			TColorParamList& clrparams = (*iter)->GetColorParams();
			TColorParamList::iterator clrit = clrparams.begin();
			for (; clrit != clrparams.end(); ++clrit)
			{
				EmitterParamItem* pItem = new EmitterParamItem(EMPRMTYPE_COLOR, clrit->second);
				pTree->AppendItem(itemid, clrit->first, -1, -1, pItem);
			}
		}

        pTree->Expand(pTree->GetRootItem());
    	pTree->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CEffectViewerFrame::OnParamSwitch), NULL, this );
    }
}


/// @brief Param switching event handler
void CEffectViewerFrame::OnParamSwitch ( wxTreeEvent& event )
{
	if ( event.GetItem() == m_pTree->GetRootItem() )
		return;

	EmitterParamItem* pData = (EmitterParamItem*)m_pTree->GetItemData(event.GetItem());
	if(pData)
	{
        LinearParamDialog* linearDlg = new LinearParamDialog(this);
        if ( linearDlg->ShowModal() == wxID_OK )
        {
        }
        linearDlg->Destroy();
    }
}
