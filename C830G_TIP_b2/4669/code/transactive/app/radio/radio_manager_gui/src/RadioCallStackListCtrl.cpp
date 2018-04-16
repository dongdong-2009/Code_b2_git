/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4669/transactive/app/radio/radio_manager_gui/src/RadioCallStackListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2008/08/29 12:05:40 $
  * Last modified by:  $Author: ripple $
  *
  * This class provides a call list which is self managing.  It will automatically
  * update with call notifications from the radio.
  *
  */


#include "app/radio/radio_manager_gui/src/stdafx.h"

#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioCallStackListCtrl.h"
#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"

#include "bus/radio/RadioEntityAccess/src/RadioEntityAccess.h"
#include "bus/radio/RadioSearchPage/src/SearchPage.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <ace/OS.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// private items in an anonymous namespace
namespace
{
    const int CALL_NOTIFICATION_MSG = WM_USER + 100;
    const int OCC_CALL_REQUEST_MSG = WM_USER + 101;
    const int OCC_CALL_RESET_MSG = WM_USER + 102;
    const int TRAIN_DETAILS_CHANGED_MSG = WM_USER + 103;

    const COLORREF EMERGENCY_TEXT_COLOUR = RGB( 255, 0, 0 );

    // Forward declare helper function
    static std::string getLocalTimeFrom( time_t ltime );
}


namespace TA_IRS_App
{

    CRadioCallStackListCtrl::CRadioCallStackListCtrl( EStackType stackType )
        : m_subscribed( false ),
          m_stackType( stackType ),
          m_canCallTrain( false ),
          m_radioEntityKey( 0 ),
          m_radioEntityLocation( 0 ),
          m_consoleId( RadioEntityAccess::getConsoleKey() ),
		  m_closeMode(false)

    {
        FUNCTION_ENTRY( "CRadioCallStackListCtrl" );

        HICON hIcon;

        m_ImageList.Create( 16, 16, ILC_COLOR8, 4, 4 );
        hIcon = AfxGetApp()->LoadIcon( IDI_CALL_SDS );
        m_ImageList.Add( hIcon );
        hIcon = AfxGetApp()->LoadIcon( IDI_EMERG_CALL_SDS );
        m_ImageList.Add( hIcon );

        hIcon = AfxGetApp()->LoadIcon( IDI_CALL_DRIVER );
        m_ImageList.Add( hIcon );
        hIcon = AfxGetApp()->LoadIcon( IDI_EMERG_CALL_DRIVER );
        m_ImageList.Add( hIcon );

        hIcon = AfxGetApp()->LoadIcon( IDI_CALL_VOICE );
        m_ImageList.Add( hIcon );
        hIcon = AfxGetApp()->LoadIcon( IDI_EMERG_CALL_VOICE );
        m_ImageList.Add( hIcon );

        hIcon = AfxGetApp()->LoadIcon( IDI_CALL_CONFERENCE );
        m_ImageList.Add( hIcon );
        hIcon = AfxGetApp()->LoadIcon( IDI_EMERG_CALL_CONFERENCE );
        m_ImageList.Add( hIcon );

        FUNCTION_EXIT;
    }


