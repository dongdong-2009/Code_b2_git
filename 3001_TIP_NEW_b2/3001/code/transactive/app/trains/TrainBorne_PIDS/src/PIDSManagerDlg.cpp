// PAManagerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "app/trains/TrainBorneManager/ITrainBorne.h"
#include "PIDSManagerDlg.h"
#include "RightsManager.h"
#include "PIDSpredefinedMessagePage.h"
#include "DisplayAttributeManager.h"
#include "DisplayRequestManager.h"
#include "PIDSAdhocMessagePage.h"
#include "TTISMessageManager.h"
#include "PIDSPredefinedMessagePage.h"
#include "PIDSAdhocMessagePage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/FunctionEntry.h"
#include <string>


namespace TA_IRS_App
{
    //IMPLEMENT_DYNAMIC( PIDSManagerDlg, ITrainBorne )

    PIDSManagerDlg::PIDSManagerDlg( CWnd* pParent /*=NULL*/ )
        : ITrainBorne( PIDSManagerDlg::IDD, pParent ),
          m_trainSelected( false ),
          m_messageSelected( false ),
          m_canSendPredefinedDisplayRequest( false ),
          m_canSendFreeTextDisplayRequest( false ),
          m_canSendClearRequest( false ),
          m_displayRequestManager( NULL ),
          m_displayAttributeManager( NULL ),
          m_locationKey( 0 )
    {
        FUNCTION_ENTRY( "PIDSManagerDlg" );
        FUNCTION_EXIT

    }

    PIDSManagerDlg::~PIDSManagerDlg()
    {
        FUNCTION_ENTRY( "~PIDSManagerDlg" );
        FUNCTION_EXIT
    }

    void PIDSManagerDlg::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        DDX_Control( pDX, IDC_MESSAGE_TAB, m_messageTab );
        DDX_Control( pDX, IDC_TIME_TYPE_PER_RUN, m_timeTypePerrun);
        DDX_Control( pDX, IDC_EDIT_PER_RUN, m_perRun);
        DDX_Control( pDX, IDC_TIME_TYPE_CONTINUOUS, m_timeTypeContinuous );
		DDX_Control( pDX, IDC_RESET, m_reset);
        DDX_Control( pDX, IDC_STATIC_PER_RUN_LABEL, m_timeTypePerrunLabel);
		DDX_Control( pDX, IDC_STATIC_CONTINUOUS_LABEL, m_timeTypeContinuousLabel);

    }

    void PIDSManagerDlg::TB_TrainSelected( TA_IRS_Bus::CommonTypes::TrainIdList selList )
    {
        if( selList.size() > 0 )
        {
            trainSelectionChanged( true );
        }
        else
        {
            trainSelectionChanged( false );
        }
    }

    bool PIDSManagerDlg::TB_DoApply()
    {
        //send request to the selected trains
        //if pre-defined message

        //else ad-hoc message

        FUNCTION_ENTRY( "OnDisplay" );

        TA_ASSERT ( m_displayAttributeManager != NULL, "Display attribute Manager is NULL" );
        TA_ASSERT ( m_displayRequestManager != NULL, "Display Request Manager is NULL" );
        TA_ASSERT ( m_parentGUI != NULL, "Train Selector GUI Manager is NULL" );

		//first check the perrun is in range
		if((m_displayAttributeManager->getMessageType() == DisplayAttributeManager::MESSAGE_PERRUN)
			&& !(m_displayAttributeManager->isPerrunInRange()))
		{
			MessageBox("Per-run is not in legal range, please set it in 1-255");
			return false;
		}

        // get message information (pre-defined or ad-hoc)
        // get the active page on the tab
        CDialog* activeMessageDialog = m_messageTab.getActiveDialog();

        // find out which page it is that's active
        // (pre-defined or ad-hoc)
        // try downcasting
        PIDSPredefinedMessagePage* predefinedPage = dynamic_cast<PIDSPredefinedMessagePage*>( activeMessageDialog );
        PIDSAdhocMessagePage* freeTextPage = dynamic_cast<PIDSAdhocMessagePage*>( activeMessageDialog );

        // if the pre-defined tab is active, and a valid message is selected
        if ( ( predefinedPage != NULL ) &&
                predefinedPage->hasValidSelection() )
        {
            // get the message
            TA_Base_Core::PredefinedMessage message = predefinedPage->getSelectedMessage();
            std::string prompt = TTISMessageManager::ASK_DISPLAY_PREDEFINED + std::string( " " ) + message.message;
            int ret = TTISMessageManager::getInstance().askQuestion( prompt.c_str() );
            if ( ret == IDYES )
            {
                m_displayRequestManager->onDisplay( *m_parentGUI, *m_displayAttributeManager, message );
            }
        }
        // otherwise if the free text page is selected and has a message entered
        else if ( ( freeTextPage != NULL ) &&
                  freeTextPage->hasValidMessage() )
        {
            // get the message
            PIDSAdhocMessagePage::AdHocMessage message = freeTextPage->getMessage();
            std::string prompt = TTISMessageManager::ASK_DISPLAY_FREETEXT+ std::string( " " ) + message.messageContent;
            int ret = TTISMessageManager::getInstance().askQuestion( prompt.c_str() );
            if ( ret == IDYES )
            {
                m_displayRequestManager->onDisplay( *m_parentGUI, *m_displayAttributeManager, message );
            }
        }
        else
        {
            TA_ASSERT( true, "The display button was active without a message selected" );
        }
        FUNCTION_EXIT;
        return true;
    }

    BOOL PIDSManagerDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );
        ITrainBorne::OnInitDialog();

        m_displayAttributeManager = new DisplayAttributeManager(
                m_timeTypeContinuous,
                m_timeTypePerrun,
                m_perRun,
                m_timeTypeContinuousLabel,
                m_timeTypePerrunLabel);

        m_displayRequestManager = new DisplayRequestManager( m_reset,
                *this );

        //Not use TB_SetObserver
        m_observer = new PIDSTrainSelectorGUI( m_parentGUI, this );

        m_canSendFreeTextDisplayRequest = RightsManager::getInstance().canDisplayFreeText();
        m_canSendPredefinedDisplayRequest = RightsManager::getInstance().canDisplayPredefined();
        m_canSendClearRequest = RightsManager::getInstance().canClear();


        // register for rights changes
        RightsManager::getInstance().registerForRightsChanges( this );


        //observe message select change
        m_messageTab.setMessageSelectionListener( this );

        m_messageTab.Init();

        FUNCTION_EXIT;
        return TRUE;
    }

    void PIDSManagerDlg::OnReset()
    {
        FUNCTION_ENTRY( "OnReset" );

        TA_ASSERT ( m_displayRequestManager != NULL, "Display Request Manager is NULL" );
        int ret = TTISMessageManager::getInstance().askQuestion( TTISMessageManager::ASK_SUBMIT_RESET);
        if ( ret == IDYES )
        {
            m_displayRequestManager->onClear( TA_Base_Core::CLEAR_ALL, *m_parentGUI );
        }

        FUNCTION_EXIT
    }


    void PIDSManagerDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        // deregister for rights changes
        RightsManager::getInstance().deregisterForRightsChanges( this );

        // delete m_observer
        if ( m_observer != NULL )
        {
            delete m_observer;
            m_observer = NULL;
        }

        // display request
        if ( m_displayRequestManager != NULL )
        {
            delete m_displayRequestManager;
            m_displayRequestManager = NULL;
        }

        // display attributes
        if ( m_displayAttributeManager != NULL )
        {
            delete m_displayAttributeManager;
            m_displayAttributeManager = NULL;
        }

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }

    void PIDSManagerDlg::trainSelectionChanged( bool trainsAreSelected )
    {
        FUNCTION_ENTRY( "trainSelectionChanged" );

        m_trainSelected = trainsAreSelected;
        TA_ASSERT ( m_displayRequestManager != NULL, "Display Request Manager is NULL" );

        bool canDisplay = false;

        // get the active message page
        CDialog *currentShow = m_messageTab.getActiveDialog();
        //If window has been destroyed
        if( FALSE == ::IsWindow( currentShow->m_hWnd ) )
        {
            return;
        }
        PIDSPredefinedMessagePage* predefinedPage =
            dynamic_cast<PIDSPredefinedMessagePage*>( currentShow );

        if ( predefinedPage != NULL )
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest );

        FUNCTION_EXIT;
    }

    void PIDSManagerDlg::OnTimeTypeContinuous()
    {
        FUNCTION_ENTRY( "onTimeTypeContinuous" );

        TA_ASSERT ( m_displayAttributeManager != NULL, "Display attribute Manager is NULL" );
        m_displayAttributeManager->onTimeTypeContinuous();

        FUNCTION_EXIT;
    }


    void PIDSManagerDlg::OnTimeTypePerrun()
    {
        FUNCTION_ENTRY( "onTimeTypePerrun" );

        TA_ASSERT ( m_displayAttributeManager != NULL, "Display attribute Manager is NULL" );
        m_displayAttributeManager->onTimeTypePerrun();

        FUNCTION_EXIT;
    }

    DisplayAttributeManager* PIDSManagerDlg::getDisplayAttributeManager()
    {
        FUNCTION_ENTRY( "getDisplayAttributeManager" );
        FUNCTION_EXIT;
        return m_displayAttributeManager;
    }

    void PIDSManagerDlg::TB_CanApply( bool bCanApplyNow )
    {
        if( IsWindowVisible() )
        {
            ITrainBorne::TB_CanApply( bCanApplyNow );
        }
    }
    void PIDSManagerDlg::TB_PageActived( bool bActived )
    {
        if( bActived )
        {
            CDialog* newPage = m_messageTab.getActiveDialog();
            PIDSPredefinedMessagePage* predefinedPage = NULL;
            PIDSAdhocMessagePage* adhocPage = NULL;
            if( predefinedPage = dynamic_cast<PIDSPredefinedMessagePage*>( newPage ) )
            {
                predefinedPage->windowShown();
            }
            else if( adhocPage = dynamic_cast<PIDSAdhocMessagePage*>( newPage ) )
            {
                // free text page
                adhocPage->windowShown();
            }
        }
    }

    void PIDSManagerDlg::messageSelectionChanged( bool messageIsSelected )
    {
        FUNCTION_ENTRY( "messageSelectionChanged" );

        m_messageSelected = messageIsSelected;
        TA_ASSERT ( m_displayRequestManager != NULL, "Display Request Manager is NULL" );

        bool canDisplay = false;

        // get the active message page
        PIDSPredefinedMessagePage* predefinedPage =
            dynamic_cast<PIDSPredefinedMessagePage*>( m_messageTab.getActiveDialog() );
        if ( predefinedPage != NULL )
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest );

        FUNCTION_EXIT;
    }

    BEGIN_MESSAGE_MAP( PIDSManagerDlg, ITrainBorne )
        ON_BN_CLICKED( IDC_RESET, OnReset)
        ON_BN_CLICKED( IDC_TIME_TYPE_CONTINUOUS, OnTimeTypeContinuous )
        ON_BN_CLICKED( IDC_TIME_TYPE_PER_RUN, OnTimeTypePerrun)
        ON_WM_DESTROY()
    END_MESSAGE_MAP()

}

