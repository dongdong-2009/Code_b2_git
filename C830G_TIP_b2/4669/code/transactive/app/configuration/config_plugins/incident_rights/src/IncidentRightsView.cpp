/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/incident_rights/src/IncidentRightsView.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// IncidentRightsView.cpp: implementation of the IncidentRightsView class.
//
//////////////////////////////////////////////////////////////////////

#include "app/configuration/config_plugins/incident_rights/src/stdafx.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsView.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsComponent.h"

#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"
#include "core/data_access_interface/incident_dai/src/CtaReportAccessFactory.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
 #include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
	const int IncidentRightsView::STATIONRIGHTS_GRID_ID = 30999;
    const int IncidentRightsView::INCIDENTRIGHTS_GRID_ID = 30998;

	IMPLEMENT_DYNCREATE(IncidentRightsView, CFormView)
		
	IncidentRightsView::IncidentRightsView()
		:m_incidentRightsComponent(NULL),
          CFormView(IncidentRightsView::IDD)
	{
		FUNCTION_ENTRY("Constructor");
		m_incidentAccessFactory =  new CtaReportAccessFactory();
		TA_ASSERT(m_incidentAccessFactory != NULL, "The report access right factory is NULL!");
 
		FUNCTION_EXIT;
	}
	
	IncidentRightsView::~IncidentRightsView()
	{
		FUNCTION_ENTRY("Destructor");
		
		if (m_incidentAccessFactory != NULL)
		{
			delete m_incidentAccessFactory;
			m_incidentAccessFactory = NULL;
		}
		FUNCTION_EXIT;
		
	}
	
	 void IncidentRightsView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(IncidentRightsView)	 
        //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(IncidentRightsView, CFormView)
	//{{AFX_MSG_MAP(IncidentRightsView)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_NOTIFY(TCN_SELCHANGE, IDC_INCIDENT_STATION_TAB, OnSelchangeIncidentStationAccessTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	void IncidentRightsView::OnInitialUpdate() 
    {
		FUNCTION_ENTRY("OnInitialUpdate");
	    CFormView::OnInitialUpdate();	 
		FUNCTION_EXIT;
    }

	void IncidentRightsView::refreshButtonAvailability(bool isAvailable)
    {
		FUNCTION_ENTRY("refreshButtonAvailability");
        GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(isAvailable);
        GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(isAvailable);
		FUNCTION_EXIT;
    }

	void IncidentRightsView::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        TA_ASSERT(m_incidentRightsComponent != NULL, "The RightsComponent must be set in this class before this method can be called");

 		CRect position;

        if(m_incidentRightsGrid.m_hWnd == NULL)
        {
            // Use our dummy static control to retrieve the positioning for the grid and then destroy it
            
            GetDlgItem(IDC_ACCESS_LIST_POSITION)->GetWindowRect(&position);
            GetDlgItem(IDC_ACCESS_LIST_POSITION)->DestroyWindow();

            // Now convert to client position rather than on the whole screen
            ScreenToClient(&position);

            // Now create the subsystem grid at the position given.
	        m_incidentRightsGrid.Create(position,this,INCIDENTRIGHTS_GRID_ID,WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER);

        }

        try
        {
			m_incidentAccessMatrix = m_incidentAccessFactory->getAccessMetrix(IncidentRightsGridControl::INCIDENT_REPORT);
 			m_incidentRightsGrid.setAccessMatrix(m_incidentAccessMatrix);

            if(m_stationRightsGrid.m_hWnd == NULL)
            {
                // Now create the subsystem grid at the position given.
	            m_stationRightsGrid.Create(position,this,STATIONRIGHTS_GRID_ID,WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER);

            }			
			
			m_stationAccessMatrix = m_incidentAccessFactory->getAccessMetrix(IncidentRightsGridControl::STATION_REPORT);
 			m_stationRightsGrid.setAccessMatrix(m_stationAccessMatrix);

            m_incidentRightsComponent->getStatusBar()->hideProgressBar();
 
            refreshButtonAvailability(false); // Disable buttons as no changes have been made yet
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_incidentRightsComponent->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            m_incidentRightsComponent->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }

	void IncidentRightsView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        TA_ASSERT(m_incidentRightsComponent != NULL, "The component stored is NULL");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();
		
		try
        {
			TA_Base_App::MessageSender::UpdateDetails updateDetails;
            updateDetails.isNewItem = false;
            updateDetails.isDelete = false;
            updateDetails.configUpdateMessage = TA_Base_App::MessageSender::INCIDENT_RIGHTS;
					

 			CProgressCtrl& progress = m_statusBar->displayProgressBar();
			progress.SetStep(1);
		                
			if (m_incidentRightsGrid.hasChanged())
			{	
				progress.OffsetPos(30);
				m_incidentAccessMatrix = m_incidentRightsGrid.getAccessMatrix();				
				m_incidentAccessFactory->updateAccessMatrix(IncidentRightsGridControl::INCIDENT_REPORT,m_incidentAccessMatrix);
				CString strChange;
				strChange.LoadString(IDS_INCIDENT_RIGHTS_CHANGED);
				updateDetails.nameOfChangedItem = (LPCTSTR)strChange;	
                updateDetails.keyOfChangedItem = IncidentRightsGridControl::INCIDENT_REPORT;
				m_incidentRightsComponent->getMessageSender().sendMessagesForTheChange( updateDetails );
				
			}

			if (m_stationRightsGrid.hasChanged())
			{
				progress.OffsetPos(30);
				m_stationAccessMatrix = m_stationRightsGrid.getAccessMatrix();
				m_incidentAccessFactory->updateAccessMatrix(IncidentRightsGridControl::STATION_REPORT,m_stationAccessMatrix);
				CString strChange;
				strChange.LoadString(IDS_STATION_RIGHTS_CHANGED);
				updateDetails.nameOfChangedItem = (LPCTSTR)strChange;
                updateDetails.keyOfChangedItem = IncidentRightsGridControl::STATION_REPORT;
				m_incidentRightsComponent->getMessageSender().sendMessagesForTheChange( updateDetails );
				
			}
			progress.OffsetPos(20);
		}
		catch (...) 
		{
			LOG_EXCEPTION_CATCH (SourceInfo, "Unknown Exception", "Unknown Exception caught when trying to save incident rights access");
		}
				
		m_stationRightsGrid.resetDataChanged();
		m_incidentRightsGrid.resetDataChanged();
		refreshButtonAvailability(false); 

		m_incidentRightsComponent->getStatusBar()->hideProgressBar();

		FUNCTION_EXIT;
    }


	void IncidentRightsView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");
		
        std::vector<std::string> rightsNotApplied; // This is not actually needed but we must pass it to the method call below
        if (m_incidentRightsComponent->areCurrentChangesPending(rightsNotApplied))
		{
			CString actionName;
            actionName.LoadString(IDS_RESET);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT response = userMsg.showMsgBox(IDS_UW_010003);
            if (response == IDNO)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "User canceled the reset");
                FUNCTION_EXIT;
                return;
			}
		}
        TA_ASSERT(m_incidentRightsComponent != NULL, "Component object stored is NULL"); 
		m_incidentRightsGrid.setAccessMatrix(m_incidentAccessMatrix);
		m_stationRightsGrid.setAccessMatrix(m_stationAccessMatrix);

		m_stationRightsGrid.resetDataChanged();
		m_incidentRightsGrid.resetDataChanged();
		refreshButtonAvailability(false); // Disable buttons as all changes have just been applied
        m_incidentRightsComponent->getStatusBar()->hideProgressBar();
		
		FUNCTION_EXIT;
    }

	
	bool IncidentRightsView::areCurrentChangesPending()
	{
		FUNCTION_ENTRY("areCurrentChangesPending");

		bool bHasChanges = FALSE;
		if (m_incidentRightsGrid.hasChanged() || m_stationRightsGrid.hasChanged() )
		{
			bHasChanges = TRUE;
		}
		
		FUNCTION_EXIT;
		return bHasChanges;		
	}
	
 
	void IncidentRightsView::redraw()
	{	
		FUNCTION_ENTRY("redraw");
		
		static_cast<CTabCtrl*>(GetDlgItem(IDC_INCIDENT_STATION_TAB))->InsertItem(0, "Incident Report");
        static_cast<CTabCtrl*>(GetDlgItem(IDC_INCIDENT_STATION_TAB))->InsertItem(1, "Station Report");
		loadItems();
		
        // hide the station report grid - only do this the first time
        m_stationRightsGrid.ShowWindow(SW_HIDE);
        m_incidentRightsGrid.BringWindowToTop();

		FUNCTION_EXIT;
		
	}
	
	void IncidentRightsView::initialise(IncidentRightsComponent* component, TA_Base_App::IProgressStatusBar* statusBar)
	{
		FUNCTION_ENTRY("initialise");
		
		TA_ASSERT(component != NULL, "The component passed in was NULL");
		m_incidentRightsComponent = component;			
		m_statusBar = statusBar; 
		
		m_incidentRightsGrid.setStatusBar(m_statusBar);
		m_stationRightsGrid.setStatusBar(m_statusBar);
		FUNCTION_EXIT;
	}
 

	void IncidentRightsView::OnSelchangeIncidentStationAccessTab(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnSelchangeIncidentStationAccessTab");
		// TODO: Add your control notification handler code here
		 if (static_cast<CTabCtrl*>(GetDlgItem(IDC_INCIDENT_STATION_TAB))->GetCurSel() == 0)
	    {
            // incident report mode
            m_stationRightsGrid.ShowWindow(SW_HIDE);
            m_incidentRightsGrid.ShowWindow(SW_SHOW);
            m_incidentRightsGrid.BringWindowToTop();

          
        }
        else if (static_cast<CTabCtrl*>(GetDlgItem(IDC_INCIDENT_STATION_TAB))->GetCurSel() == 1)
        {
            // station report mode
            m_incidentRightsGrid.ShowWindow(SW_HIDE);
            m_stationRightsGrid.ShowWindow(SW_SHOW);
            m_stationRightsGrid.BringWindowToTop();          
        }
	    
		*pResult = 0;

		FUNCTION_EXIT;
	}

	 /////////////////////////////////////////////////////////////////////////////
    // IncidentRightsView diagnostics

    #ifdef _DEBUG
    void IncidentRightsView::AssertValid() const
    {
	    CFormView::AssertValid();
    }

    void IncidentRightsView::Dump(CDumpContext& dc) const
    {
	    CFormView::Dump(dc);
    }
    #endif //_DEBUG
}

