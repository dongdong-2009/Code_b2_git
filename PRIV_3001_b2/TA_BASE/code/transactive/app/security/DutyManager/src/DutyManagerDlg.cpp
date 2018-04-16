/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class represents the main Rihgts Manager dialog window.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int DUTY_MGR_MINIMUM_WIDTH(600);
    const int DUTY_MGR_MINIMUM_HEIGHT(400);
	const int DUTY_MGR_INITIALIZE_TIMER = 1;
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionKey;
using TA_Base_Bus::SessionId;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::ProfileKey;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::TransActiveDialog;

using TA_Base_Core::DebugUtil;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// DutyManagerDlg dialog

bool DutyManagerDlg::m_isMartixDataInited = false;

extern bool g_populated;
// jinhua++
UINT  DutyManagerDlg::initialiseSubsystemDutyGridThreadFun( LPVOID pParam  )
{	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] begin DutyManagerDlg::initialiseSubsystemDutyGridThreadFun");

	DutyManagerDlg* pDlg = reinterpret_cast <DutyManagerDlg*>( pParam );
	ASSERT( pDlg != NULL );
	// Initialise the contents of the duty data store. Tell it to ignore all 
	// information relating to system operators and non-physical subsystems,
	// as we do not want to display these.	
	BOOL rtnCode = FALSE;
	try
	{
		AfxGetApp()->DoWaitCursor(1);
		TA_Base_Bus::DutyDataStore::populateStaticData();
		rtnCode = TRUE;	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DutyDataStore::populateStaticData() finish");
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DutyDataStore::populateStaticData() failed!");
	}
	pDlg->PostMessage(WM_POPULATE_DATA, rtnCode, TRUE);

	if( rtnCode )
	{
		rtnCode = FALSE;
		try
		{
			AfxGetApp()->DoWaitCursor(1);
			TA_Base_Bus::DutyDataStore::populateDynamicData(); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "populateDynamicData() finish");
			rtnCode = TRUE;	
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "populateDynamicData failed!");
		}

		pDlg->PostMessage(WM_POPULATE_DATA, rtnCode, FALSE);
	}

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] end DutyManagerDlg::initialiseSubsystemDutyGridThreadFun");

	return 0;
}
// ++jinhua

UINT DutyManagerDlg::getSessionIdsAsync( LPVOID pParam )
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"getSessionIdsAsync begin");
	
	try
	{
		TA_Base_Bus::DutyDataStore::getSessionCache().getSessionIds();
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"getSessionIdsAsync failed!");
	}
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"getSessionIdsAsync end");
	return 0;
}

