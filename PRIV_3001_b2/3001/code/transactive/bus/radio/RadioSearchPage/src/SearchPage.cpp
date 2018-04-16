/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSearchPage/src/SearchPage.cpp $
  * @author Nick Jardine / Andy Parker
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  *
  * Provides specialised search functionality for subscribers in radio directory
  *
  */

#pragma warning(disable: 4786 4284)  // identifier truncated to 255 characters, UDT

#include "bus/radio/radiosearchpage/src/stdafx.h"

#include "bus/radio/radiosearchpage/src/resource.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/radiosearchpage/src/SearchPage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/message/types/RadioComms_MessageTypes.h"

#include <iomanip>
#include <sstream>


#define WM_SUBSCRIBER_UPDATE ( WM_APP + 1 )
#define WM_SELECTION_UPDATE ( WM_APP + 2 )

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[]  = __FILE__;
#endif

namespace TA_IRS_Bus
{
    IMPLEMENT_DYNCREATE( CSearchPage,
                         CDialog )

    const int CSearchPage::TYPE_TRAIN               = 0;
    const int CSearchPage::TYPE_INDIVIDUAL          = 1;
    const int CSearchPage::TYPE_GROUP               = 2;
    const int CSearchPage::TYPE_DYNAMIC_GROUP       = 3;

    ISearchPageHelper* CSearchPage::s_dummyHelper   = 0;

    //TD8844 begin
    // HTREEITEM m_trainOutItem;
    // HTREEITEM m_trainInItem;
    //TD8844 begin

