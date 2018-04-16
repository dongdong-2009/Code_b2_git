/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDetailView.cpp $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view allows the user to configure a train radio group's details.
  */


#include "app/configuration/config_plugins/train_radio_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDetailView.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupListView.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDatabaseAccessor.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataBaseException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;


namespace TA_IRS_App
{
    
    const int TrainRadioGroupDetailView::TSI_FIELD_LENGTH = 32;


    IMPLEMENT_DYNCREATE( TrainRadioGroupDetailView, CFormView )


    TrainRadioGroupDetailView::TrainRadioGroupDetailView()
        : TA_Base_App::AbstractDetailView( TrainRadioGroupDetailView::IDD ),
          m_currentTrainRadioGroup( NULL ),
          m_databaseAccessor( NULL ),
          m_listView( NULL )
    {
        FUNCTION_ENTRY( "TrainRadioGroupDetailView" );
        FUNCTION_EXIT;
    }


    TrainRadioGroupDetailView::~TrainRadioGroupDetailView()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupDetailView" );
        FUNCTION_EXIT;
    }


    TA_Base_App::MessageSender::ConfigUpdateType TrainRadioGroupDetailView::getConfigMessageType() const
    {
        FUNCTION_ENTRY( "getConfigMessageType" );
        FUNCTION_EXIT;
        return TA_Base_App::MessageSender::NOT_APPLICABLE;
    }


    void TrainRadioGroupDetailView::DoDataExchange( CDataExchange* pDX )
    {
        CFormView::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(TrainRadioGroupDetailView)
        DDX_Control( pDX, IDC_LOCATION_LIST, m_locationList );
        DDX_Control( pDX, IDC_EDIT_NAME, m_name );
        DDX_Control( pDX, IDC_EDIT_TIS, m_tsi );
        DDX_Control( pDX, IDC_TIS_BITMAP, m_tsiBitmap );
        DDX_Control( pDX, IDC_NAME_BITMAP, m_nameBitmap );
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( TrainRadioGroupDetailView, AbstractDetailView )
        //{{AFX_MSG_MAP(TrainRadioGroupDetailView)
        ON_BN_CLICKED( IDC_BUTTON_APPLY, OnButtonApply )
        ON_BN_CLICKED( IDC_BUTTON_RESET, OnButtonReset )
        ON_BN_CLICKED( IDC_BUTTON_ADDLOCATION, OnButtonAddlocation )
        ON_EN_KILLFOCUS( IDC_EDIT_TIS, OnKillfocusEditTSI )
        ON_BN_CLICKED( IDC_BUTTON_REMOVE_LOCATION, OnButtonRemoveLocation )
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void TrainRadioGroupDetailView::OnButtonApply()
    {
        FUNCTION_ENTRY( "OnButtonApply" );

        GetDlgItem( IDC_BUTTON_APPLY )->GetFocus();

        applyButtonPressed( m_currentTrainRadioGroup );

        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::OnButtonReset()
    {
        FUNCTION_ENTRY( "OnButtonReset" );

        resetButtonPressed( m_currentTrainRadioGroup );

        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::OnButtonAddlocation()
    {
        FUNCTION_ENTRY( "OnButtonAddlocation" );

        m_locationList.addNewItems();

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::OnButtonRemoveLocation()
    {
        FUNCTION_ENTRY( "OnButtonRemoveLocation" );

        m_locationList.removeSelectedItem();

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::OnKillfocusEditTSI()
    {
        FUNCTION_ENTRY( "OnKillfocusEditTSI" );

        if ( m_currentTrainRadioGroup != NULL )
        {
            try
            {
                CString tsi;
                m_tsi.GetWindowText( tsi );
                m_currentTrainRadioGroup->setTsi( std::string( tsi.GetBuffer(0) ) );

                // need to refresh the name field after a TSI change
                m_name.SetWindowText( m_currentTrainRadioGroup->getName().c_str() );
				if ( NULL != m_listView)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Name: %s",m_currentTrainRadioGroup->getName().c_str() );

					m_listView->itemDetailsChanged(m_currentTrainRadioGroup->getUniqueIdentifier(),
						                           m_currentTrainRadioGroup->getName().c_str() );
				}
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "cant set the data" );
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox( IDS_UI_010001 );

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "dataexception", "cant set the data" );
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox( IDS_UI_030001 );

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
        return;
    }


    void TrainRadioGroupDetailView::showItemDetails( bool isShown )
    {
        GetDlgItem( IDC_STATIC_NAME )->ShowWindow( isShown );
        GetDlgItem( IDC_EDIT_NAME )->ShowWindow( isShown );
        GetDlgItem( IDC_STATIC_TIS )->ShowWindow( isShown );
        GetDlgItem( IDC_EDIT_TIS )->ShowWindow( isShown );
        GetDlgItem( IDC_NAME_BITMAP )->ShowWindow( isShown );
        GetDlgItem( IDC_TIS_BITMAP )->ShowWindow( isShown );
        GetDlgItem( IDC_STATIC_LOCATIONS )->ShowWindow( isShown );
        GetDlgItem( IDC_LOCATION_LIST )->ShowWindow( isShown );
        GetDlgItem( IDC_BUTTON_APPLY )->ShowWindow( isShown );
        GetDlgItem( IDC_BUTTON_RESET )->ShowWindow( isShown );
        GetDlgItem( IDC_BUTTON_ADDLOCATION )->ShowWindow( isShown );
        GetDlgItem( IDC_BUTTON_REMOVE_LOCATION )->ShowWindow( isShown );
        GetDlgItem( IDC_STATIC_FRAME )->ShowWindow( isShown );

        // ensure the name field is not editable
        if ( true == isShown )
        {
            GetDlgItem( IDC_EDIT_NAME )->EnableWindow( false );
        }
    }


    void TrainRadioGroupDetailView::populateItemDetails( TA_Base_Core::IConfigItem* currentItem )
    {
        FUNCTION_ENTRY( "populateItemDetails" );

        if ( ( NULL == m_databaseAccessor ) ||
             ( NULL == m_listView ) )
        {
            m_databaseAccessor = dynamic_cast<TrainRadioGroupDatabaseAccessor*>( getDatabaseAccessor() );
            m_listView = dynamic_cast<TrainRadioGroupListView*>( getListView() );

            TA_ASSERT( m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method" );
            TA_ASSERT( m_listView != NULL, "The list view has not been set yet or is not a proper TrainRadioGroupDetailView" );
        }

        m_currentTrainRadioGroup = static_cast<TA_IRS_Core::ITrainRadioGroup*>( currentItem );


        if ( ( NULL != currentItem ) &&
             ( NULL == m_currentTrainRadioGroup ) )
        {
            // this means the cast failed, this is assertable
            TA_ASSERT( false, "Downcast failed" )
        }

        // geniunely nothing was selected
        if ( NULL == m_currentTrainRadioGroup )
        {
            showItemDetails( false );
            FUNCTION_EXIT;
            return;
        }

        try
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"Name : %s, Tsi : %s ", m_currentTrainRadioGroup->getName().c_str(),
				m_currentTrainRadioGroup->getTsi().c_str() );
            m_name.SetWindowText( m_currentTrainRadioGroup->getName().c_str() );
            m_tsi.SetWindowText( m_currentTrainRadioGroup->getTsi().c_str() );

            m_locationList.populateControl( *m_currentTrainRadioGroup );

            showItemDetails( true );

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve trainRadioGroup details to display" );

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_030001 );
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve trainRadioGroup details to display" );

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_010001 );
        }

        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::refreshButtonAvailability()
    {
        FUNCTION_ENTRY( "refreshButtonAvailability" );

        if ( m_currentTrainRadioGroup == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        if ( m_currentTrainRadioGroup->isNew() )
        {
            GetDlgItem( IDC_BUTTON_RESET )->EnableWindow( false );
            GetDlgItem( IDC_BUTTON_APPLY )->EnableWindow( true );
        }
        else if ( m_currentTrainRadioGroup->hasChanged() )
        {
            GetDlgItem( IDC_BUTTON_RESET )->EnableWindow( true );
            GetDlgItem( IDC_BUTTON_APPLY )->EnableWindow( true );
        }
        else
        {
            GetDlgItem( IDC_BUTTON_RESET )->EnableWindow( false );
            GetDlgItem( IDC_BUTTON_APPLY )->EnableWindow( false );
        }

        FUNCTION_EXIT;
    }


    void TrainRadioGroupDetailView::OnInitialUpdate()
    {
        FUNCTION_ENTRY( "OnInitialUpdate" );

        CFormView::OnInitialUpdate();
        
        // limit the number of character that can be entered into fields so that they can match the database schema.
        m_tsi.SetLimitText( TSI_FIELD_LENGTH );

        // hide all train radio group detail until we are told to populate.
        showItemDetails( false );

        FUNCTION_EXIT;
    }


#ifdef _DEBUG
    void TrainRadioGroupDetailView::AssertValid() const
    {
        FUNCTION_ENTRY( "AssertValid" );

        AbstractDetailView::AssertValid();

        FUNCTION_EXIT;
    }

    void TrainRadioGroupDetailView::Dump( CDumpContext& dc ) const
    {
        FUNCTION_ENTRY( "Dump" );

        AbstractDetailView::Dump( dc );

        FUNCTION_EXIT;
    }

#endif //_DEBUG

}

