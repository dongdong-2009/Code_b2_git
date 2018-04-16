/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_zone/src/PAZoneDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an PA Zone's details.
  */

#pragma warning(disable:4786 4284)

#include <crtdbg.h>

#include "StdAfx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaZone.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"

#include "core/utilities/src/DateFormat.h" // TD12474

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using  namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    // The titles for the change dialog box
    const CString PaZoneDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";
    const CString PaZoneDetailView::CHANGE_ENTITY_NAME = "Change Equipment Entity Name";
    const CString PaZoneDetailView::SELECT_ASSOCIATION_ENTITY_NAME = "Select TIS Associated Equipment Entity Name";
    
    const int PaZoneDetailView::LOCATION_FIELD_LENGTH           = 40;
    const int PaZoneDetailView::ZONE_ID_FIELD_LENGTH            = 5;
    const int PaZoneDetailView::LABEL_FIELD_LENGTH              = 40;
    const int PaZoneDetailView::EQUIPMENT_ENTITY_NAME_FIELD_LENGTH = 85;   // length of ENTITY.NAME

    const std::string ASS_ENTITY_LIST_HEADER = "TIS Associated Equipment Entity Name";
        
    IMPLEMENT_DYNCREATE(PaZoneDetailView, CFormView)


    PaZoneDetailView::PaZoneDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(PaZoneDetailView::IDD),
        m_allEntityAssociationsValid(true)
    {
	    //{{AFX_DATA_INIT(PaZoneDetailView)
	//}}AFX_DATA_INIT
    }


    PaZoneDetailView::~PaZoneDetailView()
    {
    }


    void PaZoneDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaZoneDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

    	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);    	
        DDX_Control(pDX, IDC_EDIT_ZONE_ID, m_idEdt);
    	DDX_Control(pDX, IDC_EDIT_LABEL, m_labelEdt);
    	DDX_Control(pDX, IDC_EDIT_STN_EQUIPMENT_ENTITY_NAME, m_stnEquipmentEntityNameEdt);
        DDX_Control(pDX, IDC_EDIT_OCC_EQUIPMENT_ENTITY_NAME, m_occEquipmentEntityNameEdt);
    	
        DDX_Control(pDX, IDC_BUTTON_LOCATION, m_locationBtn);
        DDX_Control(pDX, IDC_BUTTON_STN_EQUIPMENT_ENTITY_NAME, m_stnEquipmentEntityNameBtn);
        DDX_Control(pDX, IDC_BUTTON_OCC_EQUIPMENT_ENTITY_NAME, m_occEquipmentEntityNameBtn);
    	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);

        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);
    	DDX_Control(pDX, IDC_ID_BITMAP, m_transparentImageId);
        DDX_Control(pDX, IDC_LABEL_BITMAP, m_transparentImageLabel);
    	//DDX_Control(pDX, IDC_SEN_BITMAP, m_transparentImageSen);

        DDX_Control(pDX, IDC_AEE_LIST, m_associationListCtrl);
        DDX_Control(pDX, IDC_ADD_AEE, m_addAssociationBtn);
    	DDX_Control(pDX, IDC_EDIT_AEE, m_editAssociationBtn);
    	DDX_Control(pDX, IDC_REMOVE_AEE, m_removeAssociationBtn);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaZoneDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaZoneDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
	ON_BN_CLICKED(IDC_BUTTON_STN_EQUIPMENT_ENTITY_NAME, onButtonStnEquipmentEntityName)
    ON_BN_CLICKED(IDC_BUTTON_OCC_EQUIPMENT_ENTITY_NAME, onButtonOccEquipmentEntityName)
    ON_BN_CLICKED(IDC_ADD_AEE, onButtonAddAssociation)
    ON_BN_CLICKED(IDC_EDIT_AEE, onButtonEditAssociation)
    ON_BN_CLICKED(IDC_REMOVE_AEE, onButtonRemoveAssociation)
  	ON_NOTIFY(NM_CLICK, IDC_AEE_LIST, OnClickStationList)

	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_STN_EQUIPMENT_ENTITY_NAME, onKillfocusEditStnEquipmentEntityName)
    ON_EN_KILLFOCUS(IDC_EDIT_OCC_EQUIPMENT_ENTITY_NAME, onKillfocusEditOccEquipmentEntityName)

    ON_EN_KILLFOCUS(IDC_EDIT_ZONE_ID, onKillFocusEditZoneId)
    ON_EN_KILLFOCUS(IDC_EDIT_LABEL, onKillFocusEditLabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void PaZoneDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_idEdt.SetLimitText(ZONE_ID_FIELD_LENGTH);
        m_labelEdt.SetLimitText(LABEL_FIELD_LENGTH);
        m_stnEquipmentEntityNameEdt.SetLimitText(EQUIPMENT_ENTITY_NAME_FIELD_LENGTH);
        m_occEquipmentEntityNameEdt.SetLimitText(EQUIPMENT_ENTITY_NAME_FIELD_LENGTH);
        
  		m_associationListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
        m_associationListCtrl.setHighlightColour(RGB(255, 0, 0));   // To match TA_Base_Bus::TextColourEdit

        // Add in the relevant columns (just the name at present)
        int col = m_associationListCtrl.InsertColumn(0, ASS_ENTITY_LIST_HEADER.c_str(), LVCFMT_LEFT);
        // Expand to take up full width
        m_associationListCtrl.SetColumnWidth(col, LVSCW_AUTOSIZE_USEHEADER);                

        // Hide all the PA Zone details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PaZoneDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaZoneDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaZoneDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaZoneListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaZoneDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaZone*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigPaZone* object
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

            if (TA_Base_Core::DatabaseKey::isInvalidKey(m_configItem->getId()))
            {
                m_idEdt.SetWindowText( "" );
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                buffer.Format("%d", m_configItem->getId());
                m_idEdt.SetWindowText( buffer );
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            m_labelEdt.SetWindowText( m_configItem->getLabel().c_str() );
            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );    
            
            m_stnEquipmentEntityNameEdt.SetWindowText(m_configItem->getStnEntityName().c_str());   
            // Valid with an empty entity name 
            m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            
            m_occEquipmentEntityNameEdt.SetWindowText(m_configItem->getOccEntityName().c_str());   
            // Valid with an empty entity name 
            m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            if( m_configItem->isNew() )
            {
                // This is a new PA Zone or at least a copy of an PA Zone so we indicate this in the type
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
            
            populateAssociatedEntityArea(true);

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA Zone details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA Zone details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA Zone";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }


    void PaZoneDetailView::populateAssociatedEntityArea(bool initialisation)
    {        
        // All are valid until we find an invalid one..
        m_allEntityAssociationsValid = true;
    
        m_editAssociationBtn.EnableWindow(FALSE);
        m_removeAssociationBtn.EnableWindow(FALSE);

      	m_associationListCtrl.DeleteAllItems(); 
        m_associationListCtrl.clearHighlightDataSet();
        
        // Simply display all the associated entity names, as detailed
        // straight from the pa zone interface
        std::list<std::string> entities = m_configItem->getAssociatedEquipmentEntities();
        
        for (std::list<std::string>::iterator itr = entities.begin(); itr != entities.end(); itr ++)
        {
            unsigned long entityKey = m_databaseAccessor->getEntityKey(itr->c_str());
            bool validAssociation = false;

            // For each item in list, confirm it is a valid entity
            if (entityKey >= 0)
            {
                if (m_configItem->getLocationKey() == m_databaseAccessor->getEntityLocation(entityKey))
                {
                    validAssociation = true;
                }
            }

            // If one or more are invalid, must reflect this in internal state
            if (!validAssociation)
            {
                m_allEntityAssociationsValid = false;
            }

            // Insert at rear
            int nItem = m_associationListCtrl.InsertItem(
                                m_associationListCtrl.GetItemCount(), 
                                itr->c_str());
            m_associationListCtrl.SetItemData(nItem, entityKey);
            
            // Highlight all invalid items to the user
            if (!validAssociation)
            {
                m_associationListCtrl.registerHighlightData(entityKey);
            }
        }

    }


    void PaZoneDetailView::onButtonAddAssociation() 
    {   
        updateAssociations(ADD_NEW);
    }


    void PaZoneDetailView::onButtonEditAssociation() 
    {   
        // The edit button is simply an alias for remove selected then add
        updateAssociations(EDIT_EXISTING);
    }


    void PaZoneDetailView::onButtonRemoveAssociation() 
    {   
        updateAssociations(REMOVE_EXISTING);
    }


    void PaZoneDetailView::updateAssociations(EAssocUpdate update)
    {
        FUNCTION_ENTRY("updateAssociations");
                
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
        if (!m_configItem) 
        {           
            return;
        }
        
        // Initially grab copy of current set of entities, for checking if any changes made
        std::list<std::string> entities = m_configItem->getAssociatedEquipmentEntities();

        CString selectedItem;           
        int nCurSel = m_associationListCtrl.GetNextItem(-1, LVNI_SELECTED);        
        if (nCurSel >= 0)
        {
            selectedItem = m_associationListCtrl.GetItemText(nCurSel, 0);
        }
        bool bRemoveSelectedItem = false;

        switch (update)
        {
        case ADD_NEW:   // Fallthrough
        case EDIT_EXISTING: 
            {
                TA_Base_App::ChangeDialog::ItemsForSelection items = 
                    m_databaseAccessor->getAllEntitiesAtLocation(m_configItem->getLocationKey());
                                
                if (ADD_NEW == update)
                {
                    stripExistingAssociationsFromMap(items);
                }
                else if (EDIT_EXISTING == update)
                {
                    // When stripping associations on an update, must force inclusion of 'selectedItem'
                    // NOTE: it is possible when 'editing' an invalid entity, it won't appear
                    // in the list as currently selected (since it's not available, being invalid)
                    stripExistingAssociationsFromMap(items, selectedItem);
                }

                TA_Base_App::ChangeDialog dlg(items, SELECT_ASSOCIATION_ENTITY_NAME, selectedItem, this);
                int response = dlg.DoModal();
                if (IDOK == response)
                {                       
                    // Attempt to add in the entity - will ignore request if selected an item already in list
                    m_configItem->setAssociatedEquipmentEntity(dlg.getSelectedName().GetBuffer(0), true);
                }
            }                    
            break;
        case REMOVE_EXISTING:
            if (selectedItem.GetLength() > 0)
            {
                // Attempt to remove the currently selected item
                m_configItem->setAssociatedEquipmentEntity(selectedItem.GetBuffer(0), false);
            }
            break;
        };

        if (EDIT_EXISTING == update)
        {
            // Edit existing is a special combination of remove existing and add new
            if (entities != m_configItem->getAssociatedEquipmentEntities())
            {
                // If a change had been made, means can we remove the currently selected item
                // (if no change was made, it means operation cancelled, or same item selected -
                // so it shouldn't be removed)
                if (selectedItem.GetLength() > 0)
                {
                    // Attempt to remove the currently selected item
                    m_configItem->setAssociatedEquipmentEntity(selectedItem.GetBuffer(0), false);
                }
            }
        } 

        if (entities != m_configItem->getAssociatedEquipmentEntities())
        {
            // Only refresh this section of the form if a change was made..
            populateAssociatedEntityArea();
    
            m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            refreshButtonAvailability();

        }

        FUNCTION_EXIT;        
    }


	void PaZoneDetailView::OnClickStationList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        BOOL validSel = (m_associationListCtrl.GetSelectedCount() > 0);

        m_editAssociationBtn.EnableWindow(validSel);
        m_removeAssociationBtn.EnableWindow(validSel);

		*pResult = 0;
	}

    
    void PaZoneDetailView::stripExistingAssociationsFromMap(TA_Base_App::ChangeDialog::ItemsForSelection& items, CString forceInclusion)
    {
        if (m_associationListCtrl.GetItemCount() == 0)
        {
            // Nothing to remove
            return;
        }

        std::set<std::string> existingAssociations;

		int i = 0;
        for (; i < m_associationListCtrl.GetItemCount(); i ++)
        {
            CString rString = m_associationListCtrl.GetItemText(i, 0);
            
            if (rString == forceInclusion)
            {
                // We don't want to remove this item
                continue;
            }
            existingAssociations.insert(std::set<std::string>::value_type(rString.GetBuffer(0)) );
        }

        // When adding a new item, we don't need to display the original item
        // in the list (redundant)
        std::vector<unsigned long> keysToErase;

        for (TA_Base_App::ChangeDialog::ItemsForSelection::iterator itr = items.begin(); itr != items.end(); itr ++)
        {
            if (existingAssociations.find(itr->second) != existingAssociations.end())
            {
                // Found the item, no need to display it so erase it
                keysToErase.push_back(itr->first);
            }               
        }

        for (i = 0; i < (int) keysToErase.size(); i ++)
        {
            items.erase(keysToErase[i]);
        }
    }

    void PaZoneDetailView::refreshButtonAvailability()
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


    void PaZoneDetailView::onKillfocusEditStnEquipmentEntityName() 
    {
        FUNCTION_ENTRY("onKillfocusEditStnEquipmentEntityName");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString entityString;
        m_stnEquipmentEntityNameEdt.GetWindowText(entityString);

        if (entityString.GetLength() > 0 && 
            !m_databaseAccessor->isFormattedAsPaZoneEntity(entityString.GetBuffer(0)) )
        {
            // If the name typed in doesn't correspond to a pa zone entity, can't be valid
            // (we filter out non-conforming objects when the user goes through the selection dialog)
            m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        unsigned long entityKey = m_databaseAccessor->getEntityKey((LPCTSTR)entityString);

        // If a valid entity is returned, confirm it is for the correct location
        if (entityKey != -1)
        {
            if (m_configItem->getLocationKey() != m_databaseAccessor->getEntityLocation(entityKey))
            {
                // Entity taken from a different location, this is invalid
                m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                return;
            }
        }        

        if (entityString.GetLength() > 0 && entityKey == -1)
        {
            m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the entity string has not changed then there is no point doing an update
            if ( std::string(entityString.GetBuffer(0)) == m_configItem->getStnEntityName())
            {
                m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setStnEntityName( entityString.GetBuffer(0) );
            m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void PaZoneDetailView::onKillfocusEditOccEquipmentEntityName() 
    {
        FUNCTION_ENTRY("onKillfocusEditOccEquipmentEntityName");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString entityString;
        m_occEquipmentEntityNameEdt.GetWindowText(entityString);

        if (entityString.GetLength() > 0 && 
            !m_databaseAccessor->isFormattedAsPaZoneEntity(entityString.GetBuffer(0)) )
        {
            // If the name typed in doesn't correspond to a pa zone entity, can't be valid
            // (we filter out non-conforming objects when the user goes through the selection dialog)
            m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        unsigned long entityKey = m_databaseAccessor->getEntityKey((LPCTSTR)entityString);

        // If a valid entity is returned, confirm it is for the correct location
        if (entityKey != -1)
        {
            // OCC EEN must be an OCC datapoint
            if (m_databaseAccessor->getOccLocationKey() != m_databaseAccessor->getEntityLocation(entityKey))
            {
                // Entity taken from a different location, this is invalid
                m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                return;
            }
        }        

        if (entityString.GetLength() > 0 && entityKey == -1)
        {
            m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the entity string has not changed then there is no point doing an update
            if ( std::string(entityString.GetBuffer(0)) == m_configItem->getOccEntityName())
            {
                m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setOccEntityName( entityString.GetBuffer(0) );
            m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void PaZoneDetailView::onButtonStnEquipmentEntityName() 
    {   
        FUNCTION_ENTRY("onButtonStnEquipmentEntityName");
	    
        // Retrieve the current location this PA Zone is using
        CString currentEntity("");
        m_stnEquipmentEntityNameEdt.GetWindowText( currentEntity );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        // Only show entities specific for this location when editing station entity name
        TA_Base_App::ChangeDialog::ItemsForSelection items = 
                m_databaseAccessor->getAllEntitiesAtLocation(m_configItem->getLocationKey(), true);

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_ENTITY_NAME, currentEntity, this);
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

            if (currentEntity != dlg.getSelectedName())
            {
                // Now update the entity type the details view is showing
                m_stnEquipmentEntityNameEdt.SetWindowText( dlg.getSelectedName() );

                unsigned long entityKey = m_databaseAccessor->getEntityKey((LPCTSTR)dlg.getSelectedName());
                // If a valid entity is returned, confirm it is for the correct location
                if (entityKey >= 0)
                {
                    if (m_configItem->getLocationKey() != m_databaseAccessor->getEntityLocation(entityKey))
                    {
                        // Entity taken from a different location, this is invalid
                        m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                        return;
                    }
                }

                // If this is a different status datapoint then set it in our database object.
                m_configItem->setStnEntityName( dlg.getSelectedName().GetBuffer(0) );

                m_stnEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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


    void PaZoneDetailView::onButtonOccEquipmentEntityName() 
    {   
        FUNCTION_ENTRY("onButtonOccEquipmentEntityName");
	    
        // Retrieve the current location this PA Zone is using
        CString currentEntity("");
        m_occEquipmentEntityNameEdt.GetWindowText( currentEntity );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        // Show all OCC datapoints
        unsigned long occLocKey = m_databaseAccessor->getOccLocationKey();
        TA_Base_App::ChangeDialog::ItemsForSelection items = 
                m_databaseAccessor->getAllEntitiesAtLocation(occLocKey, true);

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_ENTITY_NAME, currentEntity, this);
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

            if (currentEntity != dlg.getSelectedName())
            {
                // Now update the entity type the details view is showing
                m_occEquipmentEntityNameEdt.SetWindowText( dlg.getSelectedName() );

                unsigned long entityKey = m_databaseAccessor->getEntityKey((LPCTSTR)dlg.getSelectedName());
                // If a valid entity is returned, confirm it is for the correct location
                if (entityKey >= 0)
                {
                    if (occLocKey != m_databaseAccessor->getEntityLocation(entityKey))
                    {
                        // Entity taken from a different location, this is invalid
                        m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                        return;
                    }
                }

                // If this is a different status datapoint then set it in our database object.
                m_configItem->setOccEntityName( dlg.getSelectedName().GetBuffer(0) );

                m_occEquipmentEntityNameEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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


    void PaZoneDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
	    
        // Retrieve the current location this PA Zone is using
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


    void PaZoneDetailView::onKillfocusEditLocation() 
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void PaZoneDetailView::onKillFocusEditZoneId() 
    {
        FUNCTION_ENTRY("onKillFocusEditZoneId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString zoneIdString;
        m_idEdt.GetWindowText(zoneIdString);

        TA_Base_App::FieldValidityChecker fvc;

        // Only allow zone ID >= 0
        long zoneId = (long)atol((LPCTSTR)zoneIdString);

        if (!fvc.isValidInteger(zoneIdString.GetBuffer(0)) ||
            zoneId < 0)
        {
            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        
        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (zoneId == (long) m_configItem->getId())
            {
                
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setId( zoneId );
            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void PaZoneDetailView::onKillFocusEditLabel() 
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }  


    void PaZoneDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void PaZoneDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_locationEdt.isTextValid()    || 
             !m_idEdt.isTextValid()   ||
             !m_labelEdt.isTextValid()       ||  
             !m_stnEquipmentEntityNameEdt.isTextValid() ||
             !m_occEquipmentEntityNameEdt.isTextValid() ||
             !m_allEntityAssociationsValid )
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
        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    void PaZoneDetailView::onLocationChanged()
    {   
        // Whenever the operator changes the location, it may affect
        // validity of the equipment entity associations - so refresh their status

        // Refresh our entity name
        onKillfocusEditStnEquipmentEntityName();
        onKillfocusEditOccEquipmentEntityName();

        // Refresh associations list
        populateAssociatedEntityArea();
    }


    /////////////////////////////////////////////////////////////////////////////
    // PaZoneDetailView diagnostics

    #ifdef _DEBUG
    void PaZoneDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaZoneDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

