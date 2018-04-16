/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/pa/PAManager/src/TrainDvaVersionsPage.cpp $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2011/03/01 18:08:50 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * Controls the Train DVA version display page
  *
  */

#include "app/trains/TrainBorneManager/stdafx.h"

#include <iomanip>
#include <sstream>
#include <exception>
#include <vector>

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
//#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"

#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/trains/TrainBorneManager/resource.h"
#include "app/trains/TrainBorneManager/TrainDvaVersionsPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

const DWORD HEADER_TEXT_COLOUR = RGB(255, 255, 255);
const DWORD HEADER_BACKGROUND_COLOUR = RGB(0, 0, 0);

IMPLEMENT_DYNCREATE(TrainDvaVersionsPage, CPropertyPage)

const DWORD TRAIN_ITEM_DATA  = 0;

const std::string TrainDvaVersionsPage::TRAIN_ID_COLUMN = "TrainID/Service";
const std::string TrainDvaVersionsPage::VERSION_COLUMN = "Current Version";

const unsigned int WM_PA_TRAIN_GLOBAL_VERSION_UPDATE = WM_USER + 0x101;
const unsigned int WM_PA_TRAIN_VERSION_UPDATE = WM_USER + 0x102;

TrainDvaVersionsPage::TrainDvaVersionsPage( CWnd* pParent ) : CDialog( TrainDvaVersionsPage::IDD, pParent )
, m_versionList("versionList")
{
    FUNCTION_ENTRY( "TrainDvaVersionsPage" );

    TA_IRS_Bus::TrainAgentProxyFactory::getInstance().guiInitialise( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() );

    m_trainPaCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainPaCorbaProxy();

    FUNCTION_EXIT;
}