UINT DutyManagerDlg::initOperatorAccessFactoryAsync( LPVOID pParam  )
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"initOperatorAccessFactoryAsync begin");

	try
	{
		TA_Base_Core::OperatorAccessFactory::getInstanceNoInit().init();
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"initOperatorAccessFactoryAsync failed!");
	}

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"initOperatorAccessFactoryAsync end");
	return 0;
}
LRESULT DutyManagerDlg::OnPopulateData(WPARAM wParam, LPARAM lParam)
{
	BOOL rtnCode = (BOOL) wParam;
	BOOL typeCode = (BOOL) lParam;
	if(TRUE == rtnCode)
	{
		if ( TRUE == typeCode)
		{
			m_regionDutyPropertyPage->initialiseRegionDutyGridStatic();
			m_subsystemDutyPropertyPage->initialiseSubsystemDutyGridStatic();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "building region page static data finished");
			
			CString statusMsg;
			statusMsg.LoadString(IDS_SUBSYSTEM_STATUS);
			SendDlgItemMessage(IDC_STATUS_STATIC,WM_SETTEXT,0,(LPARAM)( (LPCSTR) statusMsg));
		}
		else
		{
			m_bridge->initialiseCurrentSession();
			// Register for subsystem duty add and remove messages
			TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
				TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);

			m_regionDutyPropertyPage->updateRegionDutyGrid();
			m_subsystemDutyPropertyPage->updateSubsystemDutyGrid();
			if ( m_showOtherSession && m_otherSesSubDutyPropertyPage != NULL )
			{
				this->getOhterSesSubDutyPropertyPage().initialiseOhterSessionDutyGrid();
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "building page dynamic data finished");

			SendDlgItemMessage(IDC_STATUS_STATIC,WM_SHOWWINDOW,SW_HIDE , 0);

			m_shownStatus = false;
			g_populated = true;
			m_isMartixDataInited = true;
		}

		this->positionControls();	
		
		//this->RedrawWindow();
		this->RedrawWindow(NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN);
		
		AfxGetApp()->DoWaitCursor(-1);

		m_regionDutyPropertyPage->redrawScrollBar();
	}
	else
	{
		std::stringstream messageInfo;
		messageInfo << "Failed to retrieve duty information. Please retry later. The dialog will be closed!";
		MessageBox::warning(messageInfo.str());

		ASSERT(::AfxGetMainWnd() != NULL);
		::AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}

	return 0;
}
DutyManagerDlg::DutyManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback) :
    TransActiveDialog(genericGUICallback, DutyManagerDlg::IDD), 
		m_bridge(NULL),
		//m_dutyPropertySheet(NULL),
		m_regionDutyPropertyPage(NULL),
		m_subsystemDutyPropertyPage(NULL),
		m_otherSesSubDutyPropertyPage(NULL),
		m_shownStatus(true)		
{
    FUNCTION_ENTRY("DutyManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback)");

	//{{AFX_DATA_INIT(DutyManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_DUTYMANAGER_ICON);

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxWidth = -1;
    properties.maxHeight = -1;
    properties.minWidth = DUTY_MGR_MINIMUM_WIDTH;
    properties.minHeight = DUTY_MGR_MINIMUM_HEIGHT;

    setResizingProperties(properties);

	// jinhua++
	// Register for subsystem duty add and remove messages
	//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
    //    TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);
	// ++jinhua

	//m_showOtherSessionStr = "CLOSE";
	m_showOtherSession = false;

	std::string showOtherSessionStr = TA_Base_Core::RunParams::getInstance().get(RPARAM_SHOWOTHERSES).c_str();

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"DutyManagerDlg::DutyManagerDlg(), showOtherSessionStr = %s",
		showOtherSessionStr.c_str());

	if ( showOtherSessionStr.compare("OPEN") == 0 )
	{
		m_showOtherSession = true;
	}

    FUNCTION_EXIT;
}


DutyManagerDlg::~DutyManagerDlg()
{
    FUNCTION_ENTRY("~DutyManagerDlg()");

	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

	delete m_bridge;
	m_bridge  = NULL;
    
	m_subsystemDutyPropertyPage->DestroyWindow();
	delete m_subsystemDutyPropertyPage;
	m_subsystemDutyPropertyPage = NULL;

	m_regionDutyPropertyPage->DestroyWindow();
	delete m_regionDutyPropertyPage;
	m_regionDutyPropertyPage = NULL;

/*
		m_dutyPropertySheet.DestroyWindow();
		delete m_dutyPropertySheet;
		m_dutyPropertySheet = NULL;
		*/
	
	FUNCTION_EXIT;
}

void DutyManagerDlg::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("DutyManagerDlg::receiveSpecialisedMessage");
    // Determine the type of duty update based on message type.
	if( 0 == TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage.getTypeKey().compare(message.messageTypeKey.in()) )
	{
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key locationKey = 0;
		
		if((message.messageState >>= locationKey) != 0)
		{
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						 "Received Duty Agent switch over message for location %d. Will clean the duty cache", 
						 locationKey );	
			
			PostMessage(WM_USER_RECEIVE_DUTY_AGENT_RECOVER_UPDATE, 0, locationKey);
			
		}
	}
    FUNCTION_EXIT;
}

LRESULT DutyManagerDlg::OnReceiveDutyAgentRecover(WPARAM wParam, LPARAM lParam)
{
	std::stringstream messageInfo;

	try
	{
		TA_Base_Core::ILocation* locattion = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(lParam);
		messageInfo << "the DutyAgent at location "<< locattion->getName();
		messageInfo << " is just recover from broken down. If you have some operation related to that location rights. Please close the Rights manager ane reopen it.";
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "can't get location info for location key %d", lParam);
		messageInfo << " One of the DutyAgent is just recover from broken down. If you have some operation related to the rights. Please close the Rihgts manager ane reopen it.";
	}

	MessageBox::info(messageInfo.str());
	return 0;
}


