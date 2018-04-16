/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/CMainFrame.cpp $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The CMainFrame class inherits TransActiveFrame. It encapsulates the 
  * functionality of a Windows single document interface frame window. 
  */
#include "core/utilities/src/FunctionEntry.h"
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/resource.h" 
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"
#include "app/incidents/incident_manager/src/CachedIncident.h"
#include "bus/generic_gui/src/AppLauncher.h"	//jiangshengguang++
//Mintao ++
//Bug 654(TD12091)
//remove the extra about dialog and add in the helpLauncher
#include "bus/generic_gui/src/HelpLauncher.h"
//Mintao ++
//Bug 654(TD12091)

#define SM_CXVIRTUALSCREEN      78		//jiangshengguang++
#define MINVALUE				2400	//jiangshengguang++
#define NUMSCREENS				3		//jiangshengguang++

namespace TA_IRS_App
{
	
	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame
	//-----------------------------------------------------------------------------
	UINT CMainFrame::WM_APP_RESELECTINCIDENT = WM_APP + 1;
	
	IMPLEMENT_DYNCREATE(CMainFrame, TransActiveFrame)
		
		BEGIN_MESSAGE_MAP(CMainFrame, TransActiveFrame)
		//{{AFX_MSG_MAP(CMainFrame)
		ON_WM_CREATE()
		ON_WM_CLOSE()
		ON_MESSAGE( WM_APP_RESELECTINCIDENT, onIncidentSelectionChange )
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_DELETE, OnUpdateCommandUI)
		ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_EDIT, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_NEW, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_PRINT, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_REOPEN, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_VIEW, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_VIEW_ALL_INCIDENTS, OnUpdateCommandUI)
		ON_UPDATE_COMMAND_UI(ID_INCIDENT_CLOSE, OnUpdateCommandUI)
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		//-----------------------------------------------------------------------------
		static UINT indicators[] =
	{
		ID_SEPARATOR,           // status line indicator
			ID_INDICATOR_CAPS,
			ID_INDICATOR_NUM,
			ID_INDICATOR_SCRL,
	};
	//-----------------------------------------------------------------------------
	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		FUNCTION_ENTRY("OnCreate");
		
		if (TransActiveFrame::OnCreate(lpCreateStruct) == -1)
			return -1;
		
		if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
			WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
			|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
			TRACE0("Failed to load toolbar resource\n");
			return -1;      // fail to load
		}
		
		if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("Failed to create status bar\n");
			return -1;      // fail to create
		}
		
		//Bugzilla Bug 161: Mintao
		//this->ShowWindow(SW_SHOWMAXIMIZED);

		//jiangshengguang++ Windows pos error problem
		int left,top,width,height;
		int screenWidth,currentScreen;
		int numScreens = 1;
		int x_pos;
		DWORD MousePos;

		MousePos = GetMessagePos();
		CPoint pt(MousePos);

		RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_GenericGui::SCREEN_CURRENT, 
																		TA_GenericGui::AREA_SCHEMATIC,
																		pt.x);
		x_pos = pt.x;
		screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		if(screenWidth >= MINVALUE)
			numScreens = NUMSCREENS;
        currentScreen = x_pos / (screenWidth / numScreens);
		left	= currentScreen * (screenWidth / numScreens)+1;
		width	= left + screenWidth / numScreens;
		top		= boundary.top;
		height	= boundary.bottom - boundary.top;

		this->MoveWindow(left,top,width,height,NULL);
		//jiangshengguang++ Windows pos error problem

		RECT rc;           
		m_wndStatusBar.GetItemRect(0, &rc);    
		m_progress.Create(WS_CHILD, rc, &m_wndStatusBar, IDC_PROGRESS);
		
		FUNCTION_EXIT;
		
		return 0;
	}
	//-----------------------------------------------------------------------------
	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		FUNCTION_ENTRY("PreCreateWindow");
		if( !TransActiveFrame::PreCreateWindow(cs) )
			return FALSE;
		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
		
		FUNCTION_EXIT;
		
		return TRUE;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame diagnostics
	