TrainDvaVersionsPage::~TrainDvaVersionsPage()
{
    FUNCTION_ENTRY( "~TrainDvaVersionsPage" );

    m_trainPaCorbaProxy->removeObserver( this );

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::DoDataExchange( CDataExchange* pDX )
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange( pDX );
    //{{AFX_DATA_MAP(TrainDvaVersionsPage)
    DDX_Control(pDX, IDC_DVA_HEADER_LST, m_firstVersionHeaderList);
    DDX_Control(pDX, IDC_DVA_VERSION_MAIN, m_versionList);
    DDX_Control(pDX, IDC_TAB_DVA_VERSIONS, m_tabDvaVersions );
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(TrainDvaVersionsPage, CDialog)
    //{{AFX_MSG_MAP(TrainDvaVersionsPage)
    ON_NOTIFY(LVN_ITEMCHANGING, IDC_DVA_HEADER_LST, OnItemchangingDvaHeaderLst)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_DVA_HELP, OnBnClickedHelp)
    ON_BN_CLICKED(IDC_DVA_CLOSE, OnBnClickedClose)
    ON_MESSAGE(WM_PA_TRAIN_GLOBAL_VERSION_UPDATE, OnGlobalTrainVersionChange)
    ON_MESSAGE(WM_PA_TRAIN_VERSION_UPDATE, OnSingleTrainVersionChange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void TrainDvaVersionsPage::postDialogCreate()
{
    FUNCTION_ENTRY( "postDialogCreate" );

    // set up the primary list of versions
    setupFirstHeaderList();

    // this must be called after the header is set
    setupMainVersionList();

    // Begin listening out for global version updates just before adding to list
    //TA_Base_Core::OnlineUpdateListener::getInstance().registerInterest( TA_Base_Core::PA_TRAIN_DVA_MESSAGE, *this, TA_Base_Core::DatabaseKey::getInvalidKey() );
    TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().registerGlobalVersionObserver( this );

    refreshISCSVersion();

    // get the PA versions
    populateInitialTrainVersions();

    // set up the train selection component
    // and PA version updates
    startSubscriptions();

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::setupFirstHeaderList()
{
    FUNCTION_ENTRY( "setupFirstHeaderList" );

    m_tabDvaVersions.SetParent( this );
    m_tabDvaVersions.InsertItem( 0, "Train Versions" );
    CRect rtTabItemRect;
    m_tabDvaVersions.GetItemRect( 0, &rtTabItemRect );

    m_firstVersionHeaderList.SetParent( &m_tabDvaVersions );
    m_firstVersionHeaderList.SetWindowPos( NULL, 15, rtTabItemRect.Height() * 2, 0, 0, SWP_NOOWNERZORDER| SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER );
    CRect rtClientHeader;
    m_firstVersionHeaderList.GetClientRect( &rtClientHeader );
    m_versionList.SetParent( &m_tabDvaVersions );
    m_versionList.SetWindowPos( NULL, 15, rtTabItemRect.Height() * 2 + rtClientHeader.Height() + 2, 0, 0, SWP_NOOWNERZORDER| SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER );

    m_firstVersionHeaderList.InsertColumn( 0, TRAIN_ID_COLUMN.c_str() );
    m_firstVersionHeaderList.InsertColumn( 1, VERSION_COLUMN.c_str() );

    setupListView( m_firstVersionHeaderList, 2, true, LVCFMT_LEFT );

    //m_firstVersionHeaderList.SetTextColor( HEADER_TEXT_COLOUR );
    //m_firstVersionHeaderList.SetTextBkColor( HEADER_BACKGROUND_COLOUR );
    //m_firstVersionHeaderList.SetBkColor( HEADER_BACKGROUND_COLOUR );

    //m_firstVersionHeaderList.InsertItem( 0, TRAIN_ID_COLUMN.c_str() );
    //m_firstVersionHeaderList.SetItemText( 0, 1, VERSION_COLUMN.c_str() );

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::setupMainVersionList()
{
    FUNCTION_ENTRY( "setupMainVersionList" );

    // set the data provider (for version info)
    m_versionList.setCustomDataProvider( this );

    // hide all the standard columns
    m_versionList.setStandardColumnSizes( 0, 0, 0 );

    LVCOLUMN stColTrainIdService;
    m_versionList.GetColumn( 1,&stColTrainIdService );

    stColTrainIdService.pszText = "TrainID/Service";
    m_versionList.SetColumn( 1, &stColTrainIdService );

    // set up custom columns with same width as the header
    int widths[2];
    widths[0] = m_firstVersionHeaderList.GetColumnWidth( 0 );
    widths[1] = m_firstVersionHeaderList.GetColumnWidth( 1 );
    m_versionList.SetColumnWidth( 1, widths[0] );

    //m_trainIdColumnIndex = m_versionList.addCustomColumn( TRAIN_ID_COLUMN, widths[0] );
    m_versionColumnIndex = m_versionList.addCustomColumn( VERSION_COLUMN, widths[1] );

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::setupListView( CListCtrl& list, DWORD columnCount, bool accountForScroll, DWORD format )
{
    FUNCTION_ENTRY( "setupListView" );

    list.SetExtendedStyle( list.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

    CRect rc;
    list.GetClientRect(rc);

    int width = rc.Width();
    int averageWidth = width / columnCount;

    for ( unsigned int nCol = 0; nCol < columnCount; ++nCol )
    {
        // Keep inserting at the back (so the formatting works
        // - formatting is always left justified on first column no
        // matter what you specify)

        if ( 0 == nCol )
        {
            // Make the first item expand to fill out any remainder
            list.SetColumnWidth( nCol, width - ( averageWidth * ( columnCount - 1 ) ) );
        }
        else
        {
            list.SetColumnWidth( nCol, averageWidth );
        }
    }

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::OnItemchangingDvaHeaderLst( NMHDR* pNMHDR, LRESULT* pResult )
{
    FUNCTION_ENTRY( "OnItemchangingDvaHeaderLst" );

    NM_LISTVIEW* pNMListView= ( NM_LISTVIEW* ) pNMHDR;

    // Only interested in state changes
    if ( pNMListView->uChanged != LVIF_STATE )
    {
        FUNCTION_EXIT;
        return;
    }

    // Reject the change in state (no selections allowed on header controls)
    *pResult = TRUE;

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    //TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests( TA_Base_Core::PA_TRAIN_DVA_MESSAGE, *this );
    TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().deRegisterGlobalVersionObserver( this );

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::onGlobalTrainVersionChanged()
{
    FUNCTION_ENTRY( "onGlobalTrainVersionChanged" );

    PostMessage( WM_PA_TRAIN_GLOBAL_VERSION_UPDATE, 0, 0 );

    FUNCTION_EXIT;
}


LRESULT TrainDvaVersionsPage::OnGlobalTrainVersionChange( UINT wParam, LONG lParam )
{
    FUNCTION_ENTRY( "OnGlobalTrainVersionChange" );

    try
    {
        // ISCS global train version has changed, update list
        refreshISCSVersion();
    }
    catch ( const std::exception& expWhat )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
    }
    catch(...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception" );
    }

    FUNCTION_EXIT;
    return 0;
}


void TrainDvaVersionsPage::refreshISCSVersion()
{
    FUNCTION_ENTRY( "refreshISCSVersion" );

    std::string iscsVersion = "";
    try
    {
        unsigned long ulLibVersion = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion();
        formatTrainDvaVersion( ulLibVersion, iscsVersion );
    }
    catch ( const TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
        iscsVersion = "N.A";
    }


    CWnd* pVersion  = GetDlgItem( IDC_EDIT_VERSION_FIELD );

    if ( 0 != pVersion )
    {
        pVersion->SetWindowText( iscsVersion.c_str() );
    }

    FUNCTION_EXIT;
}


std::string TrainDvaVersionsPage::getCustomData( unsigned int trainId, const std::string& columnName )
{
    FUNCTION_ENTRY( "getCustomData" );

    // get the version for this train from train agent
    std::stringstream strValue;

    if ( columnName == VERSION_COLUMN )
    {
        try
        {
            // get it from the cached internal values rather than asking the agent every time
            std::map<unsigned char, unsigned long>::iterator findIter = m_trainLibraryVersions.find( trainId );

            unsigned long ulVersion = 0;
            if ( findIter != m_trainLibraryVersions.end() )
            {
                ulVersion = findIter->second;
            }
            else
            {
                // that failed for some reason, check the agent directly
                // missed a message?
                ulVersion = m_trainPaCorbaProxy->getTrainPaLibraryVersion( trainId );
                m_trainLibraryVersions[trainId] = ulVersion;
            }

            std::string strTemp = "";
            formatTrainDvaVersion( ulVersion, strTemp );
            strValue << strTemp;
        }
        catch ( TA_Base_Core::TransactiveException& expWhat )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception when call TrainAgent::getTrainPaLibraryVersion" );
        }
    }
    else if ( columnName == TRAIN_ID_COLUMN )
    {
        strValue << trainId;
    }

    FUNCTION_EXIT;
    return strValue.str().c_str();
}


void TrainDvaVersionsPage::updateTrainList( const TA_IRS_Bus::ITrainSelectorGUI::EAction& action,
                                            const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails )
{
    FUNCTION_ENTRY( "updateTrainList" );

    m_versionList.updateTrainList( action, trainDetails );

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::populateInitialTrainVersions()
{
    FUNCTION_ENTRY( "populateInitialTrainVersions" );

    try
    {
        TA_IRS_Bus::PaTypes::TrainLibraryInfoList versionData = m_trainPaCorbaProxy->getTrainPaLibraryVersionList();
        for ( UINT i = 0; i < versionData.size(); ++i )
        {
            m_trainLibraryVersions[versionData[i].trainId] = versionData[i].libraryVersion;
        }
    }
    catch ( const TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& expWhat )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.details.in() );
    }
    catch ( const TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& expWhat )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.details.in() );
    }
    catch ( const CORBA::Exception& expWhat )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat._name() );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, expWhat.what() );
    }
    catch(...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception when call TrainAgent::getTrainPaLibraryVersionList" );
    }

    FUNCTION_EXIT;
}


