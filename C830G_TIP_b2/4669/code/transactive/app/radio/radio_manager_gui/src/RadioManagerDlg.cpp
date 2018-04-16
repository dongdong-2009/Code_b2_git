/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// TestDialogsDlg.cpp : implementation file
//

#include "stdafx.h"

#include "CallStackCurrentPage.h"
#include "CurrentCall.h"
#include "DirectLineCache.h"
#include "PTTMessagePoster.h"
#include "RadioForwardDialog.h"
#include "RadioManager.h"
#include "RadioManagerDlg.h"
#include "SelectTextDialog.h"
#include "SelectedCall.h"
#include <afxpriv.h>

#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"

#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "bus/radio/RadioSearchPage/src/SearchPage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/IRadioEntityData.h"
#include "core/data_access_interface/entity_access/src/IRadioSessionEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/Hostname.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "UserInitThread.h"//liqiang++

#ifdef RUN_IN_DEBUG_MODE
    #include "RadioManagerTestData.h"
#endif

//using namespace TA_Base_Core::TA_Utilities;
using TA_Base_Bus::TransActiveDialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_USER_INIT (WM_USER + 1001)

// Constant character strings
//cw++
//const std::string groupCallTypeAcknowledged   = "Acknowledged";
//const std::string groupCallTypeUnacknowledged = "Unacknowledged";
const std::string groupCallTypeAcknowledged   = "Acknowledge";
const std::string groupCallTypeUnacknowledged = "Unacknowledge";
//TD17301 ++cw
const std::string groupCallTypeBroadcast      = "Broadcast";
const std::string groupCallTypeSingle	      = "Single";
const std::string customSDSMessageName        = "Custom";


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
    ID_LINE_FORWARD_MODE
};


namespace Local
{
    class SendTextConfirmationDialog : public CDialog
    {
    public:
        SendTextConfirmationDialog(const std::string& str) : CDialog(IDD_CONFIRM_TEXT_SEND, 0),  m_msg(str) {}

        virtual BOOL OnInitDialog( ) { 
            SetDlgItemText(IDC_TEXT_MESSAGE, m_msg.c_str()); 
            return CDialog::OnInitDialog(); 
        }

    private:
        std::string m_msg;
    };    

}

/////////////////////////////////////////////////////////////////////////////
// CTestDialogsDlg dialog

CRadioManagerDialog::CRadioManagerDialog(TA_Base_Bus::IGUIAccess& controlClass)
	: TransActiveDialog(controlClass,CRadioManagerDialog::IDD),
	m_trainSelectorGUI(NULL),
	m_trainSelector(NULL),
	m_locationKey(0),
	m_radioEntityKey(0),
	m_pttButton(NULL),
	m_pttMessagePoster(NULL),
    m_radioInFallback(false),
	m_initReady(false)
{
    FUNCTION_ENTRY( "CRadioManagerDialog" );

	//{{AFX_DATA_INIT(CRadioManagerDialog)
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_vctrThrdIDBuffer.clear();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	FUNCTION_EXIT;
}

CRadioManagerDialog::~CRadioManagerDialog()
{
    FUNCTION_ENTRY( "~CRadioManagerDialog" );

    try
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

	    delete m_pttButton;
	    m_pttButton = NULL;
	    delete m_pttMessagePoster;
	    m_pttMessagePoster = NULL;
		
		if (m_rightsLibrary != NULL)
        {
            delete m_rightsLibrary;
            m_rightsLibrary = NULL;
        }

    }
    SILENT_RM_CATCH_HANDLER("~CRadioManagerDlg");

	FUNCTION_EXIT;

}

void CRadioManagerDialog::init()
{
    FUNCTION_ENTRY( "init" );
	try
    {
		unsigned long consoleKey           = getConsoleKey();
		m_locationKey                      = getLocationKey(consoleKey);
		std::string radioEntityName        = getRadioEntityName(m_locationKey);		      	    //"Radio_OCC";
		std::string radioSessionEntityName = getRadioSessionEntityName(m_locationKey,consoleKey); //"RadioSession_OCC_MFT1";
        std::string radioDirectoryEntityName = getRadioDirectoryEntityName(m_locationKey);
		//liqiang++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Consolekey: %d, LocationKey: %d, RadioEntityName: %s, RadioSessionEntityName: %s,RadioDirectoryEntityName: %s", 
			consoleKey, m_locationKey, radioEntityName.c_str(), radioSessionEntityName.c_str(), radioDirectoryEntityName.c_str());
		
		//TD14261:
        m_ConsoleComPort = getRadioSessionConsoleComPort(m_locationKey, consoleKey);
		
		// Setup the rights library
		TA_Base_Bus::RightsLibraryFactory   rlf;
        m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
		TA_ASSERT(m_rightsLibrary!=NULL,"Rights library not created.");
		
		// Setup the radio and radio session named objects
		m_radio.setEntityName(radioEntityName,false);
		m_radioSession.setEntityName(radioSessionEntityName,false);
        m_radioDirectory.setEntityName(radioDirectoryEntityName,false);
		m_CallStack.setRadioEntityName(radioEntityName);
		m_CurrentCallList.setRadioName(radioEntityName);
		
        m_RadioFacilities.getDirectLinePage()->setRadioName(radioEntityName);
        updateSelectedArea();
    }
    catch (...)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UW_632016);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Error setting up Agent references");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
    }

	//EnableToolBar(true);
	//EnableStatusBar(true);

	// Create the link to the train agent, using the MFCTrainSelectorGUI callback framework
	std::vector<CWnd*> emptyVector; // construct with an empty vector - the actual watchers will be added later
	m_trainSelectorGUI = new TA_IRS_Bus::MFCTrainSelectorGUI(emptyVector,
		TRAIN_INBOUND_MSG,TRAIN_OUTBOUND_MSG,TRAIN_UPDATE_SELECTION_MSG);
	FUNCTION_EXIT;
        
}

void CRadioManagerDialog::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {
        TA_Base_Bus::IRadioCorbaDef::SessionUpdateNotification* update = 0;

        if ( ( message.messageState >>= update ) != 0)
	    {
            processSessionUpdate(*update);
        }
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::receiveSpecialisedMessage");

    FUNCTION_EXIT;
}

void CRadioManagerDialog::processSessionUpdate(const TA_Base_Bus::IRadioCorbaDef::SessionUpdateNotification& sessionUpdate)
{
    FUNCTION_ENTRY("processSessionUpdate");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[15644]New Session = %d",sessionUpdate.sessionRef);

	// update the radio session reference.
	long radioSessionRef = sessionUpdate.sessionRef;
	m_CurrentCallList.setSessionRef(radioSessionRef);
	m_CallStack.setRadioSessionReference(radioSessionRef);

    // check against the internal state
    // if different
    // set the internal state
    if (m_radioInFallback != sessionUpdate.inFallback)
    {
        m_radioInFallback = sessionUpdate.inFallback;

        // if we just left fallback
        if (false == m_radioInFallback)
        {
            // pop up a dialog
            PostMessage(WM_DISPLAY_FALLBACK_MESSAGE,0, (LPARAM)m_radioInFallback);
        }
    }

    FUNCTION_EXIT;
}

void CRadioManagerDialog::DoDataExchange(CDataExchange* pDX)
{
 	FUNCTION_ENTRY("DoDataExchange");
	TransActiveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioManagerDialog)
    DDX_Control(pDX, IDC_SA_TETRAID, m_tetraidCombo);
    DDX_Control(pDX, IDC_TEXTMESSAGE, m_SDSMessageCombo);
    DDX_Control(pDX, IDC_CURRENTCALLLIST, m_CurrentCallList);
    DDX_Control(pDX, IDC_CALL_STACK_TAB, m_CallStack);
    DDX_Control(pDX, IDC_RADIO_FACILITIES_TAB, m_RadioFacilities);
    DDX_Control(pDX, IDC_CALLTYPE, m_CallTypeCombo);
	DDX_Control(pDX, IDC_HOLD, m_holdButton);
	DDX_Control(pDX, IDC_RELEASE, m_releaseButton);
	DDX_Control(pDX, IDC_VOLUME_UP, m_volumeUpButton);
	DDX_Control(pDX, IDC_VOLUME_DOWN, m_volumeDownButton);
	DDX_Control(pDX, IDC_INCLUDE, m_includeButton);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}

BEGIN_MESSAGE_MAP(CRadioManagerDialog, TransActiveDialog)
	//{{AFX_MSG_MAP(CRadioManagerDialog)
	ON_COMMAND(IDM_UPDATEBUTTONS, onUpdateButtons)
	ON_BN_CLICKED(IDC_LINEFORWARD, OnLineforward)
    ON_BN_CLICKED(IDC_RELEASE, OnReleaseCall)
    ON_BN_CLICKED(IDC_ACCEPT, OnAcceptCall)
	ON_BN_CLICKED(IDC_INCLUDE, OnInclude)
	ON_BN_CLICKED(IDC_HOLD, OnHoldCall)
	ON_BN_CLICKED(IDC_CALL, OnCallButton)
	ON_BN_CLICKED(IDC_HELPBUTTON, onHelpRadioController)
    ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_TOOLBAR, onViewToolbar)
	ON_COMMAND(ID_VIEW_STATUSBAR, onViewStatusbar)
	ON_COMMAND(ID_HELP_ABOUTRADIOMANAGER, onHelpAboutRadioController)
    ON_CBN_EDITCHANGE(IDC_SA_TETRAID, onChangeSelectedAreaTetraID)
	ON_BN_CLICKED(IDC_EDIT_TEXT, onEditTextMessage)
	ON_CBN_SELCHANGE(IDC_TEXTMESSAGE, onSelectTextMessage)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND_TEXT, onSendText)
    ON_COMMAND(ID_HELP_RADIOMANAGERHELP, onHelpRadioController)
    ON_MESSAGE(REFRESH_DIALOG_MSG, OnRefreshDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
    ON_CBN_SELCHANGE(IDC_SA_TETRAID, OnSelchangeSaTetraid)
	ON_BN_CLICKED(IDC_VOLUME_UP, onVolumeUp)
	ON_BN_CLICKED(IDC_VOLUME_DOWN, onVolumeDown)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_USER_INIT, OnUserInit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(PTT_NOTIFY_MSG, onPTTNotification)
	ON_MESSAGE(CURRENT_CALL_UPDATED_MSG, onCurrentCallUpdate)
    ON_MESSAGE(WM_DISPLAY_SDS_MESSAGE, onDisplaySDSMessage)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_MESSAGE(WM_DISPLAY_FALLBACK_MESSAGE, onDisplayFallbackMessage)
	ON_MESSAGE(ADD_CALL_TO_HISTORY, OnAddCallToHistory)		//TD18283 add by hongzhi
	ON_MESSAGE(WM_KILLINITTHREAD_MESSAGE, OnKillInitThread) //liqiang++
END_MESSAGE_MAP()


BOOL CRadioManagerDialog::OnInitDialog()
{
 	FUNCTION_ENTRY("OnInitDialog");

    EnableToolTips(TRUE);

    CRect windowSize;
    GetWindowRect(&windowSize);
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = false;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    setResizingProperties(properties);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
    
	//TD18481, zhanghongzhi
	EnableToolBar(true);
	EnableStatusBar(true);
	
    // Set the resizing properties again now that we have loaded the toolbar and status bar
    GetWindowRect(&windowSize);
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    setResizingProperties(properties);

/*
            // Create the link to the train agent, using the MFCTrainSelectorGUI callback framework
            std::vector<CWnd*> emptyVector; // construct with an empty vector - the actual watchers will be added later
            m_trainSelectorGUI = new TA_IRS_Bus::MFCTrainSelectorGUI(emptyVector,
                TRAIN_INBOUND_MSG,TRAIN_OUTBOUND_MSG,TRAIN_UPDATE_SELECTION_MSG);*/
        

	// Call base method
    TransActiveDialog::OnInitDialog();
	
	EnableCloseMenu (false);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CRadioManagerDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
    FUNCTION_ENTRY( "OnShowWindow" );

    TransActiveDialog::OnShowWindow(bShow, nStatus);

    if (!m_initReady)
    {
        PostMessage(WM_USER_INIT, 0, 0);
    }

    FUNCTION_EXIT;
}


LRESULT CRadioManagerDialog::OnUserInit( WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnUserInit" );

	CWaitCursor cos;	
	UserInit();
	
	FUNCTION_EXIT;
	return 0;
}

void CRadioManagerDialog::UserInit()
{
	FUNCTION_ENTRY("CRadioManagerDialog::UserInit");
	//show the initialising dialog
	m_initReady = true;

	REBProgressManager mgr;

	mgr.Init(this->m_hWnd);	//
	mgr.EnableProgress(false);
	mgr.SetVisible(TRUE);	
	mgr.EnableCancel(FALSE);
	mgr.BeginProgressDialog();

	mgr.SetStaticText(0, "Initializing the Radio Manager...");

	setRadioSession( mgr );

	m_pttMessagePoster = new TA_IRS_App::PTTMessagePoster(*this,PTT_NOTIFY_MSG);
	TA_ASSERT(m_pttMessagePoster!=NULL,"Unable to create PTT message poster.");
	m_pttButton = new TA_IRS_Bus::PTTButton(*m_pttMessagePoster);
	TA_ASSERT(m_pttButton!=NULL,"Unable to create PTTButton instance.");
	
	UserInitThread* callStackInitThread = new UserInitThread(*this, UserInitThread::CALLSTACKINIT);	
	callStackInitThread->start();
	AddThreadToBuffer (GetCurrentThreadId ());

	UserInitThread* radioFacilitiesInitThread = new UserInitThread(*this, UserInitThread::RADIOFACILITITIESINIT);	
	radioFacilitiesInitThread->start();
	AddThreadToBuffer (GetCurrentThreadId ());	

	FUNCTION_EXIT;
}

//*******************
// Message Handlers *
//*******************

void CRadioManagerDialog::OnCallButton() 
{
 	FUNCTION_ENTRY("OnCallButton");
    CWaitCursor wait;

    CSelectedCall* selectedCall = CSelectedCall::GetInstance();

    TA_ASSERT( 0 != selectedCall, "Selected call is NULL" );

    CString    tetraID;
    CString    groupType;

    CString id = selectedCall->GetID();
    CString location = selectedCall->GetLocation();

    // first get the call type and priority
    TA_IRS_Bus::CallTypes::ECallType trainCallType;
    bool isFullDuplex=false;

    CString callTypeText;
    m_CallTypeCombo.GetWindowText( callTypeText );

    if ( callTypeText.IsEmpty() )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        CString description = "No";
        userMsg << description;
        CString errMsg = userMsg.constructMessage(IDS_UW_632021);
        RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);

        FUNCTION_EXIT;
        return;
    }

    if ( callTypeText == groupCallTypeSingle.c_str() )
    {
        trainCallType = TA_IRS_Bus::CallTypes::SingleCall;
        isFullDuplex = true;
    }
    else if ( callTypeText == groupCallTypeUnacknowledged.c_str() )
    {
        trainCallType = TA_IRS_Bus::CallTypes::GroupUnacknowledgeCall;
    }
    else if ( callTypeText == groupCallTypeAcknowledged.c_str() )
    {
        trainCallType = TA_IRS_Bus::CallTypes::GroupAcknowledgeCall;
    }
    else if ( callTypeText == groupCallTypeBroadcast.c_str() )
    {
        trainCallType = TA_IRS_Bus::CallTypes::GroupBroadcastCall;
    }
    else
    {
        // None of the above

        TA_Base_Bus::TransActiveMessage userMsg;
        CString description = "Invalid";
        userMsg << description;
        CString errMsg = userMsg.constructMessage(IDS_UW_632021);
        RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);

        FUNCTION_EXIT;
        return;
    }


    bool isEmergency = (IsDlgButtonChecked(IDC_EMERGENCYMODE)==BST_CHECKED);
    TA_IRS_Bus::CallTypes::ECallPriority trainCallPriority = ( ( true == isEmergency ) ?
                                                               TA_IRS_Bus::CallTypes::DefaultEmergencyPriority:
                                                               TA_IRS_Bus::CallTypes::DefaultNormalPriority );


    // If this is a hot line then check if we wish to continue
    if ( true == selectedCall->IsHotLine() )
    {
        selectedCall->setHotLine(false);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << id;
        CString errMsg = userMsg.constructMessage(IDS_UW_632020);

        if (RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg,MB_YESNO|MB_ICONQUESTION) != IDYES)
        {
            FUNCTION_EXIT;
            return;
        }
    }


    // this will check if this is a call involving any trains and if it is it will use the train agent to make the call
    // this includes making any DGNA
    if ( true == selectedCall->prepareTrainDriverCall( trainCallType, trainCallPriority ) )
    {
        // this means the call was handled by the train agent
        // the radio manager need not make any calls

        tetraID = selectedCall->getCorrectedTetraId();

        // save the call to database
        std::string hostName = TA_Base_Core::Hostname::getHostname();
        TA_Base_Core::RASelectedCallAccessFactory::getInstance().addTetraid( hostName, tetraID.GetBuffer(0), id.GetBuffer(0), location.GetBuffer(0) );

        // update the drop down list in the selected area.
        updateSelectedArea();

        // Add call to free call drop down list
        CSelectedCall::GetInstance()->saveFreeCall();

        // Automatically clear the emergency flag
        CheckDlgButton( IDC_EMERGENCYMODE, 0 );

        // now processing is done
        FUNCTION_EXIT;
        return;
    }

    // this is a radio call

    // check if it is a DGNA or a call to a TSI
    if ( true == selectedCall->isDynamicGroup() )
    {
        // make the DGNA
        tetraID = createDynamicGroupTSI();

        // log the TSI that resulted
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "CRadioManagerDialog::OnCallButton() tetraID = %s",
            tetraID.GetBuffer(0) );

        // check for success/failure
        if ( tetraID.IsEmpty())
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString action = "Dynamic Group";
            userMsg << action;
            CString errMsg = userMsg.constructMessage(IDS_UE_083001);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);

            FUNCTION_EXIT;
            return;
        }
    }
    // this is a single or static group TSI
    else
    {
        tetraID = selectedCall->getCorrectedTetraId();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "CRadioManagerDialog::OnCallButton(else) tetraID = %s", tetraID.GetBuffer(0) );
    }


    // save the call to database
    std::string hostName = TA_Base_Core::Hostname::getHostname();
    TA_Base_Core::RASelectedCallAccessFactory::getInstance().addTetraid( hostName, tetraID.GetBuffer(0), id.GetBuffer(0), location.GetBuffer(0) );

    // update the drop down list in the selected area.
    updateSelectedArea();

    // Add call to free call drop down list
    CSelectedCall::GetInstance()->saveFreeCall();

    // This is not a train call, convert to the radio CORBA datatypes
    TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType = TA_IRS_Bus::CallTypesConversion::convertToCorba( trainCallType );
    TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority priorityCorba = TA_IRS_Bus::CallTypesConversion::convertToCorba( trainCallPriority );

    // Initiate the call
    try
    {
        CORBA_CALL( m_radioSession, makeCall, ( tetraID,
                                                callType,
                                                isFullDuplex,
                                                priorityCorba,
                                                false,
                                                TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str(),
                                                false,
                                                0 ) );//limin
    }
    catch ( TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& ex )
    {
        std::string reason = "Error when attempting to make call. Reason: ";
        reason.append( ex.what.in() );
        LOG_EXCEPTION_CATCH( SourceInfo, "IRadioSessionCorbaDef::accessDeniedException", reason.c_str() );

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString error = "Error while";
		userMsg << error;
		CString errMsg = userMsg.constructMessage(IDS_UW_632022);
		/*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            "Error while attempting to make call",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }
    catch(TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& ex)
    {
        std::string reason = "Access denied when attempting to make call. Reason: " 
            + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "IRadioSessionCorbaDef::accessDeniedException",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UW_632023);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            "Not permitted to make this call",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }
    catch(CORBA::Exception& ex)
    {
        std::string reason = "Error when attempting to make call. Reason: " 
            + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString error = "Error while";
		userMsg << error;
		CString errMsg = userMsg.constructMessage(IDS_UW_632022);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            "Error while attempting to make call",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }
    catch(...)
    {
        std::string reason = "Error when attempting to make call.";
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString error = "Error while";
		userMsg << error;
		CString errMsg = userMsg.constructMessage(IDS_UW_632022);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            "Error while attempting to make call",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }

	// Automatically clear the emergency flag
	CheckDlgButton(IDC_EMERGENCYMODE,0);

	FUNCTION_EXIT;
}


void CRadioManagerDialog::OnLineforward() 
{
 	FUNCTION_ENTRY("OnLineforward");

    CRadioForwardDialog  radioForwardDialog;
    try
    {	    
	    if (IDCANCEL == radioForwardDialog.DoModal())
        {
            FUNCTION_EXIT;
            return;
        }
    }
    catch(...)
    {
        // Catch any exceptions that leak from the dialog (other wise they'll crash the viewer)
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught an exception that was not stopped by the forward dialog.");

        FUNCTION_EXIT;
        return;
    }

	// Setup the indicator panel
	bool enable = radioForwardDialog.IsForward();
	    
    m_wndStatusBar.SetForwardingText("LINE FORWARDED " + radioForwardDialog.getForwardAddress(), enable);

	FUNCTION_EXIT;
}


void CRadioManagerDialog::OnAcceptCall()
{
    FUNCTION_ENTRY( "OnAcceptCall" );
	// Forward to the current call stack page
	m_CallStack.GetCurrentCallStack()->OnAccept();
    FUNCTION_EXIT;
}


void CRadioManagerDialog::OnReleaseCall() 
{
 	FUNCTION_ENTRY("OnReleaseCall");

	if (isCurrentCallSelected())
	{
        // Release all selected calls
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();

        for (CCurrentCall::SelectedCalls::iterator itr = calls.begin(); itr != calls.end(); ++itr )
        {	
            unsigned long callID = *itr;
            
            try
            {
		        // Add to history stack
				//TD18283, commented by hongzhi, for History Call is added when a call is answered not when released
		        // addCallToHistory(callID); 
				//TD18283
				
                CORBA_CALL( m_radioSession, releaseCall, ( callID, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
            }
            catch (...)
            {
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString actionName = "release call";
				userMsg << actionName;
				CString errMsg = userMsg.constructMessage(IDS_UW_632006);
                /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to release call.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
				// ++ TD14164 
            }
        }
	}

	FUNCTION_EXIT;
}


void CRadioManagerDialog::addCallToHistory(unsigned long callID)
{
    FUNCTION_ENTRY( "addCallToHistory" );
	// Called from above and from Current call stack (text message)
  	m_CallStack.GetHistoryCallStack()->addCallToHistory(callID);
    FUNCTION_EXIT;
}



void CRadioManagerDialog::onHelpAboutRadioController() 
{
	FUNCTION_ENTRY("onHelpAboutRadioController");
	// Display the about dialog
    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();	
	FUNCTION_EXIT;
}


void CRadioManagerDialog::onHelpRadioController() 
{
	FUNCTION_ENTRY("onHelpRadioController");
	// Launch help
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

	FUNCTION_EXIT;
}


void CRadioManagerDialog::onEditTextMessage() 
{
 	FUNCTION_ENTRY("onEditTextMessage");

	CString messageName;
	CString newMessageName;

	// Set current text and message for dialog
	m_SDSMessageCombo.GetWindowText(messageName);

    if (messageName != CString(customSDSMessageName.c_str()))
    {
        newMessageName = messageName;
    }
    
	CSelectTextDialog dlg(this,
                            m_radioEntityKey,
                            newMessageName,
                            m_currentSDSMessageText.c_str(),
                            customSDSMessageName.c_str());

	// Run the dialog
    if (dlg.DoModal()==IDOK)
	{
		//  Get the new message text
		m_currentSDSMessageText = dlg.m_messageText;

		// Set selection to show pre-defined message
		if (dlg.m_isCustom)
		{
			messageName = customSDSMessageName.c_str();
		}
		else
		{
			messageName = dlg.m_messageName;
		}

	}


	// Update predefined message list (might have added a new message or deleted one)
	setupPreDefinedSDS();

	// TES #490 - perform exact string matching
    // Select appropriate entry in the list
    int iIndex = TA_Base_Bus::SelectionHelper::findStringInComboCaseSensitive(m_SDSMessageCombo, messageName.GetBuffer(0));

    if (iIndex >= 0)
    {
	    m_SDSMessageCombo.SetCurSel(iIndex);
    }

	// re-enable buttons
    onUpdateButtons();

	FUNCTION_EXIT;
}

void CRadioManagerDialog::onSelectTextMessage() 
{
 	FUNCTION_ENTRY("onSelectTextMessage");

	// Get the text associated with the selected pre-defined message
	CString text;
	GetDlgItemText(IDC_TEXTMESSAGE,text);
	if (text==customSDSMessageName.c_str())
	{
		m_currentSDSMessageText = "";
	}
	else
	{
		m_currentSDSMessageText = CSelectTextDialog::getPredefinedSDSText(text);
	}

	// Re-enable buttons
	onUpdateButtons();

	FUNCTION_EXIT;
}

void CRadioManagerDialog::onSendText() 
{
    FUNCTION_ENTRY( "onSendText" );
    bool sentOK = false;

    try
    {
    	// Get the destination for the message from selected area
    	CString tetraTSI   = CSelectedCall::GetInstance()->GetTetraID();

        CString messageName;
        m_SDSMessageCombo.GetWindowText(messageName);

        // TES #483        
        Local::SendTextConfirmationDialog warnDlg(m_currentSDSMessageText);

        if (IDCANCEL == warnDlg.DoModal())
        {
            // User aborted
            FUNCTION_EXIT;
            return;
        }
        CORBA_CALL( m_radioSession, sendTextMessage, ( tetraTSI,m_currentSDSMessageText.c_str(),
                                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
    	CString identifier = CSelectedCall::GetInstance()->GetID();
    	CString location   = CSelectedCall::GetInstance()->GetLocation();
    
    	m_CallStack.GetHistoryCallStack()->addCallToHistory(time(NULL),
    						  TA_Base_Core::IRACallStackHistory::OUTBOUND,
    						  TA_Base_Core::IRACallStackHistory::TEXT,
    						  (const char*)identifier,
    						  (const char*)location,
    						  (const char*)tetraTSI,
							  false); // td17294 ignore the emergency information of text message? 
		sentOK = true;
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::onSendText");

    if (!sentOK)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString description = "trying to send the text message";
		userMsg << description;
		CString errMsg = userMsg.constructMessage(IDS_UW_632015);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("An error occurred while trying to send the text message");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
    }
    FUNCTION_EXIT;
}


void CRadioManagerDialog::OnDestroy() 
{	
 	FUNCTION_ENTRY("OnDestroy");

    RadioManagerErrorHandler::getInstance().setPrimaryWindowHandle(0);

#ifdef RUN_IN_DEBUG_MODE
    TA_IRS_App::RadioManager::RadioManagerTestData::getInstance().cancelThread();
#endif

    CSelectedCall::removeInstance();
    // Close down the radio session when we leave the program
    TransActiveDialog::OnDestroy();
    
	FUNCTION_EXIT;
}

void CRadioManagerDialog::OnClose() 
{
 	FUNCTION_ENTRY("OnClose");
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();
	FUNCTION_EXIT;
}



/*
 * OnInclude
 *
 * This function will add the selected subscriber to the current call inprogress
 */
void CRadioManagerDialog::OnInclude() 
{
 	FUNCTION_ENTRY("OnInclude");

	if (isCurrentCallSelected())
	{
        // Get the selected subscriber and call id
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();

        for (CCurrentCall::SelectedCalls::iterator itr = calls.begin(); itr != calls.end(); ++itr )
        {	
            unsigned long callID = *itr;
                        
            try
            {
				// AR Pre-VTT
				// the old code was trying to include someone already in the call into the call
				//CString tetraTSI = CCurrentCall::GetInstance()->GetDestinationTetraID(callID);
				CString tetraTSI = CSelectedCall::GetInstance()->GetTetraID();
            
                // Add them to the current call
		        // This will result in a disconnect for this call ID 
		        // and a new call ID being supplied.
                CORBA_CALL( m_radioSession, includeSubscriber, ( callID,
                                                                 tetraTSI,
                                                                 TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
            }
            catch (...)
            {
                // UE-630005
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString errMsg = userMsg.constructMessage(IDS_UE_630005);
				/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to include user into call.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
				// TD14164 ++
            }
        }
	}

	FUNCTION_EXIT;
}

void CRadioManagerDialog::OnHoldCall() 
{
 	FUNCTION_ENTRY("OnHoldCall");

	if (isCurrentCallSelected())
	{
		// Get current call ids
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();

        for (CCurrentCall::SelectedCalls::iterator itr = calls.begin(); itr != calls.end(); ++itr )
        {	
            unsigned long callID = *itr;

    		// Place it on hold
            try
            {
                CORBA_CALL( m_radioSession, holdCall, ( callID,TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );
            }
            catch (...)
            {
                // UE-630002
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				UINT errMsg = userMsg.showMsgBox(IDS_UE_630002);
				/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to put call on hold.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to put call on hold.", errMsg);
				// TD14164 ++
            }
        }
	}

	FUNCTION_EXIT;
}


void CRadioManagerDialog::onVolumeUp() 
{
 	FUNCTION_ENTRY("onVolumeUp");

	if (isCurrentCallSelected())
	{
		// Get current call ids
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();

        for (CCurrentCall::SelectedCalls::iterator itr = calls.begin(); itr != calls.end(); ++itr )
        {	
            unsigned long callID = *itr;

		    // adjust the volume
            try
            {
                CORBA_CALL( m_radioSession, adjustCallVolume, ( callID, true, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );
            }
            catch (...)
            {
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString actionName = "adjust call volume";
				userMsg << actionName;
				CString errMsg = userMsg.constructMessage(IDS_UW_632006);
                /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to adjust call volume.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
				// ++ TD14164 
            }
        }
	}

	FUNCTION_EXIT;
	
}

void CRadioManagerDialog::onVolumeDown() 
{
 	FUNCTION_ENTRY("onVolumeDown");

	if (isCurrentCallSelected())
	{
		// Get current call id
		// Get current call ids
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();

        for (CCurrentCall::SelectedCalls::iterator itr = calls.begin(); itr != calls.end(); ++itr )
        {	
            unsigned long callID = *itr;

		    // Place it on hold
            try
            {
                CORBA_CALL( m_radioSession, adjustCallVolume, ( callID, false, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );
            }
            catch (...)
            {
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString actionName = "adjust call volume";
				userMsg << actionName;
				CString errMsg = userMsg.constructMessage(IDS_UW_632006);
                /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to adjust call volume.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
				// ++ TD14164 
            }
        }
	}

	FUNCTION_EXIT;
	
}


void CRadioManagerDialog::refreshAnswerToolbarButton()
{
    FUNCTION_ENTRY( "refreshAnswerToolbarButton" );
	bool enable = m_CallStack.GetCurrentCallStack()->getShouldEnableAnswerButton();
	m_toolBar.GetToolBarCtrl().EnableButton( IDC_ACCEPT, enable);
    FUNCTION_EXIT;
}


/*
 * 
 * This function will enable/disable the buttons on the dialog based
 * on the current call and selected area status
 *
 */
void CRadioManagerDialog::onUpdateButtons()
{
 	FUNCTION_ENTRY("onUpdateButtons");

    CString tetraId = CSelectedCall::GetInstance()->GetTetraID();
	bool enable = CSelectedCall::GetInstance()->isNonEmptyDynamicGroupSelected() || !tetraId.IsEmpty();

	if (enable)
	{
		enable = isFunctionPermitted(TA_Base_Bus::aca_RADIO_HOLD_CALL);
	}

	GetDlgItem(IDC_CALL)->EnableWindow(enable);
	m_toolBar.GetToolBarCtrl().EnableButton( IDC_CALL,enable);

	// Answer toolbar
	refreshAnswerToolbarButton();

	// Enable send text (If text specified) and edit text
	enable = isFunctionPermitted(TA_Base_Bus::aca_RADIO_SEND_TEXT);
	GetDlgItem(IDC_TEXTMESSAGE)->EnableWindow(enable);
	GetDlgItem(IDC_SEND_TEXT)->EnableWindow(strlen(m_currentSDSMessageText.c_str())>0 &&
											(!tetraId.IsEmpty())&&enable);
	if (enable)
	{
		enable = isFunctionPermitted(TA_Base_Bus::aca_RADIO_EDIT_TEXT);
	}
	GetDlgItem(IDC_EDIT_TEXT)->EnableWindow(enable);

	GetDlgItem(IDC_EMERGENCYMODE)->EnableWindow(isFunctionPermitted(TA_Base_Bus::aca_RADIO_MAKE_EMERGENCY_CALL));


	/*
	// Get the important decision fields
	bool isInCall              = CCurrentCall::GetInstance()->IsConnected();
	bool isCurrentCallSelected = CCurrentCall::GetInstance()->IsCallDisplayed();
	bool isSelectedCall        = CSelectedCall::GetInstance()->IsCallDisplayed();
	// bool isSelectedGroupCall   = isSelectedCall && (CSelectedCall::GetInstance()->IsGroup());
	bool isACallOnHold         = m_RadioSession->IsCallHeld();
	bool isThisCallOnHold      = isACallOnHold && (m_RadioSession->GetHeldCallID()==CCurrentCall::GetInstance()->GetCallID());

	// Setup the various buttons
	this->GetDlgItem(IDC_CALL)->EnableWindow(!isInCall&&isSelectedCall);
	m_toolBar.GetToolBarCtrl().EnableButton( IDC_CALL,!isInCall&&isSelectedCall);
	this->GetDlgItem(IDC_CALLTYPE)->EnableWindow(!isInCall&&isSelectedCall);
	this->GetDlgItem(IDC_EMERGENCYMODE)->EnableWindow(!isInCall&&isSelectedCall);

	//this->GetDlgItem(IDC_ACCEPT)->EnableWindow(!isInCall&&isCurrentCallSelected&&!isThisCallOnHold);
	//m_toolBar.GetToolBarCtrl().EnableButton( IDC_ACCEPT,!isInCall&&isCurrentCallSelected&&!isThisCallOnHold);

	this->GetDlgItem(IDC_RELEASE)->EnableWindow(isInCall);
	m_toolBar.GetToolBarCtrl().EnableButton( IDC_RELEASE,isInCall);

	//this->GetDlgItem(IDC_RECALL)->EnableWindow(!isInCall&&isACallOnHold);
	this->GetDlgItem(IDC_HOLD)->EnableWindow(isInCall&&!isACallOnHold);
	this->GetDlgItem(IDC_INCLUDE)->EnableWindow(isInCall&&isSelectedCall);
	//this->GetDlgItem(IDC_KILL)->EnableWindow(!isInCall&&isCurrentCallSelected);

	//this->GetDlgItem(IDC_FREECALL)->EnableWindow(!isInCall);
	//this->GetDlgItem(IDC_LINEFORWARD)->EnableWindow(!isInCall);

	// Only enable the call type if a group is selected 
	this->GetDlgItem(IDC_CALLTYPE)->EnableWindow(isSelectedCall  isSelectedGroupCall );

	// Can only enter direct dial if not in a call
	this->GetDlgItem(IDC_SA_TETRAID)->EnableWindow(!isInCall);

	// Can only send text if some is specified
	CString textMessage;
	GetDlgItemText(IDC_TEXTMESSAGE,textMessage);
	this->GetDlgItem(IDC_SEND_TEXT)->EnableWindow(!textMessage.IsEmpty());

	// Set up the status bar text
	if (isInCall)
	{
		m_wndStatusBar.SetWindowText("Call in progress");
	}
	else if (isACallOnHold)
	{
		m_wndStatusBar.SetWindowText("Call on hold");
	}
	else
	{
		m_wndStatusBar.SetWindowText("Ready");
	}

*/
	FUNCTION_EXIT;
}

afx_msg LRESULT CRadioManagerDialog::onPTTNotification(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onPTTNotification" );

    // Check if we are monitoring
    if (!m_pttButton->isMonitoring())
    {
        FUNCTION_EXIT;
        return 0;
    }


    // Get the notification event
	TA_IRS_App::PTTMessagePoster::PTTAction   action = static_cast<TA_IRS_App::PTTMessagePoster::PTTAction>(wParam);

	// Update the GUI element to indicate the action
	CWnd* pWnd;
	switch (action)
	{
		case TA_IRS_App::PTTMessagePoster::PTT_MONITORING:
		case TA_IRS_App::PTTMessagePoster::PTT_CEASE:
			pWnd = GetDlgItem(IDC_PTT_LISTENING_TEXT);
            if (pWnd!=NULL)
            {
                pWnd->EnableWindow(TRUE);
            }

            pWnd = GetDlgItem(IDC_PTT_TALKING_TEXT);
            if (pWnd!=NULL)
            {
                pWnd->EnableWindow(FALSE);
            }

            break;

        case TA_IRS_App::PTTMessagePoster::PTT_GRANTED:
            pWnd = GetDlgItem(IDC_PTT_LISTENING_TEXT);
            if (pWnd!=NULL)
            {
                pWnd->EnableWindow(FALSE);
            }

            pWnd = GetDlgItem(IDC_PTT_TALKING_TEXT);
            if (pWnd!=NULL)
            {
                pWnd->EnableWindow(TRUE);
            }

            break;

        default:
            // Ignore the rest.
            break;
    }

    FUNCTION_EXIT;
    return 0;
}


afx_msg LRESULT CRadioManagerDialog::onCurrentCallUpdate(WPARAM wParam, LPARAM lParam)
{
 	FUNCTION_ENTRY("onCurrentCallUpdate");

    try
    {
    	// See if PTT is required (full duplex can't have multiple selections, so
        // only need to inspect first selected call)
    	if (CCurrentCall::GetInstance()->IsHalfDuplexCallSelected())
    	{
    		// One (or more) half duplex calls selected
    		// Start the monitoring
    		if (!m_pttButton->isMonitoring())
    		{
                CWaitCursor wait;
    
    			// Display the PTT required message
    			showPTTControls(true);
    
                // the callID parameter is deprecated, so it shouldn't matter what we pass in..
    			m_pttButton->startMonitoring(CCurrentCall::GetInstance()->getSelectedCallID(),
    							TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str()
    							,m_ConsoleComPort); //m_pttCommPort);
    		}
    	}
    	else
    	{
            // Stop the monitoring
    		if (m_pttButton->isMonitoring())
    		{
                CWaitCursor wait;
    
    			m_pttButton->endMonitoring();
    
    			// Hide the PTT required message
    			showPTTControls(false);
    		}
    	}
    
    	// Enable the buttons if there is a selected items
    	bool isCurrentCall = CCurrentCall::GetInstance()->IsCallSelected();
    	m_releaseButton.EnableWindow(isCurrentCall);
    	m_holdButton.EnableWindow(isCurrentCall&&isFunctionPermitted(TA_Base_Bus::aca_RADIO_HOLD_CALL));
    	m_volumeUpButton.EnableWindow(isCurrentCall);			   	
    	m_volumeDownButton.EnableWindow(isCurrentCall);	

	 	//Only enable if a single call is selected and also a single call n the current call list
    	m_includeButton.EnableWindow((isCurrentCall)&&(!CCurrentCall::GetInstance()->IsHalfDuplexCallSelected())
    											   &&(isFunctionPermitted(TA_Base_Bus::aca_RADIO_INCLUDE_CALL))
												   &&(!CSelectedCall::GetInstance()->IsGroup()));
    
    	// Toolbars
    	m_toolBar.GetToolBarCtrl().EnableButton( IDC_RELEASE,isCurrentCall);
    
        CCurrentCall::SelectedCalls calls = CCurrentCall::GetInstance()->getSelectedCallIDs();
    
        // set of calls no longer in the selection set
        CCurrentCall::SelectedCalls noLongerSelected;
        // set of calls new to set                        
        CCurrentCall::SelectedCalls newlySelected;
    
		CCurrentCall::SelectedCalls::iterator itr = calls.begin();
		for (; itr != calls.end(); ++itr )
        {
            if (m_lastSelectedCallSet.end() == m_lastSelectedCallSet.find(*itr))
            {
                // Item only present in current selection
                newlySelected.insert(*itr);   
            }
        }
                    
        for (itr = m_lastSelectedCallSet.begin();
                        itr != m_lastSelectedCallSet.end();
                        ++itr )
        {
            if (calls.end() == calls.find(*itr))
            {
                // Item only present in old selection
                noLongerSelected.insert(*itr);   
            }
        }
        
       
    	// Deselect calls no longer selected
    	if (noLongerSelected.size() > 0)
    	{
            try
            {
                for (itr = noLongerSelected.begin();
                        itr != noLongerSelected.end();
                        ++itr )
                {
                    CWaitCursor wait;

                    CORBA_CALL( m_radioSession, deselectCall, ( *itr, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );
                }
            }
    		catch (...)
    		{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to deselect last call.");
//				// TD14164 ++
//				TA_Base_Bus::TransActiveMessage userMsg;
//				CString actionName = "deselect last call";
//				userMsg << actionName;
//				CString errMsg = userMsg.constructMessage(IDS_UW_632006);
//                /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to deselect last call.");*/
//				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
//				// ++ TD14164 
    		}
    	}
    
    	// Select new call(s)
    	if (newlySelected.size() > 0)
    	{
    		try
            {
                // Now select all calls new to the set
                for (CCurrentCall::SelectedCalls::iterator itr = newlySelected.begin();
                        itr != newlySelected.end();
                        ++itr )
                {
                    CWaitCursor wait;
                    CORBA_CALL( m_radioSession, selectCall, ( *itr,TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
                }
            }
    		catch (...)
    		{
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString actionName = "select call";
				userMsg << actionName;
				CString errMsg = userMsg.constructMessage(IDS_UW_632006);
                /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to select call.");*/
				RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
				// ++ TD14164 
    		}
    	}
    
        m_lastSelectedCallSet = calls;
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::onCurrentCallUpdate");

	FUNCTION_EXIT;
	return 0;
}


void CRadioManagerDialog::showPTTControls(bool areShown)
{
    FUNCTION_ENTRY( "showPTTControls" );

    int iState = SW_HIDE;

    if (areShown)
    {
        iState = SW_NORMAL;
    }

    CWnd* pWnd = GetDlgItem(IDC_PTT_REQUIRED_TEXT);
    if (pWnd!=NULL)
    {
        pWnd->ShowWindow(iState);
    }

    pWnd = GetDlgItem(IDC_PTT_LISTENING_TEXT);
    if (pWnd!=NULL)
    {
        pWnd->ShowWindow(iState);
    }

    pWnd = GetDlgItem(IDC_PTT_TALKING_TEXT);
    if (pWnd!=NULL)
    {
        pWnd->ShowWindow(iState);
    }

    pWnd = GetDlgItem(IDC_PTT_TALKING_RECTANGLE);
    if (pWnd!=NULL)
    {
        pWnd->ShowWindow(iState);
    }

    pWnd = GetDlgItem(IDC_PTT_LISTENING_RECTANGLE);
    if (pWnd!=NULL)
    {
        pWnd->ShowWindow(iState);
    }

    FUNCTION_EXIT;
}


void CRadioManagerDialog::onChangeSelectedAreaTetraID()
{
    FUNCTION_ENTRY( "onChangeSelectedAreaTetraID" );

    // Started to enter a new tetra ID so clear other fields
    GetDlgItem(IDC_SA_ID)->SetWindowText("");
    GetDlgItem(IDC_SA_LOCATION)->SetWindowText("");

    CString tetraId = CSelectedCall::GetInstance()->GetTetraID();
    CSelectedCall::GetInstance()->setCallDisplayed(!tetraId.IsEmpty());
    onUpdateButtons();
	FUNCTION_EXIT;
}



afx_msg LRESULT CRadioManagerDialog::OnRefreshDialog(WPARAM wParam,LPARAM lParam)
{
 	FUNCTION_ENTRY("OnRefreshDialog");

    try
    {
        // Notify cache of the refresh, it may need to refresh its cache data
        CDirectLineCache::GetInstance()->onRefreshNotificationReceived();
    
    	m_RadioFacilities.getDirectLinePage()->DisplayButtons();
    	m_RadioFacilities.getDirectLinePage()->SetupHotButtons();
    
        if (1 == wParam)
        {
            // If this refresh was caused by a comms notification, must update
            // the radio line configuration page as well
    	    m_RadioFacilities.getDirectLineConfigurePage()->DisplayButtons();
    	    m_RadioFacilities.getDirectLineConfigurePage()->SetupHotButtons();
        }
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::OnRefreshDialog");

	FUNCTION_EXIT;
    
    return 0;
}

//******************************
// Manage the tool bar display
//******************************

void CRadioManagerDialog::CreateToolbar()
{
 	FUNCTION_ENTRY("CreateToolbar");
	// Create toolbar at the top of the dialog window
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
							| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_MAINTOOLBAR))
	{
		m_toolBar.LoadToolBar(IDR_MAINTOOLBAR);
		TRACE0("Failed to create toolbar\n");
	}

	//TD18481, zhanghongzhi
	m_toolBar.GetToolBarCtrl().SetButtonSize( CSize( 69, 37 ) );
	// Add text to the buttons
	// Any exception from these functions will fall through and be caught by
	// the calling member
	AddTextToButton( IDC_LINEFORWARD, "Forward" );
	AddTextToButton( IDC_HELPBUTTON, "Help" );

	AddTextToButton( IDC_CALL,    "Call" );
	AddTextToButton( IDC_RELEASE, "End Call" );
	AddTextToButton( IDC_ACCEPT,  "Answer" );

	//m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex( ID_NOW ), TBBS_CHECKBOX );
	m_toolBar.GetToolBarCtrl().EnableButton( IDC_RELEASE,false);

	FUNCTION_EXIT;
}

void CRadioManagerDialog::AddTextToButton(UINT buttonId, CString buttonLabel )
{
 	FUNCTION_ENTRY("AddTextToButton");
	int index = m_toolBar.CommandToIndex( buttonId );
	if( index >=0 )
	{
		m_toolBar.SetButtonText( index, buttonLabel );
	}
	else
	{
		TA_ASSERT(false,"Unable to find button");
	}
	FUNCTION_EXIT;
}


int CRadioManagerDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 	FUNCTION_ENTRY("OnCreate");
	if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
	{
		FUNCTION_EXIT;
		return -1;
	}
	
	// Create the status bar
	if (!m_wndStatusBar.CreateEx( this, SBT_TOOLTIPS , //| SBARS_SIZEGRIP,
								  WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
								  AFX_IDW_TOOLBAR) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
	{
		TRACE0("Failed to create status bar\n");
		//FUNCTION_EXIT;
		//return -1;      // fail to create
	}

	m_wndStatusBar.GetStatusBarCtrl().SetTipText( 0, "Ready" );
	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),	SBPS_STRETCH, NULL );

//	OnSetMessageString(AFX_IDS_IDLEMESSAGE);

	// Disable the call forward pane
	m_wndStatusBar.SetPaneStyle(1,SBPS_DISABLED);

	// Create the toolbar
	CreateToolbar();

	// Calculate the size of the toolbar and status bar
	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);
	m_toolBarHeight   = rcClientNow.top;
	m_statusBarHeight = rcClientStart.bottom - rcClientNow.bottom;
	
    m_wndStatusBar.GetStatusBarCtrl().SetText(0, 1, SBT_OWNERDRAW);

	FUNCTION_EXIT;
	return 0;
}


void CRadioManagerDialog::onViewToolbar() 
{
 	FUNCTION_ENTRY("onViewToolbar");
	bool isVisible = (m_toolBar.IsWindowVisible()==TRUE);
	EnableToolBar(!isVisible);
	m_toolBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
 	GetMenu()->CheckMenuItem(ID_VIEW_TOOLBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
	RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
	FUNCTION_EXIT;
}



void CRadioManagerDialog::EnableToolBar(bool bEnable)
{
 	FUNCTION_ENTRY("EnableToolBar");
    CPoint ptOffset;

	ptOffset.x = 0;
	
	// Check for initial enabling
	if ((bEnable) && (!m_toolBar.IsWindowVisible()))
	{
		// Creating - so move all items down
		ptOffset.y = m_toolBarHeight;
	}
	else if ((!bEnable) && (m_toolBar.IsWindowVisible()))
	{
		// Removing so move all items up
		ptOffset.y = m_toolBarHeight * -1;
	}
	else
	{
		// No change
	    FUNCTION_EXIT;
		return;
	}

	// Move the child controls
    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while (pwndChild)
    {
		if (pwndChild!=&m_toolBar) //&&(pwndChild!=&m_wndStatusBar))
		{
			pwndChild->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			rcChild.OffsetRect(ptOffset);
			pwndChild->MoveWindow(rcChild, FALSE);
			//pwndChild->MoveWindow(rcChild, TRUE);
		}
		pwndChild = pwndChild->GetNextWindow();
    }

	// Adjust the dialog size
    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.bottom += ptOffset.y;
    MoveWindow(rcWindow, TRUE);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	FUNCTION_EXIT;
}

/////////////////////////////////////////////////////////////////////////////
// CModelessMain::OnToolTipText
//      Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
//      This code is based on CFrameWnd::OnToolTipText
BOOL CRadioManagerDialog::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
 	FUNCTION_ENTRY("OnToolTipText");
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
	{
		FUNCTION_EXIT;
		return FALSE;
	}

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
	}

    if (IDC_TEXTMESSAGE == nID)
    {
        strTipText = m_SDSMessageCombo.getToolTipText();
    }
    else
    {
	    if (nID != 0) // will be zero on a separator
	    {
    		AfxLoadString(nID, szFullText);
			    // this is the command id, not the button index
		    AfxExtractSubString(strTipText, szFullText, 1, '\n');
	    }
    }

#ifndef _UNICODE
    if (pNMHDR->code == TTN_NEEDTEXTA)
    {
        lstrcpyn(pTTTA->szText, strTipText,
            (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
    }

    else
    {
        _mbstowcsz(pTTTW->szText, strTipText,
            (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    }

#else
    if (pNMHDR->code == TTN_NEEDTEXTA)
    {
        _wcstombsz(pTTTA->szText, strTipText,
            (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
    }

    else
    {
        lstrcpyn(pTTTW->szText, strTipText,
            (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    }

#endif

	*pResult = 0;
	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	FUNCTION_EXIT;
	return TRUE;    // message was handled
}


// ******************************
// Status bar
//*******************************
void CRadioManagerDialog::onViewStatusbar() 
{
 	FUNCTION_ENTRY("onViewStatusbar");
	bool isVisible = (m_wndStatusBar.IsWindowVisible()==TRUE);

	m_wndStatusBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
	EnableStatusBar(!isVisible);
 	GetMenu()->CheckMenuItem(ID_VIEW_STATUSBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
	RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
	FUNCTION_EXIT;
}


void CRadioManagerDialog::EnableStatusBar(bool bEnable)
{
 	FUNCTION_ENTRY("EnableStatusBar");
    CPoint ptOffset;

	ptOffset.x = 0;
	
	// Check for initial enabling
	if ((bEnable) && (!m_wndStatusBar.IsWindowVisible()))
	{
		// Creating - so extend dialog
		ptOffset.y = m_statusBarHeight;
	}
	else if ((!bEnable) && (m_wndStatusBar.IsWindowVisible()))
	{
		// Removing so contract dialog
		ptOffset.y = m_statusBarHeight * -1;
	}
	else
	{
		// No change
		FUNCTION_EXIT;
		return;
	}

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.bottom += ptOffset.y;
    MoveWindow(rcWindow, TRUE);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	FUNCTION_EXIT;
}



TA_IRS_Bus::TrainSelector& CRadioManagerDialog::getTrainSelector()
{
    FUNCTION_ENTRY( "getTrainSelector" );

    if (NULL != m_trainSelector)
    {
        FUNCTION_EXIT;
        return *m_trainSelector;
    }
    else
    {
        TA_THROW(TA_Base_Core::TransactiveException("TrainSelector not defined"));
    }
}


TA_IRS_Bus::MFCTrainSelectorGUI& CRadioManagerDialog::getTrainSelectorGUI()
{
    FUNCTION_ENTRY( "getTrainSelectorGUI" );

    if (NULL != m_trainSelectorGUI)
    {
        FUNCTION_EXIT;
        return *m_trainSelectorGUI;
    }
    else
    {
        TA_THROW(TA_Base_Core::TransactiveException("TrainSelectorGUI not defined"));
    }
}


void CRadioManagerDialog::resetStationMode()
{
    FUNCTION_ENTRY( "resetStationMode" );

    if (NULL != m_trainSelector)
    {
        try
        {
            m_trainSelector->setStationMode(m_stationType,m_locationKey);
        }
        SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::resetStationMode");
    }

    FUNCTION_EXIT;
}


void CRadioManagerDialog::setDepotMode()
{
    FUNCTION_ENTRY( "setDepotMode" );

    if (NULL != m_trainSelector)
    {
        try
        {
            unsigned long depotLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey();
            m_trainSelector->setStationMode(TA_IRS_Bus::LocationCache::Depot,depotLocationKey);
        }
        SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::setDepotMode");
    }

    FUNCTION_EXIT;
}


//******************
// Private Methods *
//******************


CString CRadioManagerDialog::createDynamicGroupTSI()
{
    FUNCTION_ENTRY( "createDynamicGroupTSI" );

    if ( false == CSelectedCall::GetInstance()->isDynamicGroup() )
    {
        FUNCTION_EXIT;
        return "";
    }

    try
    {
        // Retrieve the dynamic group from the search page
        TA_IRS_Bus::CSearchPage* searchPage = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage();

        std::vector<CString> tsiList = searchPage->getDynamicGroupTSIs();

        TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList corbaTsiList;
        corbaTsiList.length(tsiList.size());

        for (unsigned int i = 0; i < tsiList.size(); ++i )
        {
            // Place in list which will be forwarded to agent
            corbaTsiList[i] = CORBA::string_dup( tsiList[i].GetBuffer(0) );
        }

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        CORBA::String_member groupId;
        CORBA_CALL_RETURN( groupId, getRadioSessionCorbaDef(), makeDynamicGroup, ( corbaTsiList, sessionId.c_str() ) );//limin

        FUNCTION_EXIT;
        return groupId.in();
    }
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)
    {
    }
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException&)
    {
    }
    catch (...)
    {
    }

    FUNCTION_EXIT;
    return "";
}

unsigned long CRadioManagerDialog::getConsoleKey()
{
 	FUNCTION_ENTRY("getConsoleKey");
	unsigned long consoleKey;

    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	consoleKey = console->getKey();
	delete console;

	FUNCTION_EXIT;
	return consoleKey;
}


unsigned long CRadioManagerDialog::getLocationKey(unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getLocationKey");
	unsigned long locationKey;

	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(consoleKey);
	locationKey = console->getLocation();
	delete console;

	FUNCTION_EXIT;
	return locationKey;
}



std::string CRadioManagerDialog::getRadioEntityName(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getRadioEntityName");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData*  entity;
	std::string   radioEntityName;

	// Should be just one radio entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioEntityData::getStaticType(),locationKey);
	TA_ASSERT(entityList.size()>0,"Radio entity not found for this location.");
	TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");
	entity = entityList[0];
	radioEntityName = entity->getName();
	m_radioEntityKey = entity->getKey();
	m_radioSubsystemKey = entity->getSubsystem();
	delete entity;

	FUNCTION_EXIT;
	return radioEntityName;
}

std::string CRadioManagerDialog::getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getRadioSessionEntityName");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IRadioSessionEntityData*  radioSessionEntity;
	std::string   radioSessionEntityName;
	bool entityFound = false;

	// Should be just one radio entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioSessionEntityData::getStaticType(),locationKey);
	TA_ASSERT(entityList.size()>0,"No radio session entities found for this location.");
	while (!entityList.empty())
	{
		radioSessionEntity = dynamic_cast<TA_Base_Core::IRadioSessionEntityData*>(entityList[0]);
		TA_ASSERT(radioSessionEntity,"Invalid entity type returned from RasioSession search.");
		if (radioSessionEntity->getAssociatedConsoleKey()==consoleKey)
		{
			if (entityFound)
			{
				TA_ASSERT(false,"Multiple radio sessions associated with this console.");
			}
			else
			{
				radioSessionEntityName = radioSessionEntity->getName();
				m_radioSessionEntityKey = radioSessionEntity->getKey();
				entityFound = true;
			}
		}

		entityList.erase(entityList.begin());

		delete radioSessionEntity;
	}
	TA_ASSERT(entityFound,"Radio session not found for this console.");

	FUNCTION_EXIT;
	return radioSessionEntityName;
}

std::string CRadioManagerDialog::getRadioSessionConsoleComPort(unsigned long locationKey,unsigned long consoleKey)
{
    FUNCTION_ENTRY( "getRadioSessionConsoleComPort" );
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IRadioSessionEntityData*  radioSessionEntity;
	std::string   radioSessionConsoleComPort;
	bool entityFound = false;

	// Should be just one radio entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioSessionEntityData::getStaticType(),locationKey);
	TA_ASSERT(entityList.size()>0,"No radio session entities found for this location.");
	while (!entityList.empty())
	{
		radioSessionEntity = dynamic_cast<TA_Base_Core::IRadioSessionEntityData*>(entityList[0]);
		TA_ASSERT(radioSessionEntity,"Invalid entity type returned from RasioSession search.");
		if (radioSessionEntity->getAssociatedConsoleKey()==consoleKey)
		{
			if (entityFound)
			{
				TA_ASSERT(false,"Multiple radio sessions associated with this console.");
			}
			else
			{
				radioSessionConsoleComPort = radioSessionEntity->getConsoleComPort();
				entityFound = true;
			}
		}

		entityList.erase(entityList.begin());

		delete radioSessionEntity;
	}
	TA_ASSERT(entityFound,"Radio session not found for this console.");

	FUNCTION_EXIT;
	return radioSessionConsoleComPort;
}


std::string CRadioManagerDialog::getRadioDirectoryEntityName(unsigned long locationKey)
{
    FUNCTION_ENTRY( "getRadioDirectoryEntityName" );

    std::vector<TA_Base_Core::IEntityData*> directoryList;
    TA_Base_Core::IEntityData*  entity;
    std::string   radioDirectoryEntityName;

    // Should be just one radio entity at each location
    directoryList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::RadioDirectoryEntityData::getStaticType());
    if (directoryList.size() != 1)
    {
        // TODO: Create specific exception
        TA_THROW(TA_Base_Core::TransactiveException("Incorrect number of Radio Directory entites found. There should only be one for the system."));
    }
    entity = directoryList[0];
    radioDirectoryEntityName = entity->getName();
    delete entity;

    FUNCTION_EXIT;
    return radioDirectoryEntityName;
}


void CRadioManagerDialog::setupCallTypes()
{
    FUNCTION_ENTRY( "setupCallTypes" );

    // Remove any existing choices
    m_CallTypeCombo.ResetContent();

    // Add the choices
    m_CallTypeCombo.AddString(groupCallTypeSingle.c_str());
    m_CallTypeCombo.AddString(groupCallTypeAcknowledged.c_str());
    m_CallTypeCombo.AddString(groupCallTypeUnacknowledged.c_str());
    m_CallTypeCombo.AddString(groupCallTypeBroadcast.c_str());

    // Make the default Single.
    m_CallTypeCombo.SetCurSel(0);

    FUNCTION_EXIT;
}


void CRadioManagerDialog::setupPreDefinedSDS()
{
    FUNCTION_ENTRY( "setupPreDefinedSDS" );

    CSelectTextDialog::loadPredefinedMessageCombo(m_SDSMessageCombo);
    m_SDSMessageCombo.AddString(customSDSMessageName.c_str());

    FUNCTION_EXIT;
}


bool CRadioManagerDialog::isCurrentCallSelected()
{
    FUNCTION_ENTRY( "isCurrentCallSelected" );

    if (!CCurrentCall::GetInstance()->IsCallSelected())
    {
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632024);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Call not selected in current communications area.");*/
        RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        // ++ TD14164
        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return true;
}


bool CRadioManagerDialog::isFunctionPermitted(TA_Base_Bus::AccessControlledAction action)
{
    FUNCTION_ENTRY( "isFunctionPermitted" );

    #ifdef RUN_RIGHTS_IN_DEBUG_MODE
    if (TA_Base_Core::RunParams::getInstance().isSet("debugmode"))
    {
        FUNCTION_EXIT;
        return true;
    }
#endif

    bool        isPermitted = false;
    static bool haveWarnedUser = false;
    std::string reason;

    try
    {
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_radioSessionEntityKey);
        unsigned long resourceID = resource->getKey();
        delete resource;

//        isPermitted =  m_rightsLibrary->isActionPermittedOnResource(sessionId,
//                                                              resourceID,
//                                                              action,
//                                                              reason,
//                                                              decisionModule);
        isPermitted = m_rightsLibrary->isActionPermittedOnResourceAndLocSub(sessionId,
                                                                resourceID,
                                                                m_locationKey,
                                                                m_radioSubsystemKey,
                                                                action,
                                                                reason,
                                                                decisionModule);

        haveWarnedUser = false;  // Working again now!!
    }
    catch (...)
    {
        // Error checking rights
        // Log the problem
        std::string messageText = "Unable to check access rights, reason: " + reason;
        LOG_EXCEPTION_CATCH(SourceInfo, "isFunctionPermitted Exception",reason.c_str());

        // Let the user know (but only the first time)
        if (!haveWarnedUser)
        {
            // TD14164 ++
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << reason;
            CString errMsg = userMsg.constructMessage(IDS_UW_632025);
            /*RadioManagerErrorHandler::getInstance().AfxMessageBox(messageText.c_str());*/
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
            // ++ TD14164

            haveWarnedUser = true;
        }
    }

    FUNCTION_EXIT;
    return isPermitted;
}


LRESULT CRadioManagerDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    LRESULT result = 0;

    if (WM_NOTIFY == message)
    {
        NMHDR FAR *tem=(NMHDR FAR *)lParam;
        if (TTN_NEEDTEXT == tem->code)
        {
            int h = 2;
        }
    }

    // We shouldn't get any exceptions at this level, but just in case something was missed
    try
    {
        result = TransActiveDialog::WindowProc(message, wParam, lParam);
    }
    SILENT_RM_CATCH_HANDLER("Top level RadioManagerDlg::WindowProc");

    FUNCTION_EXIT;
    return result;
}


LRESULT CRadioManagerDialog::onDisplaySDSMessage(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onDisplaySDSMessage" );

    unsigned long callID = static_cast<unsigned long>(wParam);

    try
    {
        // Instruct helper dialog to display a new SDS Message
        m_displaySDSMessage.displaySDSMessage(callID);
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::onDisplaySDSMessage");

    FUNCTION_EXIT;
    return 0;
}


LRESULT CRadioManagerDialog::onDisplayFallbackMessage(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onDisplayFallbackMessage" );

    bool fallback = static_cast<bool>(lParam);

    try
    {
        std::stringstream msg;
        msg << "Radio session has " << (fallback?"entered":"left") << " fallback mode" << std::ends;
        RadioManagerErrorHandler::getInstance().AfxMessageBox( msg.str().c_str() );
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::onDisplayFallbackMessage");

    FUNCTION_EXIT;
    return 0;
}


//TD18283 add by hongzhi
LRESULT CRadioManagerDialog::OnAddCallToHistory(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnAddCallToHistory" );

    unsigned long callID = static_cast<unsigned long>(wParam);

    try
    {
        // add call to history
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "OnAddCallToHistory");

        addCallToHistory(callID);
    }
    SILENT_RM_CATCH_HANDLER("CRadioManagerDialog::onAddCallToHistory");

    FUNCTION_EXIT;
    return 0;
}
//TD18283

//liqiang++
void CRadioManagerDialog::setRadioSession( REBProgressManager& mgr )
{
	int retry = 0;

	while ( retry < 5 )
	{
		try
		{
			long radioSessionRef = 0;
			CORBA_CALL_RETURN( radioSessionRef, m_radioSession, getSessionReference, ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin

			m_CurrentCallList.setSessionRef(radioSessionRef);
			m_CallStack.setRadioSessionReference(radioSessionRef);
			CORBA_CALL_RETURN( m_radioInFallback, m_radio, inFallback, () );//limin
			try
			{
				//[[modify by wuzhongyi
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Befor m_radioSession->getCallForwarding","");
				TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails_var Details;
				CORBA_CALL_RETURN( Details, m_radioSession, getCallForwarding, () );//limin
				bool enable(TA_Base_Bus::IRadioSessionCorbaDef::EForwardingType::NeverForward != Details->forwardAction);
				std::string textInfo = "LINE FORWARDED " + std::string(Details->destinationTSI);
				m_wndStatusBar.SetForwardingText(textInfo.c_str(), enable);		
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "end m_radioSession->getCallForwarding","");
				//]]
			}
			catch ( ... )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while trying to get forwarding details.");
			}

			retry = 5;
		}
		catch (const TA_Base_Bus::IRadioSessionCorbaDef::sessionNotInitialiseException&)
		{
			bool shouldExit = false;
			
			if ( retry == 0 )
			{
				mgr.SetStaticText( 0, "Session error. Trying to recover ... ");
			}
			else if ( retry >= 4 )
			{
				shouldExit = true;
			}

			try
			{
				CORBA_CALL( m_radioSession, sessionLogin, ( "", TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );
				Sleep( 3000 );
				retry ++;
			}
			catch ( ... )
			{
				shouldExit = true;
			}

			if ( shouldExit)
			{
				mgr.Exit();
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				CString description = "not yet initialised. Can not recover.";
				userMsg << description;
				CString errMsg = userMsg.constructMessage(IDS_UW_632017);

				if (RadioManagerErrorHandler::getInstance().AfxMessageBox(
							errMsg,
							NULL,
							MB_RETRYCANCEL)!=IDRETRY)
				// ++ TD14164 
				{
					// User wants to quit
					exit(0);
				}
			}
		}
		catch (const TA_Base_Core::ObjectResolutionException&)
		{
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString description = "not available";
			userMsg << description;
			CString errMsg = userMsg.constructMessage(IDS_UW_632017);
			/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Radio session not available.");*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 

			exit(0);
		}
		catch ( const CORBA::Exception&)
		{
			//[[wuzhongyi,CL-20674 because we translate the transactiveException to Not transactiveException 
			/*
			// The object is invalid

			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString error = "Corba connection error to";
			userMsg << error;
			CString errMsg = userMsg.constructMessage(IDS_UW_632018);*/
			/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Corba connection error to Radio Session.");*/
			//RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 
			MessageBox("Radio System  unable to establish communication with ISCS", "",MB_OK+MB_ICONINFORMATION);
			//]]
			exit(0);
		}	
		catch (...)
		{
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString error = "Unknown error encountered querying";
			userMsg << error;
			CString errMsg = userMsg.constructMessage(IDS_UW_632018);
			/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unknown error encountered querying Radio Session.");*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			exit(0);
		}
	}

	// subscribe to session messages
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
        TA_Base_Core::RadioComms::RadioSessionUpdateNofification, 
        this, 
        m_radioSessionEntityKey, 
        0, 
        m_locationKey, 
        NULL );

    try
    {
        m_stationType = TA_IRS_Bus::LocationCache::Station;
        unsigned long depotLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey();
        unsigned long occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();

        if (m_locationKey == depotLocationKey)
        {
            m_stationType = TA_IRS_Bus::LocationCache::Depot;
        }
        else if (m_locationKey == occLocationKey)
        {
            m_stationType = TA_IRS_Bus::LocationCache::InService;
        }
	}
	catch (...)
	{
		// The object is invalid
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "determine filtering for train selection lists";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to determine filtering for train selection lists.");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
	}

	setupCallTypes();
	setupPreDefinedSDS();
	onUpdateButtons();
	
	showPTTControls(false);
	
	try
	{
	     m_trainSelector = new TA_IRS_Bus::TrainSelector(*m_trainSelectorGUI,m_locationKey,m_stationType);
    }
    catch(TA_Base_Core::TransactiveException& ex)
    {
        std::string reason = "Non-CORBA error while attempting to establish communication with Train Agent (could be LocationAccessFactory related). Reason: ";
        reason += ex.what();
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException",reason.c_str());
//        
//		// TD14164 ++
//		TA_Base_Bus::TransActiveMessage userMsg;
//		CString errMsg = userMsg.constructMessage(IDS_UW_632019);
//		/*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//            "Unable to establish communication with Train System.\nTrain components will not be available.",
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);*/
//		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//			errMsg,
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);
//		// ++ TD14164 
    }
    catch(CORBA::Exception& ex)
    {
        std::string reason = "Error while attempting to establish communication with Train Agent. Reason: " 
            + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",reason.c_str());
//        
//		// TD14164 ++
//		TA_Base_Bus::TransActiveMessage userMsg;
//		CString errMsg = userMsg.constructMessage(IDS_UW_632019);
//		/*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//            "Unable to establish communication with Train System.\nTrain components will not be available.",
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);*/
//		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//            errMsg,
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);
//		// ++ TD14164 
    }
    catch(...)
    {
		std::string reason = "Unknown error while attempting to establish communication with Train Agent.";
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",reason.c_str());
//
//		// TD14164 ++
//		TA_Base_Bus::TransActiveMessage userMsg;
//		CString errMsg = userMsg.constructMessage(IDS_UW_632019);
//        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//            "Unable to establish communication with Train System.\nTrain components will not be available.",
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);*/
//		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, 
//            errMsg,
//            "Radio Manager", 
//            MB_OK | MB_ICONEXCLAMATION);
//		// ++ TD14164 
    }

    // load the call list. This is so it is definately loaded after the train list
    m_CurrentCallList.queueSubscriptions();

#ifdef RUN_IN_DEBUG_MODE
    long sessionRef;
    CORBA_CALL_RETURN( sessionRef, m_radioSession, getSessionReference,
                       ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
    TA_IRS_App::RadioManager::RadioManagerTestData::getInstance().beginThread(m_CurrentCallList, sessionRef);
#endif
           

    RadioManagerErrorHandler::getInstance().setPrimaryWindowHandle(m_hWnd);


}

LRESULT CRadioManagerDialog::OnKillInitThread(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("CRadioManagerDialog::OnKillInitThread");
	UserInitThread* initThread = reinterpret_cast<UserInitThread*>(wParam);
	
	long lThreadID = initThread->getCurrentThreadId();

	if(initThread)
	{
		initThread->terminateAndWait();
		delete initThread;
		initThread = NULL;
	}

	RemoveThreadToBuff(lThreadID);
	EnableCloseMenu(true);

	FUNCTION_EXIT;

	return 0;
}
//++liqiang
//TD18283
//td17293
void CRadioManagerDialog::updateSelectedArea()
{
    FUNCTION_ENTRY( "updateSelectedArea" );

    m_tetraidCombo.ResetContent();
    std::string hostName =TA_Base_Core::Hostname::getHostname();
    m_history = TA_Base_Core::RASelectedCallAccessFactory::getInstance().getList(hostName);

    for (unsigned int entry = 0; entry<m_history.size();entry++)
    {
        m_tetraidCombo.InsertString(0,m_history[entry]->getTetraid().c_str());
        m_tetraidCombo.SetCurSel(0);
    }

    FUNCTION_EXIT;
}


//td17293
void CRadioManagerDialog::OnSelchangeSaTetraid()
{
    FUNCTION_ENTRY( "OnSelchangeSaTetraid" );

    CString selectedCall;

    m_tetraidCombo.GetLBText(m_tetraidCombo.GetCurSel(),selectedCall);

    std::string id;
    std::string location;

        for (unsigned int entry = 0; entry<m_history.size();entry++)
        {
            if (selectedCall == m_history[entry]->getTetraid().c_str() )
            {
                id = m_history[entry]->getID().c_str() ;
                location = m_history[entry]->getLocation().c_str() ;
                break;
            }
        }

        GetDlgItem(IDC_SA_ID)->SetWindowText(id.c_str());
        GetDlgItem(IDC_SA_LOCATION)->SetWindowText(location.c_str());

    FUNCTION_EXIT;
}

void CRadioManagerDialog::EnableCloseMenu(const bool& bSetFlag)
{
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	
	UINT nMenuf = bSetFlag ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);

	if(pSysMenu)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, nMenuf);
	}

}

void CRadioManagerDialog::AddThreadToBuffer(const long & lThreadID)
{
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Set Thread ID : %d,Start Locking",lThreadID);
	
	TA_THREADGUARD ( m_threadBufLock );
	m_vctrThrdIDBuffer.push_back(lThreadID);
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Set Thread ID : %d,End Locking ",lThreadID);

}

void CRadioManagerDialog::RemoveThreadToBuff (const long & lThreadID)
{
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Remove Thread ID : %d,Start Locking",lThreadID);
	
	TA_THREADGUARD ( m_threadBufLock );
		
	for (int nCOunter = 0; nCOunter < m_vctrThrdIDBuffer.size(); nCOunter++)
	{
		if ( m_vctrThrdIDBuffer.at (nCOunter) == lThreadID)
		{
			m_vctrThrdIDBuffer.erase(m_vctrThrdIDBuffer.begin() + nCOunter);
			break;
		}
	}
		
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Remove Thread ID : %d,End Locking ",lThreadID);	
	
}