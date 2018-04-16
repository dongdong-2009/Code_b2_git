/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ECS Station Mode's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_mode/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDetailView.h"
#include "app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDatabaseAccessor.h"
#include "app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeListView.h"
#include "app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ecs/src/IConfigECSStationMode.h"

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
    const CString ECSStationModeDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";
    
    const int ECSStationModeDetailView::STATION_MODE_FIELD_LENGTH   = 4;
    const int ECSStationModeDetailView::LOCATION_FIELD_LENGTH       = 40;
    const int ECSStationModeDetailView::OPERATION_FIELD_LENGTH      = 40;
    const int ECSStationModeDetailView::TRACK_FIELD_LENGTH          = 40;
    const int ECSStationModeDetailView::CONDITION_FIELD_LENGTH      = 20;
    const int ECSStationModeDetailView::LOGIC_FIELD_LENGTH          = 1000;

        
    IMPLEMENT_DYNCREATE(ECSStationModeDetailView, CFormView)


    ECSStationModeDetailView::ECSStationModeDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ECSStationModeDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ECSStationModeDetailView)
	//}}AFX_DATA_INIT
    }


    ECSStationModeDetailView::~ECSStationModeDetailView()
    {
    }


    void ECSStationModeDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ECSStationModeDetailView)
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_EDIT_STATION_MODE, m_stationModeEdt);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);
	DDX_Control(pDX, IDC_EDIT_OPERATION, m_operationEdt);
	DDX_Control(pDX, IDC_EDIT_TRACK, m_trackEdt);
	DDX_Control(pDX, IDC_EDIT_CONDITION, m_conditionEdt);
    DDX_Control(pDX, IDC_EDIT_CONGESTION, m_logicEdt);
	DDX_Control(pDX, IDC_BUTTON_LOCATION, m_locationBtn);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    DDX_Control(pDX, IDC_BITMAP2, m_bitmap2);
    DDX_Control(pDX, IDC_BITMAP3, m_bitmap3);
    DDX_Control(pDX, IDC_BITMAP4, m_bitmap4);
    DDX_Control(pDX, IDC_BITMAP5, m_bitmap5);
    DDX_Control(pDX, IDC_BITMAP6, m_bitmap6);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ECSStationModeDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ECSStationModeDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
	ON_EN_KILLFOCUS(IDC_EDIT_STATION_MODE, onKillfocusEditStationMode)
    ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_OPERATION, onKillfocusEditOperation)
    ON_EN_KILLFOCUS(IDC_EDIT_TRACK, onKillfocusEditTrack)
    ON_EN_KILLFOCUS(IDC_EDIT_CONDITION, onKillfocusEditCondition)
    ON_EN_KILLFOCUS(IDC_EDIT_CONGESTION, onKillfocusEditLogic)
    
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ECSStationModeDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_stationModeEdt.SetLimitText(STATION_MODE_FIELD_LENGTH);
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_operationEdt.SetLimitText(OPERATION_FIELD_LENGTH);
        m_trackEdt.SetLimitText(TRACK_FIELD_LENGTH);
        m_conditionEdt.SetLimitText(CONDITION_FIELD_LENGTH);
        m_logicEdt.SetLimitText(LOGIC_FIELD_LENGTH);

        // Hide all the ECS Station Mode details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ECSStationModeDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

        m_stationModeEdt.ShowWindow(isShown);
        m_locationEdt.ShowWindow(isShown);
        m_locationBtn.ShowWindow(isShown);
        m_operationEdt.ShowWindow(isShown);
        m_trackEdt.ShowWindow(isShown);
        m_conditionEdt.ShowWindow(isShown);
        m_logicEdt.ShowWindow(isShown);
        m_modifyDate.ShowWindow(isShown);
        m_createDate.ShowWindow(isShown);
        m_type.ShowWindow(isShown);

		 GetDlgItem(IDC_STATION_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_LOCATION_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_OPERATION_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_TRACK_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_CONDITION_STATIC)->ShowWindow(isShown);
         GetDlgItem(IDC_CONGESTION_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_CREATE_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_MOD_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		 GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		 GetDlgItem(IDC_CONDITION_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_TRACK_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_OPERATION_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_TRACK_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATION_BITMAP)->ShowWindow(isShown);


    }


    void ECSStationModeDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ECSStationModeDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ECSStationModeListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ECSStationModeDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigECSStationMode*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigECSStationMode* object
            CString buffer;

            if (m_configItem->getStationMode() == 0)
            {
                m_stationModeEdt.SetWindowText( "" );
                m_stationModeEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );                
            }
            else
            {          
                buffer.Format("%d", m_configItem->getStationMode());
                m_stationModeEdt.SetWindowText( buffer );
                m_stationModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            if (m_databaseAccessor->getLocationName(m_configItem->getLocationKey()) == "")
            {
                m_locationEdt.SetWindowText( "" );
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_locationEdt.SetWindowText( m_databaseAccessor->getLocationName(m_configItem->getLocationKey()).c_str());
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            if (m_configItem->getOperation() == "")
            {
                m_operationEdt.SetWindowText( "" );
                m_operationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_operationEdt.SetWindowText( m_configItem->getOperation().c_str() );
                m_operationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            m_logicEdt.SetWindowText( m_configItem->getCongestionLogic().c_str() );
            
            if (m_configItem->getCondition() == "")
            {
                m_conditionEdt.SetWindowText( "" );
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_conditionEdt.SetWindowText( m_configItem->getCondition().c_str() );
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            
            }

            m_trackEdt.SetWindowText( m_configItem->getTrack().c_str() );
            m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );            

            if( m_configItem->isNew() )
            {

                // This is a new ECS Station Mode or at least a copy of an ECS Station Mode so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ECS Station Mode details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ECS Station Mode details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ECS Station Mode";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void ECSStationModeDetailView::refreshButtonAvailability()
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

    void ECSStationModeDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
	    
        // Retrieve the current location this ECS Station Mode is using
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

    void ECSStationModeDetailView::onKillfocusEditStationMode() 
    {
        FUNCTION_ENTRY("onKillfocusEditStationMode");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString stationModeString;
        m_stationModeEdt.GetWindowText(stationModeString);
        unsigned int stationMode = atoi((LPCTSTR)stationModeString);

        try
        {
            // If the stationMode has not changed then there is no point doing an update
            if (stationMode == m_configItem->getStationMode())
            {
                m_stationModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setStationMode( stationMode );
            m_stationModeEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            m_stationModeEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSStationModeDetailView::onKillfocusEditLocation() 
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
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSStationModeDetailView::onKillfocusEditOperation() 
    {
        FUNCTION_ENTRY("onKillfocusEditOperation");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString operationString;
        m_operationEdt.GetWindowText(operationString);

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (operationString == m_configItem->getOperation().c_str())
            {
                
                m_operationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setOperation( (LPCTSTR) operationString );
            m_operationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            m_operationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSStationModeDetailView::onKillfocusEditTrack() 
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            m_trackEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void ECSStationModeDetailView::onKillfocusEditCondition() 
    {
        FUNCTION_ENTRY("onKillfocusEditCondition");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString conditionString;
        m_conditionEdt.GetWindowText(conditionString);

        try
        {
            // If the condition string has not changed then there is no point doing an update
            if (conditionString == m_configItem->getCondition().c_str())
            {
                
                m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setCondition( (LPCTSTR) conditionString );
            m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            m_conditionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ECSStationModeDetailView::onKillfocusEditLogic() 
    {
        FUNCTION_ENTRY("onKillfocusEditLogic");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString logicString;
        m_logicEdt.GetWindowText(logicString);

        try
        {
            // If the logic string has not changed then there is no point doing an update
            if (logicString == m_configItem->getCongestionLogic().c_str())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setCongestionLogic( (LPCTSTR) logicString );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ECS Station Mode could not be found in the database");

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }



    void ECSStationModeDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void ECSStationModeDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_stationModeEdt.isTextValid() || 
             !m_locationEdt.isTextValid()    || 
             !m_operationEdt.isTextValid()   ||
             !m_trackEdt.isTextValid()       ||  
             !m_conditionEdt.isTextValid() )
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
    // ECSStationModeDetailView diagnostics

    #ifdef _DEBUG
    void ECSStationModeDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ECSStationModeDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