void TrainDvaVersionsPage::startSubscriptions()
{
    FUNCTION_ENTRY( "startSubscriptions" );

    m_trainPaCorbaProxy->addObserver( this );

    FUNCTION_EXIT;
}



void TrainDvaVersionsPage::processTrainLibraryInfo( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& event )
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Train Library version update received version %d for train %d",
        event.libraryVersion, event.trainId );

    m_trainLibraryVersions[event.trainId] = event.libraryVersion;

    // post a message to update
    PostMessage(WM_PA_TRAIN_VERSION_UPDATE, event.trainId, 0);
}


LRESULT TrainDvaVersionsPage::OnSingleTrainVersionChange( UINT wParam, LONG lParam )
{
    FUNCTION_ENTRY( "OnSingleTrainVersionChange" );

    // refresh the column
    m_versionList.refreshCustomColumn( wParam, VERSION_COLUMN );

    FUNCTION_EXIT;
    return 0;
}

void TrainDvaVersionsPage::formatTrainDvaVersion( const unsigned long lVersion, std::string& strFormatedVersion )
{
    std::stringstream strValue;

    strValue /*<< std::setw( 3 ) << std::setfill( '0' )*/ << (( lVersion & 0xFF000000 ) >> 24) << '.'
        /*<< std::setw( 3 ) << std::setfill( '0' )*/ << (( lVersion & 0x00FF0000 ) >> 16) << '.'
        /*<< std::setw( 3 ) << std::setfill( '0' )*/ << (( lVersion & 0x0000FF00 ) >> 8) << '.'
        /*<< std::setw( 3 ) << std::setfill( '0' )*/ << ( lVersion & 0x000000FF );

    strFormatedVersion = strValue.str().c_str();
}

void TrainDvaVersionsPage::OnBnClickedHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}

void TrainDvaVersionsPage::OnBnClickedClose()
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

//void TrainDvaVersionsPage::processUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
//{
//    if ( TA_Base_Core::DatabaseKey::getInvalidKey() == updateEvent.getKey() )
//    {
//        onGlobalTrainVersionChanged();
//    }
//}

}