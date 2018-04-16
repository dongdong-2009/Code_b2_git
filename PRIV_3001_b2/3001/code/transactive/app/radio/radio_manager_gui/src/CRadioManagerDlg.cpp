/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CRadioManagerDlg.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/resource.h"

#include "app/radio/radio_manager_gui/src/CRadioManagerDlg.h"
//#include "app/radio/radio_manager_gui/src/RadioTextMessagingDialog/CTextMessagingDlg.h"
#include "app/radio/radio_manager_gui/src/CRadioMonitoringSheet.h"
//#include "app/radio/radio_manager_gui/src/GenericDialpadDialog/CGenericDialpadDlg.h"
//#include "app/radio/radio_manager_gui/src/RadioNewEntityDialog/CEntityDialog.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include"ace/OS_NS_time.h"
//#include "app/radio/radio_manager_gui/src/RadioPatchManagerDialog/PatchOperations.h"
#include "app/radio/radio_manager_gui/src/DebugMsg.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"

//#include "bus/telephony/TelephoneBus/src/TelephoneTypes.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"

#include "core/exceptions/src/RightsException.h"
#include "core/process_management/src/CtaRuntime.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "bus/security/rights_library/src/CtaRights.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "bus/generic_gui/src\applauncher.h"

#include "core\message\src\messagepublicationmanager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core\message\types\radiocomms_messagetypes.h"
#include "core/message/src/MessagePublicationManager.h"

#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

// This handles the loading of the RadioInitMsgDlg, all dialog to be add need
// to be add or load by RadioMngrDlg suggest to be added on this file.
#include "app\radio\radio_manager_gui\src\IDlgLoader.h"
#include "app\radio\radio_manager_gui\src\SingletonDlgLoader.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78	
#define MINVALUE				2400	
#define NUMSCREENS				3		

using namespace TA_Base_Bus::TA_GenericGui;

/////////////////////////////////////////////////////////////////////////////
// CRadioManagerDlg dialog

namespace
{
    const std::string RESOURCE_PARAM = "ResourceSelection";
    const std::string HISTORY_LIMIT = "HistoryLimit";
    const std::string PATCH_DLTS_PARAM = "DLTS_Number";
    const std::string RADIO_NUMBER_PARAM = "RadioNumber";
    const std::string SHOW_WINDOW = "ShowWindow";
    const int HIND_WINDOW_TIMER = 1061;	//Timer ID
}


CRadioManagerDlg::CRadioManagerDlg( TA_Base_Bus::IGUIAccess& genericGUICallback,
                                    CWnd* pParent /* NULL */)
	: TransActiveDialog(genericGUICallback, CRadioManagerDlg::IDD, pParent),
      m_CRadioMonitoringSheet(NULL),
      m_CRadioDirectoryPageDlg(NULL),
      m_CEntityDetailsAreaDlg(NULL),
      m_CEntityOperationsAreaDlg(NULL),
      m_CRadioManagerDlgStatusBar(NULL),
	  m_runInBackground(false),
	  //TD18626, jianghp
	  m_CtaRadioManagerSummary(NULL),
	  m_MFTFramework(NULL),
	  //TD18626, jianghp
	  m_radioCallback(NULL),
	  m_isLaunched(false),
	  m_isAssignedBySchematic(false)
{ 

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CRadioManagerDlg::~CRadioManagerDlg()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_LAUNCH_PATCH_DLG);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INITIALISE_MODE);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_TEXTMSG_RECEIVED); //TD16926
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_APIFAILURE_MODE);

    delete m_CRadioMonitoringSheet;
    m_CRadioMonitoringSheet = NULL;
    delete m_CRadioDirectoryPageDlg;
    m_CRadioDirectoryPageDlg = NULL;
    delete m_CEntityDetailsAreaDlg;
    m_CEntityDetailsAreaDlg = NULL;
    delete m_CEntityOperationsAreaDlg;
    m_CEntityOperationsAreaDlg = NULL;
    delete m_CRadioManagerDlgStatusBar;
    m_CRadioManagerDlgStatusBar = NULL;

	//TD18626, jianghp
	delete m_CtaRadioManagerSummary;
	m_CtaRadioManagerSummary = NULL;

	delete m_MFTFramework;
	m_MFTFramework = NULL;
	//TD18626, jianghp

	delete m_radioCallback;
	m_radioCallback = NULL;
}


void CRadioManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioManagerDlg)
	// peter.wong, disable toolbar buttons
	//DDX_Control(pDX, IDC_LOCATE_SPEAKER_BTN, m_locateBtn);
	//DDX_Control(pDX, IDC_NEW_ENTITY_BTN, m_newEntityBtn);
	//DDX_Control(pDX, IDC_INBOX_BTN, m_inboxBtn);
	//DDX_Control(pDX, IDC_DIALPAD_BTN, m_dialpadBtn);
	//DDX_Control(pDX, IDC_EMERGENCY_BTN, m_emergencyBtn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRadioManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(CRadioManagerDlg)
	// peter.wong, disable toolbar buttons
	//ON_BN_CLICKED(IDC_DIALPAD_BTN, OnDialPadToolbarButton)
	//ON_BN_CLICKED(IDC_INBOX_BTN, OnInboxToolbarButton)
	//ON_BN_CLICKED(IDC_NEW_ENTITY_BTN, OnNewEntity)
	//ON_BN_CLICKED(IDC_EMERGENCY_BTN, OnEmergencyModeToolbarButton)
	//ON_BN_CLICKED(IDC_LOCATE_SPEAKER_BTN, OnLocateSpeakerBtn)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE,onNotifiedEditMode)
	ON_MESSAGE(WM_OPERATION_MODE,onNotifiedOperationMode)
	ON_MESSAGE(WM_INITIALISE_MODE,onNotifiedInitialiseMode)
	ON_MESSAGE(WM_DATABASE_READY,onNotifiedOperationMode)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED, onNotifiedEntitySelectionChanged)
	//ON_MESSAGE(WM_LAUNCH_PATCH_DLG, onNotifiedLaunchPatchDlg)
	//ON_MESSAGE(WM_TEXTMSG_RECEIVED, onNotifiedLaunchInboxPage)
	ON_MESSAGE(WM_APIFAILURE_MODE, onNotifiedApiFailureMode)
	ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition) 
	ON_REGISTERED_MESSAGE(RadioManagerFramework::WM_RELAUNCH_RADIO_MANAGER, onRelaunchRadioManager)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioManagerDlg message handlers

//TD18626 AVteam
void CRadioManagerDlg::init()
{
	FUNCTION_ENTRY("init");
	//TD14272, Haipeng changed the 803 to813 with the modification in .rc file
    //SetWindowPos(NULL, 0, 0, 1280, 813, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);	
	CPoint   pt;   
	GetCursorPos(&pt);   
	RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
		TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
		pt.x);
	SetWindowPos(NULL, 0, 0, 1280, boundary.bottom-boundary.top, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW );
	//SetWindowPos(NULL, 0, 0, 1280, 818, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW );

	// implicitly create the singleton
	The_RadioOperations::instance();
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_LAUNCH_PATCH_DLG);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INITIALISE_MODE);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_TEXTMSG_RECEIVED); //TD16926
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this, WM_APIFAILURE_MODE);

	//register to be notified when a runparam changes
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this,RESOURCE_PARAM.c_str());
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this,PATCH_DLTS_PARAM.c_str());
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this,SHOW_WINDOW.c_str());
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this,RADIO_NUMBER_PARAM.c_str()); //TD16441
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this,"SDS"); // TD16926

    InitializeErrorMessageWindow(GetModuleHandle(NULL), m_hWnd);
	m_MFTFramework = new RadioMFTAgentFramework();

	m_radioCallback = new RadioTetraAgentCallbackSubscriber();	                      
	

	The_RadioSystemStatus::instance()->setCommandServant(m_MFTFramework->getCommandServant()); //TD16870
	
	createStatusPage();
	
	createDirectoryPage();

	createEntityDetailsArea();

	createEntityOperationsArea();

	createStatusBar();

	enableControls(FALSE);

	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Testing Log - @ init, calling loadDlg");
	loadRadioInitDlg(true,true);

//TD18786

//TD18626, jianghp
	// this must be constructed here
	
	TA_Base_Bus::IGUIAccess & guiAccess = this->getGenericGUICallbackObject();
	// create Transactive interfaces
	//TA_Base_Core::IEntityDataPtr entityData(guiAccess.getGUIEntity());
	
	// Get a new instance of this entity, otherwise this will cause crash as the instance of 
	// the entity is stored in the Generic GUI and explicitly destroyed on destructor at the
	// same time the shared_ptr will release the pointer.
	TA_Base_Core::IEntityDataPtr guiEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(guiAccess.getGUIEntity()->getKey()));


	m_CtaRadioManagerSummary = new CtaRadioManagerSummary(guiEntityData) ;
	
	m_CtaRadioManagerSummary->Subscribe();
//TD18626, jianghp

	// set Session in RadioSystemStatus
	TA_Base_Core::CtaRuntime* ptaRuntime = dynamic_cast<TA_Base_Core::CtaRuntime*>(m_CtaRadioManagerSummary);
    TA_ASSERT( ptaRuntime != NULL, "CtaRuntime* is NULL.");

	std::string sessionId = ptaRuntime->getMySessionId();
	The_RadioSystemStatus::instance()->setSession(sessionId);
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	FUNCTION_EXIT;
}
//TD18626 Avteam

BOOL CRadioManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog");

	
	CPoint   pt;   
	GetCursorPos(&pt);

	int numScreens = 1;
	int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int screenW = -1, screenH = -1;
	if(screenWidth >= MINVALUE)
		numScreens = NUMSCREENS;
	screenW = screenWidth/numScreens;

	//TD18626, jianghp
    TA_Base_Bus::ResizingProperties rp;
    rp.canMaximise = true;
	rp.maxWidth = screenW;//1280;
	rp.maxHeight = screenH;//1024;
	
    // The minimum allowed dimensions are initial (startup) dimensions
    CRect rc;
    GetClientRect(rc);
    rp.minWidth = screenW;//1280;//rc.Width();
    rp.minHeight = screenH;//1024;//rc.Height();
	
    setResizingProperties(rp);
	//TD18626, jianghp

    // Go and create dialog
    TransActiveDialog::OnInitDialog();
    bool showWindow = false;
	
	if (TA_Base_Core::RunParams::getInstance().isSet("Foreground"))
    {
        showWindow = true;
		m_runInBackground = false;
    }
	
	if (TA_Base_Core::RunParams::getInstance().isSet("ShowWindow"))
	{
        showWindow = true;
		
		int value = atoi(TA_Base_Core::RunParams::getInstance().get("ShowWindow").c_str());
		if ( !value ) 
		{
			m_runInBackground = true;
		} 
		else 
		{
			m_runInBackground = false;
		}
	}

	ModifyWindowPos();
	m_isLaunched = true;
	
	FUNCTION_EXIT;
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CRadioManagerDlg::OnDestroy() 
{
    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

	TransActiveDialog::OnDestroy();
	
}


void CRadioManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY("onRunParamChange");
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Name = %s <=> Value = %s",name.c_str(),value.c_str());

    if (name.compare(PATCH_DLTS_PARAM) == 0)
	{
		// Fix TD#13911
		//The_PatchOperations::instance()->onDLTSLaunchPatchManager(atoi(value.c_str()));
		return;
	}
	else if (name.compare(SHOW_WINDOW) == 0)
	{
		
		//Fix TD14268
		int showCmd = atoi(value.c_str());
		if(!showCmd)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
            SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		return;
	}
	else if (name.compare(RADIO_NUMBER_PARAM) == 0)
	{
        unsigned long resourceID = atol(value.c_str());
	    // find matching RadioResource
		// TD18115
		RadioResource radResource = The_RadioSystemStatus::instance()->getResourceByRID(resourceID,RR_RADIO);
		if (radResource.type != RR_NIL)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioResource from TrainMap = %s ",radResource.alias.c_str());
			The_RadioOperations::instance()->entitySelectionChange(radResource, true);
			The_RadioOperations::instance()->makePrivateCall(radResource); // TD16441
			return;
		}
		
		RadioResource tgResource = The_RadioSystemStatus::instance()->getResourceByRID(resourceID,RR_TALKGROUP);
		if (tgResource.type != RR_NIL)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioResource from TrainMap = %s ",radResource.alias.c_str());
			The_RadioOperations::instance()->entitySelectionChange(tgResource, true);
			The_RadioOperations::instance()->selectSpeaker(tgResource);
			return;
		}

		RadioResource agResource = The_RadioSystemStatus::instance()->getResourceByRID(resourceID,RR_ANNGROUP);
		if (agResource.type != RR_NIL)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioResource from TrainMap = %s ",radResource.alias.c_str());
			The_RadioOperations::instance()->entitySelectionChange(agResource, true);
			The_RadioOperations::instance()->selectSpeaker(agResource);
		}		
		// TD18115
    }
	else if (name.compare(RESOURCE_PARAM) == 0)
	{
    	CString buffer = value.c_str();
	    CString resourceID;
	    CString resourceType;

	    // extract from value = "ID-<id>,Type-<resourcetype>"
	    int n = buffer.Find(",");

		CString strID = buffer.Left(n);
		CString strType = buffer.Mid(n);

        int typeIndex = strType.Find("Type");
        int idIndex = strID.Find("ID");

        if (typeIndex != -1 && idIndex != -1)
        {
			resourceID = strID.Mid(idIndex + 3);
            resourceType = strType.Mid(typeIndex + 5);
		}

		if (0 == (unsigned long) atol(resourceType)) {
			SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			// find matching RadioResource
			std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_RADIO);

			std::vector<RadioResource>::iterator iter = resourceList.begin();

			for ( ; iter != resourceList.end(); ++iter)
			{
				if ( (*iter).id == (unsigned long) atol(resourceID) )
				{
					The_RadioOperations::instance()->entitySelectionChange(*iter, true);
					The_RadioOperations::instance()->answerPrivateCall(*iter);
					//if (!atoi(resourceType)) // TD18166, emergency mode
					//{
					//	//The_RadioOperations::instance()->answerPrivateCall(*iter);
					//	//The_RadioOperations::instance()->onPrivateCallRinging(*iter);
					//}
					break;
				}
			}
		} else if (1 == (unsigned long) atol(resourceType)){
			SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_TALKGROUP);
			std::vector<RadioResource>::iterator iter = resourceList.begin();
			for (; iter!= resourceList.end(); ++iter)
			{
				if ( (*iter).id == (unsigned long) atol(resourceID) )
				{
					The_RadioOperations::instance()->entitySelectionChange(*iter, true);
					The_RadioOperations::instance()->ackEmergencyCall(*iter);
				}
			}
		} else if (2 == (unsigned long) atol(resourceType)) {
			
			SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			// driver call

			The_RadioOperations::instance()->makeDriverCall((unsigned long) atol(resourceID));

		} else if (3 == (unsigned long) atol(resourceType)) {
			std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_RADIO);

			RadioResource r;
			std::vector<RadioResource>::iterator iter = resourceList.begin();

			for ( ; iter != resourceList.end(); ++iter)
			{
				if ( (*iter).id == (unsigned long) atol(resourceID) )
				{
					r = (*iter);
					break;
				}
			}
			if ( 0 == r.id ) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio Resource ID %u not found.", r.id);
			} else {
				The_RadioOperations::instance()->removeResource(r);
				The_RadioSystemStatus::instance()->removeResource(r);
			}
		} else if (4 == (unsigned long) atol(resourceType)) { // Make private call
			std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_RADIO);

			RadioResource r;
			std::vector<RadioResource>::iterator iter = resourceList.begin();

			for ( ; iter != resourceList.end(); ++iter)
			{
				if ( (*iter).id == (unsigned long) atol(resourceID) )
				{
					r = (*iter);
					break;
				}
			}
			if ( 0 == r.id ) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio Resource ID %u not found.", r.id);
			} else {
				The_RadioOperations::instance()->makePrivateCall(r);
			}
		}
	}
	else if (name.compare("SDS") == 0) // TD16926
	{
	}

	

	FUNCTION_EXIT;

}

CtaRadioManagerSummary* CRadioManagerDlg::getCtaRadioManagerSummary()
{
    FUNCTION_ENTRY("getCtaRadioManagerSummary");
    FUNCTION_EXIT;
	return m_CtaRadioManagerSummary;
}


void CRadioManagerDlg::createStatusPage()
{
    FUNCTION_ENTRY("createStatusPage");
    
	CWnd *pwndPropSheetHolder = GetDlgItem(IDC_PROPSHEET_HOLDER);
    RECT rc;

    //GetClientRect(&rc);

    pwndPropSheetHolder->GetClientRect(&rc);

    rc.bottom -= 100; // space for title tabs
    //rc.right -= 150; // padding
	rc.right -= 100; // padding	

	m_CRadioMonitoringSheet = new CRadioMonitoringSheet("Radio Page", this, &rc); 	

	if( !m_CRadioMonitoringSheet->Create(this,WS_CHILD,0) )
	{
		delete m_CRadioMonitoringSheet;
		m_CRadioMonitoringSheet = NULL;
		return;
	}


	CRect holder;
	pwndPropSheetHolder->GetWindowRect(&holder);
	ScreenToClient(&holder);

	int xPos = holder.left;
	int yPos = holder.top;
	int cxPos = holder.Width();
	int cyPos = holder.Height();

	m_CRadioMonitoringSheet->SetWindowPos(NULL, xPos,yPos, cxPos, cyPos, 
        SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

	//pwndPropSheetHolder->DestroyWindow();


	FUNCTION_EXIT;
}


void CRadioManagerDlg::createDirectoryPage()
{
    FUNCTION_ENTRY("createDirectoryPage");

	m_CRadioDirectoryPageDlg = new CRadioDirectoryPageDlg();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Page dlg: %d",m_CRadioDirectoryPageDlg);
	CWnd* holder = GetDlgItem(IDC_DIRECTORY_PANEL_HOLDER);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Pageholder dlg: %d",holder);
	m_CRadioDirectoryPageDlg->Create(IDD_DIRECTORY_PAGE,holder);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After create");

	CRect holderRect;
	holder->GetClientRect(&holderRect);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Got client rect");
	int xPos = holderRect.left + 5;
	int yPos = holderRect.top + 15;
	int cxPos= holderRect.Width() - 10;
	int cyPos= holderRect.Height() - 17;
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting window position");
	m_CRadioDirectoryPageDlg->SetWindowPos(NULL, xPos,yPos, cxPos,cyPos, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "affter Setting window position");
    FUNCTION_EXIT;
	
}


void CRadioManagerDlg::createEntityDetailsArea()
{
    FUNCTION_ENTRY("createEntityDetailsArea");
    
	
	m_CEntityDetailsAreaDlg = new CEntityDetailsAreaDlg();
	
	CWnd* holder = GetDlgItem(IDC_DETAILS_HOLDER);
	m_CEntityDetailsAreaDlg->Create(IDD_SELECTED_ENTITY,holder);
	m_CEntityDetailsAreaDlg->ShowWindow(SW_SHOW);
	
	FUNCTION_EXIT;
}


void CRadioManagerDlg::createEntityOperationsArea()
{
    FUNCTION_ENTRY("createEntityOperationsArea");
    
	
	m_CEntityOperationsAreaDlg = new CEntityOperationsAreaDlg();
	
	CWnd* holder = GetDlgItem(IDC_OPERATIONS_HOLDER);
	m_CEntityOperationsAreaDlg->Create(IDD_SELECTED_OPERATIONS,holder);
	m_CEntityOperationsAreaDlg->ShowWindow(SW_SHOW);

	FUNCTION_EXIT;
}


void CRadioManagerDlg::createStatusBar()
{
    FUNCTION_ENTRY("createStatusBar");
    
	m_CRadioManagerDlgStatusBar = new CRadioManagerDlgStatusBar();

	m_CRadioManagerDlgStatusBar->Create(this);

	//Three panes are set to be the same size;
	UINT BASED_CODE indicators[] =
	{
		ID_SEPARATOR,       
		IDS_STATUS_BAR_MSG_1,
		IDS_STATUS_BAR_MSG_2//,
		//IDS_STATUS_BAR_MSG_3
	};

	m_CRadioManagerDlgStatusBar->SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) );

	CRect rect;
	GetClientRect(&rect);
	
    m_CRadioManagerDlgStatusBar->SetPaneInfo(0,ID_SEPARATOR,SBPS_NORMAL,(rect.Width()/3) + 60);
	m_CRadioManagerDlgStatusBar->SetPaneInfo(1,IDS_STATUS_BAR_MSG_1,SBPS_NORMAL, rect.Width()/3 );
	m_CRadioManagerDlgStatusBar->SetPaneInfo(2,IDS_STATUS_BAR_MSG_2,SBPS_NORMAL, rect.Width()/3);
	//m_CRadioManagerDlgStatusBar->SetPaneInfo(3,IDS_STATUS_BAR_MSG_3,SBPS_STRETCH, rect.Width()/4);
	
	//TD14273, Haipeng added one line:
	m_CRadioManagerDlgStatusBar->GetStatusBarCtrl().SetMinHeight(20);
	

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,ID_SEPARATOR);
    m_CRadioManagerDlgStatusBar->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	FUNCTION_EXIT;
}


bool CRadioManagerDlg::checkRights(TA_Base_Bus::AccessControlledAction action)
{
	FUNCTION_ENTRY ("checkRights");
	bool bHasRights = false;
	
	FUNCTION_EXIT;
	return true;
	
	/*if ( m_CtaRadioManagerSummary!= NULL )
	{
		TA_Base_Bus::CtaRights* ptaRights = dynamic_cast<TA_Base_Bus::CtaRights*>(m_CtaRadioManagerSummary);
		
		TA_Base_Core::CtaRuntime* ptaRuntime = dynamic_cast<TA_Base_Core::CtaRuntime*>(m_CtaRadioManagerSummary);
    
		std::string sessionId = ptaRuntime->getMySessionId();
		unsigned long entitykey = ptaRuntime->getMyEntityKey(); 

		std::string reason;

		try
		{
			bHasRights = ptaRights->isActionPermittedOnResource(sessionId, entitykey, action, reason);
		}
		catch(TA_Base_Core::RightsException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Catch RightException: %s", e.what());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred on isActionPermittedOnResource()");
		}		
	}

	FUNCTION_EXIT;
    return bHasRights;*/
}

LRESULT CRadioManagerDlg::onNotifiedEditMode(WPARAM wParam , LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedEditMode");
	m_editMode = TRUE;
	enableControls(FALSE);
	FUNCTION_EXIT;
	return 0;
}


LRESULT CRadioManagerDlg::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedOperationMode");
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log - onNotifiedOperationMode");

	m_editMode = FALSE;
	enableControls(TRUE);

	// unload or close the initialization dialog
	unloadRadioInitDlg (true);

	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioManagerDlg::onNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedEntitySelectionChanged");
	// peter.wong, disable toolbar buttons
	if (m_editMode) return 0; // editing, no change
	FUNCTION_EXIT;
    return 0;
}

LRESULT CRadioManagerDlg::onNotifiedApiFailureMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedApiFailureMode");
	
	//Commented : Remove unnecessary pop up message dialog.
	// MFT operator is transparent for the recovery of the connection if there is a problem
	// between RadioManager and MCC Server.
	//AddErrorMessage("Unable to establish Connection with the Radio System");
	
	FUNCTION_EXIT;
	return 0;
}

void CRadioManagerDlg::enableControls(BOOL flag)
{
	FUNCTION_ENTRY("enableControls");
	FUNCTION_EXIT;
}

LRESULT CRadioManagerDlg::onNotifiedInitialiseMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedInitialiseMode");
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log -  onNotifiedInitialiseMode"); 
	
	IRadioInitDlgMsg::ERadioInitMsgState * msgState = (IRadioInitDlgMsg::ERadioInitMsgState *)SingletonDlgLoader::getInstance()->getState(SingletonDlgLoader::ERadioMsgDlgInit);
	
	if (*msgState == IRadioInitDlgMsg::EOperation && 
		!SingletonDlgLoader::getInstance()->isDlgExist(SingletonDlgLoader::ERadioMsgDlgInit))
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"Testing Log -  EOperation state and not shown, showing dialog"); 
		loadRadioInitDlg(true);
	}

	FUNCTION_EXIT;
	return 0;
}

void CRadioManagerDlg::OnClose()
{
	// TD17056
	// Don't want to shutdown Radio Manager, just hide it.
	int opt = AfxMessageBox("Do you want to close RadioManager?", MB_YESNO|MB_ICONEXCLAMATION);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing Log - OnClose");
	if (opt == IDYES)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing Log - Closing the dialog, yes as reponse");
		ShowWindow(SW_HIDE);
		unloadRadioInitDlg(false);

	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Return to previous dialog.");
	}
}

BOOL CRadioManagerDlg::DestroyWindow()
{
	FUNCTION_ENTRY("DestroyWindow");

	m_CRadioMonitoringSheet->DestroyWindow();

	m_CRadioDirectoryPageDlg->DestroyWindow();

	m_CEntityDetailsAreaDlg->DestroyWindow();

	m_CEntityOperationsAreaDlg->DestroyWindow();

	m_CRadioManagerDlgStatusBar->DestroyWindow();
	
	FUNCTION_EXIT;
	return TransActiveDialog::DestroyWindow();
}

void CRadioManagerDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == HIND_WINDOW_TIMER)
	{
		ShowWindow(SW_HIDE);
		KillTimer (nIDEvent);
	}
}


LRESULT CRadioManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY( "OnSetWindowPosition" );
	LRESULT result;

	if (m_isLaunched && (lParam == TA_Base_Bus::TA_GenericGui::REPOSITION))
	{        
		result = TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION);
	}
	else
	{
		result = TransActiveDialog::OnSetWindowPosition(wParam, lParam);
	}

	ModifyWindowPos();
	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	FUNCTION_EXIT;
	return result;
}

void CRadioManagerDlg::ModifyWindowPos()
{
	FUNCTION_ENTRY("ModifyWindowPos");

	int left,top,width,height;
	int screenWidth,currentScreen;
	int numScreens = 1;
	int x_pos;

	CPoint   pt;   
	GetCursorPos(&pt);   

	RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
		TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
		pt.x);
	x_pos = pt.x;
	screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	if(screenWidth >= MINVALUE)
		numScreens = NUMSCREENS;
	currentScreen = x_pos / (screenWidth / numScreens);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModifyWindowPos: currentScreen = %u", currentScreen);
	left	= (currentScreen) * (screenWidth / numScreens);
	width	= screenWidth / numScreens;
	top		= boundary.top;
	height	= boundary.bottom-boundary.top;

	this->MoveWindow(left,top,width,height,NULL);

	FUNCTION_EXIT;
}

LRESULT CRadioManagerDlg::onRelaunchRadioManager(WPARAM wParam, LPARAM lParam)
{
	//ModifyWindowPos();
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log - onRelaunchRadioManager"); 

	ShowWindow(SW_SHOW);

	loadRadioInitDlg(false);

	return TRUE;
}

void CRadioManagerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	FUNCTION_ENTRY("OnSetFocus");
	

	FUNCTION_EXIT;
}

void CRadioManagerDlg::loadRadioInitDlg (bool bSetSpecDlg,bool bAddDlg)
{
	FUNCTION_ENTRY ("loadRadioInitDlg");
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log : loadRadioInitDlg");

	if (bAddDlg)
	{
		SingletonDlgLoader::getInstance()->addDlgToLoad(SingletonDlgLoader::ERadioMsgDlgInit,this);	
	}
	
	if (bSetSpecDlg)
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Setting EInit as State");
		SingletonDlgLoader::SDialogSpecParam dlgParam;
		long lDlgMsg = (long)IRadioInitDlgMsg::EInit;

		dlgParam.dlgType = SingletonDlgLoader::ERadioMsgDlgInit;
		dlgParam.vData = new IRadioInitDlgMsg::ERadioInitMsgState;
		
		dlgParam.vData = (void *)&lDlgMsg;

		SingletonDlgLoader::getInstance()->setSpecAssignableForDlg(dlgParam);
	}
	
	SingletonDlgLoader::getInstance()->openDlg(SingletonDlgLoader::ERadioMsgDlgInit);
	FUNCTION_EXIT;
}

void CRadioManagerDlg::unloadRadioInitDlg (bool bSetSpecDlg)
{
	FUNCTION_ENTRY ("unloadRadioInitDlg");
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log : unloadRadioInitDlg");
	
	SingletonDlgLoader::getInstance()->closeDlg(SingletonDlgLoader::ERadioMsgDlgInit);

	if (bSetSpecDlg)
	{
		SingletonDlgLoader::SDialogSpecParam dlgParam;
		long lDlgMsg = (long)IRadioInitDlgMsg::EOperation;

		dlgParam.dlgType = SingletonDlgLoader::ERadioMsgDlgInit;
		dlgParam.vData = new IRadioInitDlgMsg::ERadioInitMsgState;;
		dlgParam.vData = (void *)&lDlgMsg;

		SingletonDlgLoader::getInstance()->setSpecAssignableForDlg(dlgParam);
	}

	FUNCTION_EXIT;	
}
void CRadioManagerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Testing Log : OnShowWindowEvent");
	loadRadioInitDlg (false);
}