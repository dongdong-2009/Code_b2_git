// TrainBorneManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainBorneManager.h"
#include "TrainBorneManagerDlg.h"
#include "trainBorneCommsDlg.h"
#include "TrainDetailTest.h"
#include "TrainDvaVersionsPage.h"
#include "bus/generic_gui/src/applauncher.h"
#include "TBTrainSelectorGUI.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/data_access_interface/entity_access/src/TrainBorneManagerEntityData.h"
#include "bus/generic_gui/src/HelpLauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define SM_CXVIRTUALSCREEN      78		//jiangshengguang++
#define MINVALUE				2400	//jiangshengguang++
#define NUMSCREENS				3		//jiangshengguang++
const std::string SHOW_WINDOW = "ShowWindow";

//#include "bus/IDLTest/IDLTest.h"
// CAboutDlg dialog used for App About
namespace TA_IRS_App
{
//	typedef TA_Base_Core::NamedObject< TA_IRS_Bus::IIDLTest,
//										TA_IRS_Bus::IIDLTest_ptr,	
//									    TA_IRS_Bus::IIDLTest_var >		IDLNamedObject;

extern bool userClose;

enum TAB_PAGES
{
    PAGE_MAIN_DLG = 0,
    PAGE_TTIS_LIBRARY,
    PAGE_TRAIN_DVA_VERSION,
  //  PAGE_TRAIN_CONTROL_MANAGER,

    PAGE_COUNT
};

// class CAboutDlg : public CDialog
// {
// public:
// 	CAboutDlg();
// 
// // Dialog Data
// 	enum { IDD = IDD_ABOUTBOX };
// 
// 	protected:
// 	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
// 
// // Implementation
// protected:
// 	DECLARE_MESSAGE_MAP()
// };
// 
// CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
// {
// }
// 
// void CAboutDlg::DoDataExchange(CDataExchange* pDX)
// {
// 	CDialog::DoDataExchange(pDX);
// }

// BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
// END_MESSAGE_MAP()


// CTrainBorneManagerDlg dialog




CTrainBorneManagerDlg::CTrainBorneManagerDlg(TA_Base_Bus::IGUIAccess& controlClass)
	:TransActiveDialog(controlClass, CTrainBorneManagerDlg::IDD, NULL),
	m_trainBorneCommsDlg(NULL),
    m_pTrainDvaVersionPage(NULL),
	m_isLaunched(false),
	m_isAssignedBySchematic(false),
	m_libraryDownloadPage( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTrainBorneManagerDlg::~CTrainBorneManagerDlg()
{
    if ( NULL != m_trainBorneCommsDlg )
    {
        delete m_trainBorneCommsDlg;
        m_trainBorneCommsDlg = NULL;
    }

    if ( NULL != m_pTrainDvaVersionPage )
    {
        delete m_pTrainDvaVersionPage;
        m_pTrainDvaVersionPage = NULL;
    }

	if ( NULL != m_libraryDownloadPage )
	{
		delete m_libraryDownloadPage;
		m_libraryDownloadPage = NULL;
	}
}

void CTrainBorneManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TB_TRAINBORNE_GUI, m_TbTrainBorneGui);

}

BEGIN_MESSAGE_MAP(CTrainBorneManagerDlg, TransActiveDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TB_TRAINBORNE_GUI, OnTcnSelchangeTbTrainBorneGui)
	ON_MESSAGE(WM_TB_CHANGEPOS, OnTBChangePos)
	ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)   
	ON_BN_CLICKED(IDC_ABOUTBUTTON, OnAboutbutton)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTrainBorneManagerDlg message handlers

BOOL CTrainBorneManagerDlg::OnInitDialog()
{

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

	TransActiveDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
// 	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
// 	ASSERT(IDM_ABOUTBOX < 0xF000);
// 
// 	CMenu* pSysMenu = GetSystemMenu(FALSE);
// 	if (pSysMenu != NULL)
// 	{
// 		CString strAboutMenu;
// 		strAboutMenu.LoadString(IDS_ABOUTBOX);
// 		if (!strAboutMenu.IsEmpty())
// 		{
// 			pSysMenu->AppendMenu(MF_SEPARATOR);
// 			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
// 		}
// 	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SHOW_WINDOW.c_str());

	std::string showWindowValue = TA_Base_Core::RunParams::getInstance().get(SHOW_WINDOW.c_str());
	if( showWindowValue == "0")
		m_showCounter = 2;
	else
		m_showCounter = 0;

	RECT rect;
	m_TbTrainBorneGui.GetWindowRect(&rect);
	rect.left-=2;
	rect.top-=5;
	rect.right-=10;
	rect.bottom-=10;

	TCITEM item[PAGE_COUNT];
	item[PAGE_MAIN_DLG].pszText ="COMMS";
	item[PAGE_TTIS_LIBRARY].pszText ="TTIS Library Download/Upgrade";
	item[PAGE_TRAIN_DVA_VERSION].pszText ="Train DVA Version";
//	item[PAGE_TRAIN_CONTROL_MANAGER].pszText ="Train Control Manager";
	for(int i=0;i<PAGE_COUNT;i++)
	{
		item[i].mask = TCIF_TEXT;
		m_TbTrainBorneGui.InsertItem(i,&item[i]);
    }

	int BulkPeriod = 500;
	TA_Base_Core::TrainBorneManagerEntityData* trainBorneManagerEntityData = NULL; // TrainBorneManager configuration
	TA_Base_Core::IEntityData* guiEntity;
	guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( "TrainBorneManager" );
	if ( guiEntity != NULL)
	{
		trainBorneManagerEntityData = dynamic_cast<TA_Base_Core::TrainBorneManagerEntityData*>(guiEntity);
		if(trainBorneManagerEntityData != NULL)
		{
			BulkPeriod = trainBorneManagerEntityData->getBulkingPollPeriod();
		}
	}

	std::ostringstream ss;
	ss << BulkPeriod;
	TA_Base_Core::RunParams::getInstance().set("BulkPeriod", ss.str().c_str());

    m_pTrainDvaVersionPage = new TrainDvaVersionsPage();
    m_pTrainDvaVersionPage->Create(IDD_TRAIN_DVA_VERSIONS, this);
    m_pTrainDvaVersionPage->SetWindowPos(NULL,rect.left,rect.top+7,rect.right - rect.left,rect.bottom-rect.top,SWP_HIDEWINDOW);
    m_pTrainDvaVersionPage->postDialogCreate();

    m_trainBorneCommsDlg = new CTrainBorneCommsDlg(this);
    m_trainBorneCommsDlg->setTrainDvaVersionPage( m_pTrainDvaVersionPage );
	m_trainBorneCommsDlg->Create(IDD_TRAINBORNEMANAGER_COMMS_DIALOG,this);
	m_trainBorneCommsDlg->SetWindowPos(NULL,rect.left,rect.top+7,rect.right - rect.left,rect.bottom-rect.top,SWP_SHOWWINDOW);

	m_libraryDownloadPage = new CLibraryDownloadPage( this );
	m_libraryDownloadPage->Create(IDD_LIBRARY_DOWNLOAD, this);
	m_libraryDownloadPage->SetWindowPos( NULL,rect.left,rect.top+7,rect.right - rect.left,rect.bottom-rect.top,SWP_HIDEWINDOW );

	
	ModifyWindowPos();
	m_isLaunched = true;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// void CTrainBorneManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
// {
// 	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
// 	{
// 		CAboutDlg dlgAbout;
// 		dlgAbout.DoModal();
// 	}
// 	else
// 	{
// 		CDialog::OnSysCommand(nID, lParam);
// 	}
// }

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrainBorneManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialog::OnPaint();
	}
}

//Windows will send WM_NCPAINT for 2 times.
void CTrainBorneManagerDlg::OnNcPaint()
{
	if(m_showCounter > 0)
	{
		m_showCounter --;
		ShowWindow(SW_HIDE);
	}
	else
		CDialog::OnNcPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrainBorneManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTrainBorneManagerDlg::OnTcnSelchangeTbTrainBorneGui(NMHDR *pNMHDR, LRESULT *pResult)
{
    switch (m_TbTrainBorneGui.GetCurSel())
    {
    case PAGE_MAIN_DLG:
        m_trainBorneCommsDlg->ShowWindow(SW_SHOW);
		m_libraryDownloadPage->ShowWindow(SW_HIDE);
        m_pTrainDvaVersionPage->ShowWindow(SW_HIDE);
        break;
	case PAGE_TTIS_LIBRARY:		
		m_trainBorneCommsDlg->ShowWindow(SW_HIDE);
		m_libraryDownloadPage->ShowWindow(SW_SHOW);
		m_pTrainDvaVersionPage->ShowWindow(SW_HIDE);
		break;
    case PAGE_TRAIN_DVA_VERSION:
        m_trainBorneCommsDlg->ShowWindow(SW_HIDE);
		m_libraryDownloadPage->ShowWindow(SW_HIDE);
        m_pTrainDvaVersionPage->ShowWindow(SW_SHOW);
        break;
    default:
        break;
    }
}

void CTrainBorneManagerDlg::OnClose()
{
	this->ShowWindow(SW_HIDE);
//	userClose = true;
//	TransActiveDialog::DestroyWindow(); //to fix the performance 
}


void CTrainBorneManagerDlg::OnDestory()
{
	TransActiveDialog::OnDestroy();
}

LRESULT CTrainBorneManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY( "OnSetWindowPosition" );
	LRESULT result;

	if (m_isLaunched && /*m_isAssignedBySchematic && */(lParam == TA_Base_Bus::TA_GenericGui::REPOSITION))
	{
		//m_isAssignedBySchematic = false;
		result = TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION_NO_RESTORE);
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

LRESULT CTrainBorneManagerDlg::OnTBChangePos(WPARAM wParam, LPARAM lParam)
{
	//ModifyWindowPos();
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Testing Log - onRelaunchRadioManager"); 
	ShowWindow(SW_SHOW);
	//ModifyWindowPos();
	return 0l;
}

void CTrainBorneManagerDlg::OnAboutbutton() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
}

void CTrainBorneManagerDlg::ModifyWindowPos()
{
	int left,top,width,height;
	int screenWidth,currentScreen;
	int numScreens = 1;
	int x_pos;

	CPoint   pt;   
	GetCursorPos(&pt);   

	// Get schematic rect
	RECT boundary;
	try
	{
		boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
		TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
		pt.x);
	}
	catch(const TA_Base_Core::ApplicationException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ApplicationException", ex.what());
		return;
	}
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

}

void CTrainBorneManagerDlg::initializeWindow()
{
	CPoint   pt;   
	GetCursorPos(&pt);   
	RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
		TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
		pt.x);
	SetWindowPos(NULL, 0, 0, 1280, boundary.bottom-boundary.top, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW );
}

void CTrainBorneManagerDlg::onRunParamChange( const std::string& name, const std::string& paramvalue )
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Name = %s <=> Value = %s",name.c_str(),paramvalue.c_str());
	if (name.compare(SHOW_WINDOW) == 0)
	{
		int showCmd = atoi(paramvalue.c_str());
		if(!showCmd)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
			SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		return;
	}
}
}