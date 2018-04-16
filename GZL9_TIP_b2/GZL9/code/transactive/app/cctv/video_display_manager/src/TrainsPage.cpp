/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/TrainsPage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"

#include "BvsStage.h"
#include "DisplayItemManager.h"
#include "SwitchAgentCommunicator.h"
#include "TrainsPage.h"
#include "videodisplaymanager.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"//limin
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/RunParams.h"

//#include "bus/trains/TrainSelectionComponent/src/LocationCache.h"//limin


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_UPDATE_INBOUND       (WM_APP+1)
#define WM_UPDATE_OUTBOUND      (WM_APP+2)
#define WM_UPDATE_SELECTION     (WM_APP+3)

using TA_IRS_Bus::LocationCache;

namespace TA_IRS_App
{

    /////////////////////////////////////////////////////////////////////////////
    // TrainsPage property page

    IMPLEMENT_DYNCREATE(TrainsPage, CPropertyPage)

    TrainsPage::TrainsPage()
      : CPropertyPage(TrainsPage::IDD),
        m_isOffsetInitialised( false ),
        m_bottomOffset( 0 ), 
        m_trainSelector( 0 ),
        m_mfcTrainSelectorGUI( 0 )
    {
        FUNCTION_ENTRY( "TrainsPage" );

        //{{AFX_DATA_INIT(TrainsPage)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    TrainsPage::~TrainsPage()
    {
        FUNCTION_ENTRY( "~TrainsPage" );

        delete m_trainSelector;
        m_trainSelector = 0;
        delete m_mfcTrainSelectorGUI;
        m_mfcTrainSelectorGUI = 0;

        FUNCTION_EXIT;
    }


    void TrainsPage::setOffset()
    {
        FUNCTION_ENTRY( "setOffset" );

        CRect thisRect, listRect;
        GetWindowRect( thisRect );
        CWnd* dlgItem = GetDlgItem( IDC_TRAINSLIST );
        dlgItem->GetWindowRect( listRect );
        m_bottomOffset = ( thisRect.bottom - listRect.bottom );

        m_isOffsetInitialised = true;

        FUNCTION_EXIT;
    }


    void TrainsPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CPropertyPage::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(TrainsPage)
        DDX_Control(pDX, IDC_TRAINSLIST_IB, m_inboundTrainsListCtrl);
        DDX_Control(pDX, IDC_TRAINSLIST_OB, m_outboundTrainsListCtrl);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(TrainsPage, CPropertyPage)
        //{{AFX_MSG_MAP(TrainsPage)
        ON_WM_SIZE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_UPDATE_INBOUND, onUpdateInbound)
        ON_MESSAGE(WM_UPDATE_OUTBOUND, onUpdateOutbound)
        ON_MESSAGE(WM_UPDATE_SELECTION, onUpdateSelection)
        ON_MESSAGE(WM_STAGE_UPDATED, onStageUpdated)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // TrainsPage message handlers

    BOOL TrainsPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        setOffset();

        CPropertyPage::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void TrainsPage::Init()
    {
        FUNCTION_ENTRY( "Init" );


        unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str() , NULL, 10 );

        m_mfcTrainSelectorGUI = new TA_IRS_Bus::MFCTrainSelectorGUI( this, WM_UPDATE_INBOUND,
            WM_UPDATE_OUTBOUND, WM_UPDATE_SELECTION );

        try
        {
            m_trainSelector = new TA_IRS_Bus::TrainSelector( *m_mfcTrainSelectorGUI, locationKey, TA_IRS_Bus::LocationCache::InService );//limin
        }
        catch( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught while trying to resolve TrainAgent" );
        }

        DisplayItemManager::getInstance().registerForStageChanges(this);

        m_inboundTrainsListCtrl.Init();
        m_outboundTrainsListCtrl.Init();

