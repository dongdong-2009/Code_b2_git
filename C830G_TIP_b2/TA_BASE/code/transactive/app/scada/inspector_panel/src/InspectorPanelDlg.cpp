/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class provides the main dialog used to display the data point information.  
  * It has a tab control with 4 associated dialogs (1 per tab).   
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include <sstream>
#include "cots/ACE/6_0_4/ACE_wrappers/ace/OS.h"

#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/resource.h"
#include "app/scada/Inspector_Panel/src/InspectorPanelDlg.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActiveDialog;
using namespace TA_Base_App;

namespace TA_Base_App
{
	static const std::string  INFORMATION_PAGE         = "INFORMATION";
	static const std::string  CONTROL_PAGE             = "CONTROL";
    static const std::string  LIMITS_PAGE			   = "LIMITS";
	static const std::string  NOTES_PAGE               = "NOTES";
	static const std::string  TAG_PAGE                 = "TAG";
	static const std::string  INHIBIT_PAGE			   = "INHIBIT";
	static const std::string  OVERRIDE_PAGE			   = "OVERRIDE";
    static const std::string  METER_PAGE			   = "METER";
	static const std::string  DIRECT_CONTROL_PAGE      = "DIRECTCONTROL";

	/////////////////////////////////////////////////////////////////////////////
	// CInspectorPanelDlg dialog

	//
	// Constructor
	//
	CInspectorPanelDlg::CInspectorPanelDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
	: TransActiveDialog(genericGUICallback, CInspectorPanelDlg::IDD, pParent), 
      m_resourceId(0), 
	  m_equipment (NULL), 
      m_rightsMgr (NULL), 
      m_heightDifference (0), 
	  m_isInitialTab (false),
      m_isInDirectControl(false)
	{
		FUNCTION_ENTRY ("CInspectorPanelDlg()");

		//{{AFX_DATA_INIT(CInspectorPanelDlg)
		//}}AFX_DATA_INIT
		// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

		m_previousTab = CMainTabControl::NoTab;
		m_requestedTab = CMainTabControl::ControlTab;
		m_requestedPage = CONTROL_PAGE;
	
        TA_Base_Bus::ScadaProxyFactory::getInstance();


		FUNCTION_EXIT;
	}

	//
	// Destructor
	//
	CInspectorPanelDlg::~CInspectorPanelDlg()
	{
		FUNCTION_ENTRY ("~CInspectorPanelDlg()");
		AccessControlHelper::getInstance()->terminateAndWait();

		// move to OnDestroy
		/*
		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}*/

		// 2008-07-17 remove below.
		//TA_Base_Bus::ScadaProxyFactory::removeInstance();

		FUNCTION_EXIT;
	}

