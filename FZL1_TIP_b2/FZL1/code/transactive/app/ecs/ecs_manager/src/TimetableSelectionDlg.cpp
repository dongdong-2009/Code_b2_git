/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/TimetableSelectionDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "stdafx.h"


// to launch the help about box
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"
#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "bus/TA_MessageResource/resource.h"

#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ScadaProxyException.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// using TA_IRS_App::TA_ECSManagerCommonDefs::LocationNames;
// using TA_IRS_App::MappedECSDataCache::OrderedLocations;

typedef TA_IRS_App::TA_ECSManagerCommonDefs::LocationNames LocationNames;
typedef TA_IRS_App::MappedECSDataCache::OrderedLocations OrderedLocations;

namespace TA_IRS_App
{

    //devender++ Defect-12345 09/09/2005
    const int CTimetableSelectionDlg::DEFAULT_TIME_TABLE_STATE = 8; //"CURRENT"
    //++devender Defect-12345 09/09/2005

	CTimetableSelectionDlg::CTimetableSelectionDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd *pParent)
		: TransActiveDialog( genericGUICallback, CTimetableSelectionDlg::IDD, pParent )
	{     
		//{{AFX_DATA_INIT(CTimetableSelectionDlg)
	//}}AFX_DATA_INIT

		m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

		TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise	= true;
        properties.maxHeight	= -1;
        properties.maxWidth		= -1;
        properties.minWidth		= 448;
        properties.minHeight	= 328;
        setResizingProperties(properties);

		TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();
	}


	void CTimetableSelectionDlg::DoDataExchange(CDataExchange* pDX)
	{
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CTimetableSelectionDlg)
		DDX_Control(pDX, ID_APPLY, m_ApplyButton);
		DDX_Control(pDX, IDC_NEW_TIMETABLE, m_NewTimetableCombo);
		DDX_Control(pDX, IDC_CURRENT_TIMETABLE, m_CurrentTimetableEdit);
		DDX_Control(pDX, IDC_STATION, m_StationCombo);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CTimetableSelectionDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CTimetableSelectionDlg)	
		ON_BN_CLICKED(ID_APPLY, OnApply)
		ON_BN_CLICKED(ID_CLOSE, OnClose)
		ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		ON_MESSAGE(WM_CHECK_PERMISSIONS, onCheckPermissions)
		ON_WM_SHOWWINDOW()
	    ON_CBN_SELCHANGE(IDC_STATION, OnSelchangeStation)
	    ON_CBN_SELCHANGE(IDC_NEW_TIMETABLE, OnSelchangeNewTimetable)
	    ON_WM_SIZE()
		ON_BN_CLICKED(ID_HELP, OnHelpButton) //TD16730
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	LRESULT CTimetableSelectionDlg::onCheckPermissions(WPARAM wParam, LPARAM lParam)
	{
		CheckPermissions();

		return 1;
	}

    void CTimetableSelectionDlg::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Get called of processEntityUpdateEvent, entitykey [%u], updateType [%d].", entityKey, updateType);

        ThreadGuard guard(m_mapLock);
        
        try
        {
        	if ( entityKey == m_currentTimetableDataPoint->getEntityKey() && 
                ( updateType == TA_Base_Bus::ConfigAvailable ||
                  updateType == TA_Base_Bus::ValueStateUpdate ||
                  updateType == TA_Base_Bus::AlarmSummaryUpdate || //limin++, TD20740
                  updateType == TA_Base_Bus::AckSummaryUpdate ) )  //limin++, TD20740
            {
                std::string currValue = m_currentTimetableDataPoint->getValueAsString();
                m_CurrentTimetableEdit.SetWindowText(currValue.c_str());

                WriteStatusLine("Refreshed current timetable.");
                UpdateWindow();

                return;
            }
        }
        catch (TA_Base_Core::ScadaProxyException &e)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught during processEntityUpdateEvent, entitykey [%u], updateType [%d].", entityKey, updateType);
        }

        try
        {
            if (entityKey == m_selectTimetableDataPoint->getEntityKey())
            { 
                if (updateType == TA_Base_Bus::ConfigAvailable)
                {   
                    m_selectTimetableStateMap.clear();
                
                    //empty combo list
                    while(m_NewTimetableCombo.GetCount() > 0)
                    {
                        m_NewTimetableCombo.DeleteString(0);
                    }
                
                    m_selectTimetableStateMap = m_selectTimetableDataPoint->getDerivedStatesAsMap();
                    int i = 0;
                    for (TimetableStateMap::iterator it = m_selectTimetableStateMap.begin();
						 it != m_selectTimetableStateMap.end(); 
						 ++ i, ++ it)
                    {
                        m_NewTimetableCombo.InsertString(i, it->second.c_str());
                        m_NewTimetableCombo.SetItemData(i, it->first);

                        //set current select to current timetable
                        CString currValue;
                        m_CurrentTimetableEdit.GetWindowText(currValue);
                        if (it->second.compare(currValue.GetBuffer(currValue.GetLength())) == 0)
                        {
                            m_NewTimetableCombo.SetCurSel(i);
                        }
                    }
                
                    WriteStatusLine("Retrieved selectable timetables successfully.");
                    m_NewTimetableCombo.EnableWindow(TRUE);
                
                    UpdateWindow();
                }
                //if receive control state update from datapoint, should display the control status
                //only response to the control status triggered by current session, but it seems sessionId is not stored when perform control in dp library...
                else if (updateType == TA_Base_Bus::ControlStateUpdate/* && 
                    m_selectTimetableDataPoint->getLastStatusChangeSessionId() == TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID)*/)
                {
                    std::string stateString = getControlStatusString(m_selectTimetableDataPoint->getControlState());
                    if (stateString.length() > 0)
                    {
                        CString status("Set new timetable to ");
                        status += m_selectTimetableDataPoint->getAgentName().c_str();
                        status += " status: ";
                        status += stateString.c_str();
                        WriteStatusLine(status);
                    }
                }
            }
        }
        catch (TA_Base_Core::ScadaProxyException &e)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
        }
        catch(...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Exception caught during processEntityUpdateEvent, entitykey [%u], updateType [%d].", entityKey, updateType);
        }

	}

	BOOL CTimetableSelectionDlg::OnInitDialog() 
	{
		TransActiveDialog::OnInitDialog();	

		// register to receive updates if SessionID changes
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
		SetIcon( m_hIcon, TRUE );  

		return TRUE;  
	}

	void CTimetableSelectionDlg::OnApply() 
	{
		if (true == m_hasChanged)
		{
			SaveChanges();
		}
	}

	void CTimetableSelectionDlg::OnClose() 
	{
		CRightsMgrSingleton::getInstance()->DeleteInstance();

		TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();		
	}


	// only executed if a new timetable different to the current one is selected
	void CTimetableSelectionDlg::SaveChanges()
	{
        int count = 0;
		
        BeginWaitCursor();

		//zn++ CL-20847: before set timetable, should check if operator do have the right
		TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		unsigned long resourceId = m_selectTimetableDataPoint->getEntityKey();
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;	
		std::string reason;
		bool result = false;
		
		try
		{
			result = rightsMgr->isActionPermittedOnResource(
				sessionId, 
				resourceId, 
				TA_Base_Bus::aca_DATA_POINT_WRITE, 
				reason, 
				decisionModule);													
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Check rights before set timetable meet exception");
			result = false;
		}

		if (!result)
		{
			WriteStatusLine("Failed to apply timetable. Reason: Permission deny.");
			return;
		}
		//++zn

		if (true == m_stationVisualLockDataPoint->getValue().getBoolean())
		{
			WriteStatusLine("Failed to apply timetable. Reason: Station visual lock.");
			return;
		}
		
		try
		{
            unsigned long currSel = m_NewTimetableCombo.GetItemData(m_NewTimetableCombo.GetCurSel());
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "New selected timetable index is %u.", currSel);

            CString status;

			// get station name
			CString sName;
			m_StationCombo.GetLBText(m_StationCombo.GetCurSel(), sName);
            
            TimetableStateMap::iterator iter = m_selectTimetableStateMap.find(currSel);
            if(iter != m_selectTimetableStateMap.end())
            {
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << iter->second;
				userMsg << sName;
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_650009);

				if (selectedButton == IDCANCEL)
				{
					EndWaitCursor();
					return;
				}

				m_NewTimetableCombo.GetWindowText(status);
				status = "Applying " + status + " Timetable...";
				WriteStatusLine(status);

                m_selectTimetableDataPoint->setValue(iter->second, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

                m_hasChanged = false;
				m_ApplyButton.EnableWindow(FALSE);				

                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Begin to send timetable changed message.");

				// send audit event
				CAuditMessageUtility auditLogWriter;

				// get new timetable
				CString sNewTimetable;
				m_NewTimetableCombo.GetLBText(m_NewTimetableCombo.GetCurSel(), sNewTimetable);

				std::string stationName(sName.GetBuffer(sName.GetLength()));
				std::string newTimetable(sNewTimetable.GetBuffer(sNewTimetable.GetLength()));
				auditLogWriter.LogTimetableChangeAuditMessage(stationName, newTimetable, ECSAudit::ECSStationTimetableChange);	
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid index number set for time table %d",currSel );
                WriteStatusLine("Failed! --Due to invalid timetable item.");
            }
		}
        catch (TA_Base_Core::ScadaProxyException &e)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
			CString errorString = getErrorMessage(e.what()).c_str();
            WriteStatusLine(errorString);
        }
        catch (...)
		{
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Exception caught during setting new timetable.");
            WriteStatusLine("Error: Unknown reason.");
		}
    
		EndWaitCursor();
    }

	
	void CTimetableSelectionDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		if (name.compare(RPARAM_SESSIONID) == 0)
			CheckPermissions();
	}

	void CTimetableSelectionDlg::OnAppAbout() 
	{
		using TA_Base_Bus::HelpLauncher;		
		HelpLauncher::getInstance().displayAboutBox();
	}

	void CTimetableSelectionDlg::LoadStations()
	{	
        //TD14442, load related locations and set default value.
        std::string sessionID = RunParams::getInstance().get( RPARAM_SESSIONID );
        
        IConsole* console = ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
        unsigned long currentUsersLocationKey = console->getLocation();

        m_pMappedECSDataCache = std::auto_ptr<MappedECSDataCache>(new MappedECSDataCache());
		m_pMappedECSDataCache->refreshOrderedLocationData();

        OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
        OrderedLocations::iterator iter = orderedLocations.begin();
		int index = 0;
        m_StationCombo.ResetContent();

        if(currentUsersLocationKey == ECSManagerConfigurationAccess::getInstance()
            .getEntityConfiguration().getCentralControlLocationKey())
		{
			while (iter != orderedLocations.end())
			{
				index = m_StationCombo.AddString(((*iter).name).c_str());             
				m_StationCombo.SetItemData(index, (*iter).key);
				iter++;
			}
		}
		else // Only load the one station
		{
			while (iter != orderedLocations.end())
			{
				if((*iter).key == currentUsersLocationKey)
				{
					index = m_StationCombo.AddString(((*iter).name).c_str());             
					m_StationCombo.SetItemData(index, (*iter).key);
					break; // There should only be one
				}
				iter++;
			}
        }
		
        if (m_StationCombo.GetCount() > 0) 
		{
			// TD14442
			if(currentUsersLocationKey == ECSManagerConfigurationAccess::getInstance()
				.getEntityConfiguration().getCentralControlLocationKey() )
			{
				m_StationCombo.EnableWindow(TRUE);	
			}
			else
			{
				m_StationCombo.EnableWindow(FALSE);
			}

			m_StationCombo.SetCurSel(0);
            OnSelchangeStation();
        }
		
		m_hasChanged = false;
		WriteStatusLine("Loaded Ordered Stations List.");
	}

	void CTimetableSelectionDlg::WriteStatusLine(const CString &lineOfText)
	{
		CString new_s, old_s, this_s;
		time_t current_time;
		time(&current_time);

		CTime now(current_time);
		CString timeStr = now.Format("%d %b - %I:%M%p");				

		// GET WHATS IN THE BOX NOW AND JUST ADD IT TO THE NEW
		this_s.Format("%s\t%s", timeStr, lineOfText);

		CWnd *wnd = GetDlgItem(IDC_STATUS_TEXT);
		if (wnd)
		{
			wnd->GetWindowText(old_s);
			if (old_s.IsEmpty())
				new_s.Format("%s", this_s);
			else
				new_s.Format("%s\r\n%s", this_s, old_s);	
			
			wnd->SetWindowText(new_s.GetBuffer(new_s.GetLength()));							
			wnd->UpdateWindow();			
		}		
	}

	void CTimetableSelectionDlg::CheckPermissions()
	{
		WriteStatusLine("Verifying Operator Permissions...");
		
		m_isAllowedToSetTimetable = false;
        
		// 10070-Start
		// Commented out for fixing bug related to avoid building rights library 
		// Now the rights library is built inside the CRightsMgrSingleton
		// TA_Base_Bus::RightsLibraryFactory rightsFactory;
        // TA_Base_Bus::RightsLibrary *rightsMgr = rightsFactory.buildRightsLibrary();
		// Added to call the Rights Mgr

		TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
		
		// 10070-End

		// get resource id for this dialog app
		std::auto_ptr<TA_Base_Core::IEntityData> p_ecsManager(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str()));

        TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(p_ecsManager->getKey());
        unsigned long resourceId = resource->getKey();
        delete resource;

		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;

		// get current session id for the operator
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);		

		// find out if we have rights to set parameters
		try
		{
			m_isAllowedToSetTimetable = rightsMgr->isActionPermittedOnResource(
										sessionId, 
										resourceId, 
										TA_Base_Bus::aca_SET_STATION_TIMETABLE_ECS, 
										reason, 
										decisionModule);													
		}
		catch (...)
		{
			m_isAllowedToSetTimetable = false;
		}   

		// 10070-Start
		// Commented out for fixing bug related to avoid building rights library 
		// delete rightsMgr;
		// 10070-End
		
		if (m_isAllowedToSetTimetable == false)
		{
			WriteStatusLine("Failed!");
		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520006);
		
			// 10070-Start
			// Delete if the RightsMgrSingleton Library has been created
			CRightsMgrSingleton::getInstance()->DeleteInstance();
			// 10070-End

			//TD18095, jianghp, to fix the performance of showing manager application
			TransActiveDialog::DestroyWindow();
		} 
		else
		{
			WriteStatusLine("Success.");
			m_StationCombo.EnableWindow();
			LoadStations();
		}
	}


	void CTimetableSelectionDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		TransActiveDialog::OnShowWindow(bShow, nStatus);
		
		PostMessage(WM_CHECK_PERMISSIONS);		
	}


	void CTimetableSelectionDlg::OnSelchangeStation() 
	{				
		if (m_StationCombo.GetCurSel() == CB_ERR)
		{
			return;
		}

		m_ApplyButton.EnableWindow(FALSE);
		m_CurrentTimetableEdit.SetWindowText("");
		m_CurrentTimetableEdit.UpdateWindow();
        
        m_NewTimetableCombo.Clear();
        m_NewTimetableCombo.EnableWindow(FALSE);
        
		CString s;
		m_StationCombo.GetLBText(m_StationCombo.GetCurSel(), s);
		s.Insert(0, "Getting Current Timetable For ");
		WriteStatusLine(s); 

		m_hasChanged = false;

		// Create Data Point Proxy
        BeginWaitCursor();	
		
		int locationKey = m_StationCombo.GetItemData(m_StationCombo.GetCurSel());		

        try
        {
			// current timetable data point proxy
            std::string sqlToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getCurrentTimeTableToken();
            TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithNameLikeToken
            ( TA_Base_Core::DataPointEntityData::getStaticType(), m_StationCombo.GetItemData(m_StationCombo.GetCurSel()), sqlToken);

            if ( entities.size() != 1 )
            {
			    WriteStatusLine("Failed!");
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid number of datapoints for %s", sqlToken.c_str() ); //TD14337 yezh++       
            }
            else
            {
				TA_Base_Core::IEntityData* entityData = entities[0];
				
				boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData( dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData) );

				TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dpEntityData, *this, m_currentTimetableDataPoint);
		    }
        
            WriteStatusLine("Retrieving selectable timetables...");

            // select timetable data point proxy
            sqlToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getSelectTimeTableToken();
            entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithNameLikeToken
                ( TA_Base_Core::DataPointEntityData::getStaticType(), m_StationCombo.GetItemData(m_StationCombo.GetCurSel()), sqlToken);
        
            if ( entities.size() != 1 )
            {
                WriteStatusLine("Failed!");
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid number of datapoints for %s", sqlToken.c_str() );  //TD14337 yezh++      
            }
            else
            {
				TA_Base_Core::IEntityData* entityData = entities[0];
				
				boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData( dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData) );
            
                TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dpEntityData, *this, m_selectTimetableDataPoint);
            }

			// station visual lock data point proxy
            sqlToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getStationVisualLockToken();
            entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithNameLikeToken
                ( TA_Base_Core::DataPointEntityData::getStaticType(), m_StationCombo.GetItemData(m_StationCombo.GetCurSel()), sqlToken);
        
            if ( entities.size() != 1 )
            {
                WriteStatusLine("Failed!");
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid number of datapoints for %s", sqlToken.c_str() );  //TD14337 yezh++      
            }
            else
            {
                TA_Base_Core::IEntityData* entityData = entities[0];
				
				boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData( dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData) );
            
                TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dpEntityData, *this, m_stationVisualLockDataPoint);
            }
        }                
        catch (TA_Base_Core::ScadaProxyException &e)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
            WriteStatusLine("Failed!");
            EndWaitCursor();
            return;
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "UnknownException caught");
            WriteStatusLine("Failed!");
            EndWaitCursor();
            return;
        }
        
        EndWaitCursor();
        WriteStatusLine("Success.");   
    }


	void CTimetableSelectionDlg::OnSelchangeNewTimetable() 
	{	
		CString original, newTimetable;
		
		m_CurrentTimetableEdit.GetWindowText(original);
		m_NewTimetableCombo.GetLBText(m_NewTimetableCombo.GetCurSel(), newTimetable);

		if (original.Compare(newTimetable) != 0)
		{
			m_ApplyButton.EnableWindow();
			m_hasChanged = true;
		} 
        else
		{
			m_ApplyButton.EnableWindow(FALSE);
			m_hasChanged = false;
		}
	}


	void CTimetableSelectionDlg::OnSize(UINT nType, int cx, int cy) 
	{
		TransActiveDialog::OnSize(nType, cx, cy);
		
		// TODO: Add your message handler code here

		CRect thisRect;
        GetWindowRect( &thisRect );
        ScreenToClient( &thisRect );
        // Start by resizing the menu line
        CWnd* menuLine = GetDlgItem( IDC_MENU_SEPERATOR );
        if ( NULL != menuLine )
        {
            // Get the rectangle for the menu line
            CRect lineRect;
            menuLine->GetWindowRect( &lineRect );
            ScreenToClient( &lineRect );

            // Don't care about it's height - it's just a line. Set the line width bounds to 
            // the same as the main application window
            lineRect.right = ( lineRect.left + cx + 2 );
            menuLine->MoveWindow( &lineRect );
        }
		CRect resizeApplyRect, resizeHelpRect, resizeCloseRect, resizeStaticRect, resizeStatusRect;
		CWnd* p_resizeApplyWnd;
		CWnd* p_resizeHelpWnd;
		CWnd* p_resizeCloseWnd;
		CWnd* p_resizeStaticWnd;
		CWnd* p_resizeStatusWnd;

		p_resizeApplyWnd = GetDlgItem(ID_APPLY);
		p_resizeHelpWnd = GetDlgItem(ID_HELP);
		p_resizeCloseWnd = GetDlgItem(ID_CLOSE);
		p_resizeStaticWnd = GetDlgItem(IDC_TIMETABLE_STATIC);
		p_resizeStatusWnd = GetDlgItem(IDC_STATUS_TEXT);

		if(p_resizeApplyWnd)
		{
			p_resizeApplyWnd->GetWindowRect(&resizeApplyRect);
			ScreenToClient(&resizeApplyRect);
			
			p_resizeApplyWnd->MoveWindow(226 + (cx-442), 246 + (cy-282), resizeApplyRect.Width(), resizeApplyRect.Height());	
		}

		if(p_resizeHelpWnd)
		{
			p_resizeHelpWnd->GetWindowRect(&resizeHelpRect);
			ScreenToClient(&resizeHelpRect);
			
			p_resizeHelpWnd->MoveWindow(12, 246 + (cy-282), resizeHelpRect.Width(), resizeHelpRect.Height());		
		}

		if(p_resizeCloseWnd)
		{
			p_resizeCloseWnd->GetWindowRect(&resizeCloseRect);
			ScreenToClient(&resizeCloseRect);
			
			p_resizeCloseWnd->MoveWindow(334 + (cx-442), 246 + (cy-282), resizeCloseRect.Width(), resizeCloseRect.Height());		
		}

		if(p_resizeStaticWnd)
		{
			p_resizeStaticWnd->GetWindowRect(&resizeStaticRect);
			ScreenToClient(&resizeStaticRect);
			
			p_resizeStaticWnd->MoveWindow(8, 132, 426 + (cx - 442), 102 + (cy -292));	
		}

		if(p_resizeStatusWnd)
		{
			p_resizeStatusWnd->GetWindowRect(&resizeStatusRect);
			ScreenToClient(&resizeStatusRect);
			
			p_resizeStatusWnd->MoveWindow(18, 154, 406 + (cx - 442), 68 + (cy -292));	
		}
		
		// Redraw all the buttons at once to prevent artifacts 
        RedrawWindow( thisRect );
		
	}


    //notice, this is a global function
    std::string getControlStatusString(TA_Base_Bus::EDataPointControlState state)
    {
        std::string returnString;

        switch ( state )
        {
        case TA_Base_Bus::written:
                returnString = TA_Base_Bus::DPT_CONTROL_WRITTEN;
                break;
        case TA_Base_Bus::pending:
                returnString = TA_Base_Bus::DPT_CONTROL_PENDING;
                break;
        case TA_Base_Bus::conditionalSuccess:
                returnString = "Success";
                break;
        case TA_Base_Bus::success:
                returnString = "Success";
                break;
        case TA_Base_Bus::failed:
                returnString = TA_Base_Bus::DPT_CONTROL_FAILED;
                break;
        case TA_Base_Bus::LCC_failed:
                returnString = TA_Base_Bus::DPT_CONTROL_LCC_FAILED;
                break;
        case TA_Base_Bus::RCC_failed:
                returnString = TA_Base_Bus::DPT_CONTROL_RCC_FAILED;
                break;
        case TA_Base_Bus::comms_failed:
                returnString = TA_Base_Bus::DPT_CONTROL_COMMS_FAILED;
                break;
        case TA_Base_Bus::noControl:
        default:
                returnString = TA_Base_Bus::DPT_CONTROL_NOCONTROL;
                break;
        }

        return returnString;
    }


	//TD16730
	void CTimetableSelectionDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
    
    
} // namespace
