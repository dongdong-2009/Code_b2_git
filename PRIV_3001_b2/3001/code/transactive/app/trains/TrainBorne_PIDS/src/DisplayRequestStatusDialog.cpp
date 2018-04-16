/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#include "stdafx.h"
#include "DisplayRequestStatusDialog.h"
#include "DisplayRequestManager.h"
#include "core/utilities/src/DebugUtil.h"

#include <iomanip>
#include <string>


namespace TA_IRS_App
{

    const int DisplayRequestStatusDialog::WM_UPDATE_TRAIN_STATUS    = WM_USER + 5;
    const int DisplayRequestStatusDialog::WM_CLEAR_TRAIN_LIST       = WM_USER + 6;
    const int DisplayRequestStatusDialog::WM_ENABLE_OK_BUTTON       = WM_USER + 7;
    const int DisplayRequestStatusDialog::TRAIN_ID_COLUMN           = 0;
    const int DisplayRequestStatusDialog::STATUS_COLUMN             = 1;

    DisplayRequestStatusDialog::DisplayRequestStatusDialog( std::string dialogName, UINT nIDTemplate )
        : CDialog( nIDTemplate ),
          m_timerUtil( TA_Base_Core::SingletonTimerUtil::getInstance() ),
          m_dialogName( dialogName ),
          m_timerId( 0L ) //m_timerId may need initialize
    {
        FUNCTION_ENTRY( "DisplayRequestStatusDialog" );
		m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_PIDS);
        FUNCTION_EXIT;
    }


    DisplayRequestStatusDialog::~DisplayRequestStatusDialog()
    {
        FUNCTION_ENTRY( "~DisplayRequestStatusDialog" );

        //cancel all timers
        //remove instance of m_timerUtil


        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::OnItemchangedProcessList( NMHDR* pNMHDR, LRESULT* pResult )
    {
        *pResult = 0;
    }

    void DisplayRequestStatusDialog::DoDataExchange( CDataExchange* pDX )
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(DisplayRequestStatusDialog)
        DDX_Control( pDX, IDC_CLOSE, m_closeBtn );
        DDX_Control( pDX, IDC_REQUEST_STATUS_LIST, m_statusList );
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP( DisplayRequestStatusDialog, CDialog )
        //{{AFX_MSG_MAP(DisplayRequestStatusDialog)
        ON_MESSAGE( DisplayRequestStatusDialog::WM_CLEAR_TRAIN_LIST, onClearTrainList )
        ON_MESSAGE( DisplayRequestStatusDialog::WM_UPDATE_TRAIN_STATUS, onUpdateTrainList )
        ON_MESSAGE( DisplayRequestStatusDialog::WM_ENABLE_OK_BUTTON, onEnableOkButton )
        ON_WM_DESTROY()
        ON_NOTIFY( LVN_ITEMCHANGED, IDC_REQUEST_STATUS_LIST, OnItemchangedProcessList )
        ON_BN_CLICKED( IDC_CLOSE, OnCloseBtn )
		ON_WM_CLOSE()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


	void DisplayRequestStatusDialog::OnClose()
	{
		hideDialog();
	}

    void DisplayRequestStatusDialog::updateViewArea()
    {

    }

    void DisplayRequestStatusDialog::clearViewArea()
    {

    }

    void DisplayRequestStatusDialog::OnDestroy()
    {
        {
            TA_THREADGUARD( m_dataLock );

            //2008-08-01 verify timer first
            if ( m_timerId > 0 )
            {
                m_timerUtil.stopPeriodicTimeOutClock( this );
                m_timerId = 0;
            }

            TA_Base_Core::SingletonTimerUtil::removeInstance();
        }
        CDialog::OnDestroy();
    }

    BOOL DisplayRequestStatusDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        /*
        CRect rect;
        GetWindowRect(&rect);

        MoveWindow(rect.left+400, rect.top+200,rect.Width(),rect.Height());
        */

        // CenterWindow
        //GetParent()->CenterWindow();
        this->CenterWindow();

        //AfxGetMainWnd()

        //SetWindowPos(NULL,0,0,1280,803,SWP_NOMOVE|SWP_NOZORDER);

        CDialog::OnInitDialog();

        SetWindowText( m_dialogName.c_str() );
		SetIcon(m_hIcon, TRUE);

        // set the list control style
        m_statusList.SetExtendedStyle( m_statusList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );

        // add the columns to the list control and set their size
        m_statusList.InsertColumn( TRAIN_ID_COLUMN, "TrainID/Service", LVCFMT_LEFT );
        m_statusList.SetColumnWidth( TRAIN_ID_COLUMN, 140 );

        m_statusList.InsertColumn( STATUS_COLUMN, "Status", LVCFMT_LEFT );
        m_statusList.SetColumnWidth( STATUS_COLUMN, 248 );

        // install the fixed header control
        m_fixedHeader.subclassHeader( m_statusList.GetHeaderCtrl() );

        // disable the OK button
        m_closeBtn.EnableWindow( FALSE );

        FUNCTION_EXIT;
        return TRUE;
    }


    void DisplayRequestStatusDialog::setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
            time_t timestamp,
            const std::string& sessionId,
            TA_Base_Core::TisCommandType originalCommand,
            unsigned long maximumRequestTime )
    {
        FUNCTION_ENTRY( "setDisplayRequest" );

        // firstly close window
        hideDialog();

        TA_THREADGUARD( m_dataLock );

        // cancel any timer
        cancelTimer();

        // clear list control
        clearStatusList();
        m_trainDetailsList = trainDetailsList;

        // set the other parameters
        m_timestamp = timestamp;
        m_sessionId = sessionId;
        m_originalCommand = originalCommand;
        m_waitTimeMillis = maximumRequestTime;

        // insert the initial train status
        // do this in a new loop, rather than the above loop
        // this means the list will already be sorted by train ID
        // given that the TrainIdList is a std::set
        //2008-08-04
        for ( TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator iter = m_trainDetailsList.begin();
                iter != m_trainDetailsList.end();
                iter++ )
        {
            updateTrainStatus( *iter, "In progress" );
        }

        // disable the ok button
        enableOkButton( false );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::displayModalWindow()
    {
        FUNCTION_ENTRY( "displayModalWindow" );

        TA_THREADGUARD( m_dataLock );

        // cancel any timer
        cancelTimer();

        // if there are still trains waiting
        if ( false == m_trainDetailsList.empty() )
        {
            // disable the ok button
            enableOkButton( false );

            // schedule a new timer
            scheduleTimer();
        }
        else
        {
            // enable the ok button so the operator can exit
            enableOkButton( true );
        }

        // show the window so the operator can see the status
        showDialog();

        FUNCTION_EXIT;
    }


	void DisplayRequestStatusDialog::handleDisplayResult( const TA_Base_Core::TTISDisplayResult& returnStatus )
    {
        FUNCTION_ENTRY( "handleDisplayResult" );

        TA_THREADGUARD( m_dataLock );
        if( ( returnStatus.timestamp != m_timestamp )
                || ( returnStatus.sessionId.in() != m_sessionId ) )
            //||( displayResult.originalCommand != m_originalCommand )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator ite = getTrainDetailsById( returnStatus.trainId );
        if( ite == m_trainDetailsList.end() )
        {
            return;
        }
        if( returnStatus.success == true )
        {
            updateTrainStatus( *ite, "Successful" );
        }
        else
        {
            updateTrainStatus( *ite, returnStatus.errorDetails.in() );
        }

        // remove the train from the list of expected trains
        m_trainDetailsList.erase( ite );

        // check if we are expecting any more responses
        if ( true == m_trainDetailsList.empty() )
        {
            // enable the ok button so the user can exit
            enableOkButton( true );

            // cancel the timer
            cancelTimer();
        }

        FUNCTION_EXIT;
    }

    void DisplayRequestStatusDialog::timerExpired( long timerId, void* userData )
    {
        FUNCTION_ENTRY( "timerExpired" );

        TA_THREADGUARD( m_dataLock );

        // check the timer Id matches
        if ( timerId != m_timerId )
        {
            FUNCTION_EXIT;
            return;
        }

        // reset timer id
        m_timerId = 0;

        // for each train no response was received for
        // mark its status timed out
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator iter = m_trainDetailsList.begin();
        while ( iter != m_trainDetailsList.end() )
        {
            updateTrainStatus( *iter, "Timed-out" );
            //m_trainDetailsList.erase( iter++ );
            iter = m_trainDetailsList.erase( iter );
        }

        // enable the ok button so the user can exit
        enableOkButton( true );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::OnCloseBtn()
    {
        FUNCTION_ENTRY( "OnCloseBtn" );

        hideDialog();

        FUNCTION_EXIT;
    }


    LRESULT DisplayRequestStatusDialog::onClearTrainList( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onClearTrainList" );

        m_statusList.DeleteAllItems();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT DisplayRequestStatusDialog::onEnableOkButton( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onEnableOkButton" );

        BOOL enable = static_cast< BOOL >( wParam );
        m_closeBtn.EnableWindow( enable );

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT DisplayRequestStatusDialog::onUpdateTrainList( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onUpdateTrainList" );

        StatusEntry* entry  = reinterpret_cast< StatusEntry* >( wParam );
        TA_ASSERT( entry != NULL, "WM_UPDATE_TRAIN_STATUS with no status structure" );

        // find the item by its item data (train ID)
        LVFINDINFO findInfo;
        findInfo.flags = LVFI_PARAM;
        findInfo.lParam = entry->trainId;
        int pos = m_statusList.FindItem( &findInfo );

        if ( pos < 0 )
        {
            // not found, insert new at the end
            pos = m_statusList.InsertItem( m_statusList.GetItemCount(), entry->trainIdSeviceString.c_str() );
            m_statusList.SetItemData( pos, entry->trainId );
        }

        // set the status string
        m_statusList.SetItemText( pos, STATUS_COLUMN, entry->statusString.c_str() );

        delete entry;
        entry = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    void DisplayRequestStatusDialog::showDialog()
    {
        FUNCTION_ENTRY( "showDialog" );

        //Use the ShowWindow call to show the window

		CenterWindow(AfxGetMainWnd());
        ShowWindow( SW_SHOW );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::hideDialog()
    {
        FUNCTION_ENTRY( "hideDialog" );

        //Use the ShowWindow call to hide the window

        if ( NULL != m_hWnd )
        {
            ShowWindow( SW_HIDE );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::clearStatusList()
    {
        FUNCTION_ENTRY( "clearStatusList" );

        PostMessage( WM_CLEAR_TRAIN_LIST, 0, 0 );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::updateTrainStatus( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails,
            const std::string& statusString )
    {
        FUNCTION_ENTRY( "updateTrainStatus" );

        StatusEntry* newEntry   = new StatusEntry();
        newEntry->trainId = trainDetails.trainId;
        std::ostringstream trainIdSeviceString;
        trainIdSeviceString << trainDetails.serviceNumber;
        newEntry->trainIdSeviceString = trainIdSeviceString.str();
        newEntry->statusString = statusString;

        PostMessage( WM_UPDATE_TRAIN_STATUS, reinterpret_cast< WPARAM >( newEntry ), 0 );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::enableOkButton( bool enable )
    {
        FUNCTION_ENTRY( "enableOkButton" );

        // convert to the Micro$oft c-style bool
        BOOL flag   = ( enable == true ) ? TRUE : FALSE;

        PostMessage( WM_ENABLE_OK_BUTTON, static_cast< WPARAM >( flag ), 0 );

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::scheduleTimer()
    {
        FUNCTION_ENTRY( "scheduleTimer" );

        cancelTimer();

        if ( m_waitTimeMillis > 0 )
        {
            m_timerId = m_timerUtil.startPeriodicTimeOutClock( this, m_waitTimeMillis, true );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestStatusDialog::cancelTimer()
    {
        FUNCTION_ENTRY( "cancelTimer" );

        // 2008-08-01 verify timer
        if ( m_timerId > 0 )
        {
            m_timerUtil.stopPeriodicTimeOutClock( this );
            m_timerId = 0;
        }

        FUNCTION_EXIT;
    }

    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator
    DisplayRequestStatusDialog::getTrainDetailsById( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator ite = m_trainDetailsList.begin();
        for( ; ite != m_trainDetailsList.end(); ++ite )
        {
            if( ite->trainId == trainId )
            {
                return ite;
            }
        }
        return ite;
    }
}