	//
	// DoDataExchange
	//
	void CInspectorPanelDlg::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CInspectorPanelDlg)
		DDX_Control(pDX, IDC_STATUS, m_statusBox);
		DDX_Control(pDX, IDC_MAINTAB, m_MainTab);
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}

	//
	// setPage
	//
	void CInspectorPanelDlg::setPage(std::string pageName)
	{
		FUNCTION_ENTRY ("setPage()");

		m_requestedPage = pageName;
		m_isInDirectControl = false;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** Requested Page = %s", m_requestedPage.c_str() );

		// For direct control page
        if (pageName==DIRECT_CONTROL_PAGE)
		{
            // Resize direct control dialog to the same as the mainTab
            CRect mainTabDimensions;
	        m_MainTab.GetWindowRect(mainTabDimensions);
	        this->ScreenToClient(mainTabDimensions);
            m_directControlDlg.MoveWindow(mainTabDimensions);

            m_MainTab.ShowWindow(SW_HIDE);
            m_directControlDlg.ShowWindow(SW_SHOW);

            m_isInDirectControl = true;

			FUNCTION_EXIT;
            return;
		}

        // Normal tabbed window 
        m_directControlDlg.ShowWindow(SW_HIDE);
        m_MainTab.ShowWindow(SW_SHOW);
		
		CMainTabControl::ETabNumbers newTab = m_previousTab;

		// Find to the required page
        if (pageName==INFORMATION_PAGE)
		{
			newTab = CMainTabControl::InformationTab;			
		}
		else if (pageName==CONTROL_PAGE)
		{
			newTab = CMainTabControl::ControlTab;
		}
		else if (pageName==LIMITS_PAGE)
		{
			newTab = CMainTabControl::LimitsTab;
		}
		else if (pageName==TAG_PAGE)
		{
			newTab = CMainTabControl::TagTab;
		}
		else if (pageName==NOTES_PAGE)
		{
			newTab = CMainTabControl::NotesTab;
		}
		else if (pageName==INHIBIT_PAGE)
		{
			newTab = CMainTabControl::InhibitTab;
		}
		else if (pageName==OVERRIDE_PAGE)
		{
			newTab = CMainTabControl::OverrideTab;
		}
		else if (pageName==METER_PAGE)
		{
			newTab = CMainTabControl::MeterTab;
		}

		if (m_equipment != NULL)
		{
			// if all data points belongs to the equipment have been properly configured
			if ( true == m_equipment->haveAllDataPointsConfigured() )
			{
				// if different tab selected
				if (newTab != m_previousTab)
				{
					// only proceed if previous tab indicates it has no unsaved data
					// and OK to change to different tab
					if ( ( false == m_MainTab.isUnsavedChangeOnTab ( m_previousTab ) ) || 
						 ( true == m_MainTab.isOkToChangeTab ( m_previousTab ) ) )
					{
						// change main tab from previous tab to the selected tab
						m_MainTab.SetCurSel(newTab);

						// save selected tab for further reference
						m_previousTab = newTab;					
					}
					m_requestedTab = m_previousTab;
				}
			}
			else
			{
				m_requestedTab = newTab;
			}
		}

		FUNCTION_EXIT;
	}

	//
	// setEquipment
	//
	void CInspectorPanelDlg::setEquipment(std::string equipmentName)
	{
		FUNCTION_ENTRY ("setEquipment()");

		// Update the current equipment
		if (m_equipment != NULL && m_equipment->getDataNode()->getEntityName() != equipmentName)
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "set equipment from [%s] to equipment [%s]", m_equipment->getDataNode()->getEntityName().c_str(), equipmentName.c_str());
			
            destroyEquipment();
		}

		if (m_equipment == NULL)
		{
			try
			{
				m_equipment = new CEquipment(equipmentName);

                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                    "new equipment [%s] created", equipmentName.c_str());
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"setEquipment() - %s", e.what());
			}
		}
	
		// Only set equipment for all Tag Dialogs
		// when datapoints had been created and all of them are config valid
		if ( 0 < m_equipment->getChildDataPointCount() && 
			true == m_equipment->haveAllDataPointsConfigured() ) 
		{
			// Update the tab control
			m_MainTab.setEquipment(m_equipment);
			m_directControlDlg.setEquipment(m_equipment);
		}
        else
        {
            // Need to update Status Tab for debugging
            m_MainTab.setStatusTabEquipment(m_equipment);
        }

		updateMainDialog(); 

		FUNCTION_EXIT;
	}


	//
	// destroyEquipment
	//
	void CInspectorPanelDlg::destroyEquipment()
	{
        TA_ASSERT(m_equipment != NULL, "Try to destroy NULL equipment");

        CEquipment* equipment = m_equipment;
		m_equipment = NULL;

		// Update the tab control
		m_MainTab.setEquipment(m_equipment);
        m_directControlDlg.setEquipment(m_equipment);

        delete equipment;
	}

	//
	// updateMainDialog
	//
	void CInspectorPanelDlg::updateMainDialog()
	{
		std::string asset = "";
		std::string label = "";
		std::string title = TA_Base_Core::RunParams::getInstance().get( RPARAM_APPNAME );

		if (m_equipment != NULL)
		{
			// Update the window title with asset name
			DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();
			try
			{
				asset = dataNode->getAssetName();
				label = dataNode->getLabel();
			}
			catch(ScadaProxyException&)
			{
			}
		}
			
		if (!asset.empty())
		{
			title += " - ";
			title += asset;
		}

		setTitleInMainThread(title);

		// For normal tabbed window
        if (!m_isInDirectControl)
        {
            setEquipmentLabelInMainThread(label.c_str());
        }

		// Check resizing
		resizeIfRequired();
	}

	//
	// onRunParamChange
	//
	void CInspectorPanelDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		FUNCTION_ENTRY ("onRunParamChange()");

        std::string* temporalName  = new std::string(name);
        std::string* temporalValue = new std::string(value);
        PostMessage(
            PROCESS_RUNPARAM_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(temporalName),
            reinterpret_cast<LONG>(temporalValue));

		FUNCTION_EXIT;
	}

	
	/////////////////////////////////////////////////////////////////////////////
	// CInspectorPanelDlg message handlers

	BEGIN_MESSAGE_MAP(CInspectorPanelDlg, TA_Base_Bus::TransActiveDialog)
	//{{AFX_MSG_MAP(CInspectorPanelDlg)
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()
		ON_NOTIFY(NM_CLICK, IDC_MAINTAB, OnClickMaintab)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
		ON_MESSAGE (UPDATED_PROXY_MESSAGE, OnProxyEntityUpdate)
		ON_MESSAGE (LOAD_DATAPOINTS_MESSAGE, OnLoadDataPoints)
        ON_MESSAGE (UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE, OnUpdateLabelInMainThread)
        ON_MESSAGE (PROCESS_RUNPARAM_IN_MAIN_THREAD_MESSAGE, OnProcessRunParamInMainThread)
        ON_MESSAGE (UPDATE_STATUS_WINDOW_MESSAGE, OnUpdateStatusWindow)
		ON_MESSAGE (UPDATE_PERMISSION, UpdatePermissions)
		ON_WM_DESTROY()
		ON_BN_CLICKED(ID_HELP_IP, OnHelpButton) //TD15124 azenitha
	END_MESSAGE_MAP()

	//TD15124 azenitha++
	void CInspectorPanelDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha


	void CInspectorPanelDlg::init()
	{
		// RR Initialise our Rights Library
        TA_Base_Bus::RightsLibraryFactory rightsFactory;
		
		if ( TA_Base_Core::RunParams::getInstance().isSet("DebugAlwaysTrueRightsLib") )
		{
			m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
		}
		else
		{
			m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
		}
		
		// Set up the proxies
		TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();

        // Initialise the DirectControlDialog
		m_directControlDlg.Create(IDD_CONFIRM_DIRECT_CONTROL_DIALOG, this);
        m_directControlDlg.resetDialog();
		
		// Get the passed screen name
		std::string screenName = TA_Base_Core::RunParams::getInstance().get(SCREEN_RPARAM);
		if (!screenName.empty())
        {
            // execute immediately
            std::string* temporalName  = new std::string(SCREEN_RPARAM);
            std::string* temporalValue = new std::string(screenName);
            OnProcessRunParamInMainThread(
                reinterpret_cast<UINT>(temporalName),
                reinterpret_cast<LONG>(temporalValue));
        }
		
		// Get the passed value
		std::string value = TA_Base_Core::RunParams::getInstance().get(VALUE_RPARAM);
		if (!value.empty())
        {
            onRunParamChange(VALUE_RPARAM, value);
        }
		
		// Get the passed equipment and page details
		std::string equipment;
		equipment = TA_Base_Core::RunParams::getInstance().get(EQUIPMENT_RPARAM);
		TA_ASSERT (!equipment.empty(),"No equipment name passed");
        onRunParamChange(EQUIPMENT_RPARAM, equipment);
		
		// Monitor any changes
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, EQUIPMENT_RPARAM);
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SCREEN_RPARAM);
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, VALUE_RPARAM);
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);		
	}
	//
	// OnInitDialog
	//
	BOOL CInspectorPanelDlg::OnInitDialog()
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CRect windowSize;
		GetWindowRect(&windowSize);

		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise = false;
		properties.maxHeight = -1;
		properties.maxWidth = -1;
		properties.minHeight = windowSize.bottom - windowSize.top;
		properties.minWidth = windowSize.right - windowSize.left;
		setResizingProperties(properties);

		TransActiveDialog::OnInitDialog();

		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon



		FUNCTION_EXIT;
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	//
	// OnPaint
	//
	void CInspectorPanelDlg::OnPaint() 
	{
		FUNCTION_ENTRY ("OnPaint()");

		if (IsIconic())
		{
			CPaintDC dc(this); // device context for painting

			SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

			// Center icon in client rectangle
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Draw the icon
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			TransActiveDialog::OnPaint();
		}

		FUNCTION_EXIT;
	}

	void CInspectorPanelDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );
		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}

        CDialog::OnDestroy();       

        FUNCTION_EXIT;
   }
	//
	// OnQueryDragIcon
	//
	HCURSOR CInspectorPanelDlg::OnQueryDragIcon()
	{
		FUNCTION_ENTRY ("OnQueryDragIcon()");
		FUNCTION_EXIT;

		return (HCURSOR) m_hIcon;
	}

	//
	// OnCancel
	//
	void CInspectorPanelDlg::OnCancel() 
	{
		FUNCTION_ENTRY ("OnCancel()");

		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();

		FUNCTION_EXIT;
	}

	//
	// OnClickMaintab
	//
	void CInspectorPanelDlg::OnClickMaintab(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY ("OnClickMaintab()");

		*pResult = 0;

		// Check to see if we are allowed to change tabs
		CMainTabControl::ETabNumbers iTab = (CMainTabControl::ETabNumbers)m_MainTab.GetCurSel();

		if (m_MainTab.isUnsavedChangeOnTab(m_previousTab))
		{
			if (!m_MainTab.isOkToChangeTab(m_previousTab))
			{
				// Don't change tabs
				m_MainTab.SetCurSel(m_previousTab);
				return;
			}
		}

		m_previousTab = iTab;

		FUNCTION_EXIT;
	}

	//
	// OnProxyEntityUpdate
	//
	LRESULT CInspectorPanelDlg::OnProxyEntityUpdate (WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY ("OnProxyEntityUpdate()");

		if (m_equipment == NULL)
		{
			return 0;
		}

        // Get the entity key and scada update type
		unsigned long key = (unsigned long)wParam;
		ScadaEntityUpdateType updateType = (ScadaEntityUpdateType)lParam;

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "Begin to process update [%d] of entity [%u]", updateType, key);
        
		// Check to see if this is a datanode update
		if (m_equipment->getDataNode()->getEntityKey() == key)
		{
			if (updateType == ConfigStateUpdate || updateType == ConfigAvailable ) 
			{
				// Load the datapoints later - this takes a reasonable amount of time.			
				setEquipment(m_equipment->getDataNode()->getEntityName());
				loadDataPointsLater();
			}
			else 
			{
				updateMainDialog();	
				m_MainTab.updateDataNode(updateType);
			}
		    FUNCTION_EXIT;
		    return 0;
		}
		//  Check to see if this is a datapoint update
		if (m_equipment->getChildDataPointIndexFromEntityKey(key)<m_equipment->getChildDataPointCount())
		{
			// Only allow those datapoints that are config valid
			m_equipment->checkAllConfigValid();
			
			if ( !m_equipment->haveAllDataPointsConfigured() )
				return 0;

			if (updateType == ConfigStateUpdate || updateType == ConfigAvailable ) 
			{
				// only proceed if there is difference between previous and requested tab
				// during startup
				if ( m_previousTab != m_requestedTab )
				{
					// check the tab requested by the user, now that we have all
					// datapoints fully updated

					if( !m_isInitialTab )
					{
						// if user requests for Control tab
						if ( m_requestedPage == CONTROL_PAGE )
						{
							// if the equipment has no control data points
							if ( false == m_equipment->hasControlPoint() )
							{
								m_requestedPage = INFORMATION_PAGE;
							}
						}
						// if user requests for Meter tab
						else if ( m_requestedPage == METER_PAGE )
						{
							// if the equipment has no metered data points
							if ( false == m_equipment->hasMeteredPoint() )
							{
								m_requestedPage = INFORMATION_PAGE;
							}
						}

						setPage ( m_requestedPage );
						m_isInitialTab = true;
					}
				}
					
				setEquipment(m_equipment->getDataNode()->getEntityName());
			}
			else
			{
				// Update the displayed list point values
				if (false == m_isInDirectControl)
				{
					m_MainTab.updateDataPoints(key, updateType);
				}
				else
				{
					m_directControlDlg.updateControlState(key, updateType);
				}
			}
		}

		FUNCTION_EXIT;	
		return 0;
	}
	
	//
	// SetupPermissions
	//
	void CInspectorPanelDlg::setupPermissions()
	{
		if (m_equipment == NULL)
			return;

		std::vector<AccessEntityInfo> entityKeys;	
        AccessEntityInfo entityInfo;

		// add the equipment data NODE first
		entityInfo.entityKey = m_equipment->getDataNode()->getEntityKey();
        if (entityInfo.entityKey == 0L)			
			return;

        entityInfo.locationKey = m_equipment->getDataNode()->getLocationKey();
        entityInfo.subSystemKey = m_equipment->getDataNode()->getPhysicalSubSystemKey();
        if (entityInfo.subSystemKey == 0)
        {
            entityInfo.subSystemKey = m_equipment->getDataNode()->getSubSystemKey();
        }

		entityKeys.push_back(entityInfo);

		// add all the data POINTS for this equipment
		if (m_equipment->getChildDataPointCount() > 0)
		{
			std::vector<DataPointProxySmartPtr*>& dpVector = m_equipment->getChildDataPointVector();
			for (std::vector<DataPointProxySmartPtr*>::iterator iter = dpVector.begin(); 
																		iter != dpVector.end(); iter++)
			{

				TA_Base_Bus::DataPointProxySmartPtr * pDataPointPtr = *iter;
				entityInfo.entityKey = (*pDataPointPtr)->getEntityKey();
                entityInfo.locationKey = (*pDataPointPtr)->getLocationKey();
                entityInfo.subSystemKey = (*pDataPointPtr)->getPhysicalSubSystemKey();
                if (entityInfo.subSystemKey == 0)
                {
                    entityInfo.subSystemKey = (*pDataPointPtr)->getSubSystemKey();
                }

                entityKeys.push_back(entityInfo);
			}
		}
		
		// initialise the access control permissions map
		std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		AccessControlHelper::getInstance()->InitAccessControlMap(m_rightsMgr, sessionId, entityKeys);
      PostMessage(UPDATE_PERMISSION, 0, 0);
	}

	//
	// OnLoadDataPoints
	//
	LRESULT CInspectorPanelDlg::OnLoadDataPoints (WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY ("OnLoadDataPoints()");

		// Get the passed equipment and page details
		try
		{	
			if (m_equipment)
			{
				if ( true == m_equipment->loadDataPoints() )
				{
					// Setup permissions here
					setupPermissions(); 
				}
			}
       		else
		    {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_081009);
            }
		}
		catch (TA_Base_Core::ScadaProxyException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"loadDataPoints() - %s", e.what());

		}
		catch (...)
		{
			std::string message = "Error: Unknown exception when getting equipment details.";
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"loadDataPoints() - %s", message.c_str());
        }

		FUNCTION_EXIT;

		return 0;
	}

	LRESULT CInspectorPanelDlg::UpdatePermissions (WPARAM wParam, LPARAM lParam)
	{
		m_MainTab.updatePermissions();
		m_directControlDlg.updatePermissions();
		if (wParam == 1 )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"UpdatePermissions");
			AccessControlHelper::getInstance()->terminateAndWait();
		}
		return 0;
	}

	//
	// resizeIfRequired
	//
	void CInspectorPanelDlg::resizeIfRequired()
	{
		FUNCTION_ENTRY ("resizeIfRequired()");

		if (m_heightDifference == 0)
		{
			CRect windowRect;
			GetWindowRect(&windowRect);

			CRect tabRect;
			m_MainTab.GetWindowRect(&tabRect);

			m_heightDifference = windowRect.Height()-tabRect.Height();
		}
		
		int yOffset = m_MainTab.resize();

		if (yOffset != 0)
		{
			// Move status group box
			CWnd *window = GetDlgItem(IDC_STATUS_GROUP_BOX);
			CRect statusGrpBoxRect;
			window->GetWindowRect(&statusGrpBoxRect);
			ScreenToClient(&statusGrpBoxRect);
			statusGrpBoxRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, statusGrpBoxRect.left, statusGrpBoxRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
	
		if (yOffset != 0)
		{
			// Move status box
			CWnd *window = GetDlgItem(IDC_STATUS);
			CRect statusBoxRect;
			window->GetWindowRect(&statusBoxRect);
			ScreenToClient(&statusBoxRect);
			statusBoxRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, statusBoxRect.left, statusBoxRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			m_statusBox.Invalidate(); 	
		}

		if (yOffset != 0)
		{
			// Move cancel button
			CWnd *window = GetDlgItem(IDCANCEL);
			CRect cancelRect;
			window->GetWindowRect(&cancelRect);
			ScreenToClient(&cancelRect);
			cancelRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, cancelRect.left, cancelRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			GetDlgItem(IDCANCEL)->Invalidate(); 
		}

		if (yOffset != 0)
		{
			// Move help button
			CWnd *window = GetDlgItem(ID_HELP_IP); //TD15124 azenitha
			CRect helpRect;
			window->GetWindowRect(&helpRect);
			ScreenToClient(&helpRect);
			helpRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, helpRect.left, helpRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			GetDlgItem(ID_HELP_IP)->Invalidate(); //TD15124 azenitha
		}

		// Resize window
		CRect windowRect, tabRect;
		GetWindowRect(&windowRect);		
		m_MainTab.GetWindowRect(&tabRect);

		// Stay on top and make sure it ll resize eventually
		SetWindowPos(&wndTopMost, 0, 0, windowRect.Width(), tabRect.Height()+m_heightDifference, SWP_NOMOVE|SWP_SHOWWINDOW);		
		Invalidate();
	
		FUNCTION_EXIT;
	}

    void CInspectorPanelDlg::setEquipmentLabelInMainThread(const std::string& label)
    {
        CWnd* window = GetDlgItem(IDC_EQUIPMENT_DESCRIPTION);
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(window),
            reinterpret_cast<LONG>(temporalString));
    }

    void CInspectorPanelDlg::setTitleInMainThread(const std::string& label)
    {
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(this),
            reinterpret_cast<LONG>(temporalString));
    }

    LRESULT CInspectorPanelDlg::OnUpdateLabelInMainThread(UINT wParam, LONG lParam)
    {
        std::string* temporalString = reinterpret_cast<std::string*>(lParam);
        CWnd* desiredLabel = reinterpret_cast<CWnd*>(wParam);

        if (temporalString)
        {
            if (desiredLabel)
            {
                CString description = temporalString->c_str();

                int pos = description.Find ( "&", 0 );
                if ( -1 != pos )
                {
                    description.Insert ( pos, "&" );
                }

                // Wrap the text around if it will not fit into the label (TD 11255)
                int charsWide = 0;
                SIZE sizeString;
                CRect clientRect;
                desiredLabel->GetClientRect( &clientRect);
                CDC* pdc = desiredLabel->GetDC();

                if (GetTextExtentExPoint( pdc->GetSafeHdc(), 
                    (LPCTSTR) description, description.GetLength(),
                    clientRect.Width(), &charsWide, NULL, &sizeString))
                {
                    if ( description.GetLength() > charsWide)
                    {
                        //  The string is too big to fit into the desiredLabel.
                        //  Default text wrapping will work if there is a space...

                        pos = description.Find ( " ", 0 );
                        if ( ( -1 == pos ) || (pos > charsWide) )
                        {
                            // ... but instead of a space, insert a linebreak to be sure.
                            description.Insert ( charsWide, "\n" );
                        }
                    }
                    desiredLabel->ReleaseDC( pdc);
                }

                desiredLabel->SetWindowText ( description );
            }
            delete temporalString;
        }
        return 0;
    }

    LRESULT CInspectorPanelDlg::OnProcessRunParamInMainThread(UINT wParam, LONG lParam)
    {
        std::string* temporalName  = reinterpret_cast<std::string*>(wParam);
        std::string* temporalValue = reinterpret_cast<std::string*>(lParam);

        try
        {
		    if (*temporalName==EQUIPMENT_RPARAM) 
		    {
                if ( m_equipment != NULL )
                {
                    // If replacing current equipment, send a message to the Status line
                    std::string message = "Changing entity to ";
                    message += *temporalValue;
                    message += ".";

					std::string* statusLineStr = new std::string(message);
					PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
                }

                // Determine whether the ControlEntity is a datanode or a datapoint
                try
                {
					std::string type = TA_Base_Core::EntityAccessFactory::getInstance().getTypeOf(*temporalValue);
					
					m_isInitialTab = false;

                    if (type == DataNodeEntityData::getStaticType())
                    {
			            setEquipment(*temporalValue);
						loadDataPointsLater();
   
						// Clear directControlDialog
                        m_directControlDlg.resetDialog();	
                    }
                    else if (type == DataPointEntityData::getStaticType())
                    {
                        std::string equipmentName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfParent(*temporalValue).getObjectName();
						
                        if (equipmentName.size() == 0)
                        {   
							// Unable to locate equipment DataNode
                            destroyEquipment();	
                        }
                        else
                        {
       			            setEquipment(equipmentName);
        		            loadDataPointsLater();

                            // Update the DirectControlDialog with the new equipment instance
							TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(*temporalValue,false);
							if ( m_equipment != NULL )
							{
								m_directControlDlg.setEquipment(m_equipment);
								m_directControlDlg.setDataPoint(corbaName.getEntityKey());  
							}
                        }
                    }
                    else
                    {
						std::string message = "Error: Unknown entity type:" + type;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", message.c_str());
                	}
                }
                catch(...)
                {
                    std::string message = "Internal Error occured whilst loading Equipment.";	    
			        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", message.c_str());
                }

		    }
		    else if (*temporalName==SCREEN_RPARAM)
		    {
			    setPage(*temporalValue);
		    }
		    else if (*temporalName == VALUE_RPARAM)
		    {
                m_directControlDlg.setNewValue(*temporalValue);
		    }		    
            else if (*temporalName == RPARAM_SESSIONID)
		    {
			    setupPermissions();
		    }
        }
        catch (...)
        {
        
        }

        delete temporalName;
        delete temporalValue;
        return 0;
    }

    std::string CInspectorPanelDlg::getEquipmentNameFromDataPointEntity(TA_Base_Core::IEntityData* entity)
    {

        // determine the equipment datanode

        // The approach is not the most efficient - 
        // 1. Obtain the datapoint's parentkey (dataNode's entityKey)
        // 2. Determine the corba name from the key
        // 3. Extract the objectName field (which is equivalent to the dataNode 
        //    entityName)
        // 4. Return the dataNode name
        // 5. CEquipment constructor takes the name and obtains the CorbaName
        //    for the second time.
        //
        // The decision for this approach is to minimise impact.

        std::string equipmentName("");

        unsigned long equipmentKey = entity->getParent();

        if ( equipmentKey!=0 )
        {
            try
            {
                TA_Base_Core::CorbaName equipmentCorbaName(
                    TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(equipmentKey,false));
                equipmentName = equipmentCorbaName.getObjectName();
            }
            catch (...)
            {

       			// Tell user no data found
			    std::string message = "Warning: Failed to obtain equipment details.";
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", message.c_str());

                // Nothing fatal - just return the NULL string
                return "";
            }
            
        }

        return equipmentName;
    }


	void CInspectorPanelDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
    {
        // Find out the coordinates of the current window
        WINDOWPLACEMENT place;
        GetWindowPlacement(&place);

        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int numScreens = GetSystemMetrics(SM_CMONITORS);

        // Determine the screen we are currently on.
        int currentScreen = place.rcNormalPosition.left / (screenWidth / numScreens);

        // If we have changed screen, then attempt to retrieve the available working area.

        if (currentScreen != m_currentScreen)
        {
            m_currentScreen = currentScreen;

            // Get the available working area from the control station
            try
            {
                m_workingArea = TA_Base_Bus::AppLauncher::getInstance().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_ALL, 
                    TA_Base_Bus::TA_GenericGui::AREA_MAINNAVIGATOR);
            }
            catch(TA_Base_Core::ApplicationException& ex)
            {
                // Log the catch, and set the workingArea rect to 0.
                std::stringstream errorMessage;
                errorMessage << "An exception was caught while attempting to retrieve the schematic area of the current"
                    << "screen. X coord = " << place.rcNormalPosition.left << ". Error Message:";
                LOG_EXCEPTION_CATCH(SourceInfo,"ApplicationException",errorMessage.str().c_str());
                LOGMORE(SourceInfo,ex.what());

                // Set the workingArea rect to 0
                m_workingArea.SetRectEmpty();
            }
        }

        // The resized window, represented by lpwndpos, cannot fall outside the schematic rectangle, so reset 
        // variables as necessary
        if (!m_workingArea.IsRectEmpty())
        {
            // Set the width
            if (lpwndpos->cx > m_workingArea.Width())
            {
                lpwndpos->cx = m_workingArea.Width();
            }

            // Set the height
            if (lpwndpos->cy > m_workingArea.Height())
            {
                lpwndpos->cy = m_workingArea.Height();
            }

            // Set the x-coordinate
            if (lpwndpos->x < m_workingArea.left)
            {
                lpwndpos->x = m_workingArea.left;
            }
            else if (lpwndpos->x + lpwndpos->cx > m_workingArea.right)
            {
                lpwndpos->x = m_workingArea.right - lpwndpos->cx;
            }

            // Set the y-coordinate
            if (lpwndpos->y < m_workingArea.top)
            {
                lpwndpos->y = m_workingArea.top;
            }
            else if (lpwndpos->y + lpwndpos->cy > m_workingArea.bottom)
            {
                lpwndpos->y = m_workingArea.bottom - lpwndpos->cy;
            }
        }

		//TD14564
		//set the window invalid and cause the window to be redrawn.
		Invalidate();   
		ShowWindow(SW_SHOW);
    }


	// TD #11930
	// Handle the PostMessage message to update the status window text.
	// The status text should be in wParam, and the timestamp in lParam
    LRESULT CInspectorPanelDlg::OnUpdateStatusWindow(UINT wParam, LONG lParam)
    {
        std::string* statusText = reinterpret_cast<std::string*>(wParam);

        if(statusText)
        {
			if(statusText->length() > 0)
			{
				CInspectorPanelDlg* dlg = dynamic_cast<CInspectorPanelDlg*>(AfxGetApp()->m_pMainWnd);

				// use the timestamp if it's set, otherwise use the current time
				time_t timeT = ( lParam > 0 ) ? lParam : time (NULL);
				struct tm localTm;
				ACE_OS::localtime_r( &timeT, &localTm );

				// wenching++ (TD13872)
				// Follow the date\time format of Event Viewer:
				std::string month="";
				switch(localTm.tm_mon)
				{
				case 0 : month = "JAN";
						 break;
				case 1 : month = "FEB";
						 break;
				case 2 : month = "MAR";
					     break;
				case 3 : month = "APR";
					     break;
				case 4 : month = "MAY";
					     break;
				case 5 : month = "JUN";
					     break;
				case 6 : month = "JUL";
					     break;
				case 7 : month = "AUG";
					     break;
				case 8 : month = "SEP";
					     break;
				case 9 : month = "OCT";
					     break;
				case 10 : month = "NOV";
					     break;
				case 11 : month = "DEC";
					     break;
				}
				
				std::stringstream timestampedStatusText;
				timestampedStatusText << ( localTm.tm_mday < 10 ? "0" : "" )
									  << localTm.tm_mday << "-"
					                  << month.c_str() << "-"
									  << localTm.tm_year+1900 << " "
									  << ( localTm.tm_hour < 10 ? "0" : "" ) 
									  << localTm.tm_hour << ":" 
									  << ( localTm.tm_min < 10 ? "0" : "" ) 
									  << localTm.tm_min << ":" 
									  << ( localTm.tm_sec < 10 ? "0" : "" ) 
									  << localTm.tm_sec << " - "
									  << *statusText;
				//++wenching (TD13872)

				// Set the new status text
				dlg->m_statusBox.SetWindowText(timestampedStatusText.str().c_str());
			}

            delete statusText;
        }

		return 0;
	}
}

void CInspectorPanelDlg::OnClose() 
{
	if (m_equipment != NULL)
	{
		delete m_equipment;
		m_equipment = NULL;
	}
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();
}

void CInspectorPanelDlg::resetList()
{
	m_MainTab.resetList();
}
