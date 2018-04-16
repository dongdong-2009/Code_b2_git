/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/IncidentManagerDoc.cpp $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class inherits from the CDocument.  It will not perform the actual
  * business logic but uses the IncidentHandler to manager all incident related
  * logics to allow the class to be tested easily.
  *
  */
#include "core/utilities/src/FunctionEntry.h"
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/IncidentManagerDoc.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "app/incidents/incident_manager/src/CachedIncident.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IncidentManagerDoc

namespace TA_IRS_App
{
	
	IMPLEMENT_DYNCREATE(IncidentManagerDoc, CDocument)
		
		BEGIN_MESSAGE_MAP(IncidentManagerDoc, CDocument)
		//{{AFX_MSG_MAP(IncidentManagerDoc)
		ON_COMMAND(ID_VIEW_ALL_INCIDENTS, onViewAllIncidents)
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		
		/////////////////////////////////////////////////////////////////////////////
		// IncidentManagerDoc diagnostics
		
#ifdef _DEBUG
		void IncidentManagerDoc::AssertValid() const
	{
		FUNCTION_ENTRY("AssertValid");
		CDocument::AssertValid();
		FUNCTION_EXIT;
	}
	
	void IncidentManagerDoc::Dump(CDumpContext& dc) const
	{
		FUNCTION_ENTRY("Dump");
		CDocument::Dump(dc);
		FUNCTION_EXIT;
	}
#endif //_DEBUG
	
	/////////////////////////////////////////////////////////////////////////////
	// IncidentManagerDoc serialization
	
