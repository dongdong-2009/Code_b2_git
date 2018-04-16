/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_platform/src/AtsPlatformDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Ats Platform's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_platform/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformDetailView.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformListView.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/IConfigAtsPlatform.h"
#include "core/data_access_interface/ats/src/AtsPlatform.h"
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
    const int AtsPlatformDetailView::PLATFORMID_FIELD_LENGTH = 10;
	const int AtsPlatformDetailView::PLATFORMNAME_FIELD_LENGTH = 50;
    const int AtsPlatformDetailView::TISPLATFORMID_FIELD_LENGTH = 1;
    
    IMPLEMENT_DYNCREATE(AtsPlatformDetailView, CFormView)

    AtsPlatformDetailView::AtsPlatformDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsPlatformDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsPlatformDetailView)
	//}}AFX_DATA_INIT
    }


    AtsPlatformDetailView::~AtsPlatformDetailView()
    {
    }


    void AtsPlatformDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsPlatformDetailView)
	DDX_Control(pDX, IDC_EDIT_PLATFORMID, m_platformIdEdit);
	DDX_Control(pDX, IDC_COMBO_STATION, m_stationCombo);
	DDX_Control(pDX, IDC_EDIT_PLATFORMNAME, m_platformNameEdit);
    DDX_Control(pDX, IDC_EDIT_TIS_PLATFORM_ID, m_tisPlatformIdEdit);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsPlatformDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsPlatformDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_PLATFORMID, onKillfocusEditPlatformId)
	ON_CBN_KILLFOCUS(IDC_COMBO_STATION, onKillfocusComboStation)
	ON_EN_KILLFOCUS(IDC_EDIT_PLATFORMNAME, onKillfocusEditPlatformName)
    ON_EN_KILLFOCUS(IDC_EDIT_TIS_PLATFORM_ID, onKillfocusEditTisPlatformId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsPlatformDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
		m_platformIdEdit.SetLimitText(PLATFORMID_FIELD_LENGTH);
        m_tisPlatformIdEdit.SetLimitText(TISPLATFORMID_FIELD_LENGTH);

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

		m_platformNameEdit.SetLimitText(PLATFORMNAME_FIELD_LENGTH);

        // Hide all the Ats Platform details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsPlatformDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

		m_platformIdEdit.ShowWindow(isShown);
        m_stationCombo.ShowWindow(isShown);
		m_platformNameEdit.ShowWindow(isShown);
        m_tisPlatformIdEdit.ShowWindow(isShown);
        m_type.ShowWindow(isShown);

		GetDlgItem(IDC_PLATFORMID_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_STATION_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_PLATFORMNAME_STATIC)->ShowWindow(isShown);
        GetDlgItem(IDC_TIS_PLATFORM_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		GetDlgItem(IDC_PLATFORMID_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_STATION_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_PLATFORMNAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_TIS_ID_BITMAP)->ShowWindow(isShown);
    }


    void AtsPlatformDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsPlatformDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsPlatformListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsPlatformDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsPlatform*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsPlatform* object
			AtsPlatformDatabaseAccessor::AtsPlatformRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());
            
			if( m_configItem->isNew() )
            {

                // This is a new Ats Platform or at least a copy of an Ats Platform so we indicate this in the type
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

			// Platform Id
			m_platformIdEdit.SetWindowText( details.platformId );
            m_platformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_platformIdEdit.EnableWindow(true);

			// Station
			m_stationCombo.SetCurSel(m_stationCombo.FindStringExact(-1, details.station));

			// Platform Name
			m_platformNameEdit.SetWindowText( details.platformName );
            m_platformNameEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_platformNameEdit.EnableWindow(true);

            // Tis Platform Id
			m_tisPlatformIdEdit.SetWindowText( details.tisPlatformId );
            m_tisPlatformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_tisPlatformIdEdit.EnableWindow(true);

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Ats Platform details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Ats Platform details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Ats Platform";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsPlatformDetailView::refreshButtonAvailability()
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

	void AtsPlatformDetailView::onKillfocusEditPlatformId() 
    {
		FUNCTION_ENTRY("onKillfocusEditPlatformId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString platformIdCString;
        m_platformIdEdit.GetWindowText(platformIdCString);
		std::string platformIdString = platformIdCString;
		std::istringstream platformIdStream; 
		platformIdStream.str(platformIdString);
		unsigned long platformId;
		if (!(platformIdStream >> platformId))
		{
			// Failed - clear out
			m_platformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

			FUNCTION_EXIT;
            return;
		}

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getPlatformId() == platformId)
            {
                m_platformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setPlatformId( platformId );
            m_platformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Platform could not be found in the database");

            m_platformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;	
	}


    void AtsPlatformDetailView::onKillfocusComboStation() 
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Platform could not be found in the database");

			m_stationCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

	void AtsPlatformDetailView::onKillfocusEditPlatformName() 
    {
		FUNCTION_ENTRY("onKillfocusEditPlatformName");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString platformNameCString;
        m_platformNameEdit.GetWindowText(platformNameCString);
		std::string platformNameString = platformNameCString;

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getName() == platformNameString)
            {
                m_platformNameEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setName( platformNameString );
            m_platformNameEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Platform could not be found in the database");

            m_platformNameEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;	
	}


    void AtsPlatformDetailView::onKillfocusEditTisPlatformId() 
    {
		FUNCTION_ENTRY("onKillfocusEditTisPlatformId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tisPlatformIdCString;
        m_tisPlatformIdEdit.GetWindowText(tisPlatformIdCString);
		std::string tisPlatformIdString = tisPlatformIdCString;
		std::istringstream tisPlatformIdStream; 
		tisPlatformIdStream.str(tisPlatformIdString);
		unsigned long tisPlatformId;
		if (!(tisPlatformIdStream >> tisPlatformId))
		{
			// Failed - clear out
			m_tisPlatformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

			FUNCTION_EXIT;
            return;
		}

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getTisPlatformId() == tisPlatformId)
            {
                m_tisPlatformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTisPlatformId( tisPlatformId );
            m_tisPlatformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Platform could not be found in the database");

            m_tisPlatformIdEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;	
	}
    


    void AtsPlatformDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsPlatformDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_platformIdEdit.isTextValid() || m_stationCombo.GetCurSel() == -1 || 
			 !m_platformNameEdit.isTextValid())
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
    // AtsPlatformDetailView diagnostics

    #ifdef _DEBUG
    void AtsPlatformDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsPlatformDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
