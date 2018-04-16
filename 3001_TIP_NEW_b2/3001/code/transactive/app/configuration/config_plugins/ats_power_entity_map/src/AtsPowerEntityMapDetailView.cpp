/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an ATS Power Entity Map's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_power_entity_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapDetailView.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapListView.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsPowerData.h"
#include "core/data_access_interface/ats/src/AtsPowerData.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
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
    const CString AtsPowerEntityMapDetailView::CHANGE_STATUS_DATAPOINT_DIALOG_TITLE    = "Change Status DataPoint";
    const CString AtsPowerEntityMapDetailView::CHANGE_DIRECTIONS_DATAPOINT_DIALOG_TITLE  = "Change Directions DataPoint";

    const int AtsPowerEntityMapDetailView::PSD_NUMBER_FIELD_LENGTH = 2;
    const int AtsPowerEntityMapDetailView::DATAPOINT_FIELD_LENGTH = 256;
    
    IMPLEMENT_DYNCREATE(AtsPowerEntityMapDetailView, CFormView)


    AtsPowerEntityMapDetailView::AtsPowerEntityMapDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsPowerEntityMapDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsPowerEntityMapDetailView)
	//}}AFX_DATA_INIT
    }


    AtsPowerEntityMapDetailView::~AtsPowerEntityMapDetailView()
    {
    }


    void AtsPowerEntityMapDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsPowerEntityMapDetailView)
	DDX_Control(pDX, IDC_BUTTON_RESET, m_resetBtn);
	DDX_Control(pDX, IDC_EDIT_SUBSECTIONNUMBER, m_subsectionEdit);
	DDX_Control(pDX, IDC_COMBO_STATION, m_stationCombo);
	DDX_Control(pDX, IDC_EDIT_DATAPOINT, m_datapointEdit);
	DDX_Control(pDX, IDC_BUTTON_DATAPOINT, m_datapointButton);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsPowerEntityMapDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsPowerEntityMapDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DATAPOINT, onButtonDatapoint)
	ON_EN_KILLFOCUS(IDC_EDIT_DATAPOINT, onKillfocusEditDatapoint)
	ON_CBN_KILLFOCUS(IDC_COMBO_STATION, onKillfocusComboStation)
	ON_EN_KILLFOCUS(IDC_EDIT_SUBSECTIONNUMBER, onKillfocusEditSubsection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsPowerEntityMapDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

		m_subsectionEdit.SetLimitText(PSD_NUMBER_FIELD_LENGTH);
		m_datapointEdit.SetLimitText(DATAPOINT_FIELD_LENGTH);

		try
		{
			std::vector<TA_Base_Core::ILocation*> locations 
				= TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

			for (std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin();
				 it != locations.end();
				 it++)
			{
				CString location = (*it)->getName().c_str();
				m_stationCombo.AddString(location);
				delete (*it);
				(*it) = NULL;
			}
		}
		catch (...)
		{
		}

        // Hide all the ATS Power Entity Map details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsPowerEntityMapDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

		m_subsectionEdit.ShowWindow(isShown);
        m_stationCombo.ShowWindow(isShown);
		m_datapointEdit.ShowWindow(isShown);
		m_datapointButton.ShowWindow(isShown);
		m_applyBtn.ShowWindow(isShown);
		m_resetBtn.ShowWindow(isShown);
		m_type.ShowWindow(isShown);

		GetDlgItem(IDC_DATAPOINT_STATIC)->ShowWindow(isShown);		
		GetDlgItem(IDC_STATION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_SUBSECTIONNUMBER_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATAPOINT_BITMAP)->ShowWindow(isShown);		
		GetDlgItem(IDC_STATION_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_SUBSECTIONNUMBER_BITMAP)->ShowWindow(isShown);
    }


    void AtsPowerEntityMapDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsPowerEntityMapDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsPowerEntityMapListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsPowerEntityMapDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsPowerData*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsPowerData* object
			AtsPowerEntityMapDatabaseAccessor::AtsPowerEntityMapRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());

			// Electrical Subsection
			if (details.subsection == "")
            {
                m_subsectionEdit.SetWindowText( "" );
                m_subsectionEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_subsectionEdit.SetWindowText( details.subsection );
                m_subsectionEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
			
			// Station
			m_stationCombo.SetCurSel(m_stationCombo.FindStringExact(-1, details.location));

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

                // This is a new ATS Power Entity Map or at least a copy of an ATS Power Entity Map so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ATS Power Entity Map details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ATS Power Entity Map details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
	        /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "ATS Power Entity Map";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsPowerEntityMapDetailView::refreshButtonAvailability()
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

    void AtsPowerEntityMapDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsPowerEntityMapDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_subsectionEdit.isTextValid() || m_stationCombo.GetCurSel() == -1 ||
			 !m_datapointEdit.isTextValid() )
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

	void AtsPowerEntityMapDetailView::onButtonDatapoint() 
	{
        FUNCTION_ENTRY("onButtonStatusDataPoint");
	    
        // Retrieve the current status datapoint this ATS Power Entity Map is using
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
                m_configItem->setTractionPowerDataPointEntityKey( dlg.getSelectedKey() );

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

	void AtsPowerEntityMapDetailView::onKillfocusEditDatapoint() 
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
            if ( entityKey == m_configItem->getTractionPowerDataPointEntityKey())
            {
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setTractionPowerDataPointEntityKey( entityKey );
            m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ATS Power Entity Map could not be found in the database");

            m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

	void AtsPowerEntityMapDetailView::onKillfocusComboStation() 
    {
        FUNCTION_ENTRY("onKillfocusComboStation");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        int stationId;

		if (m_stationCombo.GetCurSel() >= 0)
		{
			CString stationCString;
			m_stationCombo.GetLBText(m_stationCombo.GetCurSel(), stationCString);
	
			try
			{
				std::string stationString = stationCString;
				TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(stationString);

				stationId = location->getKey();

				delete location;
			}
			catch (...)
			{
				stationId = -1;
			}
		}
		else
		{
			stationId = -1;
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (stationId == static_cast<int>(m_configItem->getLocationId()) || stationId == -1)
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setLocationId( stationId );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Platform could not be found in the database");

			m_stationCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


	void AtsPowerEntityMapDetailView::onKillfocusEditSubsection() 
	{
        FUNCTION_ENTRY("onKillfocusEditSubsection");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString subsectionNumberString;
        m_subsectionEdit.GetWindowText(subsectionNumberString);
		int subsectionNumber;
		if (subsectionNumberString != "")
		{
			subsectionNumber = atoi((LPCTSTR)subsectionNumberString);
		}
		else
		{
			subsectionNumber = -1;
		}

        try
        {
            // If the zoneId has not changed then there is no point doing an update
            if (subsectionNumber == static_cast<int>(m_configItem->getElectricalSubSectionId()) || subsectionNumber == -1)
            {
                m_subsectionEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setElectricalSubSectionId( subsectionNumber );
            m_subsectionEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ATS Power Entity Map could not be found in the database");

            m_subsectionEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsPowerEntityMapDetailView diagnostics

    #ifdef _DEBUG
    void AtsPowerEntityMapDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsPowerEntityMapDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