    CSearchPage::CSearchPage()
        : m_callbackHelper( *s_dummyHelper ),
          m_messageSubscriber( 0 ),
          m_hItemUsers( 0 ),
          m_hTreeItemOcc( 0 ),
          m_hTreeItemDepot( 0 ),
          m_hTreeItemStation( 0 ),
          m_trainItem( 0 ),
          m_hGroupsItem( 0 ),
          m_hDynamicGroupsItem( 0 ),
          m_SortInfo( COL_MAX ),
          m_SearchList( COL_MAX )
    {
        FUNCTION_ENTRY( "CSearchPage" );

        initSortInfo();  //TD17304
        m_SearchList.setSortInfo( m_SortInfo );//TD17304

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // CSearchPage property page
    CSearchPage::CSearchPage( ISearchPageHelper& helper )
        : m_callbackHelper( helper ),
          m_messageSubscriber( 0 ),
          m_hItemUsers( 0 ),
          m_hTreeItemOcc( 0 ),
          m_hTreeItemDepot( 0 ),
          m_hTreeItemStation( 0 ),
          m_trainItem( 0 ),
          m_hGroupsItem( 0 ),
          m_hDynamicGroupsItem( 0 ),
          m_SortInfo( COL_MAX ),
          m_SearchList( COL_MAX ),
          CDialog( CSearchPage::IDD )
    {
        FUNCTION_ENTRY( "CSearchPage" );

        //{{AFX_DATA_INIT(CSearchPage)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        initSortInfo(); //TD17304
        m_SearchList.setSortInfo( m_SortInfo );//TD17304

        FUNCTION_EXIT;
    }


    CSearchPage::~CSearchPage()
    {
        FUNCTION_ENTRY( "~CSearchPage" );

        // Subscriber should be destroyed in OnDestroy()
        TA_ASSERT( 0 == m_messageSubscriber, "Invalid cleanup" );

        FUNCTION_EXIT;
    }


    void CSearchPage::DoDataExchange( CDataExchange* pDX )
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(CSearchPage)
        DDX_Control(pDX, IDC_SUBSCRIBERSEARCH, m_searchButton);
        DDX_Control(pDX, IDC_EDIT1, m_searchText);
        DDX_Control(pDX, IDC_LIST1, m_SearchList);
        DDX_Control(pDX, IDC_TREE1, m_DirectoryTree);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CSearchPage, CDialog)
        //{{AFX_MSG_MAP(CSearchPage)
        ON_BN_CLICKED(IDC_SUBSCRIBERSEARCH, OnSubscribersearch)
        ON_BN_CLICKED(IDC_ADD_MONITOR, OnAddToMonitor)
        ON_BN_CLICKED(IDC_BUTTON2, OnDisplayGroupDetails)
        ON_BN_CLICKED(ID_OK, OnOK)
        ON_BN_CLICKED(IDC_BUTTON3, OnAddToDynamicGroup)
        ON_MESSAGE(WM_SUBSCRIBER_UPDATE, OnSubscriberNotification)
        ON_MESSAGE(WM_SELECTION_UPDATE, OnSelectionChanged)
        ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkSearchList)
        ON_WM_DESTROY()
        ON_EN_CHANGE(IDC_EDIT1, OnChangeEditSearchText)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void CSearchPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );

        // Do nothing

        FUNCTION_EXIT;
    }


    BOOL CSearchPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        // There are three levels - OCC, DPT, and Stations
        // Create the stations here
        m_hItemUsers = m_DirectoryTree.InsertItem( "Users" );

        m_hTreeItemStation = m_DirectoryTree.InsertItem( "Stations/others", m_hItemUsers ); //zhongjie++ CL18774

        m_trainItem = m_DirectoryTree.InsertItem( "Trains" );
        m_hGroupsItem = m_DirectoryTree.InsertItem( "Groups" );

        //m_SearchList.InsertColumn(0,"User" );
        //m_SearchList.InsertColumn(1,"Number");
        m_SearchList.InsertColumn( COL_ID, "ID" );
        m_SearchList.InsertColumn( COL_TSI, "TSI" );
        m_SearchList.InsertColumn( COL_LOCATION, "Loc" );

        m_SearchList.SetColumnWidth( COL_ID, 150 );
        m_SearchList.SetColumnWidth( COL_TSI, 150 );
        m_SearchList.SetColumnWidth( COL_LOCATION, 10 );

        //////////////////
        // TES #422, provide tool tips - no header resize, and fill out header width to scrollbar position
        m_SearchList.SetColumnWidth( COL_LOCATION, LVSCW_AUTOSIZE_USEHEADER );
        // Unsure why the Width below was calculated in this way, as the size of the scrollbar will be
        // already be reduced by a reduction in the client area for the list control. This now causes a duplication
        // of the scrollbar width to be deducted causing an unused section in the listbox.
        //m_SearchList.SetColumnWidth(COL_LOCATION, m_SearchList.GetColumnWidth(COL_LOCATION)  - GetSystemMetrics(SM_CXVSCROLL));

        m_SearchList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        m_fixedHeaderCtrl.subclassHeader( m_SearchList.GetHeaderCtrl() );
        //////////////////
        FUNCTION_EXIT;
        return FALSE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CSearchPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        try
        {
            m_messageSubscriber = new TA_IRS_Bus::SimpleMessageSubscriber<TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification>( TA_Base_Core::RadioComms::RadioDirectoryNotification,
                                                                                                                                       WM_SUBSCRIBER_UPDATE,
                                                                                                                                       m_callbackHelper.getEntityLocationKey() );

            m_messageSubscriber->addListenerWindow( m_hWnd, true );
        }
        catch ( ... )
        {
            FUNCTION_EXIT;
            return;
        }

        // Load the directory tree

        // Get the parent class and retrieve the reference to the readio manager
        //CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetMainWnd());

        TA_Base_Bus::IRadioCorbaDef::SubscriberList_var subscribers;
        TA_Base_Bus::IRadioCorbaDef::SubscriberList_var groups;
        try
        {
            CORBA_CALL_RETURN( subscribers, m_callbackHelper.getRadioCorbaDef(), getIndividualSubscribers, () );//limin
            CORBA_CALL_RETURN( groups, m_callbackHelper.getRadioCorbaDef(), getGroupSubscribers, () );//limin
        }
        catch ( const CORBA::Exception& )
        {
            //ex.what();
            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
            FUNCTION_EXIT;
            return;
        }
        catch ( ... )
        {
            FUNCTION_EXIT;
            return;
        }

		//This component is ignored already since it is not been used in C955 Project as suggested by Bihui.
		//Commented to fix build error, getDepotLocationKey interface has not been used already due to multiple depot locations.
        //m_depotKey.setValue( LocationCache::getInstance().getDepotLocationKey() );

        std::string depotName = LocationCache::getInstance().getLocationByKey( m_depotKey.getValue() ); //zhongjie++


        m_occKey.setValue( LocationCache::getInstance().getOccLocationKey() );
        const std::string occName   = LocationCache::getInstance().getLocationByKey( m_occKey.getValue() );

        m_hTreeItemOcc = m_DirectoryTree.InsertItem( occName.c_str(), m_hItemUsers );
	
		if (depotName=="KCD")    //zhongjie++ CL 18774
		{
			depotName= "DCC";
		}
        m_hTreeItemDepot = m_DirectoryTree.InsertItem( depotName.c_str(), m_hItemUsers );

        unsigned int i = 0;
        for ( i = 0; i < subscribers->length(); i++ )
        {
            addNewSubscriber( subscribers[i] );
        }

        for ( i = 0; i < groups->length(); ++i )
        {
            addNewSubscriber( groups[i] );
        }

        if ( m_callbackHelper.shouldDisplayDynamicGroupNode() )
        {
            m_hDynamicGroupsItem = m_DirectoryTree.InsertItem( "Dynamic Group" );
        }

        // Register to retrieve train information
        m_callbackHelper.getTrainSelectorGUI().addCallbackWindow( this );

        // Setup the search results

        try
        {
            TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, RPARAM_SESSIONID );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "While processing registerRunParamUser" );
        }

        onUpdateRights();

        refreshLastUpdateField();

        FUNCTION_EXIT;
    }


    void CSearchPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        try
        {
            if ( 0 != m_messageSubscriber )
            {
                m_messageSubscriber->removeListenerWindow( m_hWnd );

                delete m_messageSubscriber;
                m_messageSubscriber = 0;
            }

            TA_Base_Core::RunParams::getInstance().deregisterRunParamUser( this );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "While processing CSearchPage::OnDestroy" );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::onUpdateRights()
    {
        FUNCTION_ENTRY( "onUpdateRights" );

        //CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetMainWnd());
        try
        {
            bool itemsSelected          = ( m_SearchList.GetSelectedCount() > 0 );

            bool enableGroupExpandButton= false;

            // Only enable group expand button if single item selected
            // (to remove ambiguity from operator - if multiple selections, may not be clear
            // which group we're expanding..)
            if ( 1 == m_SearchList.GetSelectedCount() )
            {
                int nItem   = m_SearchList.GetNextItem( -1, LVNI_SELECTED );

                if ( nItem >= 0 && TYPE_GROUP == m_SearchList.GetItemData( nItem ) )
                {
                    enableGroupExpandButton = true;
                }
            }

            // Clicked notification - just use currently selected item
            HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();
            if ( selectedItem == m_hDynamicGroupsItem )
            {
                m_searchButton.EnableWindow( FALSE );
                m_searchText.EnableWindow( FALSE );
            }
            else
            {
                m_searchText.EnableWindow( m_callbackHelper.isFunctionPermitted( TA_Base_Bus::aca_RADIO_SEARCH_DIRECTORY ) );
            }


            updateSearchTextButtonEnabledState( true );

            GetDlgItem( IDC_BUTTON2 )->EnableWindow( enableGroupExpandButton );
            DWORD addButtonRightId  = m_callbackHelper.getAddButtonRightIdentifier();
            GetDlgItem( IDC_BUTTON3 )->EnableWindow( itemsSelected &&
                                                     m_callbackHelper.isFunctionPermitted( addButtonRightId ) );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "While processing CSearchPage::onUpdateRights" );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::OnSubscribersearch()
    {
        FUNCTION_ENTRY( "OnSubscribersearch" );

        CString searchText;
        int iRet;
        int i;

        m_searchText.GetWindowText( searchText );
        if ( m_SearchList.m_hWnd != NULL )
        {
            m_SearchList.DeleteAllItems();
        }
        bool usersSelected  = hasSelectedUsersLevelItem();
        bool groupsSelected = hasSelectedGroupsLevelItem();

        if ( usersSelected || groupsSelected )
        {
            TA_Base_Bus::IRadioCorbaDef::SubscriberList_var searchResults;

            try
            {
                CORBA_CALL_RETURN( searchResults, m_callbackHelper.getRadioCorbaDef(), searchSubscriber, ( searchText ) );//limin
            }
            catch ( const CORBA::Exception& e )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA::Exception",
                     "While processing CSearchPage::OnSubscribersearch" );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                             TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::ObjectResolutionException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "ObjectResolutionException",
                     "While processing CSearchPage::OnSubscribersearch" );

                FUNCTION_EXIT;
                return;
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                     "While processing CSearchPage::OnSubscribersearch" );

                FUNCTION_EXIT;
                return;
            }

            // Filter by users and groups
            if ( usersSelected )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                     "Will filter returned subscribers by users" );
                HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();
                bool doInsert;

                for ( i = 0; i < searchResults->length(); i++ )
                {
                    if ( searchResults[i].type == TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER )
                    {
                        doInsert = false;

                        // Filter by location
                        if ( selectedItem == m_hItemUsers )
                        {
                            // Insert all
                            doInsert = true;
                        }
                        else if ( selectedItem == m_hTreeItemOcc )
                        {
                            // Insert only Occ users
                            if ( searchResults[i].locationKey == m_occKey.getValue() )
                            {
                                doInsert = true;
                            }
                        }
                        else if ( selectedItem == m_hTreeItemDepot )
                        {
                            // Insert only Depot users
                            if ( searchResults[i].locationKey == m_depotKey.getValue() )
                            {
                                doInsert = true;
                            }
                        }
                        else if ( selectedItem == m_hTreeItemStation )
                        {
                                 // Insert only station users
                            if ( searchResults[i].locationKey != m_occKey.getValue() &&
                                 searchResults[i].locationKey != m_depotKey.getValue() )
                            {
                                doInsert = true;
                            }
                        }
                             //TD19511
                             //else if (selectedItem == m_stationLocationMap[searchResults[i].locationKey])
                        else if ( selectedItem == m_stationLocationMap[std::string( searchResults[i].locationName )] )
                        {
                            // Selected level was a station so only insert that specific station users
                            doInsert = true;
                        }

                        if ( doInsert )
                        {
                            iRet = m_SearchList.InsertItem( i, searchResults[i].subscriberName );
                            m_SearchList.SetItemText( iRet, 1, searchResults[i].TSI );
                            m_SearchList.SetItemText( iRet, 2, searchResults[i].locationName );
                            m_SearchList.SetItemData( iRet, TYPE_INDIVIDUAL );
                        }
                    }
                }
            }
            else if ( groupsSelected )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                     "Will filter returned subscribers by group" );
                for ( i = 0; i < searchResults->length(); ++i )
                {
                    if ( searchResults[i].type == TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER )
                    {
                        iRet = m_SearchList.InsertItem( i, searchResults[i].subscriberName );
                        m_SearchList.SetItemText( iRet, 1, searchResults[i].TSI );
                        m_SearchList.SetItemText( iRet, 2, searchResults[i].locationName );
                        m_SearchList.SetItemData( iRet, TYPE_GROUP );
                    }
                }
            }
        }
        else if ( hasSelectedTrainsLevelItem() )
        {
            try
            {
                i = 0;

                // Cycle through the list and match search string with the train service number
                for ( TrainITSIMap::iterator itr = m_trainITSIs.begin(); itr != m_trainITSIs.end(); itr++ )
                {
                    CString subscriberName  = ( itr->second ).name;

                    if ( subscriberName.Find( searchText ) != -1 )
                    {
                        iRet = m_SearchList.InsertItem( i, subscriberName );
                        m_SearchList.SetItemText( iRet, 1, ( itr->second ).ITSI );
                        m_SearchList.SetItemText( iRet, 2, getLocationNameFromKey( ( itr->second ).locationKey ) );
                        m_SearchList.SetItemData( iRet, TYPE_TRAIN );
                        i++;
                    }
                }
            }
            catch ( ... )
            {
            }
        }

        onUpdateRights();

        FUNCTION_EXIT;
    }


    void CSearchPage::populateSearchListWithGroup( CString groupTSI,
                                                   bool bClearItems /* = true */ )
    {
        FUNCTION_ENTRY( "populateSearchListWithGroup" );

        TA_Base_Bus::IRadioCorbaDef::SubscriberList_var groupMembers;
        try
        {
            CORBA_CALL_RETURN( groupMembers, m_callbackHelper.getRadioCorbaDef(), getGroupMembers, ( groupTSI ) );//limin
            // Clear the list
            if ( bClearItems && m_SearchList.m_hWnd != NULL ) // TD14417
            {
                m_SearchList.DeleteAllItems();
            }

            for ( unsigned int i = 0; i < groupMembers->length(); i++ )
            {
                int iRet;
                iRet = m_SearchList.InsertItem( i, groupMembers[i].subscriberName );
                m_SearchList.SetItemText( iRet, 1, groupMembers[i].TSI );
                m_SearchList.SetItemText( iRet, 2, groupMembers[i].locationName );
                m_SearchList.SetItemData( iRet, TYPE_GROUP );
            }
        }
        catch ( const CORBA::Exception& e )
        {
            //ex.what();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                         TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
        }
        catch ( ... )
        {
        }

        FUNCTION_EXIT;
    }


    CString CSearchPage::getLocationNameFromKey( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getLocationNameFromKey" );

        //Use LocationCache to get the location name
        try
        {
            FUNCTION_EXIT;
            return LocationCache::getInstance().getLocationByKey( locationKey ).c_str();
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "getLocationNameFromKey()" );
        }

        FUNCTION_EXIT;
        return "";
    }


    void CSearchPage::populateSearchListWithTSI( CString itemTSI,
                                                 HTREEITEM parentItem,
                                                 HTREEITEM selectedItem,
                                                 bool bClearItems /* = true */ )
    {
        FUNCTION_ENTRY( "populateSearchListWithTSI" );

        try
        {
            bool isTrain            = false;
            CString subscriberName;
            CString subscriberTsi   = itemTSI;    // Usually (but not always) can remain unchanged from input TSI
            CString subscriberLocation;

            try
            {
                // First must identify whether we're dealing with a train or normal subscriber
                if ( parentItem == m_trainItem )
                {
                    isTrain = true;
                }
                else if ( parentItem == m_hDynamicGroupsItem )
                {
                    // Dynamic group section can contain a mixture of trains and normal subscribers
                    try
                    {
                        TA_IRS_Bus::TrainInformationTypes::TrainDetails details = getTrainDetailsFromTSI( itemTSI );
                        itemTSI = details.primaryTsi.c_str();
                    }
                    catch ( ... )
                    {
                    }
                }

                if ( isTrain )
                {
                    // Take the train details straight from the train agent
                    DWORD trainId                                           = m_DirectoryTree.GetItemData( selectedItem );
                    TA_IRS_Bus::TrainInformationTypes::TrainDetails details = getTrainDetailsFromTrainID( static_cast<BYTE>( trainId ) );

                    // If a train subscriber, must obtain details from alternate source
                    // (as radio directory only synchronises every 15 minutes, so location will be lagged)
                    // - in theory getSubscriberDetails will work (just not updated as frequently)
                    subscriberName = getTrainServiceNumber( details );
                    subscriberTsi = details.primaryTsi.c_str();
                    subscriberLocation = getLocationNameFromKey( details.currentLocation );

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "getTrainDetailsFromTSI() return : subscriberName = %s, subscriberTsi = %s, subscriberLocation = %s",
                                 subscriberName, subscriberTsi, subscriberLocation );
                }
                else
                {
                    TA_Base_Bus::IRadioCorbaDef::SubscriberDetails_var subscriber;
                    CORBA_CALL_RETURN( subscriber, m_callbackHelper.getRadioCorbaDef(), getSubscriberDetails, ( itemTSI ) );//limin
                    subscriberName = subscriber->subscriberName;
                    subscriberTsi = subscriber->TSI;
                    subscriberLocation = subscriber->locationName;
                }
            }
            catch ( const TA_Base_Core::TransactiveException& e )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", e.what() );
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Obtaining subscriber details" );
            }

            // Clear the list
            if ( bClearItems && m_SearchList.m_hWnd != NULL ) // TD14417
            {
                m_SearchList.DeleteAllItems();
            }

            if ( doesItemExistInSearchList( subscriberName ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "item exists in search list, ignore it." );

                FUNCTION_EXIT;
                return;
            }

            int iRet;
            //TD17287++
            iRet = m_SearchList.GetItemCount();
            iRet = m_SearchList.InsertItem( iRet, "" );
            //iRet = m_SearchList.InsertItem(0, "");
            //++TD17287
            m_SearchList.SetItemText( iRet, COL_ID, subscriberName );
            m_SearchList.SetItemText( iRet, COL_TSI, subscriberTsi );
            m_SearchList.SetItemText( iRet, COL_LOCATION, subscriberLocation );

            if ( parentItem == m_trainItem )
            {
                m_SearchList.SetItemData( iRet, TYPE_TRAIN );
            }
            else if ( parentItem == m_hDynamicGroupsItem )
            {
                m_SearchList.SetItemData( iRet, TYPE_DYNAMIC_GROUP );
            }
            else if ( parentItem == m_hGroupsItem )//TD19799
            {
                m_SearchList.SetItemData( iRet, TYPE_GROUP );
            }//TD19799
            else
            {
                m_SearchList.SetItemData( iRet, TYPE_INDIVIDUAL );
            }
        }
        catch ( const CORBA::Exception& e )
        {
            //ex.what();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                         TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
        }
        catch ( ... )
        {
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::populateSearchListWithDynamicGroup()
    {
        FUNCTION_ENTRY( "populateSearchListWithDynamicGroup" );

        try
        {
            // Clear the list
            if ( m_SearchList.m_hWnd != NULL )
            {
                m_SearchList.DeleteAllItems();
            }

            for ( DynamicGroupITSIMap::iterator it = m_dynamicGroupITSIs.begin();
                  it != m_dynamicGroupITSIs.end();
                  ++it )
            {
                TA_Base_Bus::IRadioCorbaDef::SubscriberDetails_var subscriber ;
                CORBA_CALL_RETURN( subscriber, m_callbackHelper.getRadioCorbaDef(), getSubscriberDetails, ( it->second.tsi ) );//limin
                int iRet= m_SearchList.InsertItem( m_SearchList.GetItemCount(), subscriber->subscriberName );
                m_SearchList.SetItemText( iRet, 1, subscriber->TSI );
                m_SearchList.SetItemText( iRet, 2, subscriber->locationName );
                m_SearchList.SetItemData( iRet, TYPE_DYNAMIC_GROUP );
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "While processing (2) CSearchPage::OnSubscribersearch(" );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::OnDisplayGroupDetails()
    {
        FUNCTION_ENTRY( "OnDisplayGroupDetails" );

        int item= m_SearchList.GetNextItem( -1, LVNI_SELECTED );

        if ( item < 0 )
        {
            FUNCTION_EXIT;
            return;
        }

        // Need to get the TSI for the selected group first.
        CString tsi = m_SearchList.GetItemText( item, 1 );

        // Clear the list
        if ( m_SearchList.m_hWnd != NULL )
        {
            m_SearchList.DeleteAllItems();
        }

        // Get the main window
        //CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetMainWnd());

        TA_Base_Bus::IRadioCorbaDef::SubscriberList_var groupMembers;
        try
        {
            CORBA_CALL_RETURN( groupMembers, m_callbackHelper.getRadioCorbaDef(), getGroupMembers, ( tsi ) );//limin
            for ( unsigned int i = 0; i < groupMembers->length(); i++ )
            {
                int iRet;
                if ( !doesItemExistInSearchList( groupMembers[i].subscriberName.in() ) )
                {
                    iRet = m_SearchList.InsertItem( i, groupMembers[i].subscriberName );
                    m_SearchList.SetItemText( iRet, 1, groupMembers[i].TSI );
                    m_SearchList.SetItemText( iRet, 2, groupMembers[i].locationName );
                    m_SearchList.SetItemData( iRet, TYPE_INDIVIDUAL );
                }
            }
        }
        catch ( const CORBA::Exception& e )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA::Exception",
                 "While processing CSearchPage::OnDisplayGroupDetails" );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                         TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "ObjectResolutionException",
                 "While processing CSearchPage::OnDisplayGroupDetails" );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "While processing CSearchPage::OnDisplayGroupDetails" );
        }

        onUpdateRights();

        FUNCTION_EXIT;
    }


    BOOL CSearchPage::OnNotify( WPARAM wParam,
                                LPARAM lParam,
                                LRESULT* pResult )
    {
        FUNCTION_ENTRY( "OnNotify" );

        LPNMHDR lpnm= LPNMHDR( lParam );

        // Selection changed on tree control, or item clicked on
        if ( lpnm->code == TVN_SELCHANGED || ( lpnm->code == NM_CLICK && lpnm->idFrom == IDC_TREE1 ) )
        {
            // Request an update as selection has changed
            MSG msg;
            if ( !PeekMessage( &msg, m_hWnd, WM_SELECTION_UPDATE, WM_SELECTION_UPDATE, PM_NOREMOVE ) )
            {
                // Only post if not already posted - we post a message instead of doing the
                // processing directly to delay slightly (so that two notifications blend into the one -
                // the possible combination of a click and selchanged)
                PostMessage( WM_SELECTION_UPDATE, 0, 0 );
                onUpdateRights();
            }
        }
        else if ( lpnm->code == NM_CLICK )
        {
            // Need to use click monitoring on the list because it doesn't have a SELCHANGED notification
            // But because the tree will also deliver clicks, need to make sure this comes only from the list
            if ( lpnm->idFrom == IDC_LIST1 )
            {
                LPNMITEMACTIVATE pClick = ( LPNMITEMACTIVATE ) lParam;

                if ( pClick->iItem != -1 )
                {
                    CString itsi= m_SearchList.GetItemText( pClick->iItem, 1 );
                    CString name= m_SearchList.GetItemText( pClick->iItem, 0 );
                    m_callbackHelper.SetCall( itsi, name, false, ISearchPageHelper::USER );
                }
                onUpdateRights();
            }
        }

        FUNCTION_EXIT;
        return CDialog::OnNotify( wParam, lParam, pResult );
    }


    LRESULT CSearchPage::OnSelectionChanged( UINT wParam,
                                             LONG lParam )
    {
        FUNCTION_ENTRY( "OnSelectionChanged" );

        // Clicked notification - just use currently selected item
        HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();

        if ( 0x0 == selectedItem )
        {
            onUpdateRights();

            FUNCTION_EXIT;
            return 0;
        }

        try
        {
            if ( selectedItem == m_hDynamicGroupsItem )
            {
                if ( m_SearchList.m_hWnd != NULL )
                {
                    m_SearchList.DeleteAllItems(); // TD14417
                }

                // TD#2676 - enable dynamic group functionality, set it up for calling
                CString dynamicGroupTSI ( "" );
                m_callbackHelper.SetCall( dynamicGroupTSI, m_DirectoryTree.GetItemText( selectedItem ), false,
                                          ISearchPageHelper::DYNAMIC_GROUP_ROOT );
                // Don't populate the search list (just allow selection of dynamic group as call)
                //populateSearchListWithDynamicGroup();
                m_searchButton.EnableWindow( FALSE );

                m_searchText.EnableWindow( FALSE );
            }
            else if ( m_DirectoryTree.ItemHasChildren( selectedItem ) ==
                      0/* || m_DirectoryTree.GetParentItem(selectedItem) == m_hTreeItemStation*/ )
            {
                // Get the parent item to find out if this is a blank user sub-item (i.e. not actually a user)
                HTREEITEM parentItem= m_DirectoryTree.GetParentItem( selectedItem );

                // m_hTreeItemStation nodes may sometimes have no children -
                // whether or not it does, we must ignore these selections)
                if ( parentItem == m_hTreeItemStation )
                {
                    populateSearchListWithStation( selectedItem );

                    FUNCTION_EXIT;
                    return 0;
                }

                // It's a blank user sub-tiem if the parent item is named "Users" and has a parent that is the root item
                //if( (parentItem == m_hItemUsers && m_DirectoryTree.GetParentItem(parentItem) == NULL) ||
                //    parentItem == NULL)
                //lichao++
                if ( ( ( parentItem == m_hItemUsers ) && m_DirectoryTree.GetParentItem( parentItem ) == NULL ) ||
                     parentItem == NULL )
                {
                    // In this situation, we don't want to do anything on the click, so stop here.
                    if ( m_SearchList.m_hWnd != NULL )
                    {
                        m_SearchList.DeleteAllItems(); //TD14417
                    }

                    FUNCTION_EXIT;
                    return 0;
                }

                CString tsi;


                if ( parentItem == m_trainItem )
                {
                    // user selected a train
                    DWORD trainId               = m_DirectoryTree.GetItemData( selectedItem );
                    TrainITSIMap::iterator itr  = m_trainITSIs.find( ( unsigned char ) trainId );
                    ASSERT( itr != m_trainITSIs.end() );
                    tsi = itr->second.ITSI;
                }
                else if ( parentItem == m_hDynamicGroupsItem )
                {
                    DynamicGroupITSIMap::iterator itr   = m_dynamicGroupITSIs.find( selectedItem );
                    ASSERT( itr != m_dynamicGroupITSIs.end() );
                    tsi = itr->second.tsi;
                }
                else
                {
                    // Get out the data associated with the item
                    UserITSIMap::iterator itr   = m_userITSIs.find( selectedItem );
                    ASSERT( itr != m_userITSIs.end() );
                    tsi = itr->second.tsi;
                }

                // It's a group if the parent item is "Groups" and has a parent that is the root item
                if ( parentItem == m_hGroupsItem && m_DirectoryTree.GetParentItem( parentItem ) == NULL )
                {
                    // And set it up for calling
                    m_callbackHelper.SetCall( tsi, m_DirectoryTree.GetItemText( selectedItem ), false,
                                              ISearchPageHelper::GROUP );
                    populateSearchListWithGroup( tsi );
                }
                else
                {
                    // And set it up for calling
                    m_callbackHelper.SetCall( tsi, m_DirectoryTree.GetItemText( selectedItem ), false,
                                              ISearchPageHelper::USER );

                    populateSearchListWithTSI( tsi, parentItem, selectedItem );
                }
            }
            else //TD14417
            {
                if ( m_SearchList.m_hWnd != NULL )
                {
                    m_SearchList.DeleteAllItems();
                }
                traverseTree( selectedItem );
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "While processing CSearchPage::OnSelectionChanged" );
        }

        onUpdateRights();

        FUNCTION_EXIT;
        return 0;
    }


    void CSearchPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );

        // Do nothing...

        FUNCTION_EXIT;
    }


    LRESULT CSearchPage::WindowProc( UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam )
    {
        FUNCTION_ENTRY( "WindowProc" );

        LRESULT result  = 0;

        result = CDialog::WindowProc( message, wParam, lParam );

        try
        {
            // Respond to the message from the train selection component if required
            if ( m_callbackHelper.getTrainSelectorGUI().getInboundMessageId() == ( int ) message )
            {
                OnInboundTrain( wParam );
            }
            else if ( m_callbackHelper.getTrainSelectorGUI().getOutboundMessageId() == ( int ) message )
            {
                OnOutboundTrain( wParam );
            }
            else if ( m_callbackHelper.getTrainSelectorGUI().getUpdateSelectionId() == ( int ) message )
            {
                OnComboChanged( wParam );
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "While processing CSearchPage::WindowProc" );
        }

        FUNCTION_EXIT;
        return result;
    }


    void CSearchPage::OnInboundTrain( WPARAM wParam )
    {
        FUNCTION_ENTRY( "OnInboundTrain" );

        using TA_IRS_Bus::MFCTrainSelectorGUI;

        // For the search page, all we care about is that all trains are represented - their direction is not important
        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update= reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>( wParam );

        processTrainNotification( update );

        // Delete the update when done
        delete update;
        update = NULL;

        FUNCTION_EXIT;
    }


    void CSearchPage::OnOutboundTrain( WPARAM wParam )
    {
        FUNCTION_ENTRY( "OnOutboundTrain" );

        using TA_IRS_Bus::MFCTrainSelectorGUI;

        // For the search page, all we care about is that all trains are represented - their direction is not important
        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update= reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>( wParam );

        processTrainNotification( update );

        // Delete the update when done
        delete update;
        update = NULL;

        FUNCTION_EXIT;
    }


    CString CSearchPage::getTrainServiceNumber( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& details )
    {
        FUNCTION_ENTRY( "getTrainServiceNumber" );

        CString trainServiceNumber;

        if ( details.serviceNumber.size() < 1 )
        {
            std::ostringstream tempStr;
            tempStr << "PV" << std::setw( 2 ) << std::setfill( '0' ) << details.trainId << "/_";
            trainServiceNumber = tempStr.str().c_str();
        }
        else
        {
            trainServiceNumber = details.serviceNumber.c_str();
        }

        if ( true == details.isMute )
        {
            trainServiceNumber += " (M)";
        }

        FUNCTION_EXIT;
        return trainServiceNumber;
    }


    void CSearchPage::processTrainNotification( TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate )
    {
        FUNCTION_ENTRY( "processTrainNotification" );

        using TA_IRS_Bus::MFCTrainSelectorGUI;
        using TA_IRS_Bus::ITrainSelectorGUI;

        bool isLocInCtrl= m_callbackHelper.getTrainSelectorGUI().isLocationInControl( trainListUpdate->trainDetails.currentLocation );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Notification from TrainListComponent, trainLocation = %d, trainLoc = %d, isLocInCtrl = %s",
                     trainListUpdate->trainDetails.currentLocation, trainListUpdate->trainDetails.currentLocation,
                     isLocInCtrl ? "TRUE" : "FALSE" );

        if ( NULL != trainListUpdate )
        {
            CString trainServiceNumber  = getTrainServiceNumber( trainListUpdate->trainDetails );

            TrainUpdate update;
            update.trainID = trainListUpdate->trainDetails.trainId;
            update.ITSI = trainListUpdate->trainDetails.primaryTsi.c_str();
            update.name = trainServiceNumber;
            update.locationKey = trainListUpdate->trainDetails.currentLocation;

            // Four different types of update - need to handle each one.
            if ( ITrainSelectorGUI::Add == trainListUpdate->action )
            {
                if ( isLocInCtrl )
                {
                    addTrain( update );
                }

            }
            else if ( ITrainSelectorGUI::Update == trainListUpdate->action )
            {
                if ( isLocInCtrl )
                {
                    //location in control
                    TrainITSIMap::iterator findIter = m_trainITSIs.find( trainListUpdate->trainDetails.trainId );
                    if ( findIter != m_trainITSIs.end() )
                    {
                        updateTrain( update );
                    }
                    else
                    {
                        //not exist: add it.
                        addTrain( update );
                    }
                }
                else
                {
                    //location not in control, should not show the item.
                    deleteTrain( update );
                }
            }
            else if ( ITrainSelectorGUI::Delete == trainListUpdate->action )
            {
                deleteTrain( update );
            }
            else if ( ITrainSelectorGUI::ClearAll == trainListUpdate->action )
            {
                // Clear all trains
                HTREEITEM hItem = m_DirectoryTree.GetChildItem( m_trainItem );
                while ( hItem != NULL )
                {
                    HTREEITEM hNextItem = m_DirectoryTree.GetNextSiblingItem( hItem );
                    m_DirectoryTree.DeleteItem( hItem );
                    hItem = hNextItem;
                }

                m_trainITSIs.clear();

                // also find any trains in the search list and delete them
                LVFINDINFO info;
                info.flags = LVFI_PARAM;
                info.lParam = TYPE_TRAIN;

                int nIndex  = m_SearchList.FindItem( &info );
                while ( nIndex != -1 )
                {
                    m_SearchList.DeleteItem( nIndex );
                    nIndex = m_SearchList.FindItem( &info );
                }
            }
        }

        // Control doesn't always refresh fully, so help it along..
        Invalidate( FALSE );

        FUNCTION_EXIT;
    }


    void CSearchPage::OnComboChanged( WPARAM wParam )
    {
        FUNCTION_ENTRY( "OnComboChanged" );

        // Message not used...

        FUNCTION_EXIT;
    }


    void CSearchPage::OnAddToMonitor()
    {
        FUNCTION_ENTRY( "OnAddToMonitor" );

        // When this button is clicked need to add the selected subscribers to the dymaic group (if any).
        unsigned int numberSelected = m_SearchList.GetSelectedCount();

        if ( numberSelected > 0 )
        {
            try
            {
                // If there ARE actually any selected items, pull them out of the list.
                // Start with the inbound list
                POSITION selPos                                         = m_SearchList.GetFirstSelectedItemPosition();

                std::vector<std::vector<CString> >* selectedSubscribers = new std::vector<std::vector<CString> >();
                while ( selPos != NULL )
                {
                    int index                               = m_SearchList.GetNextSelectedItem( selPos );

                    std::vector<CString> subscriberDetails  = getSubscriberDetails( index );
                    selectedSubscribers->push_back( subscriberDetails );

                    // Process all the dynamic group members being added
                    //CString subscriberName = subscriberDetails.at(0);
                    //CString subscriberTSI = subscriberDetails.at(2);
                    // Call function to add (or update) existing subscriber
                    //processDynamicGroupSubscriberUpdate(subscriberName, subscriberTSI);
                }

                //CWnd* tabCtrl =  AfxGetMainWnd()->GetDlgItem(IDC_CALL_STACK_TAB);
                //CRadioCallStack* callStack = dynamic_cast<CRadioCallStack*>(tabCtrl);
                //if (callStack != NULL)
                //{
                //callStack->addDynamicGroupMembers(selectedSubscribers);
                //}
                m_callbackHelper.addMonitors( selectedSubscribers );
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                     "While processing CSearchPage::OnAddToDynamicGroup" );
            }
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::OnAddToDynamicGroup()
    {
        FUNCTION_ENTRY( "OnAddToDynamicGroup" );

        // When this button is clicked need to add the selected subscribers to the dymaic group (if any).
        unsigned int numberSelected = m_SearchList.GetSelectedCount();

        if ( numberSelected > 0 )
        {
            try
            {
                // If there ARE actually any selected items, pull them out of the list.
                // Start with the inbound list
                POSITION selPos                                         = m_SearchList.GetFirstSelectedItemPosition();

                std::vector<std::vector<CString> >* selectedSubscribers = new std::vector<std::vector<CString> >();
                while ( selPos != NULL )
                {
                    int index                               = m_SearchList.GetNextSelectedItem( selPos );

                    std::vector<CString> subscriberDetails  = getSubscriberDetails( index );
                    selectedSubscribers->push_back( subscriberDetails );

                    // Process all the dynamic group members being added
                    CString subscriberName  = subscriberDetails.at( 0 );
                    CString subscriberTSI   = subscriberDetails.at( 2 );
                    // Call function to add (or update) existing subscriber
                    processDynamicGroupSubscriberUpdate( subscriberName, subscriberTSI );
                }

                //CWnd* tabCtrl =  AfxGetMainWnd()->GetDlgItem(IDC_CALL_STACK_TAB);
                //CRadioCallStack* callStack = dynamic_cast<CRadioCallStack*>(tabCtrl);
                //if (callStack != NULL)
                //{
                //callStack->addDynamicGroupMembers(selectedSubscribers);
                //}
                m_callbackHelper.addDynamicGroupMembers( selectedSubscribers );
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                     "While processing CSearchPage::OnAddToDynamicGroup" );
            }
        }

        FUNCTION_EXIT;
    }


    LRESULT CSearchPage::OnSubscriberNotification( UINT wParam,
                                                   LONG lParam )
    {
        FUNCTION_ENTRY( "OnSubscriberNotification" );

        if ( 0 == m_messageSubscriber || !m_messageSubscriber->isConnectedListenerWindow( m_hWnd ) )
        {
            // Must be closing down, ignore message as subscriber won't have it
            // any longer if we're no longer connected
            FUNCTION_EXIT;
            return 0;
        }

        try
        {
            const SubscriberNotification update = m_messageSubscriber->getMessageData( wParam, m_hWnd );
            processUpdate( update );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "While processing CSearchPage::OnSubscriberNotification" );
        }

        FUNCTION_EXIT;
        return 0;
    }


    void CSearchPage::refreshLastUpdateField()
    {
        FUNCTION_ENTRY( "refreshLastUpdateField" );

        const CString base  ( "Directories (Last updated: %s)" );
        CString timeStr;

        try
        {
            long time   = 0;
            CORBA_CALL_RETURN( time, m_callbackHelper.getRadioDirectoryCorbaDef(), getLastDirectorySynchronisationTime, () );//limin
            if ( 0 == time )
            {
                timeStr = "";
            }
            else
            {
                // Format: 11:03 22/08/2003
                // In format "17:30 - 8 Sep"
                const CString dateFormatStr ( "%H:%M %d/%m/%Y" );

                timeStr = CTime( time ).Format( dateFormatStr );
            }
        }
        catch ( ... )
        {
            timeStr = "Unknown";
        }

        CString windowText;
        windowText.Format( base, timeStr );

        SetDlgItemText( IDC_DIRECTORY_BOX, ( LPCTSTR ) windowText );

        FUNCTION_EXIT;
    }


    void CSearchPage::addNewSubscriber( const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details )
    {
        FUNCTION_ENTRY( "addNewSubscriber" );

        HTREEITEM hItem = 0;

        HTREEITEM hTest = findUserHItemFromTSI( details.TSI.in() );
        if ( 0 != hTest )
        {
            // The specified item already exists, so rather than add in, update it..
            updateSubscriber( details );

            FUNCTION_EXIT;
            return;
        }


        if ( TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER == details.type )
        {
            hItem = m_DirectoryTree.InsertItem( details.subscriberName, m_hGroupsItem );
            //            m_DirectoryTree.SetItemData(hItem,TYPE_GROUP);
        }
        else if ( TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER == details.type )
        {
            if ( m_occKey.getValue() == details.locationKey )
            {
                hItem = m_DirectoryTree.InsertItem( details.subscriberName, m_hTreeItemOcc );
            }
            else if ( m_depotKey.getValue() == details.locationKey )
            {
                hItem = m_DirectoryTree.InsertItem( details.subscriberName, m_hTreeItemDepot );
            }
            else
            {
                // If we get here this is a station, so find out if it's already been inserted into the tree
                //TD19511
                //std::map<unsigned int, HTREEITEM>::iterator iter = m_stationLocationMap.find(details.locationKey);
                std::map<std::string, HTREEITEM>::iterator iter = m_stationLocationMap.find( std::string( details.locationName ) );
                //lichao++
                if ( iter != m_stationLocationMap.end() )
                {
                    // The station already exists, so just insert this below the station
                    hItem = m_DirectoryTree.InsertItem( details.subscriberName, iter->second );
                }
                else
                {
                    // The station does not exist, so need to add the station before we can add the hItem itself
                    CString locationName    ( details.locationName );
                    HTREEITEM localStation  = m_DirectoryTree.InsertItem( locationName, m_hTreeItemStation );
                    hItem = m_DirectoryTree.InsertItem( details.subscriberName, localStation );

                    // Add the new tree node to the map so it can be found later
                    //TD19511
                    m_stationLocationMap.insert( std::map<std::string, HTREEITEM>::value_type( std::string( details.locationName ),
                                                                                               localStation ) );
                };
            }
        }
        else
        {
            ASSERT( FALSE );
        }

        ASSERT( hItem );

        if ( hItem != 0 )
        {
            UserDetails itemInfo;
            itemInfo.subscriberName = details.subscriberName.in();
            itemInfo.tsi = details.TSI.in();
            itemInfo.locationKey = details.locationKey;
            itemInfo.type = details.type;

            if ( !m_userITSIs.insert( UserITSIMap::value_type( hItem, itemInfo ) ).second )
            {
                // hItem already contained in map somehow?
                ASSERT( FALSE );
            }
        }

        FUNCTION_EXIT;
    }


    HTREEITEM CSearchPage::findUserHItemFromTSI( CString tsi ) const
    {
        FUNCTION_ENTRY( "findUserHItemFromTSI" );

        for ( UserITSIMap::const_iterator it = m_userITSIs.begin(); it != m_userITSIs.end(); ++it )
        {
            if ( tsi == it->second.tsi )
            {
                FUNCTION_EXIT;
                return it->first;
            }
        }

        FUNCTION_EXIT;
        return 0;
    }


    HTREEITEM CSearchPage::findDynamicGroupHItemFromTSI( CString tsi ) const
    {
        FUNCTION_ENTRY( "findDynamicGroupHItemFromTSI" );

        for ( DynamicGroupITSIMap::const_iterator it = m_dynamicGroupITSIs.begin();
              it != m_dynamicGroupITSIs.end();
              ++it )
        {
            CString debug   = it->second.tsi;

            if ( tsi == it->second.tsi )
            {
                FUNCTION_EXIT;
                return it->first;
            }
        }

        FUNCTION_EXIT;
        return 0;
    }


    void CSearchPage::updateSubscriber( const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details )
    {
        FUNCTION_ENTRY( "updateSubscriber" );

        HTREEITEM hItem = findUserHItemFromTSI( details.TSI.in() );

        if ( 0 == hItem )
        {
            // This update request is for an unrecognized item, ignore it
            FUNCTION_EXIT;
            return;
        }

        UserITSIMap::iterator itr   = m_userITSIs.find( hItem );
        if ( details.locationKey == itr->second.locationKey && details.type == itr->second.type )
        {
            // Update name for this item - doesn't have to be moved
            m_DirectoryTree.SetItemText( hItem, details.subscriberName );
        }
        else
        {
            // Otherwise position in the tree must be changed to reflect
            // new location and/or type - easiest way to do this is
            // to remove the item and re-add it..
            deleteSubscriber( details );
            addNewSubscriber( details );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::deleteSubscriber( const TA_Base_Bus::IRadioCorbaDef::SubscriberDetails& details )
    {
        FUNCTION_ENTRY( "deleteSubscriber" );

        HTREEITEM hItem = findUserHItemFromTSI( details.TSI.in() );

        if ( 0 == hItem )
        {
            // This delete request is for an unrecognized item, ignore it
            FUNCTION_EXIT;
            return;
        }

        // remove entry from m_userITSIs map, as hItem will become invalid
        UserITSIMap::iterator itr   = m_userITSIs.find( hItem );
        ASSERT( itr != m_userITSIs.end() );
        m_userITSIs.erase( itr );

        // remove from tree
        m_DirectoryTree.DeleteItem( hItem );

        FUNCTION_EXIT;
    }


    void CSearchPage::processUpdate( const SubscriberNotification& update )
    {
        FUNCTION_ENTRY( "processUpdate" );

        // When there is an update, we only need to respond by updating the tree control
        // displaying the current available subscribers.  There is no need (at this stage)
        // to propogate the changes to the matching items, or selected dynamic group items..
        switch ( update.notifyType )
        {
            case TA_Base_Bus::IRadioCorbaDef::SCAN_SUBSCRIBERS:
                refreshLastUpdateField();
                break;

            case TA_Base_Bus::IRadioCorbaDef::NEW_SUBSCRIBER:
                switch ( update.details.type )
                {
                    case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:
                        // FALLTHROUGH
                    case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
                        addNewSubscriber( update.details );
                        break;
                    case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
                        // Nothing to do (radio monitor ignores patch updates)
                        break;
                    default:
                        ASSERT( FALSE );
                }

                break;

            case TA_Base_Bus::IRadioCorbaDef::UPDATE_SUBSCRIBER:
                switch ( update.details.type )
                {
                    case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:
                        // FALLTHROUGH
                    case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
                        updateSubscriber( update.details );
                        break;
                    case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
                        // Nothing to do (radio monitor ignores patch updates)
                        break;
                    default:
                        ASSERT( FALSE );
                }

                break;

            case TA_Base_Bus::IRadioCorbaDef::DELETE_SUBSCRIBER:
                switch ( update.details.type )
                {
                    case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:
                        // FALLTHROUGH
                    case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
                        deleteSubscriber( update.details );
                        break;
                    case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
                        // Nothing to do (radio monitor ignores patch updates)
                        break;
                    default:
                        ASSERT( FALSE );
                }

                break;

            default:
                ASSERT( FALSE );
        }

        // Control doesn't always refresh fully, so help it along..
        Invalidate( FALSE );

        FUNCTION_EXIT;
    }


    std::vector<CString> CSearchPage::getSubscriberDetails( int index )
    {
        FUNCTION_ENTRY( "getSubscriberDetails" );

        TA_ASSERT( index >= 0 && index < m_SearchList.GetItemCount(), "Invalid input index" );

        std::vector<CString> subscriberDetails;

        // NOTE: If you change this ordering, make sure you assess the impact on,
        // and update the comments for ISearchPageHelper::addDynamicGroupMembers
        subscriberDetails.push_back( m_SearchList.GetItemText( index, COL_ID ) );
        subscriberDetails.push_back( m_SearchList.GetItemText( index, COL_LOCATION ) );
        subscriberDetails.push_back( m_SearchList.GetItemText( index, COL_TSI ) );

        FUNCTION_EXIT;
        return subscriberDetails;
    }


    void CSearchPage::OnDblclkSearchList( NMHDR* pNMHDR,
                                          LRESULT* pResult )
    {
        FUNCTION_ENTRY( "OnDblclkSearchList" );

        NMITEMACTIVATE* nmActivate  = ( NMITEMACTIVATE* ) pNMHDR;

        if ( nmActivate->iItem >= 0 )
        {
            try
            {
                // Double clicking on list acts as shortcut to add that item
                std::vector<std::vector<CString> >* selectedSubscribers = new std::vector<std::vector<CString> >();
                selectedSubscribers->push_back( getSubscriberDetails( nmActivate->iItem ) );
                m_callbackHelper.addDynamicGroupMembers( selectedSubscribers );
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                     "While processing CSearchPage::OnDblclkSearchList" );
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CSearchPage::removeDynamicGroupSubscriber( CString tsi )
    {
        FUNCTION_ENTRY( "removeDynamicGroupSubscriber" );

        if ( AfxGetApp()->m_nThreadID != GetCurrentThreadId() )
        {
            TA_ASSERT( FALSE, "Attempting to interact with dialog outside main thread" );
        }

        HTREEITEM hExistingItem = findDynamicGroupHItemFromTSI( tsi );

        // Remove (provided dynamic group item found)
        if ( 0 != hExistingItem )
        {
            m_dynamicGroupITSIs.erase( hExistingItem );
            m_DirectoryTree.DeleteItem( hExistingItem );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::processDynamicGroupSubscriberUpdate( CString subscriberName,
                                                           CString tsi )
    {
        FUNCTION_ENTRY( "processDynamicGroupSubscriberUpdate" );

        if ( AfxGetApp()->m_nThreadID != GetCurrentThreadId() )
        {
            TA_ASSERT( FALSE, "Attempting to interact with dialog outside main thread" );
        }

        HTREEITEM hExistingItem = findDynamicGroupHItemFromTSI( tsi );

        SubscriberDetails details;
        details.subscriberName = subscriberName;
        details.tsi = tsi;

        // Otherwise add item in - or add existing
        if ( 0 == hExistingItem )
        {
            // Add in the item
            hExistingItem = m_DirectoryTree.InsertItem( subscriberName, m_hDynamicGroupsItem );

            // Add handle to map
            m_dynamicGroupITSIs.insert( DynamicGroupITSIMap::value_type( hExistingItem, details ) );
        }
        else
        {
            m_DirectoryTree.SetItemText( hExistingItem, subscriberName );
        }

        TA_ASSERT( m_dynamicGroupITSIs.end() != m_dynamicGroupITSIs.find( hExistingItem ), "Invalid internal state" );

        // Refresh details contained for this tree entry
        m_dynamicGroupITSIs[hExistingItem] = details;

        // Control doesn't always refresh fully, so help it along..
        Invalidate( FALSE );

        FUNCTION_EXIT;
    }


    std::vector<CString> CSearchPage::getDynamicGroupTSIs()
    {
        FUNCTION_ENTRY( "getDynamicGroupTSIs" );

        std::vector<CString> result;

        for ( DynamicGroupITSIMap::const_iterator it = m_dynamicGroupITSIs.begin();
              it != m_dynamicGroupITSIs.end();
              ++it )
        {
            result.push_back( it->second.tsi );
        }

        FUNCTION_EXIT;
        return result;
    }


    void CSearchPage::updateSearchTextButtonEnabledState( bool fullRefresh )
    {
        FUNCTION_ENTRY( "updateSearchTextButtonEnabledState" );

        try
        {
            // TD3896: Only enable search button if a valid entry exists
            // in the text field (valid=non-empty)
            CString rString;
            m_searchText.GetWindowText( rString );
            bool textValid  = rString.GetLength() > 0;

            // Ensure value set to something regardless of refresh parameter
            if ( fullRefresh || !m_searchButtonAccessRight.hasBeenSet() )
            {
                m_searchButtonAccessRight.setValue( m_callbackHelper.isFunctionPermitted( TA_Base_Bus::aca_RADIO_SEARCH_DIRECTORY ) );
            }

            GetDlgItem( IDC_SUBSCRIBERSEARCH )->EnableWindow( m_searchButtonAccessRight.getValue() && textValid );
        }
        catch ( ... )
        {
            m_searchText.EnableWindow( FALSE );
            GetDlgItem( IDC_SUBSCRIBERSEARCH )->EnableWindow( FALSE );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "While processing CSearchPage::updateSearchTextButtonEnabledState" );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::onRunParamChange( const std::string& name,
                                        const std::string& value )
    {
        FUNCTION_ENTRY( "onRunParamChange" );

        if ( RPARAM_SESSIONID == name )
        {
            // Possible change in rights
            onUpdateRights();
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::OnChangeEditSearchText()
    {
        FUNCTION_ENTRY( "OnChangeEditSearchText" );

        updateSearchTextButtonEnabledState( false );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails CSearchPage::getTrainDetailsFromTSI( CString tsi )
    {
        FUNCTION_ENTRY( "getTrainDetailsFromTSI" );

        std::string properString= tsi.GetBuffer( 0 );

        FUNCTION_EXIT;
        return m_callbackHelper.getTrainSelector()->getTrainDetails( properString );
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails CSearchPage::getTrainDetailsFromTrainID( BYTE trainId )
    {
        FUNCTION_ENTRY( "getTrainDetailsFromTrainID" );

        TA_IRS_Bus::CommonTypes::TrainIdType properTrainId  = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( trainId );

        FUNCTION_EXIT;
        return m_callbackHelper.getTrainSelector()->getTrainDetails( properTrainId );
    }


    bool CSearchPage::hasSelectedUsersLevelItem()
    {
        FUNCTION_ENTRY( "hasSelectedUsersLevelItem" );

        HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();

        if ( NULL == selectedItem ) // nothing selected
        {
            FUNCTION_EXIT;
            return false;
        }
        else if ( m_hItemUsers == selectedItem ) // Users level item is selected
        {
            FUNCTION_EXIT;
            return true;
        }
        else
        {
            // Check if the selected item is under the Users Level Item
            HTREEITEM parentItem= selectedItem;

            do
            {
                parentItem = m_DirectoryTree.GetParentItem( parentItem );
            }
            while ( parentItem != NULL && parentItem != m_hItemUsers );

            if ( NULL == parentItem )
            {
                FUNCTION_EXIT;
                return false;
            }
            else if ( m_hItemUsers == parentItem )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    bool CSearchPage::hasSelectedGroupsLevelItem()
    {
        FUNCTION_ENTRY( "hasSelectedGroupsLevelItem" );

        HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();

        if ( m_hGroupsItem == selectedItem )
        {
            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


    bool CSearchPage::hasSelectedTrainsLevelItem()
    {
        FUNCTION_ENTRY( "hasSelectedTrainsLevelItem" );

        HTREEITEM selectedItem  = m_DirectoryTree.GetSelectedItem();

        if ( m_trainItem == selectedItem )
        {
            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


    void CSearchPage::populateSearchListWithStation( HTREEITEM& selectedItem,
                                                     bool bClearItems /* = true */ )
    {
        FUNCTION_ENTRY( "populateSearchListWithStation" );

        try
        {
            // Clear the list
            if ( bClearItems && m_SearchList.m_hWnd != NULL ) // TD14417
            {
                m_SearchList.DeleteAllItems();
            }

            for ( UserITSIMap::iterator it = m_userITSIs.begin(); it != m_userITSIs.end(); ++it )
            {
                HTREEITEM parentItem= m_DirectoryTree.GetParentItem( it->first );
                if ( parentItem == selectedItem )
                {
                    TA_Base_Bus::IRadioCorbaDef::SubscriberDetails_var subscriber;
                    CORBA_CALL_RETURN( subscriber, m_callbackHelper.getRadioCorbaDef(), getSubscriberDetails, ( it->second.tsi ) );//limin
                    int iRet= m_SearchList.InsertItem( m_SearchList.GetItemCount(), subscriber->subscriberName );
                    m_SearchList.SetItemText( iRet, 1, subscriber->TSI );
                    m_SearchList.SetItemText( iRet, 2, subscriber->locationName );
                    m_SearchList.SetItemData( iRet, TYPE_INDIVIDUAL );
                }
            }
        }
        catch ( const CORBA::Exception& e )
        {
            //ex.what();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                         TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& )
        {
        }
        catch ( ... )
        {
        }

        FUNCTION_EXIT;
    }


    unsigned char CSearchPage::getTrainIdForTsi( const std::string& tsi )
    {
        FUNCTION_ENTRY( "getTrainIdForTsi" );

        try
        {
            TA_IRS_Bus::TrainInformationTypes::TrainDetails details = m_callbackHelper.getTrainSelector()->getTrainDetails( tsi );

            FUNCTION_EXIT;
            return details.trainId;
        }
        catch ( TA_IRS_Bus::TrainException& )
        {
        }

        FUNCTION_EXIT;
        return 0;
    }


    CSearchPage::TrainUpdate CSearchPage::getDetailsForTrain( unsigned char trainId )
    {
        FUNCTION_ENTRY( "getDetailsForTrain" );

        CSearchPage::TrainUpdate trainUpdate;
        TA_IRS_Bus::TrainInformationTypes::TrainDetails details = m_callbackHelper.getTrainSelector()->getTrainDetails( trainId );
        trainUpdate.ITSI = details.primaryTsi.c_str();
        trainUpdate.locationKey = details.currentLocation;
        trainUpdate.name = details.serviceNumber.c_str();
        trainUpdate.trainID = details.trainId;

        FUNCTION_EXIT;
        return trainUpdate;
    }


    // TD14417
    void CSearchPage::traverseTree( HTREEITEM node,
                                    int count )
    {
        FUNCTION_ENTRY( "traverseTree" );

        count ++;
        if ( node == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        //HTREEITEM nextNode;
        if ( m_DirectoryTree.ItemHasChildren( node ) != 0 )
        {
            HTREEITEM child = m_DirectoryTree.GetChildItem( node );
            traverseTree( child, count );
            //nextNode = child;
        }
        else
        {
            // populate searchlist when the item is already a leaf
            populateSearchListWithNode( node );
            //nextNode = node;
        }

        if ( count > 1 )
        {
            HTREEITEM sibling   = m_DirectoryTree.GetNextItem( node, TVGN_NEXT );
            traverseTree( sibling, count );
        }

        FUNCTION_EXIT;
    }


    void CSearchPage::populateSearchListWithNode( HTREEITEM node )
    {
        FUNCTION_ENTRY( "populateSearchListWithNode" );

        HTREEITEM parentItem= m_DirectoryTree.GetParentItem( node );
        if ( ( parentItem == m_hTreeItemOcc ) || ( parentItem == m_hTreeItemDepot ) )
        {
            UserITSIMap::iterator itr   = m_userITSIs.find( node );
            ASSERT( itr != m_userITSIs.end() );
            CString tsi = itr->second.tsi;
            populateSearchListWithTSI( tsi, parentItem, node, false );
        }
        else if ( parentItem == m_trainItem )
        {
            DWORD trainId               = m_DirectoryTree.GetItemData( node );
            TrainITSIMap::iterator itr  = m_trainITSIs.find( ( unsigned char ) trainId );
            ASSERT( itr != m_trainITSIs.end() );
            CString tsi = itr->second.ITSI;
            populateSearchListWithTSI( tsi, parentItem, node, false );
        }
        else if ( parentItem == m_hGroupsItem && m_DirectoryTree.GetParentItem( parentItem ) == NULL )
        {
            UserITSIMap::iterator itr   = m_userITSIs.find( node );
            ASSERT( itr != m_userITSIs.end() );
            CString tsi = itr->second.tsi;
            populateSearchListWithTSI( tsi, parentItem, node, false );
            //no need to show the member of group.
            //populateSearchListWithGroup(tsi,false);
        }
        else if ( parentItem == m_hDynamicGroupsItem )
        {
            DynamicGroupITSIMap::iterator itr   = m_dynamicGroupITSIs.find( node );
            ASSERT( itr != m_dynamicGroupITSIs.end() );
            CString tsi = itr->second.tsi;
            populateSearchListWithTSI( tsi, parentItem, node, false );
        }
        else if ( m_DirectoryTree.GetParentItem( parentItem ) == m_hTreeItemStation )
        {
            UserITSIMap::iterator itr   = m_userITSIs.find( node );
            ASSERT( itr != m_userITSIs.end() );
            CString tsi = itr->second.tsi;
            populateSearchListWithTSI( tsi, parentItem, node, false );
            //populateSearchListWithStation(parentItem,false);
        }

        FUNCTION_EXIT;
    }


    // TD14417

    void CSearchPage::addTrain( TrainUpdate& update )
    {
        FUNCTION_ENTRY( "addTrain" );

        // If this is an add message, just add the item to the tree
        //m_trainOutItem(0),   m_trainInItem(0)
        HTREEITEM hItem = m_DirectoryTree.InsertItem( update.name, m_trainItem );
        m_DirectoryTree.SetItemData( hItem, update.trainID );
        // Refresh ITSI map
        m_trainITSIs[update.trainID] = update;

        FUNCTION_EXIT;
    }


    void CSearchPage::updateTrain( TrainUpdate& update )
    {
        FUNCTION_ENTRY( "updateTrain" );

        // For an update action, find the object we're talking about and update it
        //m_trainOutItem(0),   m_trainInItem(0)
        HTREEITEM hItem = m_DirectoryTree.GetChildItem( m_trainItem );
        while ( hItem != NULL )
        {
            if ( m_DirectoryTree.GetItemData( hItem ) == update.trainID )
            {
                m_DirectoryTree.SetItemText( hItem, update.name );
                break;  // Break from the loop once the item is updated
            }
            hItem = m_DirectoryTree.GetNextSiblingItem( hItem );
        }

        // get the old details
        TrainITSIMap::iterator findIter = m_trainITSIs.find( update.trainID );

        if ( findIter != m_trainITSIs.end() )
        {
            // if it changed
            if ( ( findIter->second.name != update.name ) ||
                 ( findIter->second.ITSI != update.ITSI ) ||
                 ( findIter->second.locationKey != update.locationKey ) )
            {
                // also find the old item in the search list if its there and update it
                LVFINDINFO info;
                info.flags = LVFI_STRING;
                info.psz = findIter->second.name;

                CString oldLocation = getLocationNameFromKey( findIter->second.locationKey );
                CString location    = getLocationNameFromKey( update.locationKey );

                int nIndex          = m_SearchList.FindItem( &info );
                if ( nIndex != -1 )
                {
                    m_SearchList.SetItemText( nIndex, COL_ID, update.name );
                    m_SearchList.SetItemText( nIndex, COL_TSI, update.ITSI );
                    m_SearchList.SetItemText( nIndex, COL_LOCATION, location );
                }

                // tell the dialog to update the dynamic group page, and the selected tsi edit box
                m_callbackHelper.selectedTrainDetailsUpdated( findIter->second.ITSI, findIter->second.name, oldLocation,
                                                              update.ITSI, update.name, location );
            }
        }

        // Refresh ITSI map
        m_trainITSIs[update.trainID] = update;

        FUNCTION_EXIT;
    }


    void CSearchPage::deleteTrain( TrainUpdate& update )
    {
        FUNCTION_ENTRY( "deleteTrain" );

        // For delete actions, find the object and delete it
        HTREEITEM hItem = m_DirectoryTree.GetChildItem( m_trainItem );

        while ( hItem != NULL )
        {
            if ( m_DirectoryTree.GetItemData( hItem ) == update.trainID )
            {
                m_DirectoryTree.DeleteItem( hItem );
                break;  // Break from the loop once the item is deleted
            }
            hItem = m_DirectoryTree.GetNextSiblingItem( hItem );
        }

        m_trainITSIs.erase( update.trainID );

        // also find the old item in the search list if its there and delete it
        LVFINDINFO info;
        info.flags = LVFI_STRING;
        info.psz = update.name;

        int nIndex  = m_SearchList.FindItem( &info );
        if ( nIndex != -1 )
        {
            m_SearchList.DeleteItem( nIndex );
        }

        FUNCTION_EXIT;
    }


    bool CSearchPage::doesItemExistInSearchList( CString name )
    {
        FUNCTION_ENTRY( "doesItemExistInSearchList" );

        for ( int i = 0; i < m_SearchList.GetItemCount(); i++ )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "check item exists: %s , name of item: %s.",
                         name, m_SearchList.GetItemText( i, COL_ID ) );
            if ( name.Compare( m_SearchList.GetItemText( i, COL_ID ) ) == 0 )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    // ++ TD17304
    void CSearchPage::initSortInfo()
    {
        FUNCTION_ENTRY( "initSortInfo" );

        m_SortInfo.defineSortingSemantic( COL_ID, TA_Base_Bus::AutoSortListCtrl::BY_STRING );
        m_SortInfo.defineSortingSemantic( COL_TSI, TA_Base_Bus::AutoSortListCtrl::BY_STRING );
        m_SortInfo.defineSortingSemantic( COL_LOCATION, TA_Base_Bus::AutoSortListCtrl::BY_STRING );
        m_SortInfo.setCurrentSort( COL_ID, TA_Base_Bus::AutoSortListCtrl::ASCENDING );
        m_SortInfo.activateSort( COL_ID );

        FUNCTION_EXIT;
    }


    // TD17304 ++
} // end namespace TA_Base_Bus
