/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/LocationListCtrl.cpp $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This class manages the list display used to show the available locations for the train radio group
  * in the TrainRadioGroupDetailView class
  */


#include "app/configuration/config_plugins/train_radio_groups/src/stdafx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/LocationListCtrl.h"
#include "app/configuration/config_plugins/train_radio_groups/src/resource.h"
#include "app/configuration/config_plugins/train_radio_groups/src/AddLocationDialog.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDatabaseAccessor.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_App
{
    const CString LocationListCtrl::LOCATION_NAME_COLUMN_NAME   = "Location Name";
    const int     LocationListCtrl::LOCATION_NAME_COLUMN_WIDTH  = 370;
    const int     LocationListCtrl::LOCATION_NAME_COLUMN        = 0;

    LocationListCtrl::LocationListCtrl()
        : CEditableListCtrl( true ),
          m_currentTrainRadioGroup( NULL )
    {
        FUNCTION_ENTRY( "LocationListCtrl" );
        
        // We will manage the cell requests
        SetManager( this );

        FUNCTION_EXIT;
    }


    LocationListCtrl::~LocationListCtrl()
    {
        FUNCTION_ENTRY( "~LocationListCtrl" );
        FUNCTION_EXIT;
    }


    void LocationListCtrl::addNewItems()
    {
        FUNCTION_ENTRY( "addNewItems" );

        TA_ASSERT ( m_currentTrainRadioGroup != NULL, "We do not yet have an TrainRadioGroup* item set" );

        AddLocationDialog dlg( *m_currentTrainRadioGroup );

        if ( dlg.DoModal() == IDCANCEL )
        {
            FUNCTION_EXIT;
            return;
        }

        std::vector<AddLocationDialog::Location>& locationsToAdd = dlg.getLocationsToAdd();

        for ( std::vector<AddLocationDialog::Location>::iterator iter = locationsToAdd.begin();
              iter != locationsToAdd.end(); ++iter )
        {
            try
            {
                m_currentTrainRadioGroup->addLocation( iter->key );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not set the data" );
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox( IDS_UE_010001 );

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not set the data" );
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox( IDS_UE_030001 );

                FUNCTION_EXIT;
                return;
            }
        }

        // Then repopulate the control to incorporate the changes
        populateControl( *m_currentTrainRadioGroup );

        FUNCTION_EXIT;
    }


    ICellManager::CellData LocationListCtrl::GetCellData( CEditableListCtrl* listCtrl, int item, int subItem )
    {
        FUNCTION_ENTRY( "GetCellData" );
        
        ICellManager::CellData data;
        CString str( listCtrl->GetItemText( item, subItem ) );
        data.push_back( ( LPCTSTR )str );

        FUNCTION_EXIT;
        return data;
    }


    CString LocationListCtrl::GetCellMask( CEditableListCtrl* listCtrl, int item, int subItem )
    {
        FUNCTION_ENTRY( "GetCellMask" );
        
        TA_ASSERT( false, "There are no cells of type CT_MASK" );

        FUNCTION_EXIT;
        return  "";
    }


    char LocationListCtrl::GetCellMaskPromptChar( CEditableListCtrl* listCtrl, int item, int subItem )
    {
        FUNCTION_ENTRY( "GetCellMaskPromptChar" );
        
        TA_ASSERT( false, "There are no cells of type CT_MASK" );

        FUNCTION_EXIT;
        return 0;
    }


    unsigned long LocationListCtrl::GetCellMaximumCharacters( CEditableListCtrl* listCtrl, int item, int subItem )
    {
        FUNCTION_ENTRY( "GetCellMaximumCharacters" );
        FUNCTION_EXIT;
        return 0;
    }


    ICellManager::CellType LocationListCtrl::GetCellType( CEditableListCtrl* listCtrl, int item, int subItem )
    {
        FUNCTION_ENTRY( "GetCellType" );

        TA_ASSERT( listCtrl != NULL, "List control passed is NULL" );
        TA_ASSERT ( m_currentTrainRadioGroup != NULL, "We do not yet have an TrainRadioGroup* item set" );

        // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)

        if ( item < 0 )
        {
            FUNCTION_EXIT;
            return CT_NOTEDITABLE;
        }

        FUNCTION_EXIT;
        return CT_SELECTION;
    }


    void LocationListCtrl::populateControl( TA_IRS_Core::ITrainRadioGroup& currentTrainRadioGroup )
    {
        FUNCTION_ENTRY( "populateControl" );

        m_currentTrainRadioGroup = &currentTrainRadioGroup;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {
            TA_IRS_Core::ITrainRadioGroup::LocationSet locations = m_currentTrainRadioGroup->getLocationCoverage();

            TrainRadioGroupDatabaseAccessor dbAccessor;
            std::map<unsigned long, std::string> locationNames = dbAccessor.getAllLocations();

            for ( TA_IRS_Core::ITrainRadioGroup::LocationSet::iterator iter = locations.begin();
                  iter != locations.end();
                  ++iter )
            {
                // Insert the process name in the first column
                std::string locationName( "" );
                unsigned long key = *iter;
                std::map<unsigned long, std::string>::iterator matching = locationNames.find( *iter );

                if ( matching != locationNames.end() )
                {
                    locationName = matching->second;
                }

                int position = InsertItem( -1, locationName.c_str(), 0 );

                SetItemData( position, *iter );
            }

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_010001 );

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_030001 );

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void LocationListCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        setupList();

        CEditableListCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    void LocationListCtrl::removeSelectedItem()
    {
        FUNCTION_ENTRY( "removeSelectedItem" );

        POSITION pos = GetFirstSelectedItemPosition();

        if ( pos == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        int selected = GetNextSelectedItem( pos );

        TA_ASSERT ( m_currentTrainRadioGroup != NULL, "We do not yet have an TrainRadioGroup* item set" );

        unsigned long key = GetItemData( selected );

        try
        {
            m_currentTrainRadioGroup->removeLocation( key );

            // Then repopulate the control to incorporate the changes
            populateControl( *m_currentTrainRadioGroup );
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_010001 );

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_030001 );

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;

        return;
    }


    void LocationListCtrl::setupList()
    {
        FUNCTION_ENTRY( "setupList" );

        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
                          LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES );

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn( LOCATION_NAME_COLUMN,  LOCATION_NAME_COLUMN_NAME,  LVCFMT_LEFT, LOCATION_NAME_COLUMN_WIDTH );

        FUNCTION_EXIT;
        return;
    }


    bool LocationListCtrl::UpdateCellValue( CEditableListCtrl* listCtrl, int item, int subItem, CString value )
    {
        FUNCTION_ENTRY( "UpdateCellValue" );

        TA_ASSERT ( m_currentTrainRadioGroup != NULL, "We do not yet have an TrainRadioGroup* item set" );
        TA_ASSERT( listCtrl != NULL, "List control passed is NULL" );


        unsigned long locationKey = listCtrl->GetItemData( item );

        try
        {
            m_currentTrainRadioGroup->addLocation( locationKey );

            TA_Base_App::AbstractDetailView* detailView = dynamic_cast<TA_Base_App::AbstractDetailView*>( GetParent() );

            if ( detailView != NULL )
            {
                detailView->refreshButtonAvailability();
            }
            
            FUNCTION_EXIT;
            return true;
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_010001 );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not set the data" );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox( IDS_UE_030001 );
        }

        FUNCTION_EXIT;
        return false;
    }

}