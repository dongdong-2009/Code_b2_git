// TrainBorneManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainBorneManager.h"
#include "TrainBorneCommsDlg.h"
#include "TrainDetailTest.h"

#include "TBTrainSelectorGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "core/utilities/src/DebugUtil.h"
#include "core\data_access_interface\entity_access\src\iconsole.h"
#include "core\data_access_interface\entity_access\src\consoleaccessfactory.h"
#include "app/trains/TrainBorneManager/LocationsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include "bus/IDLTest/IDLTest.h"
// CAboutDlg dialog used for App About
namespace TA_IRS_App
{


CTrainBorneCommsDlg::CTrainBorneCommsDlg(CWnd* pParent)
:CDialog(CTrainBorneCommsDlg::IDD, pParent)
	//TransActiveDialog(controlClass, CTrainBorneManagerDlg::IDD, NULL),
	,m_detailDlg(NULL)
	,m_trainSelectorGUI(NULL)
    ,m_trainSelector(NULL)
	,m_inboundTrainList(NULL)
	,m_outboundTrainList(NULL) 
	,m_bIsDraging(false)
	,m_trainListRecoveryThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_selTrainIdList.clear();
}

CTrainBorneCommsDlg::~CTrainBorneCommsDlg()
{
	if(NULL != m_trainListRecoveryThread)
	{
		m_trainListRecoveryThread->terminateAndWait();
		delete m_trainListRecoveryThread;
	}
}

void CTrainBorneCommsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TB_TRAINBORNE, m_TbTrainBorne);
	
	DDX_Control(pDX, IDC_BUTTONALLOUTBOUND, m_allOutboundButton);
    DDX_Control(pDX, IDC_BUTTONALLINBOUND, m_allInboundButton);
    DDX_Control(pDX, IDC_BUTTONCLEARALL, m_clearTrainSelectionButton);
    DDX_Control(pDX, IDC_BUTTONALLTRAINS, m_selectAllTrainsButton);
	//}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_STN_OUTBOUND_SELECTION_LST, getOutboundTrainList());
    DDX_Control(pDX, IDC_STN_INBOUND_SELECTION_LST, getInboundTrainList());
}

BEGIN_MESSAGE_MAP(CTrainBorneCommsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BU_APPLY, &CTrainBorneCommsDlg::OnBnClickedBuApply)
	ON_BN_CLICKED(IDC_BU_HELP, &CTrainBorneCommsDlg::OnBnClickedBuHelp)
	ON_BN_CLICKED(IDC_BU_CLOSE, &CTrainBorneCommsDlg::OnBnClickedBuClose)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TB_TRAINBORNE, &CTrainBorneCommsDlg::OnTcnSelchangeTbTrainBorne)
	ON_BN_CLICKED(IDC_BUTTONALLTRAINS, &CTrainBorneCommsDlg::OnButtonalltrains)
    ON_BN_CLICKED(IDC_BUTTONCLEARALL, &CTrainBorneCommsDlg::OnButtonclearall)
	ON_NOTIFY(NM_CLICK, IDC_STN_INBOUND_SELECTION_LST, &CTrainBorneCommsDlg::OnClickStnInboundSelectionLst)
    ON_NOTIFY(NM_CLICK, IDC_STN_OUTBOUND_SELECTION_LST, &CTrainBorneCommsDlg::OnClickStnOutboundSelectionLst)
	ON_BN_CLICKED(IDC_BUTTONALLOUTBOUND, &CTrainBorneCommsDlg::OnButtonalloutbound)
    ON_BN_CLICKED(IDC_BUTTONALLINBOUND, &CTrainBorneCommsDlg::OnButtonallinbound)

	ON_NOTIFY(LVN_BEGINDRAG, IDC_STN_INBOUND_SELECTION_LST, &CTrainBorneCommsDlg::OnLvnBegindragInboundList)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_STN_OUTBOUND_SELECTION_LST, &CTrainBorneCommsDlg::OnLvnBegindragOutboundList)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_TB_PREPARED, OnTbPrepared)
	ON_MESSAGE(WM_TB_WILLCLOSE, OnTbWillClosed)
	ON_MESSAGE(WM_TB_PROCESSSSTATUSCHANGED, OnTbProcessStatusChanged)
	ON_MESSAGE(WM_TB_CANAPPLY, OnTbCanApply)
	ON_MESSAGE(WM_TB_TRAIN_CTRL_UPDATE,OnTrainCtrlUpdate)
	ON_MESSAGE(TRAIN_INBOUND_MSG,OnUpdateInboundList)
	ON_MESSAGE(TRAIN_OUTBOUND_MSG,OnUpdateOutboundList)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTrainBorneManagerDlg message handlers

BOOL CTrainBorneCommsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	m_TrainBorneTabManager.setWndParent(this);
	RECT rect;
	m_TbTrainBorne.GetWindowRect(&rect);
	rect.left-=2;
	rect.top-=5;
	rect.right-=10;
	rect.bottom-=35;
	m_TrainBorneTabManager.setDisplayRect(rect);

	initialiseTrainSelector();
	LVCOLUMN col0,col1;
	m_inboundTrainList->GetColumn(0,&col0);
	m_inboundTrainList->GetColumn(1,&col1);
	//col0.fmt =LVCFMT_LEFT;
	int col0Width=m_inboundTrainList->GetColumnWidth(0);
	col0.pszText = "Location";
	col1.pszText ="TrainID/Service";
	m_inboundTrainList->SetColumnWidth(0,col0Width+40);
	m_inboundTrainList->SetColumn(0,&col0);
	m_inboundTrainList->SetColumn(1,&col1);
	m_outboundTrainList->SetColumnWidth(0,col0Width+40);
	m_outboundTrainList->SetColumn(0,&col0);
	m_outboundTrainList->SetColumn(1,&col1);
	TCITEM item[4];
	
	//item[0].pszText ="DisplayManager";
	item[0].pszText ="Traveller Information";
	item[1].pszText ="Public Address";
	item[2].pszText ="Video Surveillance";
	item[3].pszText ="Train Control";
	for(int i=0;i<4;i++)
	{
		item[i].mask = TCIF_TEXT;
		m_TbTrainBorne.InsertItem(i,&item[i]);
	}
	m_TrainBorneTabManager.setSelectorGUI(m_trainSelectorGUI);
	m_TrainBorneTabManager.setSelector(m_trainSelector);
	m_TrainBorneTabManager.PushTabItems();  

	//Check the location type if DEPOT
	TA_Base_Core::ILocation::ELocationType locationType = LocationsManager::instance().getLocationType();
	if (TA_Base_Core::ILocation::DPT == locationType )
	{
		//Initialize UI Depot Control
		initUIDepotCtrl();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// void CTrainBorneCommsDlg::OnSysCommand(UINT nID, LPARAM lParam)
// {
// 	CDialog::OnSysCommand(nID, lParam);
// }

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrainBorneCommsDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrainBorneCommsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTrainBorneCommsDlg::OnBnClickedBuApply()
{
	m_TrainBorneTabManager.DoApply();
/*	TA_Base_Core::CorbaNameList names = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
			"TVSSAgent", "OCC",true );
	IDLNamedObject obj;
	obj.setCorbaName(names[0]);
	try
    {
        obj->IDLTest(10,20);
    }
    catch ( TA_Base_Core::OperationModeException& )
    {
        obj.clear();
        obj->IDLTest(10,20);
    }
    catch ( CORBA::SystemException& ____ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ____ex ).c_str() );
        obj.clear();
        obj->IDLTest(10,20);
    }
	catch( const CORBA::Exception& cex )
    {
        std::string exceptionMsg( "Exception thrown while assigning monitor: " );
	        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

    }
    catch (...)
    {
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "...", "Unknown" );

    }*/
}

void CTrainBorneCommsDlg::OnBnClickedBuHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}

void CTrainBorneCommsDlg::OnBnClickedBuClose()
{
    CWnd* pParent = GetParent();
    if ( NULL != pParent )
    {
        pParent->PostMessage( WM_CLOSE );
    }
    else
    {
        PostMessage( WM_CLOSE );
    }
}

TA_IRS_Bus::TrainListCtrl& CTrainBorneCommsDlg::getInboundTrainList()
{
    FUNCTION_ENTRY( "getInboundTrainList" );

    if (0 == m_inboundTrainList)
    {
        m_inboundTrainList = new TA_IRS_Bus::TrainListCtrl("CTrainBorneCommsDlg.InboundTrainList");
    }

    FUNCTION_EXIT;
    return *m_inboundTrainList;
}

TA_IRS_Bus::TrainListCtrl& CTrainBorneCommsDlg::getOutboundTrainList()
{
    FUNCTION_ENTRY( "getOutboundTrainList" );

    if (0 == m_outboundTrainList)
    {
        m_outboundTrainList = new TA_IRS_Bus::TrainListCtrl("CTrainBorneCommsDlg.OutboundTrainList");
    }

    FUNCTION_EXIT;
    return *m_outboundTrainList;
}

void CTrainBorneCommsDlg::OnTcnSelchangeTbTrainBorne(NMHDR *pNMHDR, LRESULT *pResult)
{
	//Check the location type if DEPOT
	TA_Base_Core::ILocation::ELocationType locationType = LocationsManager::instance().getLocationType();
	
	//For OCC & Station
	if (TA_Base_Core::ILocation::DPT != locationType )
	{
		//if(m_TrainBorneTabManager.getCurAppName()==DISPLAY_MANAGER)
		if(m_TbTrainBorne.GetCurSel() == DISPLAY_TAB_ORDER)
		{
			GetDlgItem(IDC_BUTTONALLINBOUND)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONALLOUTBOUND)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONALLTRAINS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONCLEARALL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BU_APPLY)->EnableWindow(FALSE);
			m_inboundTrainList->deselectAll();
			m_outboundTrainList->deselectAll();
			m_inboundTrainList->ModifyStyle(0,LVS_SINGLESEL);
			m_outboundTrainList->ModifyStyle(0,LVS_SINGLESEL);
		}
		else
		{
			GetDlgItem(IDC_BUTTONALLINBOUND)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONALLOUTBOUND)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONALLTRAINS)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONCLEARALL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BU_APPLY)->EnableWindow(TRUE);
			m_inboundTrainList->ModifyStyle(LVS_SINGLESEL,0);
			m_outboundTrainList->ModifyStyle(LVS_SINGLESEL,0);
		}
	}
	//For Depot
	else
	{
		//Initialize UI Depot Control
		initUIDepotCtrl();
	}
	m_TrainBorneTabManager.TabSeleted(m_TbTrainBorne.GetCurSel());
}

void CTrainBorneCommsDlg::initialiseTrainSelector()
{
	m_trainSelectorGUI = new TBTrainSelectorGUI(&m_TrainBorneTabManager, 
                                                       m_pTrainDvaPage,
													   this,
													   TRAIN_INBOUND_MSG,
													   TRAIN_OUTBOUND_MSG,
													   getInboundTrainList(),  // pass all controls
                                                       &getOutboundTrainList(),
                                                       &m_allInboundButton,
                                                       &m_allOutboundButton,
                                                       &m_selectAllTrainsButton,
                                                       &m_clearTrainSelectionButton,
													   NULL,NULL,NULL,NULL,NULL,NULL,NULL
													   ); 
	//m_trainSelectorGUI = new TBTrainSelectorGUI(&m_TrainBorneTabManager, 
	//												   this,TRAIN_INBOUND_MSG,TRAIN_OUTBOUND_MSG,TRAIN_UPDATE_SELECTION_MSG);
    if (0 == m_trainSelector)
    {
        try
        {                
			TA_Base_Core::IConsole* console;
			std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
			console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
			unsigned long locationKey = console->getLocation();

			//Get the appropriate location type for Train selector
			LocationCache::EStationType locationType = LocationCache::getInstance().getAppropriateStationMode();

	        m_trainSelector = new TA_IRS_Bus::TrainSelector(*m_trainSelectorGUI,
                                            locationKey,locationType);
			m_trainSelectorGUI->setTrainSelector(m_trainSelector);

			m_trainSelectorGUI->setRights(true,false,false,false);
			
			if (TA_Base_Core::RunParams::getInstance().isSet("LocalTest"))
			{
				m_detailDlg= new TrainDetailTest(m_trainSelector);
				m_detailDlg->Create(IDD_TRAIN_DETAIL_TEST);
				m_detailDlg->ShowWindow(SW_SHOW);
			}


			//start train list recovery thread
			m_trainListRecoveryThread = new TrainListRecoveryThread(m_trainSelector);	
			m_trainListRecoveryThread->start();

        }
        catch (...)
        {

        }    
    }
}

void CTrainBorneCommsDlg::OnButtonalloutbound()
{
    FUNCTION_ENTRY( "OnButtonalloutbound" );

    m_trainSelectorGUI->selectAllOutbound();
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    FUNCTION_EXIT;
}


void CTrainBorneCommsDlg::OnButtonallinbound()
{
    FUNCTION_ENTRY( "OnButtonallinbound" );

    m_trainSelectorGUI->selectAllInbound();
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    FUNCTION_EXIT;
}

void CTrainBorneCommsDlg::OnButtonalltrains()
{
    FUNCTION_ENTRY( "OnButtonalltrains" );

    m_trainSelectorGUI->selectAllTrains();
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    FUNCTION_EXIT;
}


void CTrainBorneCommsDlg::OnButtonclearall()
{
    FUNCTION_ENTRY( "OnButtonclearall" );

    m_trainSelectorGUI->clearSelection();
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    FUNCTION_EXIT;
}

void CTrainBorneCommsDlg::OnClickStnInboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnClickStnInboundSelectionLst" );

    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;

    // If user clicks on inbound list without holding ctrl down, clear outbound list selection
	//TA_IRS_Bus::CommonTypes::TrainIdList list0= m_inboundTrainList->getSelectedTrainIds();
	    if (!(lpnmitem->uKeyFlags & LVKF_CONTROL))
    {
        getOutboundTrainList().deselectAll();
    }
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    *pResult = 0;
    FUNCTION_EXIT;
}


void CTrainBorneCommsDlg::OnClickStnOutboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnClickStnOutboundSelectionLst" );

    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;

    // If user clicks on outbound list without holding ctrl down, clear inbound list selection
	//TA_IRS_Bus::CommonTypes::TrainIdList list0= m_outboundTrainList->getSelectedTrainIds();

    // If user clicks on outbound list without holding ctrl down, clear inbound list selection
    if (!(lpnmitem->uKeyFlags & LVKF_CONTROL))
    {
        getInboundTrainList().deselectAll();
    }
	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
	m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
    *pResult = 0;
    FUNCTION_EXIT;
}

void CTrainBorneCommsDlg::OnLvnBegindragInboundList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMLV->iItem;
	POINT pt;
	pt.x=0; pt.y=0;
	m_imgDrag= m_inboundTrainList->CreateDragImage(index,&pt);
	m_imgDrag->BeginDrag(0, CPoint(0, 0));
	m_imgDrag->DragEnter(NULL,pNMLV->ptAction);
	m_strDragText=m_inboundTrainList->GetItemText(pNMLV->iItem,pNMLV->iSubItem);
	SetCapture();
	m_bIsDraging=true;
	*pResult = 0;
}

void CTrainBorneCommsDlg::OnLvnBegindragOutboundList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
		int index = pNMLV->iItem;
	POINT pt;
	pt.x=0; pt.y=0;
	m_imgDrag= m_outboundTrainList->CreateDragImage(index,&pt);
	m_imgDrag->BeginDrag(0, CPoint(0, 0));
	m_imgDrag->DragEnter(NULL,pNMLV->ptAction);
	m_strDragText=m_outboundTrainList->GetItemText(pNMLV->iItem,pNMLV->iSubItem);
	SetCapture();
	m_bIsDraging=true;
	*pResult = 0;
}

LRESULT CTrainBorneCommsDlg::OnTrainCtrlUpdate(WPARAM wParam,LPARAM lParam )
{
	switch(wParam)
	{
	case MNG_INBORNE_UPDATE:
	case MNG_OUTBORNE_UPDATE:
		if(m_selTrainIdList.size() != m_trainSelectorGUI->getSelectedTrains().size())
		{	m_selTrainIdList =m_trainSelectorGUI->getSelectedTrains();
			m_TrainBorneTabManager.TrainSelected(m_selTrainIdList);
		}
		break;
	default:
		break;
	}
	return 0l;
}

void CTrainBorneCommsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bIsDraging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_imgDrag->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_imgDrag->DragShowNolock(false);
		m_imgDrag->DragShowNolock(true);
		//if(DISPLAY_MANAGER==m_TrainBorneTabManager.getCurAppName())
		{
			ITrainBorne* tb=m_TrainBorneTabManager.getCurTrainBorne();
			if(NULL==tb)
				return;

			ClientToScreen(&point);
			//tb->TB_MouseAction(WM_MOUSEMOVE,nFlags,point);
			m_TrainBorneTabManager.MouseAction(WM_MOUSEMOVE,nFlags,point);
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CTrainBorneCommsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bIsDraging)
	{
		m_imgDrag->DragLeave(this);
		m_imgDrag->EndDrag();
		ReleaseCapture();
		m_bIsDraging = false;
		//RECT rect,rect1;
		//if(DISPLAY_MANAGER==m_TrainBorneTabManager.getCurAppName())
		//{
			ITrainBorne* tb=m_TrainBorneTabManager.getCurTrainBorne();
			if(NULL==tb)
				return;
			ClientToScreen(&point);
			m_TrainBorneTabManager.MouseAction(WM_LBUTTONUP,nFlags,point);
			//tb->TB_MouseAction(WM_LBUTTONUP,nFlags,point);
		//}
	}
	CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CTrainBorneCommsDlg::OnTbPrepared( WPARAM wParam, LPARAM lParam)
{
	return 0l;
}

LRESULT CTrainBorneCommsDlg::OnTbWillClosed( WPARAM wParam, LPARAM lParam)
{
	return 0l;
}

LRESULT CTrainBorneCommsDlg::OnTbProcessStatusChanged( WPARAM wParam, LPARAM lParam)
{
	return 0l;
}

LRESULT CTrainBorneCommsDlg::OnTbCanApply( WPARAM wParam, LPARAM lParam)
{
	std::string strName((char*)wParam);
	if(strName==m_TrainBorneTabManager.getCurAppName())
	{
		GetDlgItem(IDC_BU_APPLY)->EnableWindow((bool)lParam);
	}
	return 0l;
}

void CTrainBorneCommsDlg::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
	if (m_trainSelectorGUI != NULL)
    {
        m_trainSelectorGUI->setTrainSelector( NULL );
    }
	if(NULL!=m_trainSelector)
	{
		delete m_trainSelector;
		m_trainSelector = NULL;
	}
	if(NULL!=m_trainSelectorGUI)
	{
		delete m_trainSelectorGUI;
		m_trainSelectorGUI = NULL;
	}
	if(NULL!=m_inboundTrainList)
	{
		delete m_inboundTrainList;
		m_inboundTrainList=NULL;
	}
	if(NULL!=m_outboundTrainList)
	{
		delete m_outboundTrainList;
		m_outboundTrainList=NULL;
	}

	if(NULL!=m_detailDlg)
	{
		if(m_detailDlg->IsWindowVisible())
			m_detailDlg->ShowWindow(SW_HIDE);
		delete m_detailDlg;
		m_detailDlg=NULL;
	}
	CDialog::OnDestroy();
}

void CTrainBorneCommsDlg::setTrainDvaVersionPage( TrainDvaVersionsPage* pPage )
{
    m_pTrainDvaPage = pPage;
}

LRESULT CTrainBorneCommsDlg::OnUpdateInboundList(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("OnUpdateInboundList");

	TBTrainSelectorGUI::TrainListUpdate* update = reinterpret_cast<TBTrainSelectorGUI::TrainListUpdate*>(wParam);

	m_trainSelectorGUI->inboundListPostUpdate(update->action,update->trainDetails);

	delete update;

	FUNCTION_EXIT;
	return 0;
}

LRESULT CTrainBorneCommsDlg::OnUpdateOutboundList(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("OnUpdateOutboundList");

	TBTrainSelectorGUI::TrainListUpdate* update = reinterpret_cast<TBTrainSelectorGUI::TrainListUpdate*>(wParam);

	m_trainSelectorGUI->outboundListPostUpdate(update->action,update->trainDetails);

	delete update;

	FUNCTION_EXIT;
	return 0;
}

void CTrainBorneCommsDlg::initUIDepotCtrl()
{
	FUNCTION_ENTRY("initUIDepotCtrl");

	//Change the UI display for Trains list control
	//Hide some controls which is not necessary in Depot
	GetDlgItem( IDC_STN_INBOUND_SELECTION_LST )->EnableWindow(FALSE);
	GetDlgItem( IDC_STN_INBOUND_SELECTION_LST )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC_BB )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_STATIC_XB )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BUTTONALLINBOUND )->ShowWindow(SW_HIDE);
	GetDlgItem( IDC_BUTTONALLOUTBOUND )->ShowWindow(SW_HIDE);

	//Resize Outbound Window
	UINT flags = SWP_NOSENDCHANGING |SWP_NOACTIVATE | SWP_NOZORDER;
	GetDlgItem ( IDC_STN_OUTBOUND_SELECTION_LST )->SetWindowPos(NULL,35,28,393,638, flags);

 	FUNCTION_EXIT;
}

}