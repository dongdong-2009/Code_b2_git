/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_station/src/PAStationDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an PA Station's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/data_access_interface/pa/src/IConfigPaStation.h"
#include "core/data_access_interface/pa/src/PaStationHelper.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/DatabaseKey.h"

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
    const CString PaStationDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";

    const int PaStationDetailView::STATION_ID_FIELD_LENGTH            = 5;
    const int PaStationDetailView::LOCATION_FIELD_LENGTH              = 40;     

    IMPLEMENT_DYNCREATE(PaStationDetailView, CFormView)


    PaStationDetailView::PaStationDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(PaStationDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(PaStationDetailView)
	//}}AFX_DATA_INIT
    }


    PaStationDetailView::~PaStationDetailView()
    {
    }


    void PaStationDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaStationDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

        DDX_Control(pDX, IDC_EDIT_STATION_ID,   m_stationIdEdt);
    	DDX_Control(pDX, IDC_EDIT_LOCATION,     m_locationEdt);
    	
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);

        DDX_Control(pDX, IDC_STATION_ID_BITMAP, m_transparentImageId);
        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);    	
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaStationDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaStationDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_STN_LOCATION, onButtonLocation)

    ON_EN_KILLFOCUS(IDC_EDIT_STATION_ID,    onKillFocusEditStationId)
	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION,      onKillFocusEditLocation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void PaStationDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_stationIdEdt.SetLimitText(STATION_ID_FIELD_LENGTH);
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
          	
        // Hide all the PA Station details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PaStationDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaStationDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaStationDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaStationListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaStationDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaStation*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigPaStation* object
            CString buffer;

            if (TA_Base_Core::DatabaseKey::isInvalidKey(m_configItem->getModifiedKey()) )
            {
                m_stationIdEdt.SetWindowText("");
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID);
            }
            else
            {
                buffer.Format("%ld", m_configItem->getModifiedKey());
                m_stationIdEdt.SetWindowText(buffer);

                bool checkKeyForDuplicate = 
                    m_configItem->isNew() || (m_configItem->getModifiedKey() != m_configItem->getKey());

                if (checkKeyForDuplicate &&
                        TA_Base_Core::PaStationHelper::isKeyDuplicate(m_configItem->getModifiedKey()) )
                {
                    m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID);
                }
                else
                {
                    m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID);
                }
            }

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

            if( m_configItem->isNew() )
            {
                // This is a new PA Station or at least a copy of an PA Station so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA Station details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA Station details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA Station";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }
    

    void PaStationDetailView::refreshButtonAvailability()
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


    void PaStationDetailView::onKillFocusEditStationId() 
    {
        FUNCTION_ENTRY("onKillFocusEditStationId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString idString;
        m_stationIdEdt.GetWindowText(idString);
        
        TA_Base_App::FieldValidityChecker fvc;
        if (0 == idString.GetLength() || !fvc.isValidInteger(idString.GetBuffer(0)))
        {
            // Must enter a valid (non-empty) key value
            m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        long keyValue = atol(idString.GetBuffer(0));
        
        try
        {
            // If the track string has not changed then there is no point doing an update
            // (can't perform this optimisation w.r.t the isKeyDuplicate behaviour, it fails
            // under certain circumstances)
            /*
            if (keyValue == (long) m_configItem->getModifiedKey())
            {                
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }
            */

            if (keyValue <= 0)
            {
                // Only allow positive valued keys
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                return;           
            }

            // If it has changed from original, verify the new key is unique in the system            
            // (note if key matches config items actual key, as returned by getKey,
            // then it is definitely not a duplicate)
            bool checkForDuplicates = m_configItem->isNew() || keyValue != (long) m_configItem->getKey();

            if (checkForDuplicates &&
                TA_Base_Core::PaStationHelper::isKeyDuplicate(keyValue))
            {
                // Not allowed to request a duplicate key
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                return;
            }
            
            m_configItem->setKey( keyValue );
            m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Station could not be found in the database");

            m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }  


    void PaStationDetailView::onKillFocusEditLocation() 
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The PA Zone could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    } 


    void PaStationDetailView::onButtonLocation() 
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


    void PaStationDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void PaStationDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_stationIdEdt.isTextValid() ||
             !m_locationEdt.isTextValid())
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
    // PaStationDetailView diagnostics

    #ifdef _DEBUG
    void PaStationDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaStationDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

