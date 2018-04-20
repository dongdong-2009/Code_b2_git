/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_zone_group/src/PAZoneGroupDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an PA Zone Group's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/data_access_interface/pa_4669/src/IConfigPaZoneGroup.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "ZoneListHelper.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"

#include "core/utilities/src/DateFormat.h" // TD12474

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using  namespace TA_Base_App::TA_Configuration;
using namespace TA_Base_Bus;
using namespace TA_IRS_Bus;

namespace TA_IRS_App
{
    // The titles for the change dialog box
    const CString PaZoneGroupDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";
    const CString PaZoneGroupDetailView::SELECT_ZONES_TITLE = "Select Zone(s)";
    
    const int PaZoneGroupDetailView::LOCATION_FIELD_LENGTH           = 40;
    const int PaZoneGroupDetailView::LABEL_FIELD_LENGTH              = 40;  // Length of PA_ZONE_GROUP.LABEL      

    IMPLEMENT_DYNCREATE(PaZoneGroupDetailView, CFormView)


    PaZoneGroupDetailView::PaZoneGroupDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(PaZoneGroupDetailView::IDD),
        m_allZonesValid(true),
        m_applyCreationTime(0)
    {
	    //{{AFX_DATA_INIT(PaZoneGroupDetailView)
	//}}AFX_DATA_INIT
    }


    PaZoneGroupDetailView::~PaZoneGroupDetailView()
    {
    }


