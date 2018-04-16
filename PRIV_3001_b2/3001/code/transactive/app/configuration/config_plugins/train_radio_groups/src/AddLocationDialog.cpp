/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/AddLocationDialog.cpp $
  * @author:  oliverk
  * @version: $Revision: #1 $
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This dialog is used to add one or more location to trian radio group.
  */

#include "app/configuration/config_plugins/train_radio_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/AddLocationDialog.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDatabaseAccessor.h"

#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    AddLocationDialog::AddLocationDialog( TA_IRS_Core::ITrainRadioGroup& currentTrainRadioGroup )
        : CDialog( AddLocationDialog::IDD, NULL ),
          m_currentTrainRadioGroup( currentTrainRadioGroup )
    {
        FUNCTION_ENTRY( "AddLocationDialog" );
        FUNCTION_EXIT;
    }


    AddLocationDialog::~AddLocationDialog()
    {
        FUNCTION_ENTRY( "~AddLocationDialog" );
        FUNCTION_EXIT;
    }


    std::vector<AddLocationDialog::Location> AddLocationDialog::getLocationsToAdd()
    {
        FUNCTION_ENTRY( "getLocationsToAdd" );
        FUNCTION_EXIT;
        return m_locations;
    }


    void AddLocationDialog::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(AddLocationDialog)
        DDX_Control( pDX, IDC_LOCATION_LIST, m_locationList );
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( AddLocationDialog, CDialog )
        //{{AFX_MSG_MAP(AddLocationDialog)
        ON_BN_CLICKED( IDOK, onOK )
        ON_NOTIFY( NM_DBLCLK, IDC_LOCATION_LIST, onDblclkLocationList )
        ON_NOTIFY( LVN_ITEMCHANGED, IDC_LOCATION_LIST, onItemchangedLocationList )
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void AddLocationDialog::onDblclkLocationList( NMHDR* pNMHDR, LRESULT* pResult )
    {
        FUNCTION_ENTRY( "onDblclkLocationList" );
        
        onOK();
        *pResult = 0;
        
        FUNCTION_EXIT;
    }


    BOOL AddLocationDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        CRect listRect;
        m_locationList.GetWindowRect( listRect );
        m_locationList.InsertColumn( 0, "", LVCFMT_LEFT, listRect.Width() - 10 );

        m_locationList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );

        // Get all locations
        TrainRadioGroupDatabaseAccessor dbAccessor;
        std::map<unsigned long, std::string> allLocations = dbAccessor.getAllLocations();

        TA_IRS_Core::ITrainRadioGroup::LocationSet existingLocations = m_currentTrainRadioGroup.getLocationCoverage();

        // filter out all locations that are already used
        for ( std::map<unsigned long, std::string>::iterator iter = allLocations.begin();
              iter != allLocations.end(); ++iter )
        {
            TA_IRS_Core::ITrainRadioGroup::LocationSet::iterator findIter = existingLocations.find( iter->first );
            if ( existingLocations.end() == findIter )
            {
                // We can add this to the list as it is not already used
                int position = m_locationList.InsertItem( 0, iter->second.c_str() );
                m_locationList.SetItemData( position, iter->first );
            }
        }

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
    }


    void AddLocationDialog::onItemchangedLocationList( NMHDR* pNMHDR, LRESULT* pResult )
    {
        // note no FUNCTION_ENTRY/FUNCTION_EXIT because this will be called very very frequently

        NM_LISTVIEW* pNMListView = ( NM_LISTVIEW* )pNMHDR;

        POSITION pos = m_locationList.GetFirstSelectedItemPosition();

        if ( pos == NULL )
        {
            GetDlgItem( IDOK )->EnableWindow( FALSE );
        }
        else
        {
            GetDlgItem( IDOK )->EnableWindow( TRUE );
        }

        *pResult = 0;
    }


    void AddLocationDialog::onOK()
    {
        FUNCTION_ENTRY( "onOK" );

        int numSelected = m_locationList.GetSelectedCount();

        if ( numSelected <= 0 )
        {
            // Nothing to modify so let's get out of here
            CDialog::OnOK();
        }

        int item = -1; // This will hold the last selected item

        // Get all of the selected items.
        Location location;

        for ( int i = 0; i < numSelected; ++i )
        {
            item = m_locationList.GetNextItem( item, LVNI_SELECTED );

            location.key = m_locationList.GetItemData( item );
            location.name = m_locationList.GetItemText( item, 0 );

            m_locations.push_back( location );
        }

        CDialog::OnOK();

        FUNCTION_EXIT;
    }

}