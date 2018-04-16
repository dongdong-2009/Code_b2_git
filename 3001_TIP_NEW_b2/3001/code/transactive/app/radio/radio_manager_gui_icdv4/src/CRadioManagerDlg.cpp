/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CTextMessagingDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioMonitoringSheet.h"
#include "app/radio/radio_manager_gui_icdv4/src/GenericDialpadDialog/CGenericDialpadDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioNewEntityDialog/CEntityDialog.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"

#include"ace/OS_NS_time.h"
//#include "app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioErrorMsg.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	  m_CRadioInitMsgDlg(NULL),
	  m_runInBackground(true),
	  //TD18626, jianghp
	  m_CtaRadioManagerSummary(NULL),
	  m_MFTFramework(NULL),
	  //TD18626, jianghp
	  m_radioCallback(NULL)
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
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_TEXTMSG_RECEIVED); //TD16926
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
	ON_MESSAGE(WM_TEXTMSG_RECEIVED, onNotifiedLaunchInboxPage)
	ON_MESSAGE(WM_APIFAILURE_MODE, onNotifiedApiFailureMode)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
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
	SetWindowPos(NULL, 0, 0, 1280, 813, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW );

	// implicitly create the singleton
	The_RadioOperations::instance();
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_LAUNCH_PATCH_DLG);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INITIALISE_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_TEXTMSG_RECEIVED); //TD16926
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

//TD18979 marc_bugfix
//TD18786

	if(!m_CRadioInitMsgDlg)
	{
		m_CRadioInitMsgDlg =new CRadioInitMsgDlg;
		m_CRadioInitMsgDlg->Create(IDD_RADIO_INITIALISE, this);
		m_CRadioInitMsgDlg->ShowWindow(SW_SHOW);

	}

//TD18786

//TD18626, jianghp
	// this must be constructed here
	
	TA_Base_Bus::IGUIAccess & guiAccess = this->getGenericGUICallbackObject();
	// create Transactive interfaces
	TA_Base_Core::IEntityDataPtr entityData(guiAccess.getGUIEntity());
	m_CtaRadioManagerSummary = new CtaRadioManagerSummary(entityData) ;
	
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

	//TD18626, jianghp
    TA_Base_Bus::ResizingProperties rp;
    rp.canMaximise = false;
	rp.maxWidth = 1280;
	rp.maxHeight = 1024;
	
    // The minimum allowed dimensions are initial (startup) dimensions
    CRect rc;
    GetClientRect(rc);
    rp.minWidth = rc.Width();
    rp.minHeight = rc.Height();
	
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
	}
	
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
			//The_RadioOperations::instance()->selectSpeaker(tgResource);
			return;
		}

		RadioResource agResource = The_RadioSystemStatus::instance()->getResourceByRID(resourceID,RR_ANNGROUP);
		if (agResource.type != RR_NIL)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioResource from TrainMap = %s ",radResource.alias.c_str());
			The_RadioOperations::instance()->entitySelectionChange(agResource, true);
			//The_RadioOperations::instance()->selectSpeaker(agResource);
		}		
		// TD18115
    }
	else if (name.compare(RESOURCE_PARAM) == 0)
	{
		SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
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
			std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_TALKGROUP);
			std::vector<RadioResource>::iterator iter = resourceList.begin();
			for (; iter!= resourceList.end(); ++iter)
			{
				if ( (*iter).id == (unsigned long) atol(resourceID) )
				{
					The_RadioOperations::instance()->entitySelectionChange(*iter, true);
				}
			}
		} else if (2 == (unsigned long) atol(resourceType)) {
			
			// driver call

			The_RadioOperations::instance()->makeDriverCall((unsigned long) atol(resourceID));

		}
	}
	else if (name.compare("SDS") == 0) // TD16926
	{
		The_RadioOperations::instance()->launchInboxPage();
	}

	

	FUNCTION_EXIT;

}

// peter.wong, disable toolbar buttons
//
//void CRadioManagerDlg::OnEmergencyModeToolbarButton()
//{
//    FUNCTION_ENTRY("OnEmergencyModeToolbarButton");
//
//    RadioResource& resource = m_CEntityOperationsAreaDlg->getCurrentResource();
//
//    if (resource.type != RR_TALKGROUP)
//    {
//        MessageBox("Can only begin emergency for talkgroup");
//        return;
//    }
//    
//    if (!checkRights(TA_Base_Bus::aca_RADIO_OUTGOING_EMERGENCY_CALL))
//    {
//        MessageBox("Not enough rights to make emergency call");
//        return;
//    }
//        
//        
//    The_RadioSystemStatus::instance()->beginEmergency(resource);
//
//	//++Fixed TD#14138  
//	TA_Base_Core::DescriptionParameters descParams;
//
//	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
//	descParams.push_back(&param1);
//	TA_Base_Core::NameValuePair param2("CallType","Emergency");
//	descParams.push_back(&param2);
//
//	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioMakePrivateCall,descParams,resource);
//	//--Fixed TD#14138  
//
//	FUNCTION_EXIT;
//}


