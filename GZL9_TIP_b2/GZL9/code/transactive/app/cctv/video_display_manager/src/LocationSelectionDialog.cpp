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

#include "app/cctv/video_display_manager/src/LocationSelectionDialog.h"

//#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    LocationSelectionDialog::LocationSelectionDialog( unsigned long initialLocation, CWnd* pParent )
        : CDialog( LocationSelectionDialog::IDD, pParent ),
          m_initialLocation( initialLocation ),
          m_selectedLocation( initialLocation )
    {
        FUNCTION_ENTRY( "LocationSelectionDialog( initialLocation, pParent )" );
        FUNCTION_EXIT;
    }


    LocationSelectionDialog::~LocationSelectionDialog()
    {
        FUNCTION_ENTRY( "~LocationSelectionDialog()" );
        FUNCTION_EXIT;
    }


    unsigned long LocationSelectionDialog::getSelectedLocation()
    {
        FUNCTION_ENTRY( "getSelectedLocation()" );
        FUNCTION_EXIT;
        return m_selectedLocation;
    }


    BOOL LocationSelectionDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        // Calls the parent implementation of this method
        CDialog::OnInitDialog();

        // Uses LocationCache to get a list of locations,
        TA_IRS_Bus::LocationCache::LocationInfoMap locaInfoMap = TA_IRS_Bus::LocationCache::getInstance().getAllLocations();

        // the customer is going to expect locations sorted by order ID
        // currently they are sorted by location key, so its a good idea to sort by location key before inserting
        TA_IRS_Bus::LocationCache::LocationInfoMap locationByOrder;

        TA_IRS_Bus::LocationCache::LocationInfoMap::iterator iter;
        for ( iter = locaInfoMap.begin(); iter != locaInfoMap.end(); ++iter )
        {
            // filter out the list to only station locations.
            if ( ( TA_Base_Core::ILocation::STATION == iter->second.type ) &&
                 ( 0 != iter->second.key ) )
            {
                locationByOrder[ iter->second.orderId ] = iter->second;
            }
        }

        int itemToSelect = CB_ERR;

        // now its sorted, insert each item
        for ( iter = locationByOrder.begin(); iter != locationByOrder.end(); ++iter )
        {
            // populates m_locationList with all the station locations. Use the
            // location name for the text, the location key for the item data.
            int index = m_locationList.AddString( iter->second.name.c_str() );
            m_locationList.SetItemData( index, iter->second.key );

            // if this matches m_initialLocation, then store the item to select
            if ( iter->second.key == m_initialLocation)
            {
                itemToSelect = index;
            }
        }

        // if m_initialLocation is in m_locationList
        // select that item in m_locationList
        if ( ( itemToSelect > CB_ERR ) &&
             ( itemToSelect < m_locationList.GetCount() ) )
        {
            m_locationList.SetCurSel( itemToSelect );
            
            // set m_selectedLocation to this location key
            m_selectedLocation = m_initialLocation;
        }
        else
        {
            // if not, set m_selectedLocation to 0
            m_selectedLocation = 0;
        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void LocationSelectionDialog::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(LocationSelectionDialog)
        DDX_Control( pDX, IDC_LOCSEL_COMBOBOX_LOCATIONS, m_locationList );
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( LocationSelectionDialog, CDialog )
        //{{AFX_MSG_MAP(LocationSelectionDialog)
        ON_CBN_SELCHANGE( IDC_LOCSEL_COMBOBOX_LOCATIONS, onSelectionChangeLocationList )
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void LocationSelectionDialog::onSelectionChangeLocationList()
    {
        FUNCTION_ENTRY( "onSelectionChangeLocationList" );

        // Get the selected item, and its item data.
        // set m_selectedLocation to the item data
        int curItemIdx = m_locationList.GetCurSel();

        if ( CB_ERR != curItemIdx )
        {
            m_selectedLocation = m_locationList.GetItemData( curItemIdx );
        }

        FUNCTION_EXIT;
    }
}
