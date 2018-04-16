/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/10/25 16:50:09 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This view allows the user to configure an PA Station DVA Message's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaTisMessageMap.h"

#include "bus/pa_4669/pa_agent/idl/src/IPAAgentCorbaDef.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"

#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"

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
    const CString PaStationDvaMessageDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";
    const CString PaStationDvaMessageDetailView::SELECT_TIS_TITLE   = "Select TIS Message";
    
    const int PaStationDvaMessageDetailView::LOCATION_FIELD_LENGTH           = 40;
    const int PaStationDvaMessageDetailView::ID_FIELD_LENGTH                 = 5;
    const int PaStationDvaMessageDetailView::LABEL_FIELD_LENGTH              = 40;  // Length of PA_DVA_MESSAGE.LABEL      
       
    const int PaStationDvaMessageDetailView::TIS_MESSAGE_TAG_FIELD_LENGTH       = 5;
    const int PaStationDvaMessageDetailView::TIS_LIBRARY_SECTION_FIELD_LENGTH   = 5;
    const int PaStationDvaMessageDetailView::TIS_LIBRARY_VERSION_FIELD_LENGTH   = 5;
    const int PaStationDvaMessageDetailView::TIS_MESSAGE_FIELD_LENGTH = 125;    // Length of TI_PREDEFINED_MESSAGE.DESCRIPTION

    IMPLEMENT_DYNCREATE(PaStationDvaMessageDetailView, CFormView)
    
    static int CALLBACK SortTisDescendingVersion(LPARAM index1, LPARAM index2, LPARAM data)
    {
        PaStationDvaMessageDatabaseAccessor* db = reinterpret_cast<PaStationDvaMessageDatabaseAccessor*>(data);

        if (0 != db)
        {
            try
            {
                TA_Base_Core::TisMessage a = db->getTisMessage(index1);
                TA_Base_Core::TisMessage b = db->getTisMessage(index2);
    
                // Sort in descending order based on library version
                return a.tisLibraryVersion < b.tisLibraryVersion;
            }
            catch (const TA_Base_Core::TransactiveException&)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }


    PaStationDvaMessageDetailView::PaStationDvaMessageDetailView() : 
        m_locationEdt(true),
    	m_idEdt(true),
        m_labelEdt(true),
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(PaStationDvaMessageDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(PaStationDvaMessageDetailView)
	//}}AFX_DATA_INIT
    }


    PaStationDvaMessageDetailView::~PaStationDvaMessageDetailView()
    {
    }


    void PaStationDvaMessageDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaStationDvaMessageDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

    	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);    	
        DDX_Control(pDX, IDC_EDIT_ID, m_idEdt);
    	DDX_Control(pDX, IDC_EDIT_LABEL, m_labelEdt);
    	DDX_Control(pDX, IDC_COMBO_TYPE, m_msgTypeCombo);        
        
        DDX_Control(pDX, IDC_EDIT_TIS_MESSAGE_TAG, m_tisMessageTagEdt);        
        DDX_Control(pDX, IDC_EDIT_TIS_LIBRARY_SECTION, m_tisLibrarySectionEdt);        
        DDX_Control(pDX, IDC_EDIT_TIS_LIBRARY_VERSION, m_tisLibraryVersionEdt);        
        DDX_Control(pDX, IDC_EDIT_TIS_MESSAGE, m_tisMessageEdt);

        //DDX_Control(pDX, IDC_BUTTON_LOCATION, m_locationBtn);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
    	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);        
        
        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);
        DDX_Control(pDX, IDC_ID_BITMAP, m_transparentImageId);
    	DDX_Control(pDX, IDC_LABEL_BITMAP, m_transparentImageLabel);
        DDX_Control(pDX, IDC_TYPE_BITMAP, m_transparentImageType);
    	
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaStationDvaMessageDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaStationDvaMessageDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
    ON_BN_CLICKED(IDC_BUTTON_TIS_MESSAGE, onButtonTisMessage)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_TIS, onButtonClearTisMessage)
	    
	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_ID, onKillFocusEditId)
    ON_EN_KILLFOCUS(IDC_EDIT_LABEL, onKillFocusEditLabel)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, onSelchangeTypeCombo)
    
    //ON_EN_KILLFOCUS(IDC_EDIT_TIS_MESSAGE_TAG, onKillFocusEditTisMessageTag)
    //ON_EN_KILLFOCUS(IDC_EDIT_TIS_LIBRARY_SECTION, onKillFocusEditTisLibrarySection)
    //ON_EN_KILLFOCUS(IDC_EDIT_TIS_LIBRARY_VERSION, onKillFocusEditTisLibraryVersion)
    ON_EN_KILLFOCUS(IDC_EDIT_TIS_MESSAGE, onKillFocusEditTisMessage)

    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    void PaStationDvaMessageDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

        m_msgTypeCombo.ResetContent();
        CString buf;
        buf.LoadString(IDS_DVA_TYPE_NORMAL);
        m_msgTypeCombo.AddString(buf);
	    buf.LoadString(IDS_DVA_TYPE_EMERGENCY);
        m_msgTypeCombo.AddString(buf);
	    buf.LoadString(IDS_DVA_TYPE_DELETED);
        m_msgTypeCombo.AddString(buf);
	    
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_idEdt.SetLimitText(ID_FIELD_LENGTH);
        m_labelEdt.SetLimitText(LABEL_FIELD_LENGTH);

        m_tisMessageTagEdt.SetLimitText(TIS_MESSAGE_TAG_FIELD_LENGTH);
        m_tisLibrarySectionEdt.SetLimitText(TIS_LIBRARY_SECTION_FIELD_LENGTH);
        m_tisLibraryVersionEdt.SetLimitText(TIS_MESSAGE_TAG_FIELD_LENGTH);

        m_tisMessageEdt.SetLimitText(TIS_MESSAGE_FIELD_LENGTH);

        // Hide all the PA Station DVA Message details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);
        int controlID;

        while (0 != nextChild)
        {
            controlID = GetWindowLong(nextChild->GetSafeHwnd(), GWL_ID);
            //switch(controlID)
            //{
            //case IDC_STATIC:
            //case IDC_EDIT_TIS_MESSAGE:
            //case IDC_BUTTON_TIS_MESSAGE:
            //case IDC_EDIT_TIS_MESSAGE_TAG:
            //case IDC_EDIT_TIS_LIBRARY_SECTION:
            //case IDC_EDIT_TIS_LIBRARY_VERSION:
            //case IDC_BUTTON_CLEAR_TIS:
            //    nextChild->ShowWindow(FALSE);
            //    break;
            //default:
            //    nextChild->ShowWindow(isShown);
            //}
            nextChild->ShowWindow(isShown);
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaStationDvaMessageDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaStationDvaMessageDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaStationDvaMessageListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaStationDvaMessageDetailView");
        }

        TA_Base_Core::IConfigPaDvaMessage* tmpConfigItem = m_configItem;

        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaDvaMessage*>(currentItem);

        if (m_configItem == NULL)
        { 
            m_configItem = tmpConfigItem;
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigPaDvaMessage* object
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
                m_idEdt.SetWindowText("");
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID);
            }
            else
            {
                buffer.Format("%d", m_configItem->getId());
                m_idEdt.SetWindowText( (LPCTSTR)buffer );
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
                    
            m_labelEdt.SetWindowText( m_configItem->getLabel().c_str() );

            if (0 == m_configItem->getLabel().length() && TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED != m_configItem->getType())
            {
                // TES741: If message is not the deleted type, disallow empty string as label
                m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );         
            }
            else
            {
                m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            m_msgTypeCombo.SelectString(-1, 
                    PaStationDvaMessageDatabaseAccessor::getTypeString(m_configItem->getType()) );                
            

            m_paTisMsgMap = m_databaseAccessor->getPaTisMessageMapByPadmesId(m_configItem->getKey());

            if (NULL == m_paTisMsgMap)
            {
                m_tisMessageTagEdt.SetWindowText("");
                m_tisLibrarySectionEdt.SetWindowText("");
                m_tisLibraryVersionEdt.SetWindowText("");
                m_tisMessageEdt.SetWindowText("");
            }
            else
            {
                if (TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisMessageTag()))
                {
                    m_tisMessageTagEdt.SetWindowText("");
                }
                else
                {
                    buffer.Format("%d", m_paTisMsgMap->getTisMessageTag());
                    m_tisMessageTagEdt.SetWindowText( (LPCTSTR)buffer );
                }

                if (TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisLibrarySection()))
                {
                    m_tisLibrarySectionEdt.SetWindowText("");
                }
                else
                {
                    buffer.Format("%d", m_paTisMsgMap->getTisLibrarySection());
                    m_tisLibrarySectionEdt.SetWindowText( (LPCTSTR)buffer );
                }

                if (TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisLibraryVersion()))
                {
                    m_tisLibraryVersionEdt.SetWindowText("");
                }
                else
                {
                    buffer.Format("%d", m_paTisMsgMap->getTisLibraryVersion() );
                    m_tisLibraryVersionEdt.SetWindowText( (LPCTSTR)buffer );
                }

                if (TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisMessageTag()) ||
                    TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisLibrarySection()) )
                {
                    buffer = "";
                }
                else
                {
                    // Valid TIS identifier set, attempt to obtain description
                    try
                    {
                        buffer = m_databaseAccessor->getTisMessage(*m_paTisMsgMap).description.c_str();
                    }
                    catch (const TA_Base_Core::TransactiveException&)
                    {
                        buffer = "#UNKNOWN#";
                    }
                }

                m_tisMessageEdt.SetWindowText((LPCTSTR) buffer);
            }



            //m_tisMessageEdt.setTextType(TA_Base_Bus::TextColourEdit::VALID);
            //m_tisMessageTagEdt.setTextType(TA_Base_Bus::TextColourEdit::VALID);
            //m_tisLibrarySectionEdt.setTextType(TA_Base_Bus::TextColourEdit::VALID);
            //m_tisLibraryVersionEdt.setTextType(TA_Base_Bus::TextColourEdit::VALID);
            
            if( m_configItem->isNew() )
            {

                // This is a new PA Station DVA Message or at least a copy of an PA Station DVA Message so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA Station DVA Message details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA Station DVA Message details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA Station DVA Message";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::refreshButtonAvailability()
    {
        if (m_configItem == NULL)
        {
            return;
        }

        if (m_configItem->isNew())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_configItem->hasChanged() || ((NULL != m_paTisMsgMap) && (m_paTisMsgMap->isNew() || m_paTisMsgMap->hasChanged())))
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }

        CString tisMessageContent;
        m_tisMessageEdt.GetWindowText(tisMessageContent);
        GetDlgItem(IDC_BUTTON_CLEAR_TIS)->EnableWindow(!tisMessageContent.IsEmpty());
    }


    void PaStationDvaMessageDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
        /*
        // this is all populated by the hardware - it is pointless to edit here
	    
        // Retrieve the current location this PA Station DVA Message is using
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
        */
        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::onKillfocusEditLocation() 
    {
        FUNCTION_ENTRY("onKillfocusEditLocation");
        /*
        // this is all populated by the hardware - it is pointless to edit here

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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
        
        */

        FUNCTION_EXIT;
    }
    

    void PaStationDvaMessageDetailView::onKillFocusEditId() 
    {
        FUNCTION_ENTRY("onKillFocusEditid");
        /*
        // this is all populated by the hardware - it is pointless to edit here

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString idString;
        m_idEdt.GetWindowText(idString);

        TA_Base_App::FieldValidityChecker fvc;
        // Only allow positive valued id's
        long id = (long)atol((LPCTSTR)idString);

        if (!fvc.isValidInteger(idString.GetBuffer(0)) || id < 0)
        {
            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (id == (long) m_configItem->getId())
            {                
                m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setId( id );
            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_idEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
        */

        FUNCTION_EXIT;
    }  


    void PaStationDvaMessageDetailView::onKillFocusEditLabel() 
    {
        FUNCTION_ENTRY("onKillFocusEditLabel");
        /*
        // this is all populated by the hardware - it is pointless to edit here

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString labelString;
        m_labelEdt.GetWindowText(labelString);

        if (0 == labelString.GetLength() && "X" != m_configItem->getType())
        {
            // TES741: If message is not the deleted type, disallow empty string as label
            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_labelEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        */

        FUNCTION_EXIT;
    }  


    void PaStationDvaMessageDetailView::onSelchangeTypeCombo()
    {
        FUNCTION_ENTRY("onSelchangeTypeCombo");

        /*
        // this is all populated by the hardware - it is pointless to edit here

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        int curSel = m_msgTypeCombo.GetCurSel();

        if (curSel < 0)
        {
            return;
        }

        CString rString;
        m_msgTypeCombo.GetLBText(curSel, rString);

        // Default to deleted, change as necessary
        unsigned char type = 'X';
        
        CString normalStr, emergStr;
        normalStr.LoadString(IDS_DVA_TYPE_NORMAL);
        emergStr.LoadString(IDS_DVA_TYPE_EMERGENCY);
        
        if (rString == normalStr)
        {
            type = 'N';
        }
        else if (rString == emergStr)
        {
            type = 'E';
        }

        try
        {
            // If the operation string has not changed then there is no point doing an update
            std::string configType = m_configItem->getType();

            if (configType.length() == 1 && type == configType.at(0))
            {                
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setType( std::string(1, type) );            
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            FUNCTION_EXIT;
            return;
        }

        // TES741: If changed the deleted state of an item, the label may have become invalid
        // (so we need to check for this)..
        onKillFocusEditLabel();
        
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        */

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        if (!m_paTisMsgMap->isNew())
        {
            resetButtonPressed(m_paTisMsgMap);
        }
        else
        {
            m_databaseAccessor->removePaTisMap(m_configItem->getKey());
            delete m_paTisMsgMap;
        }
        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        /*
        // this is all populated by the hardware - it is pointless to edit here

        if ( !m_locationEdt.isTextValid()    || 
             !m_idEdt.isTextValid()          ||
             !m_labelEdt.isTextValid())
        {
            //UE-030060
            MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);
            return;
        }
        */
        if (TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisMessageTag()) || 
            TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisLibrarySection()) ||
            TA_Base_Core::DatabaseKey::isInvalidKey(m_paTisMsgMap->getTisLibraryVersion()) )
        {
            m_databaseAccessor->removePaTisMap(m_configItem->getKey());
            m_paTisMsgMap->deleteThisObject();
        }
        else
        {
            applyButtonPressed(m_paTisMsgMap);
        }
        applyButtonPressed(m_configItem);
        

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::onLocationChanged()
    {   
        // Whenever the operator changes the location, it may affect
        // validity of the zone list associations - so refresh their status
        // (OCC terminal can view all, stations can only associate with local zones)
        CWaitCursor wait;
    }


    void PaStationDvaMessageDetailView::onButtonClearTisMessage()
    {
        if (0 == m_configItem)
        {
            return;
        }

        // By setting to an invalid key, able to clear current TIS message
        setTisMessage(TA_Base_Core::DatabaseKey::getInvalidKey());
    }


    void PaStationDvaMessageDetailView::setTisMessage(unsigned long tisKey)
    {
        TA_ASSERT(m_configItem, "Config item is NULL");

        if (NULL == m_paTisMsgMap)
        {
            m_paTisMsgMap = m_databaseAccessor->createPaTisMessageMap(m_configItem->getKey());
        }

        if (m_databaseAccessor->updateConfigItemsTisMessage(*m_paTisMsgMap, tisKey))
        {                    
            m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            
            // Must re-populate control, to have the entire TIS section update
            populateItemDetails(m_configItem);
            refreshButtonAvailability();
        }
    }


    void PaStationDvaMessageDetailView::onButtonTisMessage() 
    {
        if (!m_configItem) 
        {
            return;
        }

        FUNCTION_ENTRY("onButtonTisMessage");

        // Pop up a multi column dialog to select a TIS association
        std::vector<TA_Base_Core::TisMessage> tisMessages =
                                m_databaseAccessor->getAllTisMessages();

        std::list<unsigned long> currentSel;
        /*
        NOTE: Leave current selection as empty, since we can't consistently uniquely identify
        the TIS Message from the config item (if the TIS table has its version updated, then
        the two tables become out of sync, and no direct referential correlation).  So, we
        simply don't have any default selection..
        try
        {
            // Attempt to select the TIS item currently set for this config item
            currentSel.push_back(m_databaseAccessor->getTisMessage(*m_configItem).pKey);
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
        }
        */

        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> headerInfo;
        TA_Base_App::MultipleChangeDialog::HeaderProperties hp;

        hp.label = "Description";
        hp.width = 350;
        headerInfo.push_back(hp);

        // These must match in order for the sorting function to work
        hp.label = "Version";
        hp.width = LVSCW_AUTOSIZE_USEHEADER;
        headerInfo.push_back(hp);       
                

        TA_Base_App::MultipleChangeDialog::ItemsForSelection items;
        for (UINT i = 0; i < tisMessages.size(); i ++)
        {
            std::vector<std::string> rowData;
            rowData.push_back(tisMessages[i].description);

            std::ostringstream ostr;
            ostr << tisMessages[i].tisLibraryVersion;
            rowData.push_back(ostr.str());

            items[tisMessages[i].pKey] = rowData;
        }
 
        TA_Base_App::MultipleChangeDialog mcd(items, SELECT_TIS_TITLE, headerInfo, currentSel, this, true);
        mcd.setSortingFunction(SortTisDescendingVersion, reinterpret_cast<unsigned long>(m_databaseAccessor));

        if (IDOK == mcd.DoModal() && !mcd.getSelectedKeys().empty())
        {
            // A selection has been made, replace current
            setTisMessage(mcd.getSelectedKeys().front());
        }

        FUNCTION_EXIT;
	}


    void PaStationDvaMessageDetailView::onKillFocusEditTisMessage()
    {
        // This field only shows the description, which is not unique throughout system
        // (associated tis message / library keys make it unique)
        /*
        FUNCTION_ENTRY("onKillFocusEditTisMessage");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tisMessageString;
        m_tisMessageEdt.GetWindowText(tisMessageString);

        PaStationDvaMessageDatabaseAccessor::TisKey tisKey = m_databaseAccessor->getTisMessageFromName(tisMessageString);

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (tisKey.tisMessageTag == m_configItem->getTisMessageTag() && 
                tisKey.tisLibrarySection == m_configItem->getTisLibrarySection() &&
                tisKey.tisLibraryVersion == m_configItem->getTisLibraryVersion())
            {
                
                m_tisMessageEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTisMessageTag( tisKey.tisMessageTag );
            m_configItem->setTisLibrarySection(tisKey.tisLibrarySection);
            m_configItem->setTisLibraryVersion(tisKey.tisLibraryVersion);

            m_tisMessageEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_tisMessageEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;       
        */ 
    }


    /*
    void PaStationDvaMessageDetailView::onKillFocusEditTisMessageTag()
    {
        FUNCTION_ENTRY("onKillFocusEditTisMessageTag");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tisMessageTagString;
        m_tisMessageTagEdt.GetWindowText(tisMessageTagString);

        TA_Base_App::FieldValidityChecker fvc;
        if (tisMessageTagString.GetLength() > 0 &&
            !fvc.isValidInteger(tisMessageTagString.GetBuffer(0)))
        {
            m_tisMessageTagEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        unsigned long tisMessageTag = (unsigned long)atol((LPCTSTR)tisMessageTagString);

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (tisMessageTag == m_configItem->getTisMessageTag())
            {
                
                m_tisMessageTagEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTisMessageTag( tisMessageTag );
            m_tisMessageTagEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_tisMessageTagEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }
              
                        
    void PaStationDvaMessageDetailView::onKillFocusEditTisLibrarySection()
    {
        FUNCTION_ENTRY("onKillFocusEditTisLibrarySection");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tisLibrarySectionString;
        m_tisLibrarySectionEdt.GetWindowText(tisLibrarySectionString);

        TA_Base_App::FieldValidityChecker fvc;
        if (tisLibrarySectionString.GetLength() > 0 &&  
            !fvc.isValidInteger(tisLibrarySectionString.GetBuffer(0)))
        {
            m_tisLibrarySectionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        unsigned long tisLibrarySection = (unsigned long)atol((LPCTSTR)tisLibrarySectionString);

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (tisLibrarySection == m_configItem->getTisLibrarySection())
            {
                
                m_tisLibrarySectionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTisLibrarySection( tisLibrarySection );
            m_tisLibrarySectionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_tisLibrarySectionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDetailView::onKillFocusEditTisLibraryVersion()
    {
        FUNCTION_ENTRY("onKillFocusEditTisLibraryVersion");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString tisLibraryVersionString;
        m_tisLibraryVersionEdt.GetWindowText(tisLibraryVersionString);

        TA_Base_App::FieldValidityChecker fvc;
        if (tisLibraryVersionString.GetLength() > 0 && 
            !fvc.isValidInteger(tisLibraryVersionString.GetBuffer(0)))
        {
            m_tisLibraryVersionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        unsigned long tisLibraryVersion = (unsigned long)atol((LPCTSTR)tisLibraryVersionString);

        try
        {
            // If the operation string has not changed then there is no point doing an update
            if (tisLibraryVersion == m_configItem->getTisLibraryVersion())
            {
                
                m_tisLibraryVersionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setTisLibraryVersion( tisLibraryVersion );
            m_tisLibraryVersionEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Station DVA Message could not be found in the database");

            m_tisLibraryVersionEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }
    */


    /////////////////////////////////////////////////////////////////////////////
    // PaStationDvaMessageDetailView diagnostics

    #ifdef _DEBUG
    void PaStationDvaMessageDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaStationDvaMessageDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

