// PidManagerDialog.cpp : implementation file
//

#include "app/signs/pids_manager/src/stdafx.h"
#include "app/signs/pids_manager/src/PidManagerDialog.h"
#include "app/signs/pids_manager/src/PidAgentException.h"
#include "app/signs/pids_manager/src/GwxDisplayWrapper.h"
#include "app/signs/pids_manager/src/SchematicDecideModel.h"
#include "app/signs/pids_manager/src/UserMessages.h"
#include "app/signs/pids_manager/src/PidSelectionManager.h"
#include "app/signs/pids_manager/src/CachedConfig.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/utilities/src/DebugUtil.h"
//#include "core/data_access_interface/PidsManager/src/TiTrainHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include <boost/tokenizer.hpp>
#include <boost/assign.hpp>
#include <algorithm>
#include <iterator>

using namespace TA_Base_Bus;
using namespace TA_Base_Core;
using namespace TA_IRS_Core;
using namespace TA_IRS_App;
using namespace boost::assign;


namespace
{
    const char*		PID_MSG_ID = "ID";
    const char*	PID_MSG_DESCRIPTION = "消息描述";
    const char*	PID_MSG_DESCRIPTION_CANNOT_BE_NULL = "消息描述不能为空";
    const char*	PID_MSG_CONTENT_CANNOT_BE_NULL = "消息内容不能为空";
    const char*	MESSAGE_TEXT_UW_070024( "确认要发送消息吗？" );
    const char*    MESSAGE_CANCEL_TRAIN = "确认要取消播放车载消息吗？";
    const char*	MESSAGE_CANCEL_STATIONS = "确认要取消播放车站消息吗？";
    const char*	MESSAGE_CANCEL_ALL = "确认要取消播放全部消息吗？";
    const string	MESSAGE_ID_UE_070127  ( "UE_070127" );
    const string	MESSAGE_ID_UW_070024  ( "消息确认" );

    const string	PARAM_ALL       ( "ALL" );
    const string	PARAM_TOKEN     ( "," );
    const string	PARAM_SHOW      ( "SHOW" );
    const string	PARAM_DISPLAY   ( "Display" );
    const string	PARAM_SELECT    ( "Select" );
    const string	PARAM_DESELECT  ( "Deselect" );
    const string	PARAM_SYNCSELECT( "SynchroniseSelected" );
    const string	PARAM_DESELECTALL( "DeselectAll" ); // wss

    const MessageTime TIME_FOR_ONCEOFF = { -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 };

    enum
    {
        CHECK_BOX_UNCHECKED = 0,
        CHECK_BOX_CHECKED = 1,
        NORMAL_MESSAGE = 0,
        EMERGENCY_MESSAGE = 1,
        SHOW_BOTTOM = 0,
        SHOW_FULL_SCREEN = 1,
        WM_APP_RPARAM_SESSIONID_CHANGED = WM_APP + 1,
        WM_APP_RPARAM_VIEW_CURRENT_MSG  = WM_APP + 5,
        WM_APP_RPARAM_CLOSE_VIEW_CURRENT_MSG  = WM_APP + 6
    };
    std::vector<std::string> tokenizeString( std::string theString, const std::string& separatorList )
    {
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens( theString, sep );

        for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
        {
            parts.push_back( *tok_iter );
        }

        // if parts is empty, then this should return the entire string
        if ( parts.size() == 0 )
        {
            parts.push_back( theString );
        }

        return parts;
    }

	const int MAX_ADHOC_MSG_LENGTH = 250;
}

namespace TA_IRS_App
{
// CPidManagerDialog dialog

IMPLEMENT_DYNAMIC( CPidManagerDialog, TransActiveDialog )
CPidManagerDialog* CPidManagerDialog::m_pidsDlgInstance = NULL;
CPidManagerDialog* CPidManagerDialog::getPidDlgInstance()
{
    return m_pidsDlgInstance;
}

CPidManagerDialog::CPidManagerDialog( TA_Base_Bus::IGUIAccess* controlClass )
        : TransActiveDialog( *controlClass, IDD_PID_MANAGER_DIALOG, NULL ),
        m_timerId( 0 ),
        m_isChanged( false ),
		m_atsTrainDataPoller(CachedConfig::instance().getAtsTrainListPollingInterval(), TrainList::instance())
{
    FUNCTION_ENTRY( "constructor" );
    // register command-line params that the manager will process
    RunParams::getInstance().registerRunParamUser( this, RPARAM_SESSIONID );
//    RunParams::getInstance().registerRunParamUser(this, PARAM_DISPLAY.c_str());
    RunParams::getInstance().registerRunParamUser( this, PARAM_SELECT.c_str() );
    RunParams::getInstance().registerRunParamUser( this, PARAM_DESELECT.c_str() );
    RunParams::getInstance().registerRunParamUser( this, PARAM_SYNCSELECT.c_str() );
    RunParams::getInstance().registerRunParamUser( this, PARAM_DESELECTALL.c_str() ); // wss

    PIDSelectionManager::getInstance()->setTrainSel( false );

    m_pidsDlgInstance = this;
    FUNCTION_EXIT;

}

CPidManagerDialog::~CPidManagerDialog()
{
    FUNCTION_ENTRY( "destructor" );
    m_pidMessages.clear();
    StationPidUpdater::removeInstance();
    FUNCTION_EXIT;
}

void CPidManagerDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_CHECK_MSG_CYCLIC, m_pCyclicCheckBox );
	DDX_Control( pDX, IDC_DATETIMEPICKER_STARTTIME, m_pCyclicStartTime );
	DDX_Control( pDX, IDC_DATETIMEPICKER_ENDTIME, m_pCyclicEndTime );
	//DDX_Control(pDX, IDC_EDIT_TIME_PERIOD, m_pCyclicPeriod);
	DDX_Control( pDX, IDC_DATETIMEPICKER_PERIOD, m_pCyclicPeriod );
	DDX_Control( pDX, IDC_RADIO_SHOW_FULLSCREEN, m_pShowFullScreen );
	DDX_Control( pDX, IDC_RADIO_SHOW_BOTTOMSLIDE, m_pShowBottom );
	DDX_Control( pDX, IDC_CHECK_MSG_EMERGENCY, m_pEmergencyCheckBox );
	DDX_Control( pDX, IDC_EDIT_MSG_CONTENT, m_pMsgContent );
	DDX_Control( pDX, IDC_EDIT_MSG_TITLE, m_pMsgTitle );
	DDX_Control( pDX, IDC_LIST_PID_MSG, m_pMsgListCtrl );
	DDX_Control(pDX, IDC_COMBO_TRAIN_SELECTION, m_trainList);
}


BEGIN_MESSAGE_MAP( CPidManagerDialog, TransActiveDialog )
    //ON_MESSAGE(WM_APP_RPARAM_SESSIONID_CHANGED, evaluateRights)
	ON_BN_CLICKED(IDC_BUTTON_HELP, &CPidManagerDialog::OnBnClickHelp )
    ON_BN_CLICKED( IDC_CHECK_MSG_CYCLIC, &CPidManagerDialog::OnBnClickedCheckMsgCyclic )
    //	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME_PERIOD, &CPidManagerDialog::OnDeltaposSpinTimePeriod)
    ON_BN_CLICKED( IDC_BUTTON_EDIT_MSG, &CPidManagerDialog::OnBnClickedButtonEditMsg )
    //	ON_NOTIFY(HDN_ITEMCLICK, 0, &CPidManagerDialog::OnHdnItemclickListPidMsg)
    ON_BN_CLICKED( IDC_BUTTON_MSG_CANCEL, &CPidManagerDialog::OnBnClickedButtonMsgCancel )
    ON_BN_CLICKED( IDC_BUTTON_MSG_SAVE, &CPidManagerDialog::OnBnClickedButtonMsgSave )
    ON_NOTIFY( NM_CLICK, IDC_LIST_PID_MSG, &CPidManagerDialog::OnNMClickListPidMsg )
    ON_BN_CLICKED( IDC_BUTTON_CANCEL_ALL, &CPidManagerDialog::OnBnClickedButtonCancelAll )
    ON_BN_CLICKED( IDC_BUTTON_CANCEL_TRAIN, &CPidManagerDialog::OnBnClickedButtonCancelTrain )
    ON_BN_CLICKED( IDC_BUTTON_CANCEL_STATION, &CPidManagerDialog::OnBnClickedButtonCancelStations )
    ON_BN_CLICKED( IDC_BUTTON_SEND_MSG, &CPidManagerDialog::OnBnClickedButtonSendMsg )
    ON_BN_CLICKED( IDC_BUTTON_DLG_CLOSE, &CPidManagerDialog::OnButtonDlgClose )
    ON_REGISTERED_MESSAGE( TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, &CPidManagerDialog::OnSetWindowPosition )
    //ON_WM_SIZE()
    //ON_WM_MOVE()
    ON_WM_PAINT()
    //ON_WM_ACTIVATE()
    ON_WM_DESTROY()
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_LIST_PID_MSG, &CPidManagerDialog::OnLvnItemchangedListPidMsg )
    ON_EN_CHANGE( IDC_EDIT_MSG_CONTENT, &CPidManagerDialog::OnEnChangeEditMsgContent )
    //	ON_EN_CHANGE(IDC_EDIT_MSG_TITLE, &CPidManagerDialog::OnEnChangeEditMsgTitle)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAIN_SELECTION, &CPidManagerDialog::OnCbnSelchangeTrainList)
END_MESSAGE_MAP()


bool CPidManagerDialog::isTimeaEqualTimeB( SYSTEMTIME timeA, SYSTEMTIME timeB )
{
    return timeA.wHour == timeB.wHour
		   && timeA.wMinute == timeB.wMinute
		   && timeA.wSecond == timeB.wSecond;
}


bool CPidManagerDialog::isTimeaAfterTimeB( SYSTEMTIME timeA, SYSTEMTIME timeB )
{
	return timeA.wHour > timeB.wHour
		   || timeA.wHour == timeB.wHour && timeA.wMinute > timeB.wMinute
		   || timeA.wHour == timeB.wHour && timeA.wMinute == timeB.wMinute && timeA.wSecond > timeB.wSecond;
}

MessageTime CPidManagerDialog::getCyclicTime()
{
    FUNCTION_ENTRY( "getCyclicTime" );

    MessageTime cyclicTime = TIME_FOR_ONCEOFF;


    SYSTEMTIME currentTime, startTime, endTime;
    FILETIME startFT, endFT;
    ::ZeroMemory( &startTime, sizeof( SYSTEMTIME ) );
    ::ZeroMemory( &endTime, sizeof( SYSTEMTIME ) );
    //get the current system time
    GetLocalTime( &currentTime );
    //get the current start and end time
    m_pCyclicStartTime.GetTime( &startTime );
    m_pCyclicEndTime.GetTime( &endTime );
    ::SystemTimeToFileTime( &startTime, &startFT );
    ::SystemTimeToFileTime( &endTime, &endFT );
    //only one case is forbiden, when the  end<start<current
    if ( isTimeaEqualTimeB( startTime, endTime ) )
    {
        MessageBox( "开始时间等于结束时间!" );
    }
    else if ( isTimeaAfterTimeB( startTime, endTime ) )
    {
        //todo add a messagebox to tell the user it is wrong
        MessageBox( "结束时间早于开始时间!" );
    }
    else if ( isTimeaAfterTimeB( currentTime, endTime ) )
    {
        MessageBox( "结束时间早于当前系统时间! " );
    }
    else if ( !isTimeaAfterTimeB( startTime, currentTime ) )
    {
        MessageBox( "开始时间早于当前系统时间! " );
    }
    else
    {
        LONG64 TotalSec = 0;
		// The datetime ctrl can only get correct hour, min and second.
		// The year, month, day should get from current system time
        cyclicTime.startTimeYears = currentTime.wYear;
        cyclicTime.startTimeMonths = currentTime.wMonth;
        cyclicTime.startTimedays = currentTime.wDay;
        cyclicTime.startTimeHours = startTime.wHour;
        cyclicTime.startTimeMins = startTime.wMinute;
        cyclicTime.startTimeSecs = startTime.wSecond;
        cyclicTime.stopTimeYears = currentTime.wYear;
        cyclicTime.stopTimeMonths = currentTime.wMonth;
        cyclicTime.stopTimedays = currentTime.wDay;
        cyclicTime.stopTimeHours = endTime.wHour;
        cyclicTime.stopTimeMins = endTime.wMinute;
        cyclicTime.stopTimeSecs = endTime.wSecond;
    }
    return cyclicTime;
    FUNCTION_EXIT;
}

// CPidManagerDialog message handlers
void CPidManagerDialog::OnPaint()
{
    TransActiveDialog::OnPaint();
    GwxDisplayWrapper::getInstance().refreshSchematic();
}

BOOL CPidManagerDialog::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CRect windowSize;
    GetWindowRect( &windowSize );
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = false;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    setResizingProperties( properties );

    TransActiveDialog::OnInitDialog();

//	SetIcon(m_hIcon, TRUE);			// Set big icon
//		SetIcon(m_hIcon, FALSE);		// Set small icon

    m_pCyclicStartTime.SetFormat( "HH:mm:ss" );
    m_pCyclicEndTime.SetFormat( "HH:mm:ss" );
    m_pCyclicPeriod.SetFormat( "mm:ss" );
    CTime tm( 2010, 1, 1, 1, 01, 0 );
    m_pCyclicPeriod.SetTime( &tm );

    m_pShowBottom.SetCheck( CHECK_BOX_CHECKED );
    m_iCurSelection = -1;
    InitPidMsgListCtrl();
    StationPidUpdater::getInstance()->InitCorbaObject();
    //Set send msg button invisible
    GetDlgItem( IDC_BUTTON_SEND_MSG )-> EnableWindow( false );

	std::vector<Train> trainList = list_of<Train>("无选项", -1)("所有列车",0);

	for(std::vector<Train>::iterator it = trainList.begin(); it != trainList.end(); ++it)
	{
		int index = m_trainList.AddString((it->first).c_str());
		if(index >= 0)
		{
			m_trainList.SetItemData(index, static_cast<DWORD_PTR>(it->second));
		}
	}
	m_trainList.SetCurSel(0);
	OnCbnSelchangeTrainList();
	/*
	try
	{
		TiTrainHelper trainHelper;
		std::vector<TiTrain> trainConfig = trainHelper.load();
		std::sort(trainConfig.begin(), trainConfig.end(), bind(&TiTrain::getTrainNo, _1) < bind(&TiTrain::getTrainNo, _2));
		//int inserted = -1;
		for(std::vector<TiTrain>::iterator it = trainConfig.begin(); it != trainConfig.end(); ++it)
		{
			m_pkeyTITrainConfigMap.insert(std::make_pair(it->getKey(), new TiTrain(*it)));
			int index = m_trainList.AddString(it->getDisplayText().c_str());
			if(index >= 0)
			{
				m_trainList.SetItemData(index, static_cast<DWORD_PTR>(it->getTrainNo()));
			}
		}
		m_trainList.SetCurSel(0);
		OnCbnSelchangeTrainList();
	}
	catch(DatabaseException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", e.what());
	}
	catch(DataException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
	}
	*/
    ShowWindow( SW_SHOW );

	TrainList::instance().registerObserver(this);
	m_atsTrainDataPoller.start();

	
    FUNCTION_EXIT;
    return TRUE;
}

void CPidManagerDialog::init()
{
    CWnd* marker = GetDlgItem( IDC_STATIC_SCHEMATIC );
    CRect markerRect;
    marker->GetWindowRect( &markerRect );
    marker->ShowWindow( SW_HIDE );

    SchematicDecideModel sdmSchematicDecide;
    unsigned long locationKey = 0;

    if ( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ) )
    {
        std::stringstream converter;
        converter << RunParams::getInstance().get( RPARAM_LOCATIONKEY );
        converter >> locationKey;
    }
    sdmSchematicDecide.setSchematicLocationKey( locationKey );
    std::string& strSchematicFile = sdmSchematicDecide.getSchematicFileName();

    int nRet = GwxDisplayWrapper::getInstance().loadSchematic( strSchematicFile, markerRect, GetSafeHwnd() );
}

void CPidManagerDialog::OnBnClickHelp()
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}

void CPidManagerDialog::OnBnClickedCheckMsgCyclic()
{
    // TODO: Add your control notification handler code here
    FUNCTION_ENTRY( "OnBnClickedCheckMsgCyclic" );
    if ( m_pCyclicCheckBox.GetCheck () == CHECK_BOX_CHECKED )
    {
        m_pCyclicStartTime.EnableWindow( TRUE );
        m_pCyclicEndTime.EnableWindow( TRUE );
        m_pCyclicPeriod.EnableWindow( TRUE );

    }
    else
    {
        m_pCyclicStartTime.EnableWindow( FALSE );
        m_pCyclicEndTime.EnableWindow( FALSE );
        m_pCyclicPeriod.EnableWindow( FALSE );
    }
    FUNCTION_EXIT;
}


void CPidManagerDialog::OnBnClickedButtonEditMsg()
{
    // TODO: Add your control notification handler code here
    FUNCTION_ENTRY( "OnBnClickedButtonEditMsg" );
    if ( m_iCurSelection == -1 )
    {
        MessageBox( "请选择要编辑的消息！" );
        return;
    }
//	m_pMsgListCtrl.EnableWindow(FALSE);
    m_pMsgContent.SetReadOnly( FALSE );
    m_pMsgTitle.SetReadOnly( FALSE );
    GetDlgItem( IDC_BUTTON_EDIT_MSG )->EnableWindow( FALSE );
    FUNCTION_EXIT;
}

//void CPidManagerDialog::OnHdnItemclickListPidMsg(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: Add your control notification handler code here
//
//	*pResult = 0;
//}

void CPidManagerDialog::ShowSelectedMsg()
{
    FUNCTION_ENTRY( "ShowSelectedMsg" );
    CString strContent = m_strMsgContent.c_str();
    CString strDescription = m_strMsgDescription.c_str ();
    m_pMsgContent.SetWindowText( strContent );
    m_pMsgTitle.SetWindowText( strDescription );
    FUNCTION_EXIT;
}
void CPidManagerDialog::OnBnClickedButtonMsgCancel()
{
    FUNCTION_ENTRY( "OnBnClickedButtonMsgCancel" );
    // TODO: Add your control notification handler code here
    ShowSelectedMsg();

//	m_pMsgListCtrl.EnableWindow(TRUE);
    GetDlgItem( IDC_BUTTON_EDIT_MSG )->EnableWindow( TRUE );
    m_pMsgContent.SetReadOnly( TRUE );
    m_pMsgTitle.SetReadOnly( TRUE );
	m_isChanged = false;
    FUNCTION_EXIT;
}

void CPidManagerDialog::saveMessage()
{
    FUNCTION_ENTRY( "saveMessage" );
    int iIndex = m_iCurSelection;

    ++iIndex;

    StationPidUpdater::getInstance()->modifyPidMessage( iIndex, m_strMsgDescription, m_strMsgContent );
    FUNCTION_EXIT;
}
void CPidManagerDialog::OnBnClickedButtonMsgSave()
{
    FUNCTION_ENTRY( "OnBnClickedButtonMsgSave" );
    // TODO: Add your control notification handler code here
    //store the message information in the memory
    if ( -1 == m_iCurSelection )
    {
        //iIndex = m_pMsgListCtrl.GetItemCount();
        MessageBox( "请选择需要编辑的消息！" );
        return;
    }
    CString str;
    CStringW strW;
    m_pMsgTitle.GetWindowText( str );

    if ( str == "" )
    {
        MessageBox( PID_MSG_DESCRIPTION_CANNOT_BE_NULL );
        m_pMsgTitle.SetFocus();
        return;
    }
    if ( str.GetLength() > MAX_ADHOC_MSG_LENGTH )
    {
        CString errMsg;
        errMsg.Format( "消息内容的长度必须不超过%d个汉字，当前长度为%d", MAX_ADHOC_MSG_LENGTH/2, int( str.GetLength() / 2.0 + 0.5 ) );
        MessageBox( errMsg );
        return;
    }
    strW = str;
    m_strMsgDescription = ( LPCWSTR )strW;//.GetBuffer();
    //str.ReleaseBuffer();

    m_pMsgContent.GetWindowText( str );
    if ( str == "" )
    {
        MessageBox( PID_MSG_CONTENT_CANNOT_BE_NULL );
        m_pMsgContent.SetFocus();
        return;
    }
    if ( str.GetLength() > MAX_ADHOC_MSG_LENGTH )
    {
        CString errMsg;
        errMsg.Format( "消息内容的长度必须不超过%d个汉字，当前长度为%d", MAX_ADHOC_MSG_LENGTH/2, int( str.GetLength() / 2.0 + 0.5 ) );
        MessageBox( errMsg );
        return;
    }
    strW = str;
    m_strMsgContent = ( LPCWSTR )strW;//.GetBuffer();
    //str.ReleaseBuffer();
    //store the message information to the database

    saveMessage();
    //update the message to the listctrl
    m_pidMessages = StationPidUpdater::getInstance()->getPidMessages();
    UpdatePidMsgListCtrl( m_strMsgDescription, m_strMsgContent, m_iCurSelection );

    //m_strMsgContent = L"" ;
    //m_strMsgDescription = L"";
    //m_iCurSelection = -1;
    //ShowSelectedMsg();

//	m_pMsgListCtrl.EnableWindow(TRUE);
    GetDlgItem( IDC_BUTTON_EDIT_MSG )->EnableWindow( TRUE );
    m_pMsgContent.SetReadOnly( TRUE );
    m_pMsgTitle.SetReadOnly( TRUE );
    m_isChanged = false;
    FUNCTION_EXIT;
}

void CPidManagerDialog::UpdatePidMsgListCtrl( const wstring& strDescription, const wstring& strContent,
        const int id )
{
    FUNCTION_ENTRY( "UpdatePidMsgListCtrl" );
    if ( -1 == id ) //show that adding new message
    {
        AddNewItemToPidMsgListCtrl( strDescription, strContent );
    }
    else// edit the selected message
    {
        EditCurSelPidMsg( strDescription, strContent, id );
    }
    FUNCTION_EXIT;
}

void CPidManagerDialog::EditCurSelPidMsg( const wstring& strDescription, const wstring& strContent,
        const int id )
{
    FUNCTION_ENTRY( "EditCurSelPidMsg" );
    CString strText = strDescription.c_str();
    m_pMsgListCtrl.SetItemText ( id, 1, strText );
    UpdatePidMsgs( strDescription, strContent, id + 1 );
    FUNCTION_EXIT;
}
void CPidManagerDialog::UpdatePidMsgs( const wstring& strDescription, const wstring& strContent,
                                       const int id )
{
    FUNCTION_ENTRY( "UpdatePidMsgs" );
    TA_IRS_Core::PidMessages::iterator iter;
    for ( iter = m_pidMessages.begin(); iter != m_pidMessages.end(); iter++ )
    {
        if ( ( *iter ).getId() == id )
        {
            //MSG_INFO* pInfo = iter->second;
            ( *iter ).setMsgContent( strContent );
            ( *iter ).setMsgDescription( strDescription );
            FUNCTION_EXIT;
            return;
        }
    }

    TA_IRS_Core::PidMessage pidMsg( id, strDescription, strContent );
    m_pidMessages.push_back( pidMsg );
    FUNCTION_EXIT;
}

void CPidManagerDialog::AddNewItemToPidMsgListCtrl( const wstring& strDescription, const wstring& strContent )
{
    FUNCTION_ENTRY( "AddNewItemToPidMsgListCtrl" );
    int iItemCount = m_pMsgListCtrl.GetItemCount();
    ++ iItemCount;
    CString str;
    str.Format( "%d", iItemCount );
    m_pMsgListCtrl.InsertItem( iItemCount - 1, str );
    str = strDescription.c_str();
    m_pMsgListCtrl.SetItemText( iItemCount - 1, 1, str );
    UpdatePidMsgs( strDescription, strContent, iItemCount );
    FUNCTION_EXIT;
}

void CPidManagerDialog::InitPidMsgListCtrl()
{
    FUNCTION_ENTRY( "InitPidMsgListCtrl" );
    //first init header
    m_pMsgListCtrl.InsertColumn( 0, PID_MSG_ID, LVCFMT_LEFT, 60 );
    m_pMsgListCtrl.InsertColumn( 1, PID_MSG_DESCRIPTION, LVCFMT_LEFT, 280 );
    m_pMsgListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT );//|LVS_EX_GRIDLINES);
    //second get message from the updater and
    //fill the listctrl

    wstring  strDBDescrip = L"";
    int j = 1;

    m_pidMessages = StationPidUpdater::getInstance()->getPidMessages();
    TA_IRS_Core::PidMessages::iterator itr = m_pidMessages.begin();
    for ( ; itr != m_pidMessages.end(); itr++ )
    {
        /*while((j < (*itr).getId()) && (j < 31))
        {
        	CString strId;
        	CString	strDefaultDescrip = "Message ";
        	strId.Format("%d", j);
        	m_pMsgListCtrl.InsertItem(j - 1, strId);
        	strDefaultDescrip += strId;
        	m_pMsgListCtrl.SetItemText(j - 1, 1, strDefaultDescrip);
        	j++;
        }
        */
        //if(j == (*itr).getId())
        //{
        CString strId;
        CString strTxt;
        strTxt = ( *itr ).getMsgDescription().c_str();
        strId.Format( "%d", ( *itr ).getId() );
        m_pMsgListCtrl.InsertItem( j - 1, strId );

        m_pMsgListCtrl.SetItemText( j - 1, 1, strTxt );
        j++;
        //}

        //j++;
    }
    /*	for(; j < 31; j ++)
    	{
    		CString strId;
    		strId.Format("%d", j);
    		CString	strDefaultDescrip = "Message ";
    		m_pMsgListCtrl.InsertItem(j - 1, strId);
    		strDefaultDescrip += strId;
    		m_pMsgListCtrl.SetItemText(j - 1, 1, strDefaultDescrip);
    	}
    	*/
    FUNCTION_EXIT;
}
void CPidManagerDialog::OnNMClickListPidMsg( NMHDR *pNMHDR, LRESULT *pResult )
{
    FUNCTION_ENTRY( "OnNMClickListPidMsg" );
    // TODO: Add your control notification handler code here
    /*	int iPos = -1;
    	int iId;
    	NMLISTVIEW* pListView = (NMLISTVIEW*)pNMHDR;
    	iPos = pListView->iItem;
    	if(iPos == -1)
    	{
    		FUNCTION_EXIT;
    		return;
    	}
    	//m_iCurSelection = iPos;
    	CString strId = m_pMsgListCtrl.GetItemText(iPos,0);
    	iId = ::atol(strId);
    	m_iCurSelection = iId - 1;
    	//map<long, MSG_INFO*> ::iterator iter = m_mapOfMsgInformation.find(iPos + 1);
    	TA_IRS_Core::PidMessages::iterator iter = m_pidMessages.begin();
    	for(; iter != m_pidMessages.end(); iter ++)
    	{
    		if((*iter).getId() == iId)
    		{
    			m_strMsgDescription = (*iter).getMsgDescription();
    			m_strMsgContent = (*iter).getMsgContent();
    			break;
    		}
    	}
    	if(iter == m_pidMessages.end())
    	{
    		 CString strDescrip = m_pMsgListCtrl.GetItemText(m_iCurSelection, 1);
    		 CStringW strW = strDescrip;
    		 m_strMsgDescription = (LPCWSTR)strW;//strDescrip.GetBuffer();
    		 //strDescrip.ReleaseBuffer();
    		 m_strMsgContent = L"";
    	}
    	//show the updated message
    	ShowSelectedMsg();
    	GetDlgItem(IDC_BUTTON_EDIT_MSG)->EnableWindow(TRUE);
    	*pResult = 0;*/
    FUNCTION_EXIT;
}

void CPidManagerDialog::onRunParamChange( const string& name, const string& value )
{
    FUNCTION_ENTRY( "onRunParamChange" );
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "onRunParamChange:%s,%s", name.c_str(), value.c_str() );
    PIDSelectionManager *pidselectManager = PIDSelectionManager::getInstance();
    PIDSelectionManager::Euoperator op = pidselectManager->parseString( name, value );
    std::vector<std::string>& parVector = tokenizeString( value, PIDSelectionManager::TOKEN );
    switch ( op )
    {
        case PIDSelectionManager::SELECT_ALL:
            pidselectManager->selectAll();
            break;
        case PIDSelectionManager::DESELECT_ALL:
            pidselectManager->deselectAll();
            break;
        case PIDSelectionManager::SELECT_STATION_ALL:
            pidselectManager->selectStationAll( parVector[0] );
            break;
        case PIDSelectionManager::DESELECT_STATION_ALL:
            pidselectManager->deselectStationAll( parVector[0] );
            break;
        case PIDSelectionManager::SELECT_STATION_ONE_TRACK:
            pidselectManager->selectStationOneTrack( parVector[0], parVector[2] );
            break;
        case PIDSelectionManager::DESELECT_STATION_ONE_TRACK:
            pidselectManager->deselectStationOneTrack( parVector[0], parVector[2] );
            break;
        case PIDSelectionManager::SELECT_TRACK_ALL:
            pidselectManager->selectTrackAll( parVector[2] );
            break;
        case PIDSelectionManager::DESELECT_TRACK_ALL:
            pidselectManager->deselectTrackAll( parVector[2] );
            break;
        case PIDSelectionManager::SELECT_TRAIN:
            pidselectManager->selectTrain();
            break;
        case PIDSelectionManager::DESELECT_TRAIN:
            pidselectManager->deselectTrain();
            break;
        default:
            break;
    }


    FUNCTION_EXIT;
}

void CPidManagerDialog::OnBnClickedButtonSendMsg()
{
	FUNCTION_ENTRY( "OnBnClickedButtonSendMsg" );

    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList &pidsList= PIDSelectionManager::getInstance()->getPids();
	
    CTime tempStartTime, tempEndtime;
    bool isCyclicMessage = false;
    int interval = 0;
    bool msgType = NORMAL_MESSAGE;
    bool showType = SHOW_BOTTOM;
    TA_IRS_Core::PidMessage pidMsg;
    MessageTime messageTime = TIME_FOR_ONCEOFF;
	int trainNo = static_cast<int>(m_trainList.GetItemData(m_trainList.GetCurSel()));
	//if not select any station,and not select train
    if ( (pidsList.length() == 0) && trainNo < 0 )
    {
        MessageBox( "请选择要发送的区域!" );
        FUNCTION_EXIT;
        return;
    }
    if ( m_iCurSelection == -1 )
    {
        MessageBox( "请选择要发送的消息!" );
        FUNCTION_EXIT;
        return;
    }
	if (MessageBox(MESSAGE_TEXT_UW_070024,MESSAGE_ID_UW_070024.c_str(),MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
	{
		FUNCTION_EXIT;
		return;
	}
	else
    {
        if ( m_pEmergencyCheckBox.GetCheck() == CHECK_BOX_CHECKED )
        {
            msgType = EMERGENCY_MESSAGE;
        }

        if ( m_pShowFullScreen.GetCheck() == CHECK_BOX_CHECKED )
        {
            showType = SHOW_FULL_SCREEN;
        }

        pidMsg.setMsgContent( m_strMsgContent );
        pidMsg.setMsgDescription( m_strMsgDescription );
        if ( m_pCyclicCheckBox.GetCheck() == CHECK_BOX_CHECKED )
        {
            isCyclicMessage = true;
            messageTime = getCyclicTime();
            SYSTEMTIME repeat;
            m_pCyclicPeriod.GetTime( &repeat );
            interval += 60 * repeat.wMinute + repeat.wSecond;
            if ( messageTime.startTimeHours <= TIME_FOR_ONCEOFF.startTimeHours )
            {
                MessageBox( "请检查发送时间!" );
                FUNCTION_EXIT;
                return;
            }
        }
        else
        {
            isCyclicMessage = false;
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                 "Pid message description is %s, message content is %s ",
                 pidMsg.getMsgDescription().c_str(), pidMsg.getMsgContent().c_str() );

    TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage	pidMessage;
    TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime targetTimeStart, targetTimeStop;

    //pidMessage = StationPidUpdater::getInstance()->convertBackChineseDesc( pidMsg.getMsgContent().c_str() );
	pidMessage = StationPidUpdater::getInstance()->wstringToCorbaSequence(pidMsg.getMsgContent());

    targetTimeStart.year    = messageTime.startTimeYears;
    targetTimeStart.month   = messageTime.startTimeMonths;
    targetTimeStart.day     = messageTime.startTimedays;
    targetTimeStart.hour    = messageTime.startTimeHours;
    targetTimeStart.minute  = messageTime.startTimeMins;
    targetTimeStart.second  = messageTime.startTimeSecs;
    targetTimeStop.year     = messageTime.stopTimeYears;
    targetTimeStop.month    = messageTime.stopTimeMonths;
    targetTimeStop.day      = messageTime.stopTimedays;
    targetTimeStop.hour     = messageTime.stopTimeHours;
    targetTimeStop.minute   = messageTime.stopTimeMins;
    targetTimeStop.second   = messageTime.stopTimeSecs;
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                 "the start time is (%d:%d:%d), the end time is (%d:%d:%d)",
                 targetTimeStart.hour, targetTimeStart.minute, targetTimeStart.second,
                 targetTimeStop.hour, targetTimeStop.minute, targetTimeStop.second );

    try
    {
        short nRet;
        CORBA_CALL_RETURN( nRet, StationPidUpdater::getInstance()->getPidsAgentObject(),
                           submitAdhocDisplayRequest, ( pidsList, pidMessage, msgType, showType, trainNo,
                                                        targetTimeStart, targetTimeStop, interval, isCyclicMessage ) );
        if ( nRet == 0 )
        {
            UserMessages::getInstance().displayInfo( UserMessages::REQUEST_SEND_SUCCESSFUL );
        }
        else
        {
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法连接到FEP" ).c_str() );
        }
    }
    catch ( const TA_Base_Core::ObjectResolutionException& ore )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
        UserMessages::getInstance().displayError(
            str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法解析PidsAgent" ).c_str() );
    }
    catch ( const CORBA::Exception& ce )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                             TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );
        UserMessages::getInstance().displayError(
            str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现Corba异常" ).c_str() );
    }
    catch ( const TA_Base_Core::TransactiveException& ex )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", ex.what() );
        UserMessages::getInstance().displayError(
            str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现异常" ).c_str() );
    }
    catch ( ... )
    {
        UserMessages::getInstance().displayError( UserMessages::REQUEST_SEND_FAIL );
    }
    FUNCTION_EXIT;
}


void CPidManagerDialog::OnBnClickedButtonCancelAll()
{
    FUNCTION_ENTRY( "OnBnClickedButtonCancelAll" );
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList &pidsList = PIDSelectionManager::getAllStationsList();
	int trainNo = 0;			//select all train
    if ( MessageBox( MESSAGE_CANCEL_ALL, MESSAGE_ID_UW_070024.c_str(), MB_OKCANCEL | MB_ICONWARNING ) == IDOK )
    {

        try
        {
            short nRet;
            CORBA_CALL_RETURN( nRet, StationPidUpdater::getInstance()->getPidsAgentObject(), submitClearRequest, ( pidsList, trainNo ) );
            if ( nRet == 0 )
            {
                UserMessages::getInstance().displayInfo( UserMessages::REQUEST_SEND_SUCCESSFUL );
            }
            else
            {
                UserMessages::getInstance().displayError(
                    str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法连接到FEP" ).c_str() );
            }
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法解析PidsAgent" ).c_str() );
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现Corba异常" ).c_str() );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", ex.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现异常" ).c_str() );
        }
        catch ( ... )
        {
            UserMessages::getInstance().displayError( UserMessages::REQUEST_SEND_FAIL );
        }
    }
    FUNCTION_EXIT;
}

void CPidManagerDialog::OnBnClickedButtonCancelStations()
{
    //get all the station pids
    FUNCTION_ENTRY( "OnBnClickedButtonCancelStations" );
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList &pidsList = PIDSelectionManager::getInstance()->getPids();

    if ( MessageBox( MESSAGE_CANCEL_STATIONS, MESSAGE_ID_UW_070024.c_str(), MB_OKCANCEL | MB_ICONWARNING ) == IDOK )
    {
        if ( pidsList.length() == 0 )
        {
            MessageBox( "请先选择要取消消息广播的区域!" );
            FUNCTION_EXIT;
            return;
        }
        try
        {
            short nRet;
            CORBA_CALL_RETURN( nRet, StationPidUpdater::getInstance()->getPidsAgentObject(), submitClearRequest, ( pidsList, -1 ) );
            if ( nRet == 0 )
            {
                UserMessages::getInstance().displayInfo( UserMessages::REQUEST_SEND_SUCCESSFUL );
            }
            else
            {
                UserMessages::getInstance().displayError(
                    str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法连接到FEP" ).c_str() );
            }
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法解析PidsAgent" ).c_str() );
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现Corba异常" ).c_str() );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", ex.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现异常" ).c_str() );
        }
        catch ( ... )
        {
            UserMessages::getInstance().displayError( UserMessages::REQUEST_SEND_FAIL );
        }
    }
    FUNCTION_EXIT;
}

void CPidManagerDialog::OnBnClickedButtonCancelTrain()
{
    FUNCTION_ENTRY( "OnBnClickedButtonCancelTrain" );
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList pidsList;
	pidsList.length(0);
	int trainNo = static_cast<int>(m_trainList.GetItemData(m_trainList.GetCurSel()));
    if ( MessageBox( MESSAGE_CANCEL_TRAIN, MESSAGE_ID_UW_070024.c_str(), MB_OKCANCEL | MB_ICONWARNING ) == IDOK )
    {
		if ( trainNo < 0 )
		{
			MessageBox( "请先选择要取消消息的列车!");
			FUNCTION_EXIT;
			return;
		}
        try
        {
            short nRet;
			CORBA_CALL_RETURN( nRet, StationPidUpdater::getInstance()->getPidsAgentObject(), submitClearRequest, ( pidsList, trainNo ) );
            if ( nRet == 0 )
            {
                UserMessages::getInstance().displayInfo( UserMessages::REQUEST_SEND_SUCCESSFUL );
            }
            else
            {
                UserMessages::getInstance().displayError(
                    str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法连接到FEP" ).c_str() );
            }
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",无法解析PidsAgent" ).c_str() );
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现Corba异常" ).c_str() );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", ex.what() );
            UserMessages::getInstance().displayError(
                str( format( UserMessages::REQUEST_SEND_FAIL ) % ",出现异常" ).c_str() );
        }
        catch ( ... )
        {
            UserMessages::getInstance().displayError( UserMessages::REQUEST_SEND_FAIL );
        }
    }
    FUNCTION_EXIT;
}

/*
void CPidManagerDialog::OnSize( UINT nType, int cx, int cy )
{
    FUNCTION_ENTRY( "OnSize" );

    TransActiveDialog::OnSize( nType, cx, cy );

    if ( SIZE_RESTORED == nType )
    {
        //GwxDisplayWrapper::getInstance().refreshSchematic();
    }

    FUNCTION_EXIT;
}
*/

/*
void CPidManagerDialog::OnMove( int x, int y )
{
    FUNCTION_ENTRY( "OnMove" );

    TransActiveDialog::OnMove( x, y );

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Window moved to (%d,%d)", x, y );

    if ( x == -32000 && y == -32000 )
    {
        // This is a special case situation that occurs in response
        // to a WM_MINIMIZE notification
        FUNCTION_EXIT;
        return;
    }

    CRect screen;
    ::GetClientRect( ::GetDesktopWindow(), screen );
    CRect rc;
    GetWindowRect( rc );

    enum { BUFFER_DISTANCE = 8 };

    int adjustedX = x;
    int adjustedY = y;
    if ( adjustedY < BUFFER_DISTANCE )
    {
        // Don't allow movement into negative on Y (at all)
        adjustedY = BUFFER_DISTANCE;
    }
    // Don't allow to move completely below bottom of screen
    if ( adjustedY > screen.Height() - BUFFER_DISTANCE )
    {
        adjustedY = screen.Height() - BUFFER_DISTANCE;
    }

    // Allow at least BUFFER_DISTANCE pixel inside of screen either side
    if ( adjustedX + rc.Width() < BUFFER_DISTANCE )
    {
        adjustedX = -rc.Width() + BUFFER_DISTANCE;
    }
    if ( adjustedX > screen.Width() - BUFFER_DISTANCE )
    {
        adjustedX = screen.Width() - BUFFER_DISTANCE;
    }

    static bool overridingOnMove = false;
    // To avoid recursion, confirm this OnMove request is not in response
    // To an onMove override
    if ( !overridingOnMove )
    {
        if ( x != adjustedX || y != adjustedY )
        {
            CRect client;
            GetClientRect( client );
            ClientToScreen( client );
            adjustedY -= ( client.top - rc.top );   // Subtract height of menu
            adjustedX -= ( client.left - rc.left ); // Subtract width of left border

            overridingOnMove = true;

            // Reposition as necessary
            ::MoveWindow( m_hWnd, adjustedX, adjustedY, rc.Width(), rc.Height(), TRUE );

            overridingOnMove = false;

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Window outside screen, repositioned to (%d,%d)", adjustedX, adjustedY );
        }
    }

    //GwxDisplayWrapper::getInstance().refreshSchematic();

    FUNCTION_EXIT;
}
*/

void CPidManagerDialog::OnButtonDlgClose()
{
    FUNCTION_ENTRY( "OnButtonDlgClose" );
    DestroyWindow();

    FUNCTION_EXIT;
}

void CPidManagerDialog::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

	TrainList::instance().unregisterObserver(this);
	m_atsTrainDataPoller.terminateAndWait();
    try
    {
        GwxDisplayWrapper::getInstance().closeSchematic();
    }
    catch ( ... )
    {
    }
    TransActiveDialog::OnDestroy();

    FUNCTION_EXIT;

}


LRESULT CPidManagerDialog::OnSetWindowPosition( WPARAM wParam, LPARAM lParam )
{

    return TransActiveDialog::OnSetWindowPosition( wParam, lParam );
}
LRESULT CPidManagerDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "WindowProc" );

    LRESULT result = 0;

    try
    {
        result = TransActiveDialog::WindowProc( message, wParam, lParam );
    }
    catch ( ... )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknow exception occured" );
    }

    FUNCTION_EXIT;
    return result;
}

void CPidManagerDialog::OnLvnItemchangedListPidMsg( NMHDR *pNMHDR, LRESULT *pResult )
{
    FUNCTION_ENTRY( "OnLvnItemchangedListPidMsg" );
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );
    // TODO: Add your control notification handler code here
    if ( pNMLV->uChanged == LVIF_STATE )
    {
        if ( pNMLV->uNewState & LVIS_SELECTED )
        {
            if ( m_isChanged == true )
            {
                if ( MessageBox( "消息内容已修改，需要保存吗?", "提示", MB_YESNO ) == IDYES )
                {
                    OnBnClickedButtonMsgSave();
                }
            }
            m_isChanged = false;
            int iPos = -1;
            int iId;
            iPos = pNMLV->iItem;
            if ( iPos == -1 )
            {
                FUNCTION_EXIT;
                return;
            }
            //m_iCurSelection = iPos;
            CString strId = m_pMsgListCtrl.GetItemText( iPos, 0 );
            iId = ::atol( strId );
            m_iCurSelection = iId - 1;

			//Set send msg button state
			PIDSelectionManager::getInstance()->setEnable();
            TA_IRS_Core::PidMessages::iterator iter = m_pidMessages.begin();
            for ( ; iter != m_pidMessages.end(); iter ++ )
            {
                if ( ( *iter ).getId() == iId )
                {
                    m_strMsgDescription = ( *iter ).getMsgDescription();
                    m_strMsgContent = ( *iter ).getMsgContent();
                    break;
                }
            }
            if ( iter == m_pidMessages.end() )
            {
                CString strDescrip = m_pMsgListCtrl.GetItemText( m_iCurSelection, 1 );
                CStringW strW = strDescrip;
                m_strMsgDescription = ( LPCWSTR )strW;//strDescrip.GetBuffer();
                //strDescrip.ReleaseBuffer();
                m_strMsgContent = L"";
            }
            //show the updated message
            ShowSelectedMsg();
            GetDlgItem( IDC_BUTTON_EDIT_MSG )->EnableWindow( TRUE );
        }
    }
    *pResult = 0;
    FUNCTION_EXIT;
}

void CPidManagerDialog::OnEnChangeEditMsgContent()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the __super::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_isChanged = true;
}

bool CPidManagerDialog::isSelectMsg()
{
	if(-1 == m_iCurSelection)
		return false;
	return true;
}
void CPidManagerDialog::OnCbnSelchangeTrainList()
{
	// TODO: Add your control notification handler code here
	int trainNo = static_cast<int>(m_trainList.GetItemData(m_trainList.GetCurSel()));
	PIDSelectionManager *pidselectManager = PIDSelectionManager::getInstance();
	if (trainNo >= 0 )
	{
		pidselectManager->selectTrain();
	} 
	else
	{
		pidselectManager->deselectTrain();
	}
}

void CPidManagerDialog::notifyTrainListUpdate( const std::vector<Train>& trainList )
{
	m_trainList.ResetContent();

	std::vector<Train> list = list_of<Train>("无选项", -1)("所有列车",0);
	std::copy(trainList.begin(), trainList.end(), std::back_inserter(list));

	for(std::vector<Train>::iterator it = list.begin(); it != list.end(); ++it)
	{
		int index = m_trainList.AddString((it->first).c_str());
		if(index >= 0)
		{
			m_trainList.SetItemData(index, static_cast<DWORD_PTR>(it->second));
		}
	}
	m_trainList.SetCurSel(0);
	OnCbnSelchangeTrainList();
}

}