/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/EcsOverallTimetableSettingDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #10 $
  *
  * Last modification: $DateTime: 2013/10/07 08:59:52 $
  * Last modified by:  $Author: huangjian $
  *
  */
// OverallEcsTimetableSettingDlg.cpp : implementation file
//

#include  <afxpriv.h>

#include "boost/shared_ptr.hpp"


#include "app/ecs/ecs_manager/src/stdafx.h"

// to launch the help about box
#include "app/ecs/ecs_manager/src/EcsOverallTimetableSettingDlg.h"
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"
#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"

#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/generic_gui/src/TransactiveMessage.h"

#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/TransactiveException.h"

//zn++ CL-20847
#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"
//++zn

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_INITIALISE           (WM_APP+1)


/////////////////////////////////////////////////////////////////////////////
// COverallEcsTimetableSettingDlg dialog

namespace TA_IRS_App
{
	const int LEFT_STATIC_OFFSET = 14;
	const int TOP_STATIC_OFFSET = 12;
	const int STATIC_WIDTH = 852;
	const int topGridControlOffSet = 36;
	const int leftGridControlOffSet = 32;	
    const int TOP_GRIDCTRL2_OFFSET = 220;
	const int gridControlWidth = 816;
	const int TOP_GRIDCTRL3_OFFSET = 404;
    const int TOP_STATUSEDIT_OFFSET = 622;    
    const int STATUSEDIT_HEIGHT = 20;	
	const int STATIC2_WIDTH = 856;	
    const int STATIC2_HEIGHT = 51;    
    const int TOP_STATIC2_OFFSET = 600;
	const int VERT_OFFSET = 708;
	const int TOP_BUTTON_OFFSET = 666;
	const int LEFT_HELP_OFFSET = 14;
	const int HELP_BUTTON_WIDTH = 30;
	const int LEFT_APPLY_ALL_OFFSET = 422;
	const int LEFT_APPLY_OFFSET = 658;
	const int LEFT_CLOSE_OFFSET = 770;
	const int LIST_COLUMN_WIDTH = 80;

    
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	COverallEcsTimetableSettingDlg::COverallEcsTimetableSettingDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd *pParent)
		: TransActiveDialog( genericGUICallback, COverallEcsTimetableSettingDlg::IDD, pParent )
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

        // get a handle to the ScadaProxyFactory
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		//TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
		if (NULL == m_proxyFactory)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SCADA Proxy Factory not created" );
			TA_THROW( TA_Base_Core::TransactiveException("SCADA Proxy Factory not created" ));
		}
		m_proxyFactory->setProxiesToControlMode();

		m_isInitialise = false;
    }

	void COverallEcsTimetableSettingDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(COverallEcsTimetableSettingDlg)
		DDX_Control(pDX, IDC_OVERALL_ECS_TIMETABLE_LIST, m_OverallECSTimetableList);
		DDX_Control(pDX, IDC_STATUS_TEXT, m_statusEditControl);
		DDX_Control(pDX, ID_CLOSE, m_closeButton);
		
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(COverallEcsTimetableSettingDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(COverallEcsTimetableSettingDlg)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_OVERALL_ECS_TIMETABLE_LIST, OnColumnclickOverallEcsTimetableList)	
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	ON_WM_SIZE()
	ON_COMMAND(ID_APP_ABOUT, onAppAbout)
	ON_COMMAND(ID_HELP_INDEX, OnAppHelp)
	ON_BN_CLICKED(ID_HELP, OnHelpButton) //TD16730
	ON_MESSAGE_VOID(WM_INITIALISE, onInitialise)
	ON_MESSAGE(WM_ENABLE_CLOSE, onEnableCloseButton)
	ON_MESSAGE(WM_SESSION_CHANGE, onSessionChanged)	// CL-20847
	ON_WM_TIMER()
    ON_MESSAGE(WM_ENTITY_UPDATE_MESSAGE, OnEntityUpdateMessage)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void COverallEcsTimetableSettingDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		//zn++ CL-20847
		if (name.compare(RPARAM_SESSIONID) == 0)
		{
			PostMessage(WM_SESSION_CHANGE, 0, 0);
		}
		//++zn
	}

	//zn++ CL-20847
	LRESULT COverallEcsTimetableSettingDlg::onSessionChanged(WPARAM, LPARAM)
	{
		CheckPermissions();
		return NULL;
	}
	//++zn

	void COverallEcsTimetableSettingDlg::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Get called of processEntityUpdateEvent, entitykey [%u], updateType [%d].", entityKey, updateType);
        
        PostMessage(WM_ENTITY_UPDATE_MESSAGE, updateType, entityKey);
        
	}

	void COverallEcsTimetableSettingDlg::init()
	{
		if (!CheckPermissions())		// CL-20847
		{
			return;
		}

		InitializeListControl();		
		
//		PostMessage( WM_INITIALISE );
		LoadStations();
		//initialiseDataPointsProxy();
		m_closeButton.EnableWindow(TRUE);
//		SetTimer(TIMER_ID, TIMER_INTERVAL, NULL);
		
		//Launch manager ESC jiangshengguang
		HANDLE	hThread;
		DWORD	threadID;
		hThread = CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )ThreadFunc, this, 0, &threadID );
		CloseHandle( hThread );
		//Launch manager ESC jiangshengguang
	}

	DWORD WINAPI COverallEcsTimetableSettingDlg::ThreadFunc( LPVOID lpThreadParameter )
	{
		COverallEcsTimetableSettingDlg* CMC = (COverallEcsTimetableSettingDlg*) lpThreadParameter;
        CMC->backgroundInitialiseDataPointsProxy();
        CMC->PostMessage( WM_ENABLE_CLOSE, static_cast<WPARAM>(TRUE) );
		return TRUE;
	}

	void COverallEcsTimetableSettingDlg::backgroundInitialiseDataPointsProxy()
	{
		GetDlgItem(ID_APPLY)->EnableWindow(FALSE);
		GetDlgItem(ID_CANCEL)->EnableWindow(FALSE);
		GetDlgItem(ID_CLOSE)->EnableWindow(FALSE);

		initialiseDataPointsProxy();

		GetDlgItem(ID_APPLY)->EnableWindow(TRUE);
		GetDlgItem(ID_CANCEL)->EnableWindow(TRUE);
		GetDlgItem(ID_CLOSE)->EnableWindow(TRUE);
	}

	BOOL COverallEcsTimetableSettingDlg::OnInitDialog() 
    {
        TA_Base_Bus::ResizingProperties rp;
        rp.canMaximise = false;
        rp.maxWidth = 1680;
        rp.maxHeight = 1050;
        CRect rtClient;
        GetClientRect(rtClient);
        rp.minWidth = rtClient.Width();
        rp.minHeight = rtClient.Height();
        setResizingProperties(rp);

		setupOffsets();

		TransActiveDialog::OnInitDialog();
		
		// register to receive updates if SessionID changes
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
		SetIcon(m_hIcon,TRUE);  

		m_closeButton.EnableWindow(FALSE);
		return TRUE;  					  
	}

	void COverallEcsTimetableSettingDlg::setupOffsets()
    {
        FUNCTION_ENTRY( "setupOffsets" );

        CRect thisRect;
        GetWindowRect(&thisRect);

        CRect timetableSelectionRect;
        GetDlgItem( IDC_OVERALL_TIMETABLE_STATIC_LIST )->GetWindowRect( &timetableSelectionRect );

        m_rightRelativeOffset = thisRect.right - timetableSelectionRect.right;
        m_bottomRelativeOffset = thisRect.bottom - timetableSelectionRect.bottom;

        m_areOffsetsSetup = true;

        FUNCTION_EXIT;
    }

	void COverallEcsTimetableSettingDlg::onInitialise()
    {
        FUNCTION_ENTRY( "onInitialise" );

		//SetTimer(TIMER_ID, TIMER_INTERVAL, NULL);
		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/09/11 11:00:00
		
//		LoadStations();
		
		//Launch Manager/////////////////////////////////////////////////////

        FUNCTION_EXIT;
    }
	LRESULT COverallEcsTimetableSettingDlg::onEnableCloseButton(WPARAM wParam, LPARAM lParam)
	{
        FUNCTION_ENTRY( "onEnableCloseButton" );

		BOOL doEnableCloseButton = static_cast< BOOL >( wParam );
		
		//m_applyStationModeButton.EnableWindow( doEnableApplyStationModeControl );
		m_closeButton.EnableWindow(true);
		
        FUNCTION_EXIT;
		return 0;
	}
	void COverallEcsTimetableSettingDlg::OnTimer(UINT nIDEvent) 
    {
		if (false == m_isInitialise)
		{
			/////////////////////////////////////////////////////////////////////
			//Launch Manager modified by: zhangjunsheng 2008/09/11 11:00:00
			
			//LoadStations();
			
			//Launch Manager/////////////////////////////////////////////////////
            
			initialiseDataPointsProxy();
			m_isInitialise = true;

			TransActiveDialog::OnTimer(nIDEvent);
		}
		else
		{
			KillTimer(TIMER_ID);	
		}
		
        
    }

	void COverallEcsTimetableSettingDlg::LoadStations()
	{
		FUNCTION_ENTRY("LoadStations");
		TA_Base_Bus::SessionInfo sessionInfo;
		std::string m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		unsigned long locationKey = 0;

		bool isCentralProfile = false;

		// Get some info about the current session.
		try
		{
			TA_Base_Bus::AuthenticationLibrary authLibrary(true);
			sessionInfo = authLibrary.getSessionInfo(m_sessionId, m_sessionId);
		}
		catch(TA_Base_Core::AuthenticationAgentException&)
		{
			WriteStatusLine("Session information could not be retrieved. Locations will not be loaded");
			LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", "Could not retrieve session information");

			FUNCTION_EXIT;
			return;
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			WriteStatusLine("Session information could not be retrieved. Locations will not be loaded");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve Authentication Agent");

			FUNCTION_EXIT;
			return;
		}

		TA_Base_Core::IProfile* profile = 0;

		for (std::vector<unsigned long>::iterator pitr = sessionInfo.ProfileId.begin();
			 pitr != sessionInfo.ProfileId.end(); 
			 ++pitr)
		{
			try
			{
				profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*pitr);
				
				if (profile->getType() == 0)
				{	
					locationKey = sessionInfo.LocationId;
				}
				else  // PROFILE_CENTRAL
				{
					isCentralProfile = true;
				}

				if(profile != NULL)
				{
					delete profile;
					profile = NULL;
				}		
			}
			catch(TA_Base_Core::DataException&)
			{
				if(profile != NULL)
				{
					delete profile;
					profile = NULL;
				}

				WriteStatusLine("Error retrieving profile information. Locations will not be loaded");
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve profile information");

				FUNCTION_EXIT;
				return;
			}
			catch(TA_Base_Core::DatabaseException&)
			{
				if(profile != NULL)
				{
					delete profile;
					profile = NULL;
				}

				WriteStatusLine("Error retrieving profile information. Locations will not be loaded");
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve profile information");

				FUNCTION_EXIT;
				return;
			}
		}
		
		m_pMappedECSDataCache = std::auto_ptr<MappedECSDataCache>(new MappedECSDataCache());
		m_pMappedECSDataCache->refreshOrderedLocationData();

        OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
        OrderedLocations::iterator iter = orderedLocations.begin();
		int iColCount =0;
		m_OverallECSTimetableList.DeleteAllItems();
        
        m_currentTimetableDataPoints.clear();
        m_selectTimetableDataPoints.clear();

		while (iter != orderedLocations.end())
		{
			
			int index = m_OverallECSTimetableList.InsertItem(iColCount,((*iter).name).c_str(),-1);
            RowHighlight *pLight = new RowHighlight;
            pLight->lLocationKey = (*iter).key;
            pLight->iCurrentSelection = 0;
            pLight->iClickedSelection = -1;				
            m_OverallECSTimetableList.SetItemData(index, (unsigned long)pLight);
            
			iColCount++;				
			iter++;
		}

        m_OverallECSTimetableList.Invalidate();

		if (orderedLocations.empty())
		{
			WriteStatusLine("Ordered Stations List is empty for this profile.");
		}
		else
		{
			WriteStatusLine("Loaded Ordered Stations List.");
		}

		FUNCTION_EXIT;

	}

	void COverallEcsTimetableSettingDlg::initialiseDataPointsProxy()
	{

		FUNCTION_ENTRY("initialiseDataPointsProxy");

		// Create Data Point Proxies

		// launching manager time reducing - no need refresh again, cuz already refreshed in load locations
		//m_pMappedECSDataCache = std::auto_ptr<MappedECSDataCache>(new MappedECSDataCache());
		//m_pMappedECSDataCache->refreshOrderedLocationData();

        OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
        OrderedLocations::iterator iter;
		std::string currToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getCurrentTimeTableToken();
		std::string selToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getSelectTimeTableToken();
		std::string staToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getStationVisualLockToken();
		TA_Base_Core::IEntityDataList curentities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(),currToken);
		TA_Base_Core::IEntityDataList selentities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(),selToken);
		TA_Base_Core::IEntityDataList staentities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(),staToken);
		int entityCount =  0;

		//zn++
		std::vector<unsigned long> locationKeys;
		for (iter = orderedLocations.begin(); iter != orderedLocations.end(); ++iter)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn] locationkey->%lu, name->%s", (*iter).key, (*iter).name.c_str());
			locationKeys.push_back((*iter).key);
		}
		
		createDataPointsProxies(locationKeys, curentities, m_currentTimetableDataPoints);
		createDataPointsProxies(locationKeys, selentities, m_selectTimetableDataPoints);
		createDataPointsProxies(locationKeys, staentities, m_stationVisualLockDataPoints);
		//++zn

		FUNCTION_EXIT;
	}

	void COverallEcsTimetableSettingDlg::WriteStatusLine(const CString &lineOfText)
	{
		CString new_s, old_s, this_s;
		time_t current_time;
		time(&current_time);

		CTime now(current_time);
		CString timeStr = now.Format("%d %b - %I:%M%p");				

		// GET WHATS IN THE BOX NOW AND JUST ADD IT TO THE NEW
		this_s.Format("%s\t%s", timeStr, lineOfText);

		m_statusEditControl.GetWindowText(old_s);
		if (old_s.IsEmpty())
			new_s.Format("%s", this_s);
		else
			new_s.Format("%s\r\n%s", this_s, old_s);	
		
		m_statusEditControl.SetWindowText(new_s.GetBuffer(new_s.GetLength()));							
		m_statusEditControl.UpdateWindow();			
		
	}

	void COverallEcsTimetableSettingDlg::OnColumnclickOverallEcsTimetableList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		m_OverallECSTimetableList.SelectColumn(pNMListView->iSubItem);		
		*pResult = 0;
	}

	void COverallEcsTimetableSettingDlg::InitializeListControl()
	{

		HICON hCurSelction,hSelected;
		CString sColumnList[] = {"Station","Weekday","Saturday","Sunday","Holiday","Holiday1","Holiday2","Special", "Current", ""};
		
		// Initialize the Columns and auto size them
		for(int iCt = 0; sColumnList[iCt] != ""; ++ iCt)
		{
			m_OverallECSTimetableList.InsertColumn(iCt,sColumnList[iCt],LVCFMT_CENTER,LIST_COLUMN_WIDTH);				
		}
        
		// Set the extended styles
		
		//LVCOLUMN lvCol;
        //::ZeroMemory(&lvCol, sizeof(LVCOLUMN));
        //lvCol.mask = LVCF_FMT;
        //lvCol.fmt = LVCFMT_CENTER;
        //m_OverallECSTimetableList.SetColumn( 0, &lvCol );
		
		
		DWORD dwExStyle = m_OverallECSTimetableList.GetExtendedStyle();
		dwExStyle |= LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES|LVS_EX_FULLROWSELECT ;
		m_OverallECSTimetableList.SetExtendedStyle(dwExStyle);
		
		// Create and attach the image list
		
		m_OverallECSTimetableList.m_StationSelectionImageList.Create ( 16, 16, ILC_COLOR16 | ILC_MASK, 2, 1 );	

		hCurSelction = reinterpret_cast<HICON>(::LoadImage ( AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_CURRENTSETTINGS),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ));

		hSelected = reinterpret_cast<HICON>(::LoadImage ( AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_NEWSELECTION),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ));	

		m_OverallECSTimetableList.m_StationSelectionImageList.Add(hCurSelction);
		m_OverallECSTimetableList.m_StationSelectionImageList.Add(hSelected);
        
		m_OverallECSTimetableList.SetImageList(&m_OverallECSTimetableList.m_StationSelectionImageList,LVSIL_SMALL);
		

		RedrawWindow();
	}

	void COverallEcsTimetableSettingDlg::OnClose() 
	{
		//zn++ CL-20847
		CRightsMgrSingleton::getInstance()->DeleteInstance();
		//++zn

		// remove handle to ScadaProxyFactory
		m_proxyFactory->setProxiesToMonitorMode();
		TA_Base_Bus::ScadaProxyFactory::removeInstance();	// Dialog will crash if dont remove proxy instance, the poa already been removed
		m_proxyFactory = NULL;

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
	}

	void COverallEcsTimetableSettingDlg::OnApply() 
	{
		SaveChanges();
        m_OverallECSTimetableList.ClearClickedItems();
	}

	// only executed if a new timetable different to the current one is selected
	void COverallEcsTimetableSettingDlg::SaveChanges()
	{
		int count = 0;
		std::map<unsigned long, std::string> stationToTimetableMap;
		std::map<unsigned long, std::string> stationToNameMap;
		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator itSelect;
	
		std::stringstream ssMessage;
		
		for(int iRow = 0; iRow <m_OverallECSTimetableList.GetItemCount(); ++ iRow)
		{
            CString strStation = m_OverallECSTimetableList.GetItemText(iRow,0);
			std::string stationName(strStation.GetBuffer(strStation.GetLength()));
            
            try
            {
			    RowHighlight *pLight = (RowHighlight *)m_OverallECSTimetableList.GetItemData(iRow);

                if (pLight->iCurrentSelection != pLight->iClickedSelection && pLight->iClickedSelection != -1)
                {	
                    itSelect = m_selectTimetableDataPoints.find(pLight->lLocationKey);
                    if (itSelect != m_selectTimetableDataPoints.end())
                    {
						TA_Base_Bus::DataPointProxySmartPtr* dp = itSelect->second; 

                        std::map<unsigned long, std::string> timetableStateMap = (*dp)->getDerivedStatesAsMap();
                        std::map<unsigned long, std::string>::iterator iter = timetableStateMap.find(pLight->iClickedSelection);
                        if (iter != timetableStateMap.end())
                        {
							
                            std::string newTimetable(iter->second);

							stationToTimetableMap[pLight->lLocationKey] = newTimetable;
							stationToNameMap[pLight->lLocationKey] = stationName;
							ssMessage << newTimetable << " to station " << stationName << "\n";
                        }
                        else
                        {
							std::string statusString = "Cannot find timetable state for station ";
							statusString += strStation;
							WriteStatusLine(statusString.c_str());
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The select datapoint derived states are not well configed for this station");
                        }
                    }
                    else
                    {
						std::string statusString = "No corresponding timetable datapoint created for station ";
						statusString += strStation;
						WriteStatusLine(statusString.c_str());
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The select datapoint is not found for this station");
                    }
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
				CString errorString = getErrorMessage(e.what()).c_str();
				WriteStatusLine(errorString);
            }
            catch (...)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught during setting new timetable.");
				std::string statusString = "Exception caught during retrieving new timetable.";
                WriteStatusLine(statusString.c_str());
            }	
        }

		
		TA_Base_Bus::TransActiveMessage userMsg;
		CString message = ssMessage.str().c_str();
		userMsg << message;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_650011);

		if (selectedButton == IDCANCEL)
		{
			return;
		}

		WriteStatusLine("Applying The New Timetable...");
		WriteStatusLine("Set New Timetable to Station...");		//TD17272 -- "Apply new timetable" instead of particular timetable.

		//zn++ CL-20847
		TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		//++zn
		
		std::map<unsigned long, std::string>::iterator stationToTimetableMapIt;

		for (stationToTimetableMapIt = stationToTimetableMap.begin();
		     stationToTimetableMapIt != stationToTimetableMap.end();
			 ++stationToTimetableMapIt)
		{
			//CString status("Set New Timetable to Station ");		//TD17272 -- "Apply new timetable" instead of particular timetable.

			try
			{
				unsigned long locationKey = stationToTimetableMapIt->first;

				//status += (stationToNameMap[locationKey]).c_str();	//TD17272 -- "Apply new timetable" instead of particular timetable.

				//zn++ CL-20847: before set timetable, should check if operator do have the right
				itSelect = m_selectTimetableDataPoints.find(locationKey);
				if (itSelect != m_selectTimetableDataPoints.end())
				{
					unsigned long resourceId = (*itSelect->second)->getEntityKey();
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
						CString status("Failed to apply timetable to ");
						status += (*itSelect->second)->getEntityName().c_str();
						status += ". Reason: Permission deny.";
						WriteStatusLine(status);
						continue;
					}
				}
				//++zn

				// Check Station locked
				itSelect = m_stationVisualLockDataPoints.find(locationKey);
				if (itSelect != m_stationVisualLockDataPoints.end())
				{
					TA_Base_Bus::DataPointProxySmartPtr* dp = itSelect->second; 

					if (true == (*dp)->getValue().getBoolean())
					{
						WriteStatusLine("Failed to set selected timetable. Reason: Station visual lock.");
						continue;
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Cannot find station visual lock data point proxy for station %s",
						(stationToNameMap[locationKey]).c_str());
				}	

				// Set new timetable
				itSelect = m_selectTimetableDataPoints.find(locationKey);
                if (itSelect != m_selectTimetableDataPoints.end())
				{
					TA_Base_Bus::DataPointProxySmartPtr* dp = itSelect->second; 

					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Begin to set new value [%s] to datapoint [%s].", 
						((stationToTimetableMapIt->second).c_str()), (*dp)->getEntityName().c_str());

					(*dp)->setValue(stationToTimetableMapIt->second, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Succeed to set new value [%s] to datapoint [%s].", 
						((stationToTimetableMapIt->second).c_str()), (*dp)->getEntityName().c_str());

					// send audit event
					CAuditMessageUtility auditLogWriter;

					std::string stationName = stationToNameMap[locationKey];
					auditLogWriter.LogTimetableChangeAuditMessage(stationName, stationToTimetableMapIt->second, ECSAudit::ECSStationTimetableChange);	

					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Succeed to send timetable changed message.");

					
					//status += "...";		//TD17272 -- "Apply new timetable" instead of particular timetable.
					//WriteStatusLine(status);		//TD17272 -- "Apply new timetable" instead of particular timetable.

				}

			}
			catch (TA_Base_Core::ScadaProxyException &e)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
				CString errorString = getErrorMessage(e.what()).c_str();

				//status += " Failed";		//TD17272 -- "Apply new timetable" instead of particular timetable.
                //WriteStatusLine(status);		//TD17272 -- "Apply new timetable" instead of particular timetable.
				WriteStatusLine(errorString);
            }
		}

        //don't set to success before get the control status
        WriteStatusLine("Finished Setting the New Timetable.");
        
	}
    
	void COverallEcsTimetableSettingDlg::OnSize(UINT nType, int cx, int cy)
	{
		// Let the CDialog base class do any re-sizing processing it needs.
        TransActiveDialog::OnSize(nType, cx, cy);

		if(m_areOffsetsSetup && nType != SIZE_MAXHIDE && nType != SIZE_MINIMIZED)
        {
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
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Child windows were not constructed yet.");
                return;
            }
			
		
            CRect resizeRect;
            CWnd* p_resizeWnd = NULL;

            p_resizeWnd = GetDlgItem( IDC_OVERALL_TIMETABLE_STATIC_LIST );
			if(p_resizeWnd)
			{
				p_resizeWnd->GetWindowRect( &resizeRect );
				ScreenToClient( &resizeRect );
			}
        
            // Use OverallTimetable selection GroupBox offsets as relative guide 
            // for all adjustments.
            int rightAdjust = (resizeRect.right + m_rightRelativeOffset) - thisRect.right;
            int bottomAdjust = (resizeRect.bottom + m_bottomRelativeOffset) - thisRect.bottom;
            
            // Station Mode selection GroupBox.
            resizeRect.right = resizeRect.right - rightAdjust;
            resizeRect.bottom = resizeRect.bottom - bottomAdjust;
            
            if(p_resizeWnd)
			{
				p_resizeWnd->MoveWindow( resizeRect );
			}

            // Master Modes Grid Ctrl.
			CRect listRect;
            m_OverallECSTimetableList.GetWindowRect( &listRect );
            ScreenToClient( &listRect );
            listRect.right = listRect.right - rightAdjust;
            listRect.bottom = listRect.bottom - bottomAdjust;
            m_OverallECSTimetableList.MoveWindow( &listRect );

            
            // Station Mode Status Edit Control
            CWnd* p_resizeStatusWnd = NULL;

			m_statusEditControl.GetWindowRect( &resizeRect );
			ScreenToClient( &resizeRect );

			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			m_statusEditControl.MoveWindow(&resizeRect);

			// Station Mode Status Edit Value
            CWnd* p_resizeStatusGroupWnd = NULL;
            p_resizeStatusGroupWnd = GetDlgItem( IDC_OVERALL_TIMETABLE_STATIC );
			if(p_resizeStatusGroupWnd)
			{
				p_resizeStatusGroupWnd->GetWindowRect( &resizeRect );
       			ScreenToClient( &resizeRect );
			}

			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			if(p_resizeStatusGroupWnd)
			{
				p_resizeStatusGroupWnd->MoveWindow(&resizeRect);
			}

            // The buttons along the bottom.
			if(p_resizeWnd)
			{
				p_resizeWnd = GetDlgItem( ID_CLOSE );
				p_resizeWnd->GetWindowRect( &resizeRect );
				ScreenToClient( &resizeRect );
				p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
					( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE );
			
				p_resizeWnd = GetDlgItem( ID_CANCEL );
				p_resizeWnd->GetWindowRect( &resizeRect );
				ScreenToClient( &resizeRect );
				p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
					( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE );
		
				p_resizeWnd = GetDlgItem( ID_APPLY );
				p_resizeWnd->GetWindowRect( &resizeRect );
				ScreenToClient( &resizeRect );
				p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
                ( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE );
        
				// Only adjust vertical for Help button as its left aligned. 
				p_resizeWnd = GetDlgItem( ID_HELP );
				p_resizeWnd->GetWindowRect( &resizeRect );
				ScreenToClient( &resizeRect );
				p_resizeWnd->MoveWindow( resizeRect.left, ( resizeRect.top - bottomAdjust ), 
					resizeRect.Width(), resizeRect.Height(), FALSE );
			
				// Redraw all the buttons at once to prevent artifacts (these were 
				// appearing on Apply button sometimes during resizing)
				thisRect.top = __min( resizeRect.top, ( resizeRect.top - bottomAdjust ) );
				thisRect.bottom = __max( resizeRect.bottom, ( resizeRect.bottom - bottomAdjust ) );
				
			}

			RedrawWindow( thisRect );
			Invalidate();
			m_OverallECSTimetableList.RedrawItems(0, m_OverallECSTimetableList.GetItemCount()-1);
        }
	}

	void COverallEcsTimetableSettingDlg::onAppAbout() 
	{
		FUNCTION_ENTRY( "onAppAbout" );

		// Display the about box
		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

		FUNCTION_EXIT;
	}

	//TD16730
	void COverallEcsTimetableSettingDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}

	void COverallEcsTimetableSettingDlg::OnAppHelp() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

	void COverallEcsTimetableSettingDlg::OnCancel() 
	{
		m_OverallECSTimetableList.ClearClickedItems();
	}

	void COverallEcsTimetableSettingDlg::createDataPointsProxies( std::vector<unsigned long>& currentLocations, 
		TA_Base_Core::IEntityDataList& sourceList, 
		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>& resultMap )
	{
		FUNCTION_ENTRY("initialiseDataPointsProxy");

		TA_Base_Core::IEntityDataList::iterator entityIter;

		for (entityIter = sourceList.begin(); entityIter != sourceList.end(); ++entityIter)
		{	
			unsigned long location = (*entityIter)->getLocation();
			
			if (std::find(currentLocations.begin(), currentLocations.end(), location) != currentLocations.end())
			{
				try
				{
					TA_Base_Core::IEntityData* cuurentityData = *entityIter;
					boost::shared_ptr<TA_Base_Core::DataPointEntityData> currdpEntityData( dynamic_cast<TA_Base_Core::DataPointEntityData*>(cuurentityData) );
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPoint: [%lu]-[%s] will be created", cuurentityData->getKey(), cuurentityData->getName().c_str());
					
					TA_Base_Bus::DataPointProxySmartPtr *datapoint = new TA_Base_Bus::DataPointProxySmartPtr();
					m_proxyFactory->createDataPointProxy(currdpEntityData, *this, *datapoint);
					resultMap[location] = datapoint;                    
					
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Create DataPoint Proxy: ScadaProxyException caught: %s", e.what());
				}
				catch (...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Create DataPoint Proxy: UnknownException caught");
				}

			}
		}

		FUNCTION_EXIT;
	}

	//zn++ CL-20847: do the rights check before launch, just like station timetable selection
	bool COverallEcsTimetableSettingDlg::CheckPermissions()
	{
		WriteStatusLine("Verifying Operator Permissions...");
		
		bool isAllowedToSetTimetable = false;
        
		TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
		
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
			isAllowedToSetTimetable = rightsMgr->isActionPermittedOnResource(
				sessionId, 
				resourceId, 
				TA_Base_Bus::aca_SET_STATION_TIMETABLE_ECS, 
				reason, 
				decisionModule);													
		}
		catch (...)
		{
			isAllowedToSetTimetable = false;
		}   
		
		if (!isAllowedToSetTimetable)
		{
			WriteStatusLine("Failed!");
			
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520006);

			OnClose();

			return false;
		} 
		else
		{
			WriteStatusLine("Success.");
			return true;
		}
	}
	//++zn

    LRESULT COverallEcsTimetableSettingDlg::OnEntityUpdateMessage(WPARAM wParam, LPARAM lParam)
    {

        unsigned long entityKey = lParam; 
        int updateType = wParam;
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "OnEntityUpdateMessage, entitykey [%u], updateType [%d].", entityKey, updateType);
        //refresh current timetable to list
        if ( updateType == TA_Base_Bus::ConfigAvailable ||
             updateType == TA_Base_Bus::ValueStateUpdate ||
             updateType == TA_Base_Bus::AlarmSummaryUpdate ||
             updateType == TA_Base_Bus::AckSummaryUpdate )
        {
            try
            {
                //search current datapoint in map
                std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator itCurrent;
                for (itCurrent = m_currentTimetableDataPoints.begin(); 
					 itCurrent != m_currentTimetableDataPoints.end(); 
					 ++itCurrent)
                {
			        TA_Base_Bus::DataPointProxySmartPtr* dp = itCurrent->second; 
                    //if found a current datapoint
                    if ((*dp)->getEntityKey() == entityKey)
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Found matched current datapoint");
                        //search the list of timetable
                        for (int row = 0; row < m_OverallECSTimetableList.GetItemCount(); ++ row)
                        {
                            //if found the location, update the current timetable
                            RowHighlight *pLight = (RowHighlight *)m_OverallECSTimetableList.GetItemData(row);
                            if (pLight->lLocationKey == itCurrent->first)
                            {
                                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Found matched station");
                                
                                unsigned long value = (*dp)->getValue().getEnum();
                                
                                if (pLight->iCurrentSelection != value)
                                {
                                    pLight->iCurrentSelection = value;
                                    
                                    //refresh the list
                                    m_OverallECSTimetableList.Invalidate();
                                }
                                
                                break;
                            }
                        }
                    
                        break;
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
        
        //display the control status for select datapoints
        if (updateType == TA_Base_Bus::ControlStateUpdate)
        {
            try
            {
                //search select datapoint in map
                std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator itSelect;
                for (itSelect = m_selectTimetableDataPoints.begin(); itSelect != m_selectTimetableDataPoints.end(); ++ itSelect)
                {
					TA_Base_Bus::DataPointProxySmartPtr* dp = itSelect->second; 

                    //if found a select datapoint, set the control status
                    //only response to the control status triggered by current session, but it seems sessionId is not stored when perform control in dp library...
                    if ((*dp)->getEntityKey() == entityKey)
                    {
                        std::string stateString = getControlStatusString((*dp)->getControlState());
                        if (stateString.length() > 0)
                        {
                            CString status("Set new timetable to ");
                            status += (*dp)->getAgentName().c_str();
                            status += " status: ";
                            status += stateString.c_str();
                            WriteStatusLine(status);
                        }
                        
                        break;
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
        return 0L;
    }
}