    void PaZoneGroupDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaZoneGroupDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

    	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);    	
        DDX_Control(pDX, IDC_EDIT_LABEL, m_labelEdt);
    	DDX_Control(pDX, IDC_CHECK_IS_EVENT_GROUP, m_isEventGroupBtn);
    	
        DDX_Control(pDX, IDC_BUTTON_LOCATION, m_locationBtn);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);

        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);
    	DDX_Control(pDX, IDC_LABEL_BITMAP, m_transparentImageLabel);
    	
        DDX_Control(pDX, IDC_ZONE_LIST, m_zoneListCtrl);
        DDX_Control(pDX, IDC_ADD_ZONE, m_addZoneBtn);
    	DDX_Control(pDX, IDC_REMOVE_ZONE, m_removeZoneBtn);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaZoneGroupDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaZoneGroupDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
	ON_BN_CLICKED(IDC_CHECK_IS_EVENT_GROUP, onButtonIsEventGroup)
    ON_BN_CLICKED(IDC_ADD_ZONE, onButtonAddZone)
    ON_BN_CLICKED(IDC_REMOVE_ZONE, onButtonRemoveZone)
    ON_NOTIFY(NM_CLICK, IDC_AEE_LIST, OnClickZoneList)

	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_LABEL, onKillFocusEditLabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void PaZoneGroupDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_labelEdt.SetLimitText(LABEL_FIELD_LENGTH);
        
  		m_zoneListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
        m_zoneListCtrl.setHighlightColour(RGB(255, 0, 0));   // To match TA_Base_Bus::TextColourEdit

        // Add in the relevant columns for zone list
        ZoneListHelper::formatListControl(m_zoneListCtrl);

        // Hide all the PA Zone Group details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    std::list<unsigned long> PaZoneGroupDetailView::popupZoneSelectionDialog()
    {
        TA_ASSERT(0 != m_databaseAccessor, "DatabaseAccessor is null");

        PaZoneRecords allZones = m_databaseAccessor->getAllZonesForLocation(m_configItem->getLocationKey());

        std::set<unsigned long> existingZones = getCurrentZones();
        
        TA_Base_App::MultipleChangeDialog::ItemsForSelection dialogItems;

        bool isOccZoneGroup = m_databaseAccessor->isOccLocationKey(m_configItem->getLocationKey());
        unsigned long depotLocationKey = m_databaseAccessor->getDepotLocationKey();
        PaZoneGroupCachedMap::ZoneGroupFilter exclusionFilter(isOccZoneGroup, depotLocationKey);

        // We don't add in the items that are already in this group
        // All instances of 'existingZones' in the zone array can be disregarded
        // (since they're already in our selection) 
        for (PaZoneRecords::iterator itr = allZones.begin(); itr != allZones.end(); itr ++)
        {
            if (existingZones.find(itr->m_pKey) == existingZones.end())
            {
                // Additionally, ensure not displaying items not valid for zone group creation
                if (exclusionFilter.isMatch(*itr))
                {
                    std::vector<std::string> rowStrings = ZoneListHelper::getStringElements(*itr);

                    // Not an already selected zone, may add to selection list
                    // Assign the array of detail strings for the given key
                    dialogItems[itr->m_pKey] = rowStrings;
                }
            }
        }
    
        const std::list<unsigned long> emptySel;
        TA_Base_App::MultipleChangeDialog dlg(dialogItems, 
                                    SELECT_ZONES_TITLE, 
                                    ZoneListHelper::getZoneListHeaderProps(),
                                    emptySel, 
                                    this);

        int response = dlg.DoModal();
        
        if (IDOK == response)
        {                     
            return dlg.getSelectedKeys();
        }
        else
        {
            return emptySel;
        }
    }


    void PaZoneGroupDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaZoneGroupDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaZoneGroupDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaZoneGroupListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaZoneGroupDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaZoneGroup*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        // Tell this configurable item to recreate itself in the database if some
        // external source happens to delete it during modifications
        m_configItem->setRecreateOnDeletionFlag(true);

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigPaZoneGroup* object
            CString buffer;

            if (TA_Base_Core::DatabaseKey::isInvalidKey(m_configItem->getLocationKey()))
            {
                m_locationEdt.SetWindowText("");
                m_locationEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
            }
            else
            {
                buffer = m_databaseAccessor->getLocationName(m_configItem->getLocationKey()).c_str();
                m_locationEdt.SetWindowText(buffer);
                m_locationEdt.setTextType( buffer == "" ? TA_Base_Bus::TextColourEdit::INVALID : TA_Base_Bus::TextColourEdit::VALID );
            }
                        
            m_labelEdt.SetWindowText( m_configItem->getLabel().c_str() );
            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            
            m_isEventGroupBtn.SetCheck(m_configItem->getIsEventGroup() ? TRUE : FALSE);
            
            if( m_configItem->isNew() )
            {

                // This is a new PA Zone Group or at least a copy of an PA Zone Group so we indicate this in the type
                // field and make sure the modify and create dates are blank.

                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);

                // Disable the reset button as this is a new item so reset has no functionality
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
            }
            else
            {

                m_type.SetWindowText( NO_VALUE );

				TA_Base_Core::DateFormat dateFormat; // TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_configItem->getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
					// TD12474 ++
                    //m_modifyDate.SetWindowText( dateModified.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
					//m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
					std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
					m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
					// ++ TD12474
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_configItem->getDateCreated() );
				// TD12474 ++
				//m_createDate.SetWindowText( dateCreated.Format(TA_Base_App::TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
				// ++ TD12474

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }            

            populateZoneListArea(true);

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& ex)
        {
            ex.what();
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA Zone Group details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++
        }
        catch ( const TA_Base_Core::DatabaseException& ex)
        {
            ex.what();
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA Zone Group details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA Zone Group";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }

    
    std::set<unsigned long> PaZoneGroupDetailView::getCurrentZones()
    {    
        TA_ASSERT(m_configItem != 0, "Config item is null");

        return m_configItem->getQueuedAssociatedPaZoneKeys();
    }
     

    void PaZoneGroupDetailView::populateZoneListArea(bool initialisation)
    {        
        // All are valid until we find an invalid one..
        m_allZonesValid = true;
    
        m_removeZoneBtn.EnableWindow(FALSE);

      	m_zoneListCtrl.DeleteAllItems(); 
        m_zoneListCtrl.clearHighlightDataSet();
                
        std::set<unsigned long> zones = getCurrentZones();
    
        for (std::set<unsigned long>::iterator itr = zones.begin(); itr != zones.end(); itr ++)
        {
            bool isValid = true;

            try
            {
                // Confirm each zone in turn is valid
                PaZoneRecord zone = m_databaseAccessor->getZoneFromKey(*itr);                

                // If we're at the OCC terminal, zones can have any location key
                if (!m_databaseAccessor->isOccLocationKey(m_configItem->getLocationKey()))
                {
                    // If the station key doesn't match zone location key, then it's invalid
                    if (m_configItem->getLocationKey() != zone.m_locationKey)
                    {
                        isValid = false;
                    }
                }

                int nPos = ZoneListHelper::insertRow(m_zoneListCtrl, zone);
                m_zoneListCtrl.SetItemData(nPos, *itr);
            }
            catch (...)
            {
                // Failure to obtain zone from cached maps - list as invalid and 'unknown' record
                int nPos = ZoneListHelper::insertNullRow(m_zoneListCtrl, *itr);
                m_zoneListCtrl.SetItemData(nPos, *itr);
                isValid = false;
            }                              

            if (!isValid)
            {
                m_allZonesValid = false;
                m_zoneListCtrl.registerHighlightData(*itr);
            }                      
        }
    }


    void PaZoneGroupDetailView::onButtonAddZone() 
    {   
        updateZoneList(ADD_NEW);
    }


    void PaZoneGroupDetailView::onButtonRemoveZone() 
    {   
        updateZoneList(REMOVE_EXISTING);
    }


    void PaZoneGroupDetailView::onButtonIsEventGroup()
    {
        TA_ASSERT(m_configItem != NULL, "Config item null");
        
        if (m_isEventGroupBtn.GetCheck() & BST_CHECKED)
        {
            m_configItem->setIsEventGroup(true);
        }
        else
        {
            m_configItem->setIsEventGroup(false);
        }

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }


    void PaZoneGroupDetailView::updateZoneList(EZoneUpdate update)
    {
        FUNCTION_ENTRY("updateZoneList");
                
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
        if (!m_configItem) 
        {           
            return;
        }
        
        // Initially grab copy of current set of entities, for checking if any changes made
        std::set<unsigned long> zones = getCurrentZones();
        
        bool changesMade = false;

        switch (update)
        {
        case ADD_NEW: 
            {
                // When adding new elements, use a multiple selection dialog
                std::list<unsigned long> keys = popupZoneSelectionDialog();
                
                if (keys.size() > 0)
                {
                    // Attempt to add in the entity - will ignore request if selected an item already in list
                    m_configItem->queueAdditionOfPaZoneGroupHelperRecords(keys);                

                    changesMade = true;
                }
            }                    
            break;
        case REMOVE_EXISTING:
            std::vector<unsigned int> sel = TA_Base_Bus::SelectionHelper::getSelectedItemIndices(m_zoneListCtrl);
            std::list<unsigned long> keysToRemove;

            for (std::vector<unsigned int>::iterator itr = sel.begin(); itr != sel.end(); itr ++)            
            {
                DWORD key = m_zoneListCtrl.GetItemData(*itr);

                // Remove the selected item, provided it has a valid pKey
                if (key >= 0)
                {
                    keysToRemove.push_back(key);
                }
            }
            
            // Perform the actual removal
            if (keysToRemove.size() > 0)
            {             
                m_configItem->queueDeletionOfPaZoneGroupHelperRecords(keysToRemove);                
                changesMade = true;                
            }
            break;
        }

        if (changesMade)
        {
            // Only refresh this section of the form if a change was made..
            populateZoneListArea();
    
            m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            refreshButtonAvailability();

        }

        FUNCTION_EXIT;        
    }


	void PaZoneGroupDetailView::OnClickZoneList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        BOOL validSel = (m_zoneListCtrl.GetSelectedCount() > 0);

        m_removeZoneBtn.EnableWindow(validSel);

		*pResult = 0;
	}


    void PaZoneGroupDetailView::refreshButtonAvailability()
    {
        if (m_configItem == NULL)
        {
            return;
        }

        if (m_configItem->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_configItem->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }


    void PaZoneGroupDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
	    
        // Retrieve the current location this PA Zone Group is using
        CString currentLocation("");
        m_locationEdt.GetWindowText( currentLocation );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllLocations();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_LOCATION_DIALOG_TITLE, currentLocation, this);
        dlg.setSortItems( false );
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a location");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new location which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentLocation != dlg.getSelectedName())
            {
                // If this is a different status datapoint then set it in our database object.
                m_configItem->setLocationKey( dlg.getSelectedKey() );

                onLocationChanged();

                // Now update the entity type the details view is showing
                m_locationEdt.SetWindowText( dlg.getSelectedName() );
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve status datapoint key so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve status datapoint key so cannot update existing");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }


    void PaZoneGroupDetailView::onKillfocusEditLocation() 
    {
        FUNCTION_ENTRY("onKillfocusEditLocation");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString locationString;
        m_locationEdt.GetWindowText(locationString);
        unsigned long locationKey = m_databaseAccessor->getLocationKey((LPCTSTR)locationString);

        if (locationKey == -1)
        {
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the location string has not changed then there is no point doing an update
            if ( locationKey == m_configItem->getLocationKey())
            {
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setLocationKey( locationKey );

            onLocationChanged();

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone Group could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void PaZoneGroupDetailView::onKillFocusEditLabel() 
    {
        FUNCTION_ENTRY("onKillFocusEditLabel");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString labelString;
        m_labelEdt.GetWindowText(labelString);

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (labelString == m_configItem->getLabel().c_str())
            {                
                m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setLabel( (LPCTSTR) labelString );
            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone Group could not be found in the database");

            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }  


    void PaZoneGroupDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void PaZoneGroupDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_locationEdt.isTextValid()    || 
             !m_labelEdt.isTextValid()       ||  
             !m_allZonesValid )
        {
            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "invalid names still exist in the fields";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164 
            return;
        }
        
        {
            // Must set up the audit data for the config item, 
            // otherwise it will fail when attempting to write changes
            TA_Base_Core::IConfigPaZoneGroup::AuditInfo auditInfo;

    		std::auto_ptr< TA_Base_Core::IEntityData > data(
    			TA_Base_Core::EntityAccessFactory::getInstance().getEntity( 
    			    TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) ) );
    	
    		if ( data.get() == 0 )
    		{
                auditInfo.entityKey = 0;
            }
            else
            {
                auditInfo.entityKey = data->getKey();
            }
    		
            auditInfo.sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

            m_configItem->setAuditData(auditInfo);
        }

        if (!m_configItem->isNew())
        {
            m_applyCreationTime = m_configItem->getDateCreated();
        }
        else
        {
            m_applyCreationTime = 0;
        }
        
        // Now audit data set up, can safely call applyButtonPressed
        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    void PaZoneGroupDetailView::sendMessagesForTheChange(TA_Base_App::MessageSender::UpdateDetails updateDetails)
    {
        if (0 != m_applyCreationTime)
        {
            // If we have a creation time setting, means this item was modified
            // (as opposed to created)
            // If the creation time of the modified item has changed, it means
            // the item was re-created internally (not simply modified) within the applyChanges
            // function.  This occurs when someone is editing a zone group item, and 
            // apply the changes - but a separate application (PA Manager)
            // has deleted the object before changes applied

            // So check new created time of config item..
            if (m_configItem->getDateCreated() > m_applyCreationTime)
            {
                // The date created has advanced, must have been created again
                // Flag appropriately, so update correctly propogated to applications
                // (if it remains a modification update, other applications won't get
                // the update, because they think the item has been deleted and hence are
                // not listening out for it anymore)
                updateDetails.isNewItem = true;
            }
        }

        AbstractDetailView::sendMessagesForTheChange(updateDetails);
    }


    void PaZoneGroupDetailView::onLocationChanged()
    {   
        // Whenever the operator changes the location, it may affect
        // validity of the zone list associations - so refresh their status
        // (OCC terminal can view all, stations can only associate with local zones)
        CWaitCursor wait;

        // Refresh associations list
        populateZoneListArea();
    }


    /////////////////////////////////////////////////////////////////////////////
    // PaZoneGroupDetailView diagnostics

    #ifdef _DEBUG
    void PaZoneGroupDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaZoneGroupDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