	void IncidentManagerDoc::Serialize(CArchive& ar)
	{
		FUNCTION_ENTRY("Serialize");
		if (ar.IsStoring())
		{
			// TODO: add storing code here
		}
		else
		{
			// TODO: add loading code here
		}
		FUNCTION_EXIT;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// IncidentManagerDoc commands
	BOOL IncidentManagerDoc::OnNewDocument()
	{
		FUNCTION_ENTRY("OnNewDocument");
		if (!CDocument::OnNewDocument())
			return FALSE;
		
		FUNCTION_EXIT;
		return TRUE;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::onViewAllIncidents()
	{
		FUNCTION_ENTRY("onViewAllIncidents");
		m_showAll = !m_showAll; // toggle the flag
		
		// get the main frame window
		CIncidentManagerListView *pListView = NULL;
		
		POSITION pos = GetFirstViewPosition();
		if (pos != NULL)
			pListView = (CIncidentManagerListView *) GetNextView(pos);
		
		if ( pListView == NULL )
		{
			FUNCTION_EXIT;
			return;
		}
		
		pListView->updateIncidentList();
		
		CMainFrame *pMainFrame = (CMainFrame*) pListView->GetParentFrame();
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(pMainFrame != NULL, "onViewAllIncidents has NULL pointer CMainFrame");
		
		// press/release the button as needed
		CToolBarCtrl &toolBarCtrl = (pMainFrame->getMainFrameToolBar())->GetToolBarCtrl();
		toolBarCtrl.PressButton( ID_VIEW_ALL_INCIDENTS, m_showAll );
		
		// check/uncheck the menu item "View/All Incidents" as needed
		CMenu *pMenu = pMainFrame->GetMenu();
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(pMenu != NULL, "onViewAllIncidents has NULL pointer CMenu");
		pMenu->CheckMenuItem( ID_VIEW_ALL_INCIDENTS, MF_BYCOMMAND | (m_showAll ? MF_CHECKED : MF_UNCHECKED ) );
		
		FUNCTION_EXIT;
	}
	//---------------------------------------------------------------------------
	IncidentManagerDoc::IncidentManagerDoc():m_incidentHandler(NULL)
	{
		FUNCTION_ENTRY("IncidentManagerDoc");
		m_bInitialised = false;
		m_showAll = false;
		FUNCTION_EXIT;
	}
	
	IncidentManagerDoc::~IncidentManagerDoc()
	{
		FUNCTION_ENTRY("Destructor");
		
		// lizettejl - TD13868   
		// move the clean up on the prepareForClose() function 
		// to be actually called before everything else goes out of scope
		//TD15126 - insists proper cleanup in case prepareForClose() 
		//is not called at all
		CachedIncident::removeInstance();
		delete m_incidentHandler;
		m_incidentHandler = NULL;   // bug fixed #156
		
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::initialise(ItaIncidentReportFactory& incidentReportFactory, 
		ItaIncidentRights& incidentRights, 
		ItaLocationAccessFactory& locationAccessFactory, 
		TA_Base_Bus::ItaUserMessage& userMessage)
	{
		FUNCTION_ENTRY("initialise");
		
		m_incidentReportFactory = &incidentReportFactory;
		m_incidentRights = &incidentRights;
		m_locationAccessFactory = &locationAccessFactory;
		m_userMessage = &userMessage;
		
		//set up the rights for the cached incident
		CachedIncident::getInstance().setIncidentRights(incidentRights); //TD12923 
		m_incidentHandler = new IncidentHandler( *m_incidentRights, *m_incidentReportFactory, *this );
		
		UpdateAllViews(NULL);   // update all view, including the list view
		
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_incidentHandler != NULL, "initialise has NULL pointer IncidentHandler");
		m_bInitialised = true;
		m_showAll = false;
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	bool IncidentManagerDoc::isInitialised()
	{
		FUNCTION_ENTRY("isInitialised");
		FUNCTION_EXIT;
		return m_bInitialised;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::prepareForClose()
	{
		FUNCTION_ENTRY("prepareForClose");
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		// send message to the view
		SendMessage( pListView->m_hWnd, WM_CLOSE, 0, 0 );
		
		// lizettejl - TD13868   
		// moved the data clean up here instead from having it on 
		// the constructor
		CachedIncident::removeInstance();
		if (m_incidentHandler != NULL)
		{
			delete m_incidentHandler;
			m_incidentHandler = NULL;   // bug fixed #156
		}
		
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	std::vector<ItaIncidentReportData*> IncidentManagerDoc::getIncidents()
	{
		FUNCTION_ENTRY("getIncidents");
		FUNCTION_EXIT;
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_incidentHandler != NULL, "getIncidents has NULL pointer IncidentHandler");
		return m_incidentHandler->getIncidents(m_showAll);
	}
	//-----------------------------------------------------------------------------
	ItaLocationAccessFactory& IncidentManagerDoc::getLocationAccessFactory()
	{
		FUNCTION_ENTRY("getLocationAccessFactory");
		FUNCTION_EXIT;
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_locationAccessFactory != NULL, "getLocationAccessFactory has NULL pointer m_locationAccessFactory");
		return *m_locationAccessFactory;
	}
	//-----------------------------------------------------------------------------
	ItaIncidentRights& IncidentManagerDoc::getIncidentRights()
	{
		FUNCTION_ENTRY("getIncidentRights");
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_incidentRights != NULL, "getIncidentRights has NULL pointer m_incidentRights");
		return *m_incidentRights;
	}
	//-----------------------------------------------------------------------------
	ItaIncidentReportFactory& IncidentManagerDoc::getIncidentReportFactory()
	{
		FUNCTION_ENTRY("getIncidentReportFactory");
		FUNCTION_EXIT;
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_incidentReportFactory != NULL, "getIncidentReportFactory has NULL pointer m_incidentReportFactory");
		return *m_incidentReportFactory;
	}   
	//-----------------------------------------------------------------------------
	TA_Base_Bus::ItaUserMessage& IncidentManagerDoc::getUserMessage()
	{
		FUNCTION_ENTRY("getUserMessage");
		FUNCTION_EXIT;
		//Bugzilla Bug 178: Mintao
		TA_ASSERT(m_userMessage != NULL, "getUserMessage has NULL pointer m_userMessage");
		return *m_userMessage;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::reloadIncidents()
	{
		FUNCTION_ENTRY("reloadIncidents");
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL) 
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL) 
		{
			FUNCTION_EXIT;
			return;
		}
		
		// send message to the view
		PostMessage(pListView->m_hWnd, 
			CIncidentManagerListView::WM_APP_RELOADINCIDENTS, 
			0, 
			0 );
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::createIncident(const std::string& alarmId)
	{
		FUNCTION_ENTRY("createIncident");
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL) 
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		// send message to the view
		// CL18904 comment: send message does not use the LPARAM
		PostMessage(pListView->m_hWnd, 
			CIncidentManagerListView::WM_APP_CREATEALARM, 
			(WPARAM) &alarmId, 
			0 );
		FUNCTION_EXIT;
	}
    //Mintao ++
    //Bug 652 (TD12313)
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::removeIncident(std::string incidentId)
	{
		FUNCTION_ENTRY("removeIncident");
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		// send message to the view
		PostMessage(pListView->m_hWnd, 
			CIncidentManagerListView::WM_APP_REMOVEINCIDENT, 
			(WPARAM) (incidentId.c_str()), 
			0 );
		FUNCTION_EXIT;
	}
    //Mintao ++
    //Bug 652 (TD12313)
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::updateIncident( ItaIncidentReportData *incident )
	{
		FUNCTION_ENTRY("updateIncident");
		//Bugzilla Bug 178: Mintao
		if (incident == NULL)
		{
			return;
		}
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL) 
		{
			FUNCTION_EXIT;
			return;
		}
		
		// send message to the view
		PostMessage(pListView->m_hWnd, 
			CIncidentManagerListView::WM_APP_UPDATEINCIDENT, 
			(WPARAM) incident, 
			0 );
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentManagerDoc::updateRights()
	{
		FUNCTION_ENTRY("updateRights");
		// get the view
		POSITION pos = GetFirstViewPosition();
		if (pos == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		CIncidentManagerListView *pListView = (CIncidentManagerListView *) GetNextView(pos);
		if (pListView == NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		//TD12918
		//CMainFrame *pMainFrame = (CMainFrame*) pListView->GetParentFrame();
		//pMainFrame->PostMessage( CMainFrame::WM_APP_RESELECTINCIDENT, (WPARAM) tempArray, 0 );
		
		// send message to the view
		PostMessage(pListView->m_hWnd, 
			CIncidentManagerListView::WM_APP_UPDATERIGHTS, 
			0, 
			0 );
		FUNCTION_EXIT;
	}
}// end TA_IRS_App