        FUNCTION_EXIT;
    }


    void TrainsPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForStageChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    void TrainsPage::OnSize(UINT nType, int cx, int cy)
    {
        FUNCTION_ENTRY( "OnSize" );

        CPropertyPage::OnSize(nType, cx, cy);

        if ( m_isOffsetInitialised )
        {
            CRect rect;
            m_inboundTrainsListCtrl.GetWindowRect( rect );
            ScreenToClient( rect );
            int diff = ( rect.bottom - ( cy - m_bottomOffset ) );
            m_inboundTrainsListCtrl.SetWindowPos(NULL, 0, 0, rect.Width(), ( rect.Height() - diff ),
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

            m_outboundTrainsListCtrl.GetWindowRect( rect );
            ScreenToClient( rect );
            diff = ( rect.bottom - ( cy - m_bottomOffset ) );
            m_outboundTrainsListCtrl.SetWindowPos(NULL, 0, 0, rect.Width(), ( rect.Height() - diff ),
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        FUNCTION_EXIT;
    }


    LRESULT TrainsPage::onUpdateInbound(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateInbound" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate =
            reinterpret_cast< TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* >( wParam );

        m_inboundTrainsListCtrl.updateTrainList( trainListUpdate->action, trainListUpdate->trainDetails);

        delete trainListUpdate;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT TrainsPage::onUpdateOutbound(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateOutbound" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate =
            reinterpret_cast< TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* >( wParam );

        m_outboundTrainsListCtrl.updateTrainList( trainListUpdate->action, trainListUpdate->trainDetails);

        delete trainListUpdate;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT TrainsPage::onUpdateSelection(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateSelection" );

        TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate* selListUpdate =
            reinterpret_cast< TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate* >( wParam );
        delete selListUpdate;

        FUNCTION_EXIT;
        return 0;
    }


    unsigned int TrainsPage::getSelectedTrainId() const
    {
        FUNCTION_ENTRY( "getSelectedTrainId" );

        HWND focusHWND = ::GetFocus();
        if ( m_inboundTrainsListCtrl.GetSafeHwnd() == focusHWND )
        {
            FUNCTION_EXIT;
            return m_inboundTrainsListCtrl.getSelectedTrainId();
        }
        else if ( m_outboundTrainsListCtrl.GetSafeHwnd() == focusHWND )
        {
            FUNCTION_EXIT;
            return m_outboundTrainsListCtrl.getSelectedTrainId();
        }

        FUNCTION_EXIT;
        return 0;
    }


    VideoSourcesListCtrl::TrainDetails* TrainsPage::getSelectedTrainDetails()
    {
        FUNCTION_ENTRY( "getSelectedTrainDetails" );

        HWND focusHWND = ::GetFocus();
        if ( m_inboundTrainsListCtrl.GetSafeHwnd() == focusHWND )
        {
            FUNCTION_EXIT;
            return m_inboundTrainsListCtrl.getSelectedTrainDetails();
        }
        else if ( m_outboundTrainsListCtrl.GetSafeHwnd() == focusHWND )
        {
            FUNCTION_EXIT;
            return m_outboundTrainsListCtrl.getSelectedTrainDetails();
        }

        FUNCTION_EXIT;
        return NULL;
    }


    VideoSourcesListCtrl::TrainDetails* TrainsPage::getTrainDetails(unsigned short trainId)
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        VideoSourcesListCtrl::TrainDetails* details = NULL;

        details = m_inboundTrainsListCtrl.getTrainDetails(trainId);

        if ( details == NULL )
        {
            details = m_outboundTrainsListCtrl.getTrainDetails(trainId);
        }

        FUNCTION_EXIT;
        return details;
    }


    LRESULT TrainsPage::onStageUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onStageUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Stage update received for NULL display item");
        BvsStage* input = dynamic_cast<BvsStage*>(item);
        TA_ASSERT(input != NULL, "Stage update received for NULL display item");

        // update the lists
        m_inboundTrainsListCtrl.stageUpdated(input);
        m_outboundTrainsListCtrl.stageUpdated(input);

        FUNCTION_EXIT;
        return 0;
    }


} //TA_IRS_App
