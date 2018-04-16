/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ECS Zone's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_zone/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDetailView.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDatabaseAccessor.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneListView.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ecs/src/IConfigECSZone.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

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
    const CString ECSZoneDetailView::CHANGE_OCC_EQUIPMENT_DIALOG_TITLE  = "Change OCC Equipment";
    const CString ECSZoneDetailView::CHANGE_STN_EQUIPMENT_DIALOG_TITLE  = "Change Station Equipment";

    const int ECSZoneDetailView::ECS_ZONE_ID_FIELD_LENGTH = 10;
    const int ECSZoneDetailView::ECS_LOCATION_FIELD_LENGTH = 20;
    const int ECSZoneDetailView::TRACK_FIELD_LENGTH = 40;
    const int ECSZoneDetailView::OCC_EQUIPMENT_FIELD_LENGTH = 256;
    const int ECSZoneDetailView::STN_EQUIPMENT_FIELD_LENGTH = 256;

    
    IMPLEMENT_DYNCREATE(ECSZoneDetailView, CFormView)


    ECSZoneDetailView::ECSZoneDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ECSZoneDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ECSZoneDetailView)
	//}}AFX_DATA_INIT
    }


    ECSZoneDetailView::~ECSZoneDetailView()
    {
    }


    void ECSZoneDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ECSZoneDetailView)
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_EDIT_ECS_ZONE_ID, m_zoneIdEdt);
	DDX_Control(pDX, IDC_EDIT_STN_EQUIPMENT, m_stnEquipmentEdt);
	DDX_Control(pDX, IDC_EDIT_ECS_LOCATION, m_locationEdt);
	DDX_Control(pDX, IDC_EDIT_OCC_EQUIPMENT, m_occEquipmentEdt);
	DDX_Control(pDX, IDC_EDIT_TRACK, m_trackEdt);
	DDX_Control(pDX, IDC_BUTTON_OCC_EQUIPMENT, m_occEquipmentBtn);
	DDX_Control(pDX, IDC_BUTTON_STN_EQUIPMENT, m_stnEquipmentBtn);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    DDX_Control(pDX, IDC_BITMAP2, m_bitmap2);
    DDX_Control(pDX, IDC_BITMAP3, m_bitmap3);
    DDX_Control(pDX, IDC_BITMAP4, m_bitmap4);
    DDX_Control(pDX, IDC_BITMAP5, m_bitmap5);
    DDX_Control(pDX, IDC_BITMAP6, m_bitmap6);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//TD17826 - marc_bugfix_start
	//Support IS_IGNORING_CONGESTION field
	DDX_Control(pDX, IDC_CHECK_IGNORECONGESTION, m_IgnoringCongestion);
	//TD17826 - marc_bugfix_end

	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ECSZoneDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ECSZoneDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_OCC_EQUIPMENT, onButtonOccEquipment)
	ON_BN_CLICKED(IDC_BUTTON_STN_EQUIPMENT, onButtonStnEquipment)
	//TD17826 - marc_bugfix_start
	ON_BN_CLICKED(IDC_CHECK_IGNORECONGESTION, onButtonIgnoreCongestion)
	//TD17826 - marc_bugfix_end
	ON_EN_KILLFOCUS(IDC_EDIT_ECS_ZONE_ID, onKillfocusEditZoneId)
    ON_EN_KILLFOCUS(IDC_EDIT_ECS_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_TRACK, onKillfocusEditTrack)
    ON_EN_KILLFOCUS(IDC_EDIT_OCC_EQUIPMENT, onKillfocusEditOccEquipment)
    ON_EN_KILLFOCUS(IDC_EDIT_STN_EQUIPMENT, onKillfocusEditStnEquipment)
    
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ECSZoneDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_zoneIdEdt.SetLimitText(ECS_ZONE_ID_FIELD_LENGTH);
        m_locationEdt.SetLimitText(ECS_LOCATION_FIELD_LENGTH);
        m_trackEdt.SetLimitText(TRACK_FIELD_LENGTH);
        m_occEquipmentEdt.SetLimitText(OCC_EQUIPMENT_FIELD_LENGTH);
        m_stnEquipmentEdt.SetLimitText(STN_EQUIPMENT_FIELD_LENGTH);

        // Hide all the ECS Zone details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ECSZoneDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

        m_zoneIdEdt.ShowWindow(isShown);
        m_locationEdt.ShowWindow(isShown);
        m_trackEdt.ShowWindow(isShown);
        m_occEquipmentEdt.ShowWindow(isShown);
        m_stnEquipmentEdt.ShowWindow(isShown);
        m_occEquipmentBtn.ShowWindow(isShown);
        m_stnEquipmentBtn.ShowWindow(isShown);
        m_modifyDate.ShowWindow(isShown);
        m_createDate.ShowWindow(isShown);
        m_type.ShowWindow(isShown);
		//TD17826 - marc_bugfix_start
		m_IgnoringCongestion.ShowWindow(isShown);
		//TD17826 - marc_bugfix_end

		 GetDlgItem(IDC_ZONE_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_LOCATION_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATUS_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_TRACK_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_DIR_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_CREATE_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_MOD_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		 GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		 GetDlgItem(IDC_ZONE_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_TRACK_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATUS_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_DIR_BITMAP)->ShowWindow(isShown);
         //TD17826 - marc_bugfix_start
		 GetDlgItem(IDC_IGNORECONGESTION_STATIC)->ShowWindow(isShown);
    	 //TD17826 - marc_bugfix_end
	
    }


    void ECSZoneDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ECSZoneDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ECSZoneListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ECSZoneDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigECSZone*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigECSZone* object

            if (m_configItem->getECSZoneId() == -1)
            {
                m_zoneIdEdt.SetWindowText( "" );
                m_zoneIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                m_zoneIdEdt.EnableWindow(true);
            }
            else
            {
                CString buffer;
                buffer.Format("%d", m_configItem->getECSZoneId());
                m_zoneIdEdt.SetWindowText( buffer );
                m_zoneIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                m_zoneIdEdt.EnableWindow(false);
            }

            m_locationEdt.SetWindowText( m_configItem->getECSLocation().c_str() );
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_trackEdt.SetWindowText( m_configItem->getTrack().c_str() );
            m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );


            if (m_databaseAccessor->getDataNodeName(m_configItem->getOccEquipmentEntityKey()) == "")
            {
                m_occEquipmentEdt.SetWindowText( "" );
                m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_occEquipmentEdt.SetWindowText( m_databaseAccessor->getDataNodeName(m_configItem->getOccEquipmentEntityKey()).c_str());
                m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            if (m_databaseAccessor->getDataNodeName(m_configItem->getStnEquipmentEntityKey()) == "")
            {
                m_stnEquipmentEdt.SetWindowText( "" );
                m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_stnEquipmentEdt.SetWindowText( m_databaseAccessor->getDataNodeName(m_configItem->getStnEquipmentEntityKey()).c_str());
                m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            //TD17826 - marc_bugfix_start
			if (m_configItem->getIsIgnoringCongestion() == true)
			{
				m_IgnoringCongestion.SetCheck(1);
			}
			else
			{
				m_IgnoringCongestion.SetCheck(0);
			}

			//TD17826 - marc_bugfix_end

            if( m_configItem->isNew() )
            {

                // This is a new ECS Zone or at least a copy of an ECS Zone so we indicate this in the type
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
            
            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ECS Zone details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Zone details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ECS Zone";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }

    void ECSZoneDetailView::refreshButtonAvailability()
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

    void ECSZoneDetailView::onButtonOccEquipment() 
    {
        FUNCTION_ENTRY("onButtonOccEquipment");
	    
        // Retrieve the current occEquipment this ECS Zone is using
        CString currentOccEquipment("");
        m_occEquipmentEdt.GetWindowText( currentOccEquipment );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllECSDataNodes();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_OCC_EQUIPMENT_DIALOG_TITLE, currentOccEquipment, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of an OCC Equipment ");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new OCC Equipment which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentOccEquipment != dlg.getSelectedName())
            {
                // If this is a different occEquipmentthen set it in our database object.
                m_configItem->setOccEquipmentEntityKey( dlg.getSelectedKey() );

                // Now update the entity type the details view is showing
                m_occEquipmentEdt.SetWindowText( dlg.getSelectedName() );
                m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve OCC Equipment key so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve OCC Equipment key so cannot update existing");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }


    void ECSZoneDetailView::onButtonStnEquipment() 
    {
        FUNCTION_ENTRY("onButtonStnEquipment");
	    
        // Retrieve the current stnEquipment dataNode this ECS Zone is using
        CString currentStnEquipment("");
        m_stnEquipmentEdt.GetWindowText( currentStnEquipment );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllECSDataNodes();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_STN_EQUIPMENT_DIALOG_TITLE, currentStnEquipment, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a station datanode");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new station datanode which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentStnEquipment!= dlg.getSelectedName())
            {
                // If this is a different stnEquipment then set it in our database object.
                m_configItem->setStnEquipmentEntityKey( dlg.getSelectedKey() );

                // Now update the entity type the details view is showing
                m_stnEquipmentEdt.SetWindowText( dlg.getSelectedName() );
                m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve station datanode key so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve station datanode key so cannot update existing");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }


    void ECSZoneDetailView::onKillfocusEditLocation() 
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

        try
        {
            // If the location string has not changed then there is no point doing an update
            if (locationString == m_configItem->getECSLocation().c_str())
            {
                
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setECSLocation( (LPCTSTR) locationString );
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSZoneDetailView::onKillfocusEditZoneId() 
    {
        FUNCTION_ENTRY("onKillfocusEditZoneId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString zoneIdString;
        m_zoneIdEdt.GetWindowText(zoneIdString);
        int zoneId = atoi((LPCTSTR)zoneIdString);

        try
        {
            // If the zoneId has not changed then there is no point doing an update
            if (zoneId == m_configItem->getECSZoneId())
            {
                m_zoneIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setECSZoneId( zoneId );
            m_zoneIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone could not be found in the database");

            m_zoneIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSZoneDetailView::onKillfocusEditTrack() 
    {
        FUNCTION_ENTRY("onKillfocusEditTrack");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString trackString;
        m_trackEdt.GetWindowText(trackString);

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (trackString == m_configItem->getTrack().c_str())
            {
                
                m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTrack( (LPCTSTR) trackString );
            m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone could not be found in the database");

            m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSZoneDetailView::onKillfocusEditOccEquipment() 
    {
        FUNCTION_ENTRY("onKillfocusEditOccEquipment");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString occEquipmentString;
        m_occEquipmentEdt.GetWindowText(occEquipmentString);
        unsigned long occEquipmentKey = m_databaseAccessor->getDataNodeKey((LPCTSTR)occEquipmentString);

        if (occEquipmentKey == -1)
        {
            m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the occEquipment string has not changed then there is no point doing an update
            if ( occEquipmentKey == m_configItem->getOccEquipmentEntityKey())
            {
                m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setOccEquipmentEntityKey( occEquipmentKey );
            m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone could not be found in the database");

            m_occEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ECSZoneDetailView::onKillfocusEditStnEquipment() 
    {
        FUNCTION_ENTRY("onKillfocusEditStnEquipment");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString stnEquipmentString;
        m_stnEquipmentEdt.GetWindowText(stnEquipmentString);
        unsigned long stnEquipmentKey = m_databaseAccessor->getDataNodeKey((LPCTSTR)stnEquipmentString);

        if (stnEquipmentKey == -1)
        {
            m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the stnEquipment string has not changed then there is no point doing an update
            if ( stnEquipmentKey == m_configItem->getStnEquipmentEntityKey())
            {
                m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setStnEquipmentEntityKey( stnEquipmentKey );
            m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Zone could not be found in the database");

            m_stnEquipmentEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSZoneDetailView::onButtonIgnoreCongestion() 
    {
        FUNCTION_ENTRY("onButtonIgnoreCongestion");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }
		if (m_IgnoringCongestion.GetCheck() == 1)
		{
			m_configItem->setIsIgnoringCongestion(true);
		}
		else
		{
			m_configItem->setIsIgnoringCongestion(false);
		}	

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();


    }


    void ECSZoneDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void ECSZoneDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_zoneIdEdt.isTextValid() || !m_locationEdt.isTextValid() || 
             !m_trackEdt.isTextValid()  || !m_occEquipmentEdt.isTextValid()   || !m_stnEquipmentEdt.isTextValid())
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


    /////////////////////////////////////////////////////////////////////////////
    // ECSZoneDetailView diagnostics

    #ifdef _DEBUG
    void ECSZoneDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ECSZoneDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