#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		TransActiveFrame::AssertValid();
	}
	//-----------------------------------------------------------------------------
	void CMainFrame::Dump(CDumpContext& dc) const
	{
		TransActiveFrame::Dump(dc);
	}
	
#endif //_DEBUG
	
	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame message handlers
	
	void CMainFrame::OnClose() 
	{
		// TODO: Add your message handler code here and/or call default
		FUNCTION_ENTRY("OnClose");
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(m_IncidentManagerFramework != NULL, "CMainFrame::OnClose has NULL pointer m_IncidentManagerFramework");
		m_IncidentManagerFramework->prepareForClose();
		TransActiveFrame::OnClose();
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
		// TODO: Add your specialized code here and/or call the base class
		return TransActiveFrame::OnNotify(wParam, lParam, pResult);
	}
	//-----------------------------------------------------------------------------
	void CMainFrame::OnUpdateCommandUI(CCmdUI* pCmdUI) 
	{
		FUNCTION_ENTRY("OnUpdateCommandUI");
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(pCmdUI != NULL, "CMainFrame::OnUpdateCommandUI has NULL pointer pCmdUI");
		
		switch( pCmdUI->m_nID )
		{
        case ID_INCIDENT_DELETE:
            pCmdUI->Enable( m_bDeleteEnable );
            break;
        case ID_INCIDENT_EDIT:
            pCmdUI->Enable( m_bEditEnable );
            break;
			
        case ID_INCIDENT_NEW: 
            pCmdUI->Enable( m_bNewEnable );
            break;
			
        case ID_INCIDENT_PRINT:
            pCmdUI->Enable( m_bPrintEnable );
            break;
			
        case ID_INCIDENT_REOPEN:
            pCmdUI->Enable( m_bReopenEnable );
            break;
			
        case ID_INCIDENT_VIEW:
            pCmdUI->Enable( m_bViewEnable );
            break;
			
        case ID_INCIDENT_CLOSE:
			{
				// TD15865 Yanrong++
				//pCmdUI->Enable( m_bCloseEnable );
				bool closeEnable = (m_bCloseEnable && !m_beEdited);
				pCmdUI->Enable( closeEnable );
				// ++Yanrong
			}
            break;
		}
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	CMainFrame::CMainFrame():
	m_bNewEnable(false),
		m_bEditEnable(false),
		m_bViewEnable(false),
		m_bDeleteEnable(false),
		m_bCloseEnable(false),
		m_bReopenEnable(false),
		m_bPrintEnable(false),
		// TD15865 Yanrong++
		m_beEdited(false)
		// ++Yanrong
	{
		FUNCTION_ENTRY("CMainFrame");
		//Mintao++ (10th, Nov, 2005)
		//TD12091
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise = true;
		properties.maxHeight = -1; 
		properties.maxWidth = -1;
		//properties.minWidth = 400;
		properties.minWidth = 619; // so can contain all toolbar buttons
		properties.minHeight = 400;
		setResizingProperties(properties);	
		//Mintao++ (10th, Nov, 2005)
		//TD12091
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	CMainFrame::~CMainFrame()
	{
		FUNCTION_ENTRY("~CMainFrame");
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CMainFrame::setCallbackObject(TA_Base_Bus::IGUIAccess& gui)
	{
		FUNCTION_ENTRY("setCallbackObject");
		//Mintao++ (10th, Nov, 2005)
		//TD12091
		TransActiveFrame::setCallbackObject(gui);
		//Mintao++ (10th, Nov, 2005)
		//TD12091
		
		m_IncidentManagerFramework = dynamic_cast<IncidentManagerFramework*>(&gui);
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(m_IncidentManagerFramework != NULL, "setCallbackObject has NULL pointer m_IncidentManagerFramework");
/*		
		m_incidentRights = &m_IncidentManagerFramework->getIncidentRightsChecker();
		

				CIncidentManagerListView* pView = (CIncidentManagerListView*) GetActiveView();
				//Bugzilla Bug 176: Mintao
				TA_ASSERT(pView != NULL, "setCallbackObject has NULL pointer pView");
				m_IncidentManagerDoc = dynamic_cast<IncidentManagerDoc*>(pView->GetDocument());
				//Bugzilla Bug 176: Mintao
				TA_ASSERT(m_IncidentManagerDoc != NULL, "setCallbackObject has NULL pointer m_IncidentManagerDoc");
				m_IncidentManagerDoc->initialise(   m_IncidentManagerFramework->getIncidentReportFactory(), 
					*m_incidentRights, 
					m_IncidentManagerFramework->getLocationAccessFactory(), 
					m_IncidentManagerFramework->getUserMessage() );
				//if the Incident Manager is launched with "alarm-id" with valid id
				//we need to pre-populated the incident report dialog
				std::string alarmId = m_IncidentManagerFramework->getAlarmIdFromCommandLine();
				if (!alarmId.empty())
				{
					pView->onIncidentNewAlarmAssociation(alarmId);
				}*/
		
		
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	long CMainFrame::onIncidentSelectionChange(WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("onIncidentSelectionChange");
		
		std::vector<ItaIncidentReportData*> *incidents = (std::vector<ItaIncidentReportData*> *) wParam;
		
		CToolBarCtrl &toolBarCtrl = m_wndToolBar.GetToolBarCtrl();
		
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(m_incidentRights != NULL, "onIncidentSelectionChange has NULL pointer m_incidentRights");
		//Mintao ++
		//Bug 653(TD12264) 
		bool allowedNew = m_incidentRights->isControlPermitted((ItaIncidentReportData*)NULL);
		//Mintao ++
		//Bug 653(TD12264) 
		
		//Mintao ++
		//Bug 748(TD12916) 
		m_bNewEnable = allowedNew;
		//Mintao ++
		//Bug 748(TD12916) 
		
		if ( (*incidents).size() == 0 )
		{
			// disable Delete, CLose, Reopen, Edit and View buttons
			m_bDeleteEnable = false;
			m_bCloseEnable = false;
			m_bReopenEnable = false;
			m_bEditEnable = false;
			m_bViewEnable = false;
			//disable print button
			m_bPrintEnable = false;
		}	
		else if ( (*incidents).size() == 1 )    // one incident selected
		{
			//TD12923 changed CachedIncident instance
			bool allowedControl = CachedIncident::getInstance().getCachedIncidentControlRight((*incidents)[0]->getReportId());
			//always enable printing whenever one incident is selected
			
			if ( allowedControl == false )
			{
				// disable Delete, Close, Reopen and Edit buttons
				m_bDeleteEnable = false;
				m_bCloseEnable = false;
				m_bReopenEnable = false;
				m_bEditEnable = false;
				
				//TD12923 
				bool allowedView = CachedIncident::getInstance().getCachedIncidentViewRight((*incidents)[0]->getReportId());
				m_bViewEnable = allowedView; 
				//TD18644
				m_bPrintEnable = allowedView;
			} 
			else    // allowedControl == true
			{
				m_bViewEnable = true; 
				m_bPrintEnable = true;
				bool isClosed = (*incidents)[0]->isClosed();
				
				m_bDeleteEnable = isClosed;
				m_bReopenEnable = isClosed;
				m_bCloseEnable = !isClosed;
				//Mintao ++
				//Bug 649 
				m_bEditEnable = !isClosed;
				//Mintao ++
				//Bug 649 
			}
		}
		else // multiple selections
		{	   
			// disable Edit, View, Delete, Close, Reopen buttons
			m_bEditEnable = false;
			m_bViewEnable = false;
			m_bDeleteEnable = false;
			m_bCloseEnable = false;
			m_bReopenEnable = false;
			//disable print button
			m_bPrintEnable = false;
			
			// find an incident that can be closed and can be deleted/reopened
			bool foundClosed = false;
			bool bForClose = false;  //TD15132
			int size = (*incidents).size();
			
			for ( int i = 0; i < size; i++ )
			{
				//TD12923 
				bool righttoctrl =  CachedIncident::getInstance().getCachedIncidentControlRight((*incidents)[i]->getReportId());
				
				//TD15132
				//if (righttoctrl == true && ((*incidents)[i]->isClosed() == true) )
				if (righttoctrl == true) 
				{
					if ((*incidents)[i]->isClosed() == true)  
						foundClosed = true;
					else
						bForClose = true;
					
					
					// if incidents that are for close and re-openning are already found
					// there's no use looking into the rest of the cached incidents
					// so therefore must exit the loop 
					if (foundClosed && bForClose)
						break; 
				}
			}
			if ( foundClosed ) // at least an incident was closed
			{
				m_bDeleteEnable = true;
				m_bReopenEnable = true;
				//TD15132
				if (bForClose)
					m_bCloseEnable = true;
				
			} 
			else // none of them are closed
			{
				m_bCloseEnable = true;
			}
		}
		
		// TD15865 Yanrong++
		// default there is no report being edited.
		m_beEdited = false;
		std::vector<ItaIncidentReportData*>::iterator it = incidents->begin();
		for( ; it != incidents->end(); ++it )
		{
			// if there is one report can not be closed, means at least one report being edited.
			if( !(*it)->canClose())
			{
				m_beEdited = true;
				break;
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_beEdited = %d, m_bCloseEnable = %d, incidents->size() = %d",
			(int)m_beEdited, (int)m_bCloseEnable, incidents->size() );
		// ++Yanrong

		delete incidents; // we are responsible for deleting the vector
		incidents = NULL;
		FUNCTION_EXIT;
		return 0;
	}

	//Bugzilla Bug 162: Mintao
	void CMainFrame::OnAppAbout() 
	{
	    FUNCTION_ENTRY("OnAppAbout");  
	    //Mintao ++
	    //Bug 654(TD12091)
	   	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	    //Mintao ++
	    //Bug 654(TD12091)
	    FUNCTION_EXIT;
	}

	void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
	{
	    FUNCTION_ENTRY("OnUpdateFrameTitle");
	    
		// TD13540 - modify function's exit point
		// if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		// return; // leave it alone! 
		
		// SetWindowText(_T("IncidentManager"));
		if ((GetStyle() & FWS_ADDTOTITLE) != 0)
		{
			SetWindowText(_T("IncidentManager"));
		}
		// otherwise, leave it alone!
	    FUNCTION_EXIT;
	}

	void CMainFrame::init()
	{
		CWaitCursor cur;
		m_incidentRights = &m_IncidentManagerFramework->getIncidentRightsChecker();

		CIncidentManagerListView* pView = (CIncidentManagerListView*) GetActiveView();
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(pView != NULL, "setCallbackObject has NULL pointer pView");
		m_IncidentManagerDoc = dynamic_cast<IncidentManagerDoc*>(pView->GetDocument());
		//Bugzilla Bug 176: Mintao
		TA_ASSERT(m_IncidentManagerDoc != NULL, "setCallbackObject has NULL pointer m_IncidentManagerDoc");
		m_IncidentManagerDoc->initialise(   m_IncidentManagerFramework->getIncidentReportFactory(), 
			*m_incidentRights, 
			m_IncidentManagerFramework->getLocationAccessFactory(), 
			m_IncidentManagerFramework->getUserMessage() );
		//if the Incident Manager is launched with "alarm-id" with valid id
		//we need to pre-populated the incident report dialog
		std::string alarmId = m_IncidentManagerFramework->getAlarmIdFromCommandLine();
		std::string alarmValue = m_IncidentManagerFramework->getAlarmValueFromCommandLine();
		if (!alarmId.empty())
		{
			pView->onIncidentNewAlarmAssociation(alarmId, alarmValue);
		}
		cur.Restore();
	}

}// end TA_IRS_App
