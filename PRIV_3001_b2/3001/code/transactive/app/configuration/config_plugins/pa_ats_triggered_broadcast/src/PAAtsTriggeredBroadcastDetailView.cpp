/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an PA ATS Triggered Broadcast's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaAtsTriggeredBroadcast.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
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
    const CString PaAtsTriggeredBroadcastDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";
    const CString PaAtsTriggeredBroadcastDetailView::SELECT_ZONE_GROUP_TITLE = "Select Zone Group";
    const CString PaAtsTriggeredBroadcastDetailView::SELECT_PLATFORM_ID_TITLE = "Select Platform ID";
    const CString PaAtsTriggeredBroadcastDetailView::SELECT_DVA_MESSAGE_TITLE = "Select DVA Message";
    const CString PaAtsTriggeredBroadcastDetailView::SELECT_EVENT_TYPE_TITLE = "Select Event Type";
 
    const int PaAtsTriggeredBroadcastDetailView::LOCATION_FIELD_LENGTH           = 40;

    IMPLEMENT_DYNCREATE(PaAtsTriggeredBroadcastDetailView, CFormView)

    PaAtsTriggeredBroadcastDetailView::PaAtsTriggeredBroadcastDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(PaAtsTriggeredBroadcastDetailView::IDD),
        m_zoneGroupEdt(true),
        m_platformIdEdt(true),
        m_dvaMessageEdt(true),
        m_eventTypeEdt(true)
    {
	    //{{AFX_DATA_INIT(PaAtsTriggeredBroadcastDetailView)
	    //}}AFX_DATA_INIT
    }      

    PaAtsTriggeredBroadcastDetailView::~PaAtsTriggeredBroadcastDetailView()
    {
    }


    void PaAtsTriggeredBroadcastDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PaAtsTriggeredBroadcastDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);

    	DDX_Control(pDX, IDC_EDIT_LOCATION, m_locationEdt);    	
        DDX_Control(pDX, IDC_EDIT_DEST_LOCATION, m_destLocationEdt);
    	DDX_Control(pDX, IDC_EDIT_ZONE_GROUP, m_zoneGroupEdt);
    	DDX_Control(pDX, IDC_EDIT_PLATFORM_ID, m_platformIdEdt);        
        
        DDX_Control(pDX, IDC_EDIT_STATION_DVA_MESSAGE, m_dvaMessageEdt);        
        DDX_Control(pDX, IDC_EDIT_DVA_VERSION, m_dvaVersionEdt);        
        DDX_Control(pDX, IDC_EDIT_EVENT_TYPE, m_eventTypeEdt);        
        
        DDX_Control(pDX, IDC_CHECK_LAST_TRAIN, m_isLastTrainCheck);
        DDX_Control(pDX, IDC_CHECK_IS_ENABLED, m_isEnabledCheck);

        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);        

        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_transparentImageLocation);
        DDX_Control(pDX, IDC_DEST_LOCATION_BITMAP, m_transparentImageDestLocation);
    	DDX_Control(pDX, IDC_ZONE_GROUP_BITMAP, m_transparentImageZoneGroup);    	
        DDX_Control(pDX, IDC_PLATFORM_ID_BITMAP, m_transparentImagePlatformId);
    	DDX_Control(pDX, IDC_DVA_MESSAGE_BITMAP, m_transparentImageDvaMessage);
    	DDX_Control(pDX, IDC_DVA_VERSION_BITMAP, m_transparentImageDvaVersion);
    	DDX_Control(pDX, IDC_EVENT_TYPE_BITMAP, m_transparentImageEventType);
    	
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PaAtsTriggeredBroadcastDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PaAtsTriggeredBroadcastDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)

    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
    ON_BN_CLICKED(IDC_BUTTON_DEST_LOCATION, onButtonDestinationLocation)
    ON_BN_CLICKED(IDC_BUTTON_ZONE_GROUP, onButtonZoneGroup)
    ON_BN_CLICKED(IDC_BUTTON_PLATFORM_ID, onButtonPlatformId)
    ON_BN_CLICKED(IDC_BUTTON_STATION_DVA_MESSAGE, onButtonStationDvaMessage)
    ON_BN_CLICKED(IDC_BUTTON_EVENT_TYPE, onButtonEventType)

    ON_BN_CLICKED(IDC_CHECK_LAST_TRAIN, onButtonIsLastTrain)
    ON_BN_CLICKED(IDC_CHECK_IS_ENABLED, onButtonIsEnabled)
    
	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, onKillfocusEditLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_DEST_LOCATION, onKillfocusEditDestLocation)    

    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    void PaAtsTriggeredBroadcastDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_destLocationEdt.SetLimitText(LOCATION_FIELD_LENGTH);        

        // Hide all the PA ATS Triggered Broadcast details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void PaAtsTriggeredBroadcastDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating zone list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PaAtsTriggeredBroadcastDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PaAtsTriggeredBroadcastListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PaAtsTriggeredBroadcastDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigPaAtsTriggeredBroadcast*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {   
            PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord record =
                            m_databaseAccessor->getRecordFrom(*m_configItem);

            //  Now populate all the fields with the data retrieved from the IConfigPaAtsTriggeredBroadcast* object
            CString buffer;
            m_locationEdt.SetWindowText((LPCTSTR)record.location);
            m_locationEdt.setTextType(m_locationEdt.GetWindowTextLength() > 0 ? 
                                TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
            m_destLocationEdt.SetWindowText((LPCTSTR)record.destLocation);
            m_destLocationEdt.setTextType(m_destLocationEdt.GetWindowTextLength() > 0 ? 
                                TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
            
            //m_zoneGroupEdt.SetWindowText((LPCTSTR)record.zoneGroupName);            
            refreshZoneGroupEdit();
            //m_platformIdEdt.SetWindowText((LPCTSTR)record.platformId);
            refreshPlatformIdEdit();

            //m_dvaMessageEdt.SetWindowText((LPCTSTR)record.dvaMessage);
            //m_dvaVersionEdt.SetWindowText((LPCTSTR)record.dvaMessageVersion);
            refreshDvaMessageEdit();

            //m_eventTypeEdt.SetWindowText((LPCTSTR)record.eventType);
            refreshEventTypeEdit();

            m_isLastTrainCheck.SetCheck(
                m_configItem->getIsLastTrain() ? BST_CHECKED : BST_UNCHECKED);
            
            m_isEnabledCheck.SetCheck(
                m_configItem->getIsEnabledParameter() ? BST_CHECKED : BST_UNCHECKED);            

            if( m_configItem->isNew() )
            {
                // This is a new PA ATS Triggered Broadcast or at least a copy of an PA ATS Triggered Broadcast so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve PA ATS Triggered Broadcast details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve PA ATS Triggered Broadcast details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "PA ATS Triggered Broadcast";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastDetailView::refreshButtonAvailability()
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


    void PaAtsTriggeredBroadcastDetailView::onButtonLocation() 
    {        
        FUNCTION_ENTRY("onButtonLocation");
	    
        if (displayLocationDialog(m_locationEdt))
        {
            onLocationChanged();
        }
    }

    void PaAtsTriggeredBroadcastDetailView::onButtonDestinationLocation()
    {        
        displayLocationDialog(m_destLocationEdt);
    }


    bool PaAtsTriggeredBroadcastDetailView::displayLocationDialog(TA_Base_Bus::TextColourEdit& locationEdit)
    {
        FUNCTION_ENTRY("displayLocationDialog");

        // Retrieve the current location this PA ATS Triggered Broadcast is using
        CString currentLocation("");
        locationEdit.GetWindowText( currentLocation );

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
            return false;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new location which is %s", dlg.getSelectedName());

        bool changesMade = false;

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentLocation != dlg.getSelectedName())
            {
                // Special case handling for config item update
                if (&locationEdit == &m_locationEdt)
                {
                    m_configItem->setLocationKey( dlg.getSelectedKey() );
                }
                else if (&locationEdit == &m_destLocationEdt)
                {
                    m_configItem->setDestinationLocationKey( dlg.getSelectedKey() );
                }
                else
                {
                    TA_ASSERT(FALSE, "Invalid control specified");
                }

                changesMade = true;

                // Now update the entity type the details view is showing
                locationEdit.SetWindowText( dlg.getSelectedName() );
                locationEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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

        return changesMade;
    }

    void PaAtsTriggeredBroadcastDetailView::onButtonZoneGroup()
    {
        // Display zone group selection, location specific
        doSelectionDialog(PaAtsTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP, true);        
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonPlatformId()
    {
        // Display platform list, location specific
        doSelectionDialog(PaAtsTriggeredBroadcastDatabaseAccessor::DATA_PLATFORM_ID, true);        
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonStationDvaMessage()
    {
        // Display list of messages for selection, location specific
        doSelectionDialog(PaAtsTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE, true);        
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonEventType()
    {
        // Display list of events to choose from
        doSelectionDialog(PaAtsTriggeredBroadcastDatabaseAccessor::DATA_EVENT_TYPE, false);        
    }


    void PaAtsTriggeredBroadcastDetailView::doSelectionDialog
    (
        PaAtsTriggeredBroadcastDatabaseAccessor::EDataSet dataSet,
        bool filterBySourceLocation
    )
    {
        TA_ASSERT(0 != m_databaseAccessor, "DatabaseAccessor is null");
        TA_ASSERT(0 != m_configItem, "ConfigItem is null");
        
        unsigned long locationKey = TA_Base_Core::DatabaseKey::getInvalidKey();
        if (filterBySourceLocation)
        {
            locationKey = m_configItem->getLocationKey();
        }
        TA_Base_App::MultipleChangeDialog::ItemsForSelection selectionData = 
                        m_databaseAccessor->getDataForSelection(dataSet, locationKey);
        
        // Now by default we should select the item that is currently set
        // for this config item
        unsigned long currentKey = -1;
        CString windowTitle = "";

        switch (dataSet)
        {
        case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP:
            currentKey = m_configItem->getPaZoneGroupKey();
            windowTitle = SELECT_ZONE_GROUP_TITLE;
            break;
        case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_PLATFORM_ID:
            currentKey = m_configItem->getPlatformId();
            windowTitle = SELECT_PLATFORM_ID_TITLE;
            break;
        case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE:
            currentKey = m_configItem->getPaDvaMessageKey();
            windowTitle = SELECT_DVA_MESSAGE_TITLE;
            break;
        case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_EVENT_TYPE:
            currentKey = m_configItem->getAtsEventType();
            windowTitle = SELECT_EVENT_TYPE_TITLE;
            break;
        default:
            TA_ASSERT(FALSE, "Unrecognized enum input");
        };

        std::list<unsigned long> initialSel;
        if (TA_Base_Core::DatabaseKey::isValidKey(currentKey))
        {
            initialSel.push_back(currentKey);
        }
    
        bool singleItemSelection = true;

        TA_Base_App::MultipleChangeDialog dlg(selectionData, 
                                    windowTitle, 
                                    m_databaseAccessor->getDataSetHeaderProperties(dataSet),
                                    initialSel, 
                                    this,
                                    singleItemSelection);
        
        if (IDOK == dlg.DoModal() && 1 == dlg.getSelectedKeys().size())
        {                     
            // Go ahead and update the relevant text field with the new key
            unsigned long newKeyValue = dlg.getSelectedKeys().front();
            
            bool changesMade = false;            

            switch (dataSet)
            {
            case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_ZONE_GROUP: 
                if (m_configItem->getPaZoneGroupKey() != newKeyValue)
                {
                    changesMade = true;
                    m_configItem->setPaZoneGroupKey(newKeyValue);
                    refreshZoneGroupEdit();     
                }
                break;
            case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_PLATFORM_ID:
                if (m_configItem->getPlatformId() != newKeyValue)
                {
                    changesMade = true;
                    m_configItem->setPlatformId(newKeyValue);
                    refreshPlatformIdEdit();
                }
                break;
            case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_DVA_MESSAGE:
                {
                    // Must update the version to reflect latest available version
                    std::string newVersion = m_databaseAccessor->getLatestVersionForMessage(newKeyValue);
                    
                    if (m_configItem->getPaDvaMessageKey() != newKeyValue ||
                        m_configItem->getDvaMessageVersion() != newVersion)
                    {
                        changesMade = true;
                        m_configItem->setPaDvaMessageKey(newKeyValue);
                        m_configItem->setDvaMessageVersion(newVersion);
                        
                        refreshDvaMessageEdit();
                    }
                }
                break;
            case PaAtsTriggeredBroadcastDatabaseAccessor::DATA_EVENT_TYPE:                
                if (m_configItem->getAtsEventType() != newKeyValue)
                {
                    changesMade = true;
                    m_configItem->setAtsEventType(newKeyValue);
                    refreshEventTypeEdit();
                }
                break;           
            default:
                TA_ASSERT(FALSE, "Unrecognized enum input");
            }
            
            // Refresh required
            if (changesMade)
            {
                m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
                refreshButtonAvailability();
            }
        }        
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonIsLastTrain()
    {
        TA_ASSERT(m_configItem != NULL, "Config item null");
        
        if (m_isLastTrainCheck.GetCheck() & BST_CHECKED)
        {
            m_configItem->setIsLastTrain(true);
        }
        else
        {
            m_configItem->setIsLastTrain(false);
        }

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonIsEnabled()
    {    
        TA_ASSERT(m_configItem != NULL, "Config item null");
        
        if (m_isEnabledCheck.GetCheck() & BST_CHECKED)
        {
            m_configItem->setIsEnabledParameter(true);
        }
        else
        {
            m_configItem->setIsEnabledParameter(false);
        }

        // Is Event Group status changed
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();
    }


    void PaAtsTriggeredBroadcastDetailView::onKillfocusEditLocation() 
    {
        if (onKillfocusLocation(m_locationEdt))
        {
            onLocationChanged();
        }
    }


    void PaAtsTriggeredBroadcastDetailView::onKillfocusEditDestLocation() 
    {
        onKillfocusLocation(m_destLocationEdt);
    }


    bool PaAtsTriggeredBroadcastDetailView::onKillfocusLocation(TA_Base_Bus::TextColourEdit& editControl)
    {
        FUNCTION_ENTRY("onKillfocusEditLocation");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return false;
        }

        CString locationString;
        editControl.GetWindowText(locationString);
        unsigned long locationKey = m_databaseAccessor->getLocationKey((LPCTSTR)locationString);

        if (locationKey == -1)
        {
            editControl.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return false;
        }
        
        bool changesAccepted = false;

        try
        {            
            // Special case handling for config item update
            if (&editControl == &m_locationEdt)
            {
                // If the location has not changed then there is no point doing an update
                if ( locationKey == m_configItem->getLocationKey())
                {
                    editControl.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                    FUNCTION_EXIT;
                    return false;
                }

                m_configItem->setLocationKey( locationKey );
            }
            else if (&editControl == &m_destLocationEdt)
            {
                // If the location has not changed then there is no point doing an update
                if ( locationKey == m_configItem->getDestinationLocationKey())
                {
                    editControl.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                    FUNCTION_EXIT;
                    return false;
                }

                m_configItem->setDestinationLocationKey( locationKey );
            }
            else
            {
                TA_ASSERT(FALSE, "Invalid control specified");
            }

            editControl.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            changesAccepted = true;            
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The ATS Triggered Broadcast could not be found in the database");

            editControl.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return false;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
        return changesAccepted;
    }
    

    void PaAtsTriggeredBroadcastDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastDetailView::checkDvaMessageVersionOnApply()
    {   
        unsigned long currentDvaMessageKey = m_configItem->getPaDvaMessageKey();
        
        if (TA_Base_Core::DatabaseKey::isInvalidKey(currentDvaMessageKey))
        {
            // If no suitable DVA Message set, can skip this check
            return;
        }

        std::string currentVersion = m_databaseAccessor->getLatestVersionForMessage(currentDvaMessageKey);

        if (m_configItem->getDvaMessageVersion() != currentVersion)
        {
			// TD14164 ++
            /*std::ostringstream msg;
            msg << "Current DVA Message Version: " << m_configItem->getDvaMessageVersion() << std::endl;
            msg << "Latest DVA Message Version: " << currentVersion << std::endl;
            msg << "Do you want to automatically update this ATS records DVA Message Version"
                << " to reflect the most up to date DVA Message Version?";*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << m_configItem->getDvaMessageVersion();
			userMsg << currentVersion;
			UINT selectedButton = userMsg.showMsgBox(IDS_UW_610022);

            /*if (IDYES == MessageBox(msg.str().c_str(), "ATS Triggered Broadcast: Warning", MB_YESNO))*/
			if (IDYES == selectedButton)
			// ++ TD14164 
            {
                // Update the DVA Version internally for the item
                m_configItem->setDvaMessageVersion(currentVersion);

                // Refresh the contents of detail view having made the change
                refreshDvaMessageEdit();
                m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
                refreshButtonAvailability();
            }
        }
    }


    void PaAtsTriggeredBroadcastDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        checkDvaMessageVersionOnApply();
        
        if ( !m_locationEdt.isTextValid()       || 
             !m_destLocationEdt.isTextValid()   ||
             !m_zoneGroupEdt.isTextValid()      ||
             !m_platformIdEdt.isTextValid()     ||
             !m_dvaMessageEdt.isTextValid()     ||
             !m_eventTypeEdt.isTextValid())
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


    void PaAtsTriggeredBroadcastDetailView::onLocationChanged()
    {   
        // Whenever the operator changes the location, it may affect
        // validity of the zone list associations - so refresh their status
        // (OCC terminal can view all, stations can only associate with local zones)
        CWaitCursor wait;

        // Refresh those text fields whose validity is location dependent
        refreshZoneGroupEdit();
        refreshPlatformIdEdit();
        refreshDvaMessageEdit();
    }

    /////////////////////////////////////////////////////////////////////////////
    // PaAtsTriggeredBroadcastDetailView diagnostics

    #ifdef _DEBUG
    void PaAtsTriggeredBroadcastDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void PaAtsTriggeredBroadcastDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

    
    void PaAtsTriggeredBroadcastDetailView::refreshZoneGroupEdit()
    {
        unsigned long key = m_configItem->getPaZoneGroupKey();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_zoneGroupEdt.SetWindowText("");
            m_zoneGroupEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {
            unsigned long locKey;

            // Use this key 
            m_zoneGroupEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getZoneGroupName(key, &locKey) );
            m_zoneGroupEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                                            TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
        }
    }
        
    void PaAtsTriggeredBroadcastDetailView::refreshPlatformIdEdit()
    {
        unsigned long key = m_configItem->getPlatformId();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_platformIdEdt.SetWindowText("");
            m_platformIdEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {
            unsigned long locKey;

            // Use this key 
            m_platformIdEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getPlatformName(key, &locKey) );

            m_platformIdEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                                            TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);
        }
    }

    void PaAtsTriggeredBroadcastDetailView::refreshDvaMessageEdit()
    {    
        unsigned long key = m_configItem->getPaDvaMessageKey();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_dvaMessageEdt.SetWindowText("");
            m_dvaMessageEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {   
            unsigned long locKey;
            
            // Use this key 
            m_dvaMessageEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getDvaMessageName(key, &locKey));
            // Only valid if key of dva message matches local key
            m_dvaMessageEdt.setTextType(m_configItem->getLocationKey() == locKey ?
                                            TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID);                                       
        }

        // Additionally must update the DVA version associated with this DVA Message
        m_dvaVersionEdt.SetWindowText((LPCTSTR) m_configItem->getDvaMessageVersion().c_str());        
    }


    void PaAtsTriggeredBroadcastDetailView::refreshEventTypeEdit()
    {        
        unsigned long key = m_configItem->getAtsEventType();

        if (TA_Base_Core::DatabaseKey::isInvalidKey(key))
        {
            m_eventTypeEdt.SetWindowText("");
            m_eventTypeEdt.setTextType(TA_Base_Bus::TextColourEdit::INVALID);
        }
        else
        {
            // Use this key 
            m_eventTypeEdt.SetWindowText((LPCTSTR) m_databaseAccessor->getAtsEventTypeName(key) );
            m_eventTypeEdt.setTextType(TA_Base_Bus::TextColourEdit::VALID);
        }
    }        
}

