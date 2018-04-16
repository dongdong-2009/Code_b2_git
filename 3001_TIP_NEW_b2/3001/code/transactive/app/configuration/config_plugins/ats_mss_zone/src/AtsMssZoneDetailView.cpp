/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an Ats MSS Zone's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_mss_zone/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneDetailView.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneListView.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsMssZone.h"
#include "core/data_access_interface/ats/src/AtsMssZone.h"
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
    const int AtsMssZoneDetailView::ATS_BRANCH_ID_FIELD_LENGTH = 10;
    
    IMPLEMENT_DYNCREATE(AtsMssZoneDetailView, CFormView)

    AtsMssZoneDetailView::AtsMssZoneDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsMssZoneDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsMssZoneDetailView)
	//}}AFX_DATA_INIT
    }


    AtsMssZoneDetailView::~AtsMssZoneDetailView()
    {
    }


    void AtsMssZoneDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsMssZoneDetailView)
	DDX_Control(pDX, IDC_COMBO_STATION, m_stationCombo);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_COMBO_MSSZONE, m_mssZoneCombo);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsMssZoneDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsMssZoneDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_CBN_KILLFOCUS(IDC_COMBO_STATION, onKillfocusComboStation)
	ON_CBN_KILLFOCUS(IDC_COMBO_MSSZONE, onKillfocusComboMssZone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsMssZoneDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
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
				m_mssZoneCombo.AddString(location);
				delete (*it);
				(*it) = NULL;
			}
		}
		catch (...)
		{
		}

        // Hide all the Ats MSS Zone details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsMssZoneDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

        m_stationCombo.ShowWindow(isShown);
        m_mssZoneCombo.ShowWindow(isShown);
        m_type.ShowWindow(isShown);

		GetDlgItem(IDC_STATION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_MSSZONE_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		GetDlgItem(IDC_STATION_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_MSSZONE_BITMAP)->ShowWindow(isShown);
    }


    void AtsMssZoneDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsMssZoneDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsMssZoneListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsMssZoneDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsMssZone*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsMssZone* object
			AtsMssZoneDatabaseAccessor::AtsMssZoneRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());
            
			if( m_configItem->isNew() )
            {

                // This is a new Ats MSS Zone or at least a copy of an Ats MSS Zone so we indicate this in the type
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

			m_stationCombo.SetCurSel(m_stationCombo.FindStringExact(-1, details.station));
			m_mssZoneCombo.SetCurSel(m_mssZoneCombo.FindStringExact(-1, details.mssZone));

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Ats MSS Zone details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// ++ TD14164
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Ats MSS Zone details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Ats MSS Zone";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsMssZoneDetailView::refreshButtonAvailability()
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

    void AtsMssZoneDetailView::onKillfocusComboStation() 
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
            if (stationId == static_cast<int>(m_configItem->getStationId()) || stationId == -1)
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setStationId( stationId );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats MSS Zone could not be found in the database");

			m_stationCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AtsMssZoneDetailView::onKillfocusComboMssZone() 
    {
        FUNCTION_ENTRY("onKillfocusComboMssZone");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString mssZoneString;
		
		if (m_mssZoneCombo.GetCurSel() >= 0)
		{
			m_mssZoneCombo.GetLBText(m_mssZoneCombo.GetCurSel(), mssZoneString);
		}
		else
		{
			mssZoneString = "";
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (mssZoneString == m_configItem->getMssZone().c_str() || mssZoneString == "")
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setMssZone( (LPCTSTR) mssZoneString );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats MSS Zone could not be found in the database");

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AtsMssZoneDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsMssZoneDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( m_stationCombo.GetCurSel() == -1 || m_mssZoneCombo.GetCurSel() == -1 )
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
    // AtsMssZoneDetailView diagnostics

    #ifdef _DEBUG
    void AtsMssZoneDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsMssZoneDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
