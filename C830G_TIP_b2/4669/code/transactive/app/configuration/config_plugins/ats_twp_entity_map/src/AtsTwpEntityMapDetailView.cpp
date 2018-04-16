/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_twp_entity_map/src/AtsTwpEntityMapDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ATS PSD Entity Map's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_twp_entity_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_twp_entity_map/src/AtsTwpEntityMapDetailView.h"
#include "app/configuration/config_plugins/ats_twp_entity_map/src/AtsTwpEntityMapDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_twp_entity_map/src/AtsTwpEntityMapListView.h"
#include "app/configuration/config_plugins/ats_twp_entity_map/src/AtsTwpEntityMapComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsTWPData.h"
#include "core/data_access_interface/ats/src/AtsTWPData.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    // The titles for the change dialog box
    const CString AtsTwpEntityMapDetailView::CHANGE_STATUS_DATAPOINT_DIALOG_TITLE    = "Change Status DataPoint";
    const CString AtsTwpEntityMapDetailView::CHANGE_DIRECTIONS_DATAPOINT_DIALOG_TITLE  = "Change Directions DataPoint";

    const int AtsTwpEntityMapDetailView::DATAPOINT_FIELD_LENGTH = 256;
    
    IMPLEMENT_DYNCREATE(AtsTwpEntityMapDetailView, CFormView)


    AtsTwpEntityMapDetailView::AtsTwpEntityMapDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsTwpEntityMapDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsTwpEntityMapDetailView)
	//}}AFX_DATA_INIT
    }


    AtsTwpEntityMapDetailView::~AtsTwpEntityMapDetailView()
    {
    }


    void AtsTwpEntityMapDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsTwpEntityMapDetailView)
	DDX_Control(pDX, IDC_BUTTON_RESET, m_resetBtn);
	DDX_Control(pDX, IDC_EDIT_DATAPOINT, m_datapointEdit);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_commandCombo);
	DDX_Control(pDX, IDC_BUTTON_DATAPOINT, m_datapointButton);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsTwpEntityMapDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsTwpEntityMapDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_CBN_KILLFOCUS(IDC_COMBO_COMMAND, onKillfocusComboCommand)
	ON_BN_CLICKED(IDC_BUTTON_DATAPOINT, onButtonDatapoint)
	ON_EN_KILLFOCUS(IDC_EDIT_DATAPOINT, onKillfocusEditDatapoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsTwpEntityMapDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

		m_datapointEdit.SetLimitText(DATAPOINT_FIELD_LENGTH);

		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::NO_WASH.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DAILY_RINSE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DAILY_DETERGENT.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::WEEKLY_WASH.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::CAB_END_WASH.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::STOP_WP.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::RESTART_WP.c_str());
	
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::WP_STATUS.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::NO_WASH_SELECTED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DAILY_RINSE_SELECTED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DAILY_DETERGENT_SELECTED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::WEEKLY_WASH_SELECTED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::CAB_END_WASH_COMPLETED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::TRAIN_WASH_CYCLE_COMPLETED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::TRAIN_SPEED_OUT_OF_RANGE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::TRAIN_STALLED_IN_TWP.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::TRAIN_INCORRECTLY_POSITIONED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::WASH_CYCLE_START_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::PH_CHECK.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::MAINS_WATER_LOW.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::SECOND_BRUSH_RINSE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_HOR.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_VER.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::CAB_END_WASH_BRUSH_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::EMERGENCY_STOP_ACTIVATED.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::PNEUMATIC_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::BRUSH_ACTUATION_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::SIDE_ROOF_BRUSHING_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::END_BRUSHING_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::AIR_BLOW_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::PRE_WET_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::RECYCLED_RINSE_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::WATER_RECYCLING_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DETERGENT_FAILURE.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::DETERGENT_LOW_ALARMS.c_str());	
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::AIR_PRESSURE_LOW.c_str());
		m_commandCombo.AddString(TA_IRS_Core::AtsTWPData::NON_CRITICAL_SUMMARY_ALARM.c_str());

        // Hide all the ATS PSD Entity Map details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsTwpEntityMapDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

		m_datapointEdit.ShowWindow(isShown);
		m_commandCombo.ShowWindow(isShown);
		m_datapointButton.ShowWindow(isShown);
		m_applyBtn.ShowWindow(isShown);
		m_resetBtn.ShowWindow(isShown);
		m_type.ShowWindow(isShown);

		GetDlgItem(IDC_COMMAND_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATAPOINT_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATAPOINT_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_COMMAND_BITMAP)->ShowWindow(isShown);
    }


    void AtsTwpEntityMapDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsTwpEntityMapDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsTwpEntityMapListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsTwpEntityMapDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsTWPData*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsTWPData* object
			AtsTwpEntityMapDatabaseAccessor::AtsTwpEntityMapRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());            
			
			// Command
			if (details.command == "")
            {
                m_commandCombo.SetCurSel(-1);
            }
            else
            {
				m_commandCombo.SetCurSel(m_commandCombo.FindStringExact(-1, details.command));
            }

			// Datapoint
			if (details.dpName == "")
            {
                m_datapointEdit.SetWindowText( "" );
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_datapointEdit.SetWindowText( details.dpName );
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            if( m_configItem->isNew() )
            {

                // This is a new ATS PSD Entity Map or at least a copy of an ATS PSD Entity Map so we indicate this in the type
                // field and make sure the modify and create dates are blank.

                m_type.SetWindowText(NEW_LABEL);
               
                // Disable the reset button as this is a new item so reset has no functionality
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }
            
            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ATS PSD Entity Map details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ATS PSD Entity Map details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ATS PSD Entity Map";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsTwpEntityMapDetailView::refreshButtonAvailability()
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

    void AtsTwpEntityMapDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsTwpEntityMapDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_datapointEdit.isTextValid() || m_commandCombo.GetCurSel() == -1)
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

	void AtsTwpEntityMapDetailView::onKillfocusComboCommand() 
	{
        FUNCTION_ENTRY("onKillfocusComboCommand");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString commandCString;
		
		if (m_commandCombo.GetCurSel() >= 0)
		{
			m_commandCombo.GetLBText(m_commandCombo.GetCurSel(), commandCString);
		}
		else
		{
			commandCString = "";
		}

		std::string commandString = commandCString;

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (commandString == m_configItem->getTWPCommand())
            {
                FUNCTION_EXIT;
                return;
            }

			m_configItem->setTWPCommand(commandString);

			if (commandString == TA_IRS_Core::AtsTWPData::NO_WASH ||
				commandString == TA_IRS_Core::AtsTWPData::DAILY_RINSE ||
				commandString == TA_IRS_Core::AtsTWPData::DAILY_DETERGENT ||
				commandString == TA_IRS_Core::AtsTWPData::WEEKLY_WASH ||
				commandString == TA_IRS_Core::AtsTWPData::CAB_END_WASH ||
				commandString == TA_IRS_Core::AtsTWPData::STOP_WP ||
				commandString == TA_IRS_Core::AtsTWPData::RESTART_WP)
			{
				m_configItem->setTWPDataPointType(TA_IRS_Core::AtsTWPData::OUTPUT_TYPE);
			}
			else
			{
				m_configItem->setTWPDataPointType(TA_IRS_Core::AtsTWPData::INPUT_TYPE);
			}
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats TWP Entity Map could not be found in the database");

			m_commandCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

	void AtsTwpEntityMapDetailView::onButtonDatapoint() 
	{
        FUNCTION_ENTRY("onButtonStatusDataPoint");
	    
        // Retrieve the current status datapoint this ATS PSD Entity Map is using
        CString currentDataPoint("");
        m_datapointEdit.GetWindowText( currentDataPoint );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllPsdDataPoints();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_STATUS_DATAPOINT_DIALOG_TITLE, currentDataPoint, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a status datapoint");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new status datapoint which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentDataPoint != dlg.getSelectedName())
            {
                // If this is a different status datapoint then set it in our database object.
                m_configItem->setTWPDataPointEntityKey( dlg.getSelectedKey() );

                // Now update the entity type the details view is showing
                m_datapointEdit.SetWindowText( dlg.getSelectedName() );
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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

	void AtsTwpEntityMapDetailView::onKillfocusEditDatapoint() 
	{
        FUNCTION_ENTRY("onKillfocusEditDatapoint");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString dataPointString;
        m_datapointEdit.GetWindowText(dataPointString);
        unsigned long entityKey = m_databaseAccessor->getDataPointKey((LPCTSTR)dataPointString);

        if (entityKey == -1)
        {
            m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the status string has not changed then there is no point doing an update
            if ( entityKey == m_configItem->getTWPDataPointEntityKey())
            {
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setTWPDataPointEntityKey( entityKey );
            m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ATS PSD Entity Map could not be found in the database");

            m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsTwpEntityMapDetailView diagnostics

    #ifdef _DEBUG
    void AtsTwpEntityMapDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsTwpEntityMapDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
