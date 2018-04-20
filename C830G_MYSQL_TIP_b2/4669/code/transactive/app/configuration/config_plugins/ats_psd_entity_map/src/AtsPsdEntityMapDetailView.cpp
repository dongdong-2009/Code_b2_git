/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ATS PSD Entity Map's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_psd_entity_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDetailView.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapListView.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsPSDData.h"
#include "core/data_access_interface/ats/src/AtsPSDData.h"
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
    const CString AtsPsdEntityMapDetailView::CHANGE_STATUS_DATAPOINT_DIALOG_TITLE    = "Change Status DataPoint";
    const CString AtsPsdEntityMapDetailView::CHANGE_DIRECTIONS_DATAPOINT_DIALOG_TITLE  = "Change Directions DataPoint";

    const int AtsPsdEntityMapDetailView::PSD_NUMBER_FIELD_LENGTH = 2;
    const int AtsPsdEntityMapDetailView::DATAPOINT_FIELD_LENGTH = 256;
    
    IMPLEMENT_DYNCREATE(AtsPsdEntityMapDetailView, CFormView)


    AtsPsdEntityMapDetailView::AtsPsdEntityMapDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsPsdEntityMapDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsPsdEntityMapDetailView)
	//}}AFX_DATA_INIT
    }


    AtsPsdEntityMapDetailView::~AtsPsdEntityMapDetailView()
    {
    }


    void AtsPsdEntityMapDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsPsdEntityMapDetailView)
	DDX_Control(pDX, IDC_BUTTON_RESET, m_resetBtn);
	DDX_Control(pDX, IDC_EDIT_PSDNUMBER, m_psdNumberEdit);
	DDX_Control(pDX, IDC_EDIT_DATAPOINT, m_datapointEdit);
	DDX_Control(pDX, IDC_COMBO_PLATFORM, m_platformCombo);
	DDX_Control(pDX, IDC_BUTTON_DATAPOINT, m_datapointButton);
	DDX_Control(pDX, IDC_COMBO_DPTYPE, m_dpTypeCombo);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsPsdEntityMapDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsPsdEntityMapDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_CBN_KILLFOCUS(IDC_COMBO_DPTYPE, onKillfocusComboDptype)
	ON_CBN_KILLFOCUS(IDC_COMBO_PLATFORM, onKillfocusComboPlatform)
	ON_BN_CLICKED(IDC_BUTTON_DATAPOINT, onButtonDatapoint)
	ON_EN_KILLFOCUS(IDC_EDIT_DATAPOINT, onKillfocusEditDatapoint)
	ON_EN_KILLFOCUS(IDC_EDIT_PSDNUMBER, onKillfocusEditPsdnumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsPsdEntityMapDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

		m_psdNumberEdit.SetLimitText(PSD_NUMBER_FIELD_LENGTH);
		m_datapointEdit.SetLimitText(DATAPOINT_FIELD_LENGTH);

		m_dpTypeCombo.AddString(TA_IRS_Core::AtsPSDData::OUTPUT_DOOR_INHIBIT_STATE.c_str());
		m_dpTypeCombo.AddString(TA_IRS_Core::AtsPSDData::OUTPUT_PRE_CLOSE_WARNING.c_str());
		m_dpTypeCombo.AddString(TA_IRS_Core::AtsPSDData::INPUT_DOOR_STATUS.c_str());

        // Hide all the ATS PSD Entity Map details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsPsdEntityMapDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

		m_psdNumberEdit.ShowWindow(isShown);
		m_datapointEdit.ShowWindow(isShown);
		m_platformCombo.ShowWindow(isShown);
		m_datapointButton.ShowWindow(isShown);
		m_dpTypeCombo.ShowWindow(isShown);
		m_applyBtn.ShowWindow(isShown);
		m_resetBtn.ShowWindow(isShown);
		m_type.ShowWindow(isShown);

		GetDlgItem(IDC_PLATFORM_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATAPOINT_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DPTYPE_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_PSDNUMBER_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_DATAPOINT_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_PSDNUMBER_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_PLATFORM_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_DPTYPE_BITMAP)->ShowWindow(isShown);
    }


    void AtsPsdEntityMapDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsPsdEntityMapDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsPsdEntityMapListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsPsdEntityMapDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsPSDData*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsPSDData* object
			AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());            
			
			// Platform Name
			// Get all the items for the combo
			try
			{
				TA_IRS_Core::IAtsPlatforms platforms = 
					TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAllAtsPlatforms();
			
				m_platformCombo.ResetContent();

				for (TA_IRS_Core::IAtsPlatformsIt it = platforms.begin();
					 it != platforms.end();
					 it++)
				{
					unsigned long locationId = (*it)->getStationId();
					TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationId);

					std::ostringstream platform;

					platform << location->getName().c_str() << " - ";
					platform << (*it)->getName().c_str() << " (";
					platform << (*it)->getPlatformId() << ")";

					delete location;

					CString platformCString = platform.str().c_str();
					m_platformCombo.AddString(platformCString);

					delete (*it);
					(*it) = NULL;
				}
			}
			catch (...)
			{
			}

			if (details.platformName == "")
            {
                m_platformCombo.SetCurSel(-1);
            }
            else
            {
				std::ostringstream platform;

				platform << std::string(details.station) << " - ";
				platform << std::string(details.platformName) << " (";
				platform << m_configItem->getPlatformId() << ")";

				CString fullPlatformName = platform.str().c_str();
				m_platformCombo.SetCurSel(m_platformCombo.FindStringExact(-1, fullPlatformName));
            }

			// PSD Number
			if (details.psdNumber == "")
            {
                m_psdNumberEdit.SetWindowText( "" );
                m_psdNumberEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_psdNumberEdit.SetWindowText( details.psdNumber );
                m_psdNumberEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

			// Datapoint type
			if (details.dpType == "")
            {
                m_dpTypeCombo.SetCurSel(-1);
            }
            else
            {
				m_dpTypeCombo.SetCurSel(m_dpTypeCombo.FindStringExact(-1, details.dpType));
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

    void AtsPsdEntityMapDetailView::refreshButtonAvailability()
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

    void AtsPsdEntityMapDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsPsdEntityMapDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_psdNumberEdit.isTextValid() || !m_datapointEdit.isTextValid() || 
             m_platformCombo.GetCurSel() == -1 || m_dpTypeCombo.GetCurSel() == -1)
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

	void AtsPsdEntityMapDetailView::onKillfocusComboDptype() 
	{
        FUNCTION_ENTRY("onKillfocusComboDptype");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString dpTypeString;
		
		if (m_dpTypeCombo.GetCurSel() >= 0)
		{
			m_dpTypeCombo.GetLBText(m_dpTypeCombo.GetCurSel(), dpTypeString);
		}
		else
		{
			dpTypeString = "";
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (dpTypeString == m_configItem->getPSDDataPointNameOfType().c_str())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setPSDDataPointNameOfType( (LPCTSTR) dpTypeString );

			if (dpTypeString == TA_IRS_Core::AtsPSDData::INPUT_DOOR_STATUS.c_str())
			{
				m_configItem->setPSDDataPointType( TA_IRS_Core::AtsPSDData::INPUT_TYPE );
			}
			else
			{
				m_configItem->setPSDDataPointType( TA_IRS_Core::AtsPSDData::OUTPUT_TYPE );
			}
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

			m_dpTypeCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

	void AtsPsdEntityMapDetailView::onKillfocusComboPlatform() 
	{
        FUNCTION_ENTRY("onKillfocusComboPlatform");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString platformCString;
		
		if (m_platformCombo.GetCurSel() >= 0)
		{
			m_platformCombo.GetLBText(m_platformCombo.GetCurSel(), platformCString);
		}
		else
		{
			platformCString = "";
		}

		std::string platformString = platformCString;
		std::string::size_type pos = platformString.find_last_of("(");
		int platformId = -1;

		if (pos != std::string::npos)
		{
			std::istringstream platformStream;
			platformStream.str(platformString.substr(pos+1)); // Skip the bracket;
			platformStream >> platformId;
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (platformId == static_cast<int>(m_configItem->getPlatformId()))
            {
                FUNCTION_EXIT;
                return;
            }

			// Need to find the platform key at this point!
			std::vector<int> keys;
			
			try
			{
				keys = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAtsPlatformIdsWhere(platformId);
			}
			catch (...)
			{
			}

			if (keys.size() != 1)
			{
				return;
			}

			m_configItem->setPlatformKey(keys[0]);
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

			m_platformCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

	void AtsPsdEntityMapDetailView::onButtonDatapoint() 
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
                m_configItem->setPSDDataPointEntityKey( dlg.getSelectedKey() );

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

	void AtsPsdEntityMapDetailView::onKillfocusEditDatapoint() 
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
            if ( entityKey == m_configItem->getPSDDataPointEntityKey())
            {
                m_datapointEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            
            m_configItem->setPSDDataPointEntityKey( entityKey );
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


	void AtsPsdEntityMapDetailView::onKillfocusEditPsdnumber() 
	{
        FUNCTION_ENTRY("onKillfocusEditPsdnumber");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString psdNumberString;
        m_psdNumberEdit.GetWindowText(psdNumberString);
		int psdNumber;
		if (psdNumberString != "")
		{
			psdNumber = atoi((LPCTSTR)psdNumberString);
		}
		else
		{
			psdNumber = -1;
		}

        try
        {
            // If the zoneId has not changed then there is no point doing an update
            if (psdNumber == m_configItem->getPSDNumber() || psdNumber == -1)
            {
                m_psdNumberEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setPSDNumber( psdNumber );
            m_psdNumberEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ATS PSD Entity Map could not be found in the database");

            m_psdNumberEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsPsdEntityMapDetailView diagnostics

    #ifdef _DEBUG
    void AtsPsdEntityMapDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsPsdEntityMapDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