void DutyManagerDlg::init()
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] begin DutyManagerDlg::init");

	TA_ASSERT(this->m_hWnd != NULL, "the windows is not created yet!");
	// TD11122: Store the original title text.
    CString title;
    GetWindowText( title );
	std::string strTitle((LPCTSTR)title);
	
	if (this->m_bridge == NULL)
	{
		m_bridge =  new DutyManagerBridge(this, strTitle);
	}

	m_regionDutyPropertyPage =  new RegionDutyPropertyPage;
	m_subsystemDutyPropertyPage = new SubsystemDutyPropertyPage;
	if (m_showOtherSession)
	{
		m_otherSesSubDutyPropertyPage = new OtherSesSubDutyPropertyPage;
	}
	//m_regionDutyPropertyPage->Create(IDD_REGIONS_DLG);
	//m_subsystemDutyPropertyPage->Create(IDD_SUBSYSTEMS_DLG);
	// Initialise the tabs.
    m_dutyPropertySheet.AddPage(&(*m_regionDutyPropertyPage));
    m_dutyPropertySheet.AddPage(&(*m_subsystemDutyPropertyPage));
	//m_dutyPropertySheet = new DutyManagerPropertySheet;

	if (m_showOtherSession)
	{
		m_dutyPropertySheet.AddPage(&(*m_otherSesSubDutyPropertyPage));
	}
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] in DutyManagerDlg::init, after AddPage OtherSesSubDutyPropertyPage");

	m_dutyPropertySheet.Create(this, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	
    // Finally, position all the controls.
    //TD17082++
    positionControls();
    // positionControls();
	SetTimer(DUTY_MGR_INITIALIZE_TIMER,0, NULL);	
    //++TD17082
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyManagerDlg::init, after Settimer");
}

void DutyManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(DutyManagerDlg)
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// DutyManagerDlg message handlers

BEGIN_MESSAGE_MAP(DutyManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(DutyManagerDlg)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_HELP_DUTY_MGR,OnTransActiveHelp)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_RECEIVE_DUTY_AGENT_RECOVER_UPDATE, OnReceiveDutyAgentRecover)
	ON_MESSAGE(WM_POPULATE_DATA, OnPopulateData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL DutyManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog()");

	TransActiveDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialise current session details.
    //TD17082 - must be performed after onInitdialog to improve launch performance
    //initialiseCurrentSession();
	CWaitCursor waitCursor;	 
	CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
	statusCtrl->SetWindowText("checking the subsystem status...");	

	CRect windowRect;
	GetWindowRect(windowRect);
	ScreenToClient(windowRect);
	
	CRect statusBarRect;
	statusCtrl->GetWindowRect(statusBarRect);
	ScreenToClient(statusBarRect);
	statusBarRect.left = windowRect.left + 5;
	statusBarRect.right = windowRect.right - 9;
	statusBarRect.bottom = windowRect.bottom - 5;
	statusBarRect.top = statusBarRect.bottom - 20;
	statusCtrl->MoveWindow(statusBarRect);

	statusCtrl->ShowWindow(SW_SHOW);
	waitCursor.Restore();

    FUNCTION_EXIT;	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void DutyManagerDlg::OnTransActiveHelp()
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}
//TD17082
void DutyManagerDlg::OnTimer(UINT nIDEvent)
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"[CL-21875] begin DutyManagerDlg::OnTimer");

	TA_ASSERT(m_bridge != NULL, "the duty brige object is NULL");
	if (nIDEvent == DUTY_MGR_INITIALIZE_TIMER)
	{
		KillTimer(DUTY_MGR_INITIALIZE_TIMER);
		
		CString statusMsg;
		statusMsg.LoadString(IDS_LOCATION_STATUS);
		
		this->SendDlgItemMessage(IDC_STATUS_STATIC,WM_SETTEXT,0, (LPARAM)( (LPCSTR) statusMsg));

		TA_Base_Bus::DutyDataStore::setPopulateCondition(true, true, m_showOtherSession); 

		AfxBeginThread(initOperatorAccessFactoryAsync, this);

		AfxBeginThread( initialiseSubsystemDutyGridThreadFun, this );

		AfxBeginThread( getSessionIdsAsync, this);
	} 
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyManagerDlg::OnTimer");
}