// peter.wong, disable toolbar buttons
//
//void CRadioManagerDlg::OnDialPadToolbarButton() 
//{
//    FUNCTION_ENTRY("OnDialPadToolbarButton");
//
//    int maxDialLength = 10;
//    int maxHistoryLength = 10;
//
//    try
//    {
//        maxDialLength = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::DIALPAD_MAXNUMBER_LENGTH); 
//		maxHistoryLength = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::DIALPAD_HISTORY_LENGTH);
//    }
//    catch (...)
//    {
//    }
//
//	CGenericDialpadDlg dlg(this, maxDialLength, maxHistoryLength);
//    dlg.setRecentlyDialedList(&m_recentDialedList);
//
//    if (IDOK == dlg.DoModal())
//    {
//	    CString sNumber = dlg.userDialedNumber();
//	    
//	    if ( !sNumber.IsEmpty() )
//	    {
//		    RadioResource resource = The_RadioSystemStatus::instance()->getResourceBySSI(atoi(sNumber));
//
//		    if (resource.type != RR_NIL)
//		    {
//			    The_RadioOperations::instance()->entitySelectionChange(resource, true);
//		    }
//	    }	
//    }
//
//	FUNCTION_EXIT;
//}
//
//void CRadioManagerDlg::OnInboxToolbarButton() 
//{
//    FUNCTION_ENTRY("OnInboxToolbarButton");
//  
//	CTextMessagingDlg dlg("Radio Text", this);
//    dlg.DoModal();
//
//	FUNCTION_EXIT;
//}
//
//void CRadioManagerDlg::OnNewEntity() 
//{
//    FUNCTION_ENTRY("OnNewEntity");
//
//	CEntityDialog dlg(this);
//	dlg.DoModal();
//
//	FUNCTION_EXIT;
//}


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

	//try
	//{
	//	//m_CRadioMonitoringSheet->m_CRadioHistoryPage->setHistoryLimit(
	//	//	The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::HISTORY_LIMIT)); 
	//}
 //   catch(TA_Base_Core::TransactiveException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
 //   }
 //   catch (...)
 //   {
 //       LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for HistoryLimit");
 //   }

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
	bool bHasRights = false;
return true;
	if ( m_CtaRadioManagerSummary!= NULL )
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

    return bHasRights;
}

// peter.wong, disable toolbar buttons
//void CRadioManagerDlg::OnLocateSpeakerBtn() 
//{
//	if ( !The_RadioOperations::instance()->locateSelectSpeaker())
//		AfxMessageBox("No resource is currently selected");
//	
//}


LRESULT CRadioManagerDlg::onNotifiedEditMode(WPARAM wParam , LPARAM lParam)
{
    m_editMode = TRUE;
	enableControls(FALSE);
	return 0;
}


LRESULT CRadioManagerDlg::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
    m_editMode = FALSE;
	enableControls(TRUE);

	if(m_CRadioInitMsgDlg != NULL)
	{
		delete m_CRadioInitMsgDlg;
		m_CRadioInitMsgDlg = NULL;
	}

	return 0;
}

LRESULT CRadioManagerDlg::onNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM unused)
{
    if (m_editMode) return 0; // editing, no change

	// peter.wong, disable toolbar buttons
	//RadioResource* radioResource = (RadioResource*)pRadioResource;

	
	//if (radioResource->type != RR_TALKGROUP)
 //   {
 //       //m_emergencyBtn.EnableWindow(FALSE);
 //   }
 //   else
 //   {
 //       //m_emergencyBtn.EnableWindow(TRUE);
 //   }

    return 0;
}

LRESULT CRadioManagerDlg::onNotifiedApiFailureMode(WPARAM wParam, LPARAM lParam)
{
	//if (0 == wParam) {
	MessageBox("Unable to establish Connection with the Radio System", "Connection Error", MB_OK);
	//} else {
	//	MessageBox("Agent cannot be contacted","Connection Error", MB_OK);
	//}
	return 0;
}