    CRadioCallStackListCtrl::~CRadioCallStackListCtrl()
    {
        FUNCTION_ENTRY( "~CRadioCallStackListCtrl" );

		setCloseMode(true);
        stopSubscription();

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP( CRadioCallStackListCtrl, ListCtrlSelNoFocus )
        //{{AFX_MSG_MAP(CRadioCallStackListCtrl)
        //}}AFX_MSG_MAP
        ON_MESSAGE( CALL_NOTIFICATION_MSG, onCallNotification )
        ON_MESSAGE( OCC_CALL_REQUEST_MSG, onOccCallRequest )
        ON_MESSAGE( OCC_CALL_RESET_MSG, onOccCallReset )
        ON_MESSAGE( TRAIN_DETAILS_CHANGED_MSG, onTrainDetailsChanged )
    END_MESSAGE_MAP()


    void CRadioCallStackListCtrl::setRadioName( std::string radioName )
    {
        FUNCTION_ENTRY( "setRadioName" );

        try
        {
            // create the train agent proxy
            createTrainProxy();

            // Set the remote name for the radio object
            m_radioAgent.setEntityName( radioName,false );

            // Determine entity key for the local radio agent
            // This is used to filter the incoming notifications from the radio agents
            m_radioEntityLocation = RadioEntityAccess::getLocationKey( m_consoleId );
            m_radioEntityKey = RadioEntityAccess::getRadioEntityKey( m_radioEntityLocation );

            // Start the subscription and load the list
            startSubscription();
        }
        catch ( ... )
        {
            LOG( SourceInfo,
                 TA_Base_Core::DebugUtil::ExceptionCatch,
                 "Unknown",
                 "While processing CRadioCallStackListCtrl::setRadioName" );
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::setCallTrainPermitted( bool canCallTrain )
    {
        FUNCTION_ENTRY( "setCallTrainPermitted" );

        // if the permission is different
        if ( m_canCallTrain != canCallTrain )
        {
            m_canCallTrain = canCallTrain;

            // refresh the train calls
            loadTrainCalls();
        }

        FUNCTION_EXIT;
    }


    BOOL CRadioCallStackListCtrl::setColumnName( int nCol, char*  columnName )
    {
        FUNCTION_ENTRY( "setColumnName" );

        LVCOLUMN column;

        column.mask = LVCF_TEXT | LVCF_SUBITEM;
        column.iSubItem = nCol;
        column.pszText = columnName;

        FUNCTION_EXIT;
        return SetColumn( nCol, &column );
    }


    bool CRadioCallStackListCtrl::IsCallSelected()
    {
        FUNCTION_ENTRY( "IsCallSelected" );

        POSITION SelPos = GetFirstSelectedItemPosition();

        FUNCTION_EXIT;
        return ( SelPos != NULL );
    }


    long CRadioCallStackListCtrl::getSelectedCallID()
    {
        FUNCTION_ENTRY( "getSelectedCallID" );

        POSITION SelPos = GetFirstSelectedItemPosition();
        TA_ASSERT ( SelPos != NULL, "This should not be called when nothing is selected" );

        int nCol = GetNextSelectedItem( SelPos );

        TA_ASSERT( nCol >= 0, "Must have a call selected to call this function" );

        FUNCTION_EXIT;
        return GetItemData( nCol );
    }


    long CRadioCallStackListCtrl::getSelectedItemWithType( ECallSource& source )
    {
        FUNCTION_ENTRY( "getSelectedItemWithType" );

        POSITION SelPos = GetFirstSelectedItemPosition();
        TA_ASSERT ( SelPos != NULL, "This should not be called when nothing is selected" );

        int itemIndex = GetNextSelectedItem( SelPos );

        TA_ASSERT( itemIndex >= 0, "Must have a call selected to call this function" );

        source = Radio;
        int     trainImageNumber = 2; // train icon
        LVITEM selectedLvitem;
        selectedLvitem.iItem = itemIndex;
        selectedLvitem.iSubItem = 0;
        selectedLvitem.mask = LVIF_IMAGE;
        GetItem( &selectedLvitem );

        if ( ( selectedLvitem.iImage == trainImageNumber ) ||
             ( selectedLvitem.iImage == ( trainImageNumber + 1 ) ) )
        {
            source = Train;
        }

        FUNCTION_EXIT;
        return GetItemData( itemIndex );
    }


    void CRadioCallStackListCtrl::getSelectedCallList( std::vector<long>& callList )
    {
        FUNCTION_ENTRY( "getSelectedCallList" );

        callList.clear();

        POSITION SelPos = GetFirstSelectedItemPosition();

        while ( SelPos != NULL )
        {
            int nCol = GetNextSelectedItem( SelPos );
            callList.push_back( GetItemData( nCol ) );
        }

        FUNCTION_EXIT;
    }


    bool CRadioCallStackListCtrl::selectCall( long callId )
    {
        FUNCTION_ENTRY( "selectCall" );

        // clear any previous selection
        deselectAll();

        // Find the call in the stack
        int iPos = 0;

        while ( iPos < GetItemCount() )
        {
            // Get the stored reference
            long selectedCallID = GetItemData( iPos );

            // Is it the one we want?
            if ( selectedCallID == callId )
            {
                // select it
                SetItemState( iPos, LVIS_SELECTED, LVIS_SELECTED );

                // done
                FUNCTION_EXIT;
                return true;
            }

            ++iPos;
        }

        FUNCTION_EXIT;
        return false;
    }


    void CRadioCallStackListCtrl::deselectAll()
    {
        FUNCTION_ENTRY( "deselectAll" );

        UINT uSelectedCount = GetSelectedCount();

        if ( uSelectedCount > 0 )
        {
            // Remove all selection masks
            for ( UINT i = 0; i < uSelectedCount;i++ )
            {
                int nItem = GetNextItem( nItem, LVNI_SELECTED );

                TA_ASSERT( nItem != -1, "Operation failed" );

                SetItemState( nItem, ~LVIS_SELECTED, LVIS_SELECTED );
            }
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::processOccCallRequest( const TA_IRS_Bus::CallTypes::OccCallRequest& event )
    {
        FUNCTION_ENTRY( "processOccCallRequest" );

        TA_IRS_Bus::CallTypes::OccCallRequest* request = new TA_IRS_Bus::CallTypes::OccCallRequest( event );

        PostMessage( OCC_CALL_REQUEST_MSG, 0, reinterpret_cast< LPARAM >( request ) );

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::processOccCallReset( const TA_IRS_Bus::CallTypes::OccCallReset& event )
    {
        FUNCTION_ENTRY( "processOccCallReset" );

        TA_IRS_Bus::CallTypes::OccCallReset* reset = new TA_IRS_Bus::CallTypes::OccCallReset( event );

        PostMessage( OCC_CALL_RESET_MSG, 0, reinterpret_cast< LPARAM >( reset ) );

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event )
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );
        
        // do nothing

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::processTrainDetails( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& event )
    {
        FUNCTION_ENTRY( "processTrainDetails" );

		if (m_closeMode) {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Radio Manager is closing, no need to update train details");
			FUNCTION_EXIT;
			return;
		}
        
        // if there is an OCC call for this train the location and service numbers need to be updated

		if ( this->m_hWnd != NULL ) {
			TA_IRS_Bus::TrainInformationTypes::TrainDetails* update = new TA_IRS_Bus::TrainInformationTypes::TrainDetails( event );

			PostMessage( TRAIN_DETAILS_CHANGED_MSG, 0, reinterpret_cast< LPARAM >( update ) );
		}
        FUNCTION_EXIT;
    }

	void CRadioCallStackListCtrl::setCloseMode(bool mode)
	{
		FUNCTION_ENTRY("setCloseMode");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Radio manager in closing mode, no need to receive any more update message");
		m_closeMode = true;
		FUNCTION_EXIT;
	}

    void CRadioCallStackListCtrl::processTrainSelectionUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& event )
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );
        
        // do nothing

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        try
        {
            // The data structure you want
            TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatus = NULL;

            // Extract to the messageState
            if ( ( message.messageState >>= callStatus ) != 0 )
            {
                // process messageState now
                // Copy Message as processing will be done after function returns
                TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusEvent = new TA_Base_Bus::IRadioCorbaDef::CallStatusEvent( *callStatus );

                // Post message for processing by main GUI thread
                PostMessage( CALL_NOTIFICATION_MSG, 0, reinterpret_cast< LPARAM >( callStatusEvent ) );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Event does not contain CallStatusEvent" );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                "While processing CRadioCallStackListCtrl::receiveSpecialisedMessage" );
        }

        FUNCTION_EXIT;
    }


    BOOL CRadioCallStackListCtrl::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext )
    {
        FUNCTION_ENTRY( "Create" );

        BOOL bResult = CWnd::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );

        try
        {
            if ( bResult != 0 )
            {
                setupListControl();
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo,
                 TA_Base_Core::DebugUtil::ExceptionCatch,
                 "Unknown",
                 "While processing CRadioCallStackListCtrl::Create" );
        }

        FUNCTION_EXIT;
        return bResult;
    }


    void CRadioCallStackListCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        ListCtrlSelNoFocus::PreSubclassWindow();
        setupListControl();

        FUNCTION_EXIT;
    }