void DutyManagerDlg::OnAppAbout()
{
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppAbout:1" );
	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppAbout:2" );
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void DutyManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

        // TD11803: A call to SendMessage was made when the window did not exist, for now just make sure
        // it won't generate an assertion and log the fact so it can be debugged further.
        if ( ::IsWindow(m_hWnd) )
        {
            SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempted to SendMessage when window does not exist." );
        }

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
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR DutyManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void DutyManagerDlg::OnSize(UINT nType, int cx, int cy) 
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnSize:1");
	TransActiveDialog::OnSize(nType, cx, cy);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnSize:2");
    positionControls();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnSize:3");
}


//TD17082
void DutyManagerDlg::positionControls()
{
	if (::IsWindow(m_dutyPropertySheet.GetSafeHwnd()))
    {
        CRect windowRect;
        GetWindowRect(windowRect);
        ScreenToClient(windowRect);

        CRect propSheetRect;
        m_dutyPropertySheet.GetWindowRect(propSheetRect);
        ScreenToClient(propSheetRect);
        propSheetRect.left = windowRect.left + 3;
        propSheetRect.top = windowRect.top + 35;
        propSheetRect.right = windowRect.right - 9;

        //TD17082 - check if status bar should be displayed or not
		if (m_shownStatus)
		{	
			CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
			CRect statusBarRect;
			statusCtrl->GetWindowRect(statusBarRect);
			ScreenToClient(statusBarRect);
			statusBarRect.left = windowRect.left + 5;
			statusBarRect.right = windowRect.right - 9;
			statusBarRect.bottom = windowRect.bottom - 5;
			statusBarRect.top = statusBarRect.bottom - 20;
			statusCtrl->MoveWindow(statusBarRect);
			propSheetRect.bottom = statusBarRect.top - 2;
		}
		else		
		{
			CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
			CRect statusBarRect;
			statusCtrl->GetWindowRect(statusBarRect);
			ScreenToClient(statusBarRect);

			statusBarRect.left = windowRect.left -10;
			statusBarRect.top = windowRect.top - 10;

			statusBarRect.right = statusBarRect.left + 5;
			statusBarRect.bottom = statusBarRect.top + 5;
			
			statusCtrl->MoveWindow(statusBarRect);

			propSheetRect.bottom = windowRect.bottom - 5;
			//propSheetRect.bottom = windowRect.bottom + 10;  // Not OK
			//propSheetRect.bottom = windowRect.bottom - 20;  // No effect
		}
        //++TD17082

        m_dutyPropertySheet.MoveWindow(propSheetRect);  // The prop sheet will size the pages
    }
}

void DutyManagerDlg::OnAppExit() 
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppExit:1");
	delete m_bridge;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppExit:2");
	m_bridge  = NULL;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppExit:3");
	DestroyWindow();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnAppExit:4");
}

void DutyManagerDlg::OnClose() 
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnClose:1");
	delete m_bridge;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnClose:2");
	m_bridge  = NULL;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnClose:3");
	DestroyWindow();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing :DutyManagerDlg::OnClose:4");

}


// jinhua++
SubsystemDutyPropertyPage& DutyManagerDlg::getSubsytemDutyPropertyPage()
{
	FUNCTION_ENTRY( "DutyManagerDlg::getSubsytemDutyPropertyPage" );
	FUNCTION_EXIT;
	return *m_subsystemDutyPropertyPage;
}
// ++jinhua

OtherSesSubDutyPropertyPage& DutyManagerDlg::getOhterSesSubDutyPropertyPage()
{
	FUNCTION_ENTRY("getOhterSesSubDutyPropertyPage");

	FUNCTION_EXIT;
	return *m_otherSesSubDutyPropertyPage;
	
}