void CRadioManagerDlg::enableControls(BOOL flag)
{
	// peter.wong, disable toolbar buttons
	//m_locateBtn.EnableWindow(flag);
	//m_dialpadBtn.EnableWindow(flag);
	//m_emergencyBtn.EnableWindow(flag);

	//m_inboxBtn.EnableWindow(flag);
 //   m_newEntityBtn.EnableWindow(flag);

	//if (flag)
 //   {
	//    if (!checkRights(TA_Base_Bus::aca_RADIO_PATCHGROUP) &&
	//        !checkRights(TA_Base_Bus::aca_RADIO_DYNAMICREGROUP) &&
 //   	    !checkRights(TA_Base_Bus::aca_RADIO_MULTISELECT) &&
 //           !checkRights(TA_Base_Bus::aca_RADIO_MULTISITE)) 
	//    {
 //           m_newEntityBtn.EnableWindow(FALSE);
	//    }

 //       RadioResource& resource = m_CEntityOperationsAreaDlg->getCurrentResource();
	//    if (resource.type != RR_TALKGROUP)
 //       {
 //           m_emergencyBtn.EnableWindow(FALSE);
 //       }
 //   }

}

/*
LRESULT CRadioManagerDlg::onNotifiedLaunchPatchDlg(WPARAM wParam, LPARAM lParam)
{
	if (wParam != 0)
	{
		if (dynamic_cast<CPatchManagerDlg*>(this->GetLastActivePopup()) == 0)
		{
			//TD18755 marc_bugfix Changed from SW_MINIMIZE to SW_HIDE
			this->ShowWindow(SW_HIDE);
			TA_IRS_Bus::TelephoneResource tResource;
			tResource.type = TA_IRS_Bus::TR_DLTS;
			tResource.number = wParam;

			CPatchManagerDlg dlg(CPatchManagerDlg::FROM_TELEPHONE,this);
			dlg.setTelephoneResource(tResource);
			dlg.DoModal();
		}
		else
		{
			MessageBox("Patch Manager dialog is already launched","Warning");
		}
		
	}

	return 1;
}
*/

LRESULT CRadioManagerDlg::onNotifiedInitialiseMode(WPARAM wParam, LPARAM lParam)
{
	if(!m_CRadioInitMsgDlg)
	{
		m_CRadioInitMsgDlg =new CRadioInitMsgDlg;
		m_CRadioInitMsgDlg->Create(IDD_RADIO_INITIALISE, this);
		m_CRadioInitMsgDlg->ShowWindow(SW_SHOW);
	}
	return 0;
}

void CRadioManagerDlg::OnClose()
{
	// TD17056
	// Don't want to shutdown Radio Manager, just hide it.
	int opt = AfxMessageBox("Do you want to close RadioManager?", MB_YESNO|MB_ICONEXCLAMATION);

//	if(opt == IDNO)
//	{
//		ShowWindow(SW_HIDE);
//	}
//	else
//	{
//		MessageBox("NO Incoming calls will be received by this MFT when Radio Manager is shutdown.","Warning");
//		TransActiveDialog::OnClose();
//	}
	if (opt == IDYES)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Return to previous dialog.");
		DestroyWindow();
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

	m_CRadioInitMsgDlg->DestroyWindow();

	

		//	CRadioMonitoringSheet *m_CRadioMonitoringSheet;

		//CRadioDirectoryPageDlg *m_CRadioDirectoryPageDlg;

		//CEntityDetailsAreaDlg *m_CEntityDetailsAreaDlg;

		//CEntityOperationsAreaDlg *m_CEntityOperationsAreaDlg;

		//CRadioManagerDlgStatusBar *m_CRadioManagerDlgStatusBar;
		//
		//CRadioInitMsgDlg *m_CRadioInitMsgDlg;

		//RadioMFTAgentFramework* m_MFTFramework;
	
	return TransActiveDialog::DestroyWindow();
	FUNCTION_EXIT;
}

void CRadioManagerDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == HIND_WINDOW_TIMER)
	{
		ShowWindow(SW_HIDE);
		KillTimer (nIDEvent);
	}
}

// TD16926
LRESULT CRadioManagerDlg::onNotifiedLaunchInboxPage(WPARAM wParam, LPARAM lParam)
{
	if ((dynamic_cast<CRadioInboxPage*>(this->GetLastActivePopup()) == 0) && (wParam ==1))
	{
		CTextMessagingDlg dlg("Radio Text", this);
		dlg.DoModal();	
	}
	return 0;
}
//TD18672 marc_bugfix
LRESULT CRadioManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case FOCUS:
		ShowWindow(SW_SHOW);
		return true;
	default:
		return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
	}
}
//TD18672 marc_bugfix