    afx_msg LRESULT CRadioCallStackListCtrl::onCallNotification( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onCallNotification" );

        TA_Base_Bus::IRadioCorbaDef::CallStatusEvent*  callStatusEvent = NULL;

        try
        {
            callStatusEvent = reinterpret_cast< TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* >( lParam );

            TA_ASSERT( NULL != callStatusEvent, "No CallStatusEvent event given" );

            processUpdate( *callStatusEvent );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                "While processing CRadioCallStackListCtrl::onCallNotification" );
        }

        delete callStatusEvent;
        callStatusEvent = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    afx_msg LRESULT CRadioCallStackListCtrl::onOccCallRequest( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onOccCallRequest" );

        TA_IRS_Bus::CallTypes::OccCallRequest* request = NULL;

        try
        {
            request = reinterpret_cast< TA_IRS_Bus::CallTypes::OccCallRequest* >( lParam );

            TA_ASSERT( NULL != request, "No OccCallRequest event given" );

            addCall( *request );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                "While processing CRadioCallStackListCtrl::onOccCallRequest" );
        }

        delete request;
        request = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    afx_msg LRESULT CRadioCallStackListCtrl::onOccCallReset( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onOccCallReset" );

        TA_IRS_Bus::CallTypes::OccCallReset* reset = NULL;

        try
        {
            reset = reinterpret_cast< TA_IRS_Bus::CallTypes::OccCallReset* >( lParam );

            TA_ASSERT( NULL != reset, "No OccCallReset event given" );

            removeCall( reset->trainId );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                "While processing CRadioCallStackListCtrl::onOccCallReset" );
        }

        delete reset;
        reset = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    afx_msg LRESULT CRadioCallStackListCtrl::onTrainDetailsChanged( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onTrainDetailsChanged" );

        TA_IRS_Bus::TrainInformationTypes::TrainDetails* update = NULL;

        try
        {
            update = reinterpret_cast< TA_IRS_Bus::TrainInformationTypes::TrainDetails* >( lParam );

            TA_ASSERT( NULL != update, "No TrainDetails event given" );

            // try to update the call if it exists
            LVFINDINFO info;

            info.flags = LVFI_PARAM;
            info.lParam = update->trainId;

            int iPosition = FindItem( &info );

            if ( -1 != iPosition )
            {
                // caller name is simply the PV/Service
                SetItemText( iPosition, 1, update->serviceNumber.c_str() );

                // TSI can also be used directly
                SetItemText( iPosition, 2, update->primaryTsi.c_str() );

                // location is a little more complicated
                std::string locationString( "???" );
                if ( 0 != update->currentLocation )
                {
                    locationString = TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( update->currentLocation );
                }
                else if ( 0 != update->communicatingLocation )
                {
                    // just need to make sure the location is not set to 'OCC' as this is not an actual location
                    // on the mainline, all others such as stations/depot are.
                    if ( TA_IRS_Bus::LocationCache::getInstance().getOccLocationKey() != update->communicatingLocation )
                    {
                        locationString = TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( update->communicatingLocation );
                    }
                }

                SetItemText( iPosition, 3, locationString.c_str() );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                "While processing CRadioCallStackListCtrl::onTrainDetailsChanged" );
        }

        delete update;
        update = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    void CRadioCallStackListCtrl::setupListControl()
    {
        FUNCTION_ENTRY( "setupListControl" );

        // Create the required columns
        InsertColumn( 1, "Date/Time" ); // Changed Aug-13-04 for TES #570
        InsertColumn( 2, "" );
        InsertColumn( 3, "" );
        InsertColumn( 4, "" );
        InsertColumn( 5, "" );
        InsertColumn( 6, "" );
        InsertColumn( 7, "" );

        // Attach the image list
        SetImageList( &m_ImageList, LVSIL_SMALL );

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::createTrainProxy()
    {
        FUNCTION_ENTRY( "createTrainProxy" );

        // get the operator session ID
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // initialise the train proxy factory (in case it isnt already)
        TA_IRS_Bus::TrainAgentProxyFactory& trainAgentProxyFactory = TA_IRS_Bus::TrainAgentProxyFactory::getInstance();
        trainAgentProxyFactory.guiInitialise( sessionId );

        // get the train proxy
        m_trainDriverCallCorbaProxy = trainAgentProxyFactory.createTrainDriverCallCorbaProxy();
        m_trainInformationCorbaProxy = trainAgentProxyFactory.createTrainInformationCorbaProxy();

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::startSubscription()
    {
        FUNCTION_ENTRY( "startSubscription" );

        if ( false == m_subscribed )
        {
            // train agent proxy does the message subscription for train updates
            // but these are only for the incoming stack
            if ( CRadioCallStackListCtrl::EStackType::IncomingStack == m_stackType )
            {
                m_trainDriverCallCorbaProxy->addObserver( this );
                m_trainInformationCorbaProxy->addObserver( this );
            }

            // Start subscription to changes
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "subscribeTo RadioCall Status: m_radioEntityKey = %d, m_radioEntityLocation = %d",
                         m_radioEntityKey, m_radioEntityLocation );

            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::RadioComms::RadioCallStatus, this,
                m_radioEntityKey, 0, m_radioEntityLocation, NULL );

            m_subscribed = true;

            // Load all call details
            loadCalls();
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::stopSubscription()
    {
        FUNCTION_ENTRY( "stopSubscription" );

        // doesnt hurt to remove a non-existent observer
        m_trainDriverCallCorbaProxy->removeObserver( this );
        m_trainInformationCorbaProxy->removeObserver( this );

        if ( true == m_subscribed )
        {
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
        }

        // always assume unsubscribed
        m_subscribed = false;

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::loadCalls()
    {
        FUNCTION_ENTRY( "loadCalls" );

        // Clear current list - might have error message in it
        if ( this->m_hWnd != NULL )
        {
            DeleteAllItems();
        }

        loadRadioCalls();

        loadTrainCalls();

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::loadRadioCalls()
    {
        FUNCTION_ENTRY( "loadRadioCalls" );

        try
        {
            TA_Base_Bus::IRadioCorbaDef::CallList_var callList;
            CORBA_CALL_RETURN( callList, m_radioAgent, getCallList, ( m_consoleId ) );//limin

            // queue each call to be added
            for ( unsigned int iCall = 0; iCall < callList->length(); ++iCall )
            {
                // Copy Message as processing will be done after function returns
                TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusEvent = new TA_Base_Bus::IRadioCorbaDef::CallStatusEvent( callList[iCall] );

                // Post message for processing by main GUI thread
                PostMessage( CALL_NOTIFICATION_MSG, 0, reinterpret_cast< LPARAM >( callStatusEvent ) );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While retreiving calls from the radio agent");
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::loadTrainCalls()
    {
        FUNCTION_ENTRY( "loadTrainCalls" );

        // only the incoming stack needs train calls
        if ( CRadioCallStackListCtrl::EStackType::IncomingStack != m_stackType )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // call requests need to be sorted by timestamp not train id
            TA_IRS_Bus::CallTypes::OccCallRequestList callRequestList = m_trainDriverCallCorbaProxy->getAllOccCallRequests();

            typedef std::map< time_t, TA_IRS_Bus::CallTypes::OccCallRequest > CallRequestsByTime;
            
            CallRequestsByTime sortedCallRequestList;
            
            for ( TA_IRS_Bus::CallTypes::OccCallRequestList::iterator callRequestIter = callRequestList.begin();
                  callRequestIter != callRequestList.end(); ++callRequestIter )
            {
                sortedCallRequestList.insert( CallRequestsByTime::value_type( callRequestIter->timestamp, *callRequestIter ) );
            }

            for ( CallRequestsByTime::iterator sortedCallRequestIter = sortedCallRequestList.begin();
                  sortedCallRequestIter != sortedCallRequestList.end(); ++sortedCallRequestIter )
            {
                processOccCallRequest( sortedCallRequestIter->second );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While retreiving calls from the train agent");
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::addCall( const TA_IRS_Bus::CallTypes::OccCallRequest& occCallRequest )
    {
        FUNCTION_ENTRY( "addCall" );

        // Only add train calls on the IncomingStack, else ignore
        if ( CRadioCallStackListCtrl::EStackType::IncomingStack != m_stackType )
        {
            FUNCTION_EXIT;
            return;
        }

        // dont add any calls if the operator can not call trains
        if ( false == m_canCallTrain )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Incoming Call Stack received OCC Call [TrainId: %d][Timestamp: %d]",
                     occCallRequest.trainId, occCallRequest.timestamp );

        // try to update the call if it exists
        // otherwise insert a new item
        LVFINDINFO info;

        info.flags = LVFI_PARAM;
        info.lParam = occCallRequest.trainId;

        int iPosition = FindItem( &info );

        if ( -1 == iPosition )
        {
            // new item, insert it
            int imageNumber = 2; // train icon
            iPosition = InsertItem( 0, getLocalTimeFrom( occCallRequest.timestamp ).c_str(), imageNumber );

            // set the item data to the train ID
            SetItemData( iPosition, occCallRequest.trainId );
        }

        // populate caller name, tsi, location and status
        // need the train details for this, so get them from the search page

        TA_IRS_Bus::CSearchPage* searchPage = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage();

        try
        {
            TA_IRS_Bus::TrainInformationTypes::TrainDetails trainDetails = searchPage->getTrainDetailsFromTrainID( occCallRequest.trainId );

            // caller name is simply the PV/Service
            SetItemText( iPosition, 1, trainDetails.serviceNumber.c_str() );

            // TSI can also be used directly
            SetItemText( iPosition, 2, trainDetails.primaryTsi.c_str() );

            // location is a little more complicated
            std::string locationString( "???" );
            if ( 0 != trainDetails.currentLocation )
            {
                locationString = TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainDetails.currentLocation );
            }
            else if ( 0 != trainDetails.communicatingLocation )
            {
                // just need to make sure the location is not set to 'OCC' as this is not an actual location
                // on the mainline, all others such as stations/depot are.
                if ( TA_IRS_Bus::LocationCache::getInstance().getOccLocationKey() != trainDetails.communicatingLocation )
                {
                    locationString = TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainDetails.communicatingLocation );
                }
            }

            SetItemText( iPosition, 3, locationString.c_str() );
        }
        catch( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            CString callerName;
            callerName.Format( "PV%02d", occCallRequest.trainId );

            SetItemText( iPosition, 1, callerName );
            SetItemText( iPosition, 2, "" );
            SetItemText( iPosition, 3, "???" );
        }

        SetItemText( iPosition, 4, "CallBack" );

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::processUpdate( TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatusEvent )
    {
        FUNCTION_ENTRY( "processUpdate" );

        switch ( m_stackType )
        {
            case CRadioCallStackListCtrl::EStackType::OthersStack:
            {
                // Check if call belongs to this call stack
                if ( callStatusEvent.consoleKey != m_consoleId )
                {
                    // Add, Update or remove the call
                    if ( TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing == callStatusEvent.callStatus )
                    {
                        addCall( callStatusEvent.callID );
                    }
                    else if ( TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Deleted != callStatusEvent.callStatus )
                    {
                        updateCall( callStatusEvent.callID, callStatusEvent.callStatus );
                    }
                    else
                    {
                        removeCall( callStatusEvent.callID );
                    }
                }
            }
            break;

            case CRadioCallStackListCtrl::EStackType::IncomingStack:
            {
                if ( callStatusEvent.consoleKey == m_consoleId )
                {
                    if ( TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing == callStatusEvent.callStatus )
                    {
                        addCall( callStatusEvent.callID );
                    }
                    else
                    {
                        // If its there remove it
                        removeCall( callStatusEvent.callID );
                    }
                }
            }
            break;

            case CRadioCallStackListCtrl::EStackType::HeldStack:
            {
                // Check if call belongs to this call stack
                if ( callStatusEvent.consoleKey == m_consoleId )
                {
                    if ( TA_Base_Bus::IRadioCorbaDef::ECallStatusType::OnHold == callStatusEvent.callStatus)
                    {
                        addCall( callStatusEvent.callID );
                    }
                    else
                    {
                        removeCall( callStatusEvent.callID );
                    }
                }
            }
            break;

            default:
                TA_ASSERT( false, "Invalid call stack type" );
                break;
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::addCall( long callID )
    {
        FUNCTION_ENTRY( "addCall" );

        TA_Base_Bus::IRadioCorbaDef::CallDetailsType_var callDetails;
        int iPosition;
        int imageNumber;

        // Get the call details
        try
        {
            CORBA_CALL_RETURN( callDetails, m_radioAgent, getCallDetails, ( callID, m_consoleId ) );//limin

            switch ( callDetails->callCategory )
            {

                case TA_Base_Bus::IRadioCorbaDef::ECallCategory::TextMessage:
                    imageNumber = 0;
                    break;

                case TA_Base_Bus::IRadioCorbaDef::ECallCategory::CallDriver:
                    imageNumber = 2;
                    break;

                case TA_Base_Bus::IRadioCorbaDef::ECallCategory::FullDuplex:
                    imageNumber = 4;
                    break;

                case TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex:
                    imageNumber = 6;
                    break;

                default:
                    imageNumber = -1;
            }

            if ( ( imageNumber >= 0 ) &&
                 ( true == callDetails->isEmergency ) )
            {
                ++imageNumber;
            }

            if ( -1 == imageNumber )
            {
                iPosition = InsertItem( 0, callDetails->callTime );
            }
            else
            {
                iPosition = InsertItem( 0, callDetails->callTime, imageNumber );
            }

            SetItemData( iPosition, callDetails->callID );

            CString callStatusStr = getStatusText( callDetails->callStatus ).c_str();

            if ( m_stackType == CRadioCallStackListCtrl::EStackType::OthersStack )
            {
                // Add to the list control
                SetItemText( iPosition, 1, callDetails->callerName.in() );
                SetItemText( iPosition, 2, callDetails->callerLocation.in() );
                SetItemText( iPosition, 3, callDetails->calledName.in() );
                SetItemText( iPosition, 4, callDetails->calledLocation.in() );
                SetItemText( iPosition, 5, callStatusStr );
            }
            else
            {
                // Add to the list control
                SetItemText( iPosition, 1, callDetails->callerName.in() );
                SetItemText( iPosition, 2, callDetails->callerTSI.in() );
                SetItemText( iPosition, 3, callDetails->callerLocation.in() );
                SetItemText( iPosition, 4, callStatusStr );
            }

            // TES #492 - emergency calls should be in red
            if ( callDetails->isEmergency )
            {
                setItemColour( callDetails->callID,
                               EMERGENCY_TEXT_COLOUR,
                               TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                               TA_Base_Bus::ListCtrlSelNoFocus::CT_FOREGROUND );
            }
            else
            {
                // Just in case any colour settings managed to stay present from
                // old items (shouldn't be able to happen), clear any item data
                //  colour settings for the normal calls
                try
                {
                    clearItemColour( callDetails->callID, TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA );
                }
                catch ( ... )
                {
                }
            }
        }
        catch ( TA_Base_Bus::IRadioCorbaDef::invalidCallIDException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo,
                "TA_Base_Bus::IRadioCorbaDef::invalidCallIDException", "in CRadioCallStackListCtrl::addCall" );

            // Cannot get call details - ignore the call as it must have been deleted
            // by another user between getList and this call.
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "in CRadioCallStackListCtrl::addCall" );
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::updateCall( long callID, TA_Base_Bus::IRadioCorbaDef::ECallStatusType status )
    {
        FUNCTION_ENTRY( "updateCall" );

        // Find the call in the stack
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = callID;

        int iPos = FindItem( &info );

        if ( -1 != iPos )
        {
            CString callStatusStr = getStatusText( status ).c_str();

            // Update the status

            if ( m_stackType == CRadioCallStackListCtrl::EStackType::OthersStack )
            {
                SetItemText( iPos, 5, callStatusStr );
            }
            else
            {
                SetItemText( iPos, 4, callStatusStr );
            }
        }

        FUNCTION_EXIT;
    }


    void CRadioCallStackListCtrl::removeCall( long callID )
    {
        FUNCTION_ENTRY( "removeCall" );

        try
        {
            // Find the call in the stack
            LVFINDINFO info;
            info.flags = LVFI_PARAM;
            info.lParam = callID;

            int iPos = FindItem( &info );

            if ( -1 != iPos )
            {
                // Clear any possible colour mappings for item prior to removal
                // (these mappings will only exist for emergency calls)
                try
                {
                    clearItemColour( callID, TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA );
                }
                catch ( ... )
                {
                }

                // Remove from list
                DeleteItem( iPos );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While processing CRadioCallStackListCtrl::removeCall" );
        }

        FUNCTION_EXIT;
    }


    std::string CRadioCallStackListCtrl::getStatusText( TA_Base_Bus::IRadioCorbaDef::ECallStatusType status ) const
    {
        FUNCTION_ENTRY( "getStatusText" );

        switch ( status )
        {
            case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing:
                FUNCTION_EXIT;
                return "Ringing";

            case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Connected:
                FUNCTION_EXIT;
                return "Connected";

            case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Disconnected:
                FUNCTION_EXIT;
                return "Disconnected";

            case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::OnHold:
                FUNCTION_EXIT;
                return "On hold";

            default:
                break;
        }

        FUNCTION_EXIT;
        return "Unknown";
    }


}

namespace
{

    static std::string getLocalTimeFrom( time_t ltime )
    {
        char szBuffer[100];
        struct tm today;

        ACE_OS::localtime_r( &ltime, &today );

        strftime( szBuffer, sizeof( szBuffer ), "%d-%b %H:%M:%S", &today );
        return std::string( szBuffer );
    }

}