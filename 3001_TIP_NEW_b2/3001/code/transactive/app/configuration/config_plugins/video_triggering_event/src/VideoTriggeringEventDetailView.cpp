/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/video_triggering_event/src/VideoTriggeringEventDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an Video Station's details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/data_access_interface/video_switch_agent/src/IConfigVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigVideoTriggeringEvent.h"

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
    const CString VideoTriggeringEventDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Video Station";
	const CString VideoTriggeringEventDetailView::CHANGE_ALARMTYPE_DIALOG_TITLE = "Change Alarm Type";

    const int VideoTriggeringEventDetailView::EVENT_ID_FIELD_LENGTH     = 8;
    const int VideoTriggeringEventDetailView::LOCATION_FIELD_LENGTH     = 30;
    const int VideoTriggeringEventDetailView::TRIGGERING_FIELD_LENGTH   = 70;
	const int VideoTriggeringEventDetailView::ALARM_FIELD_LENGTH   = 30;

    IMPLEMENT_DYNCREATE(VideoTriggeringEventDetailView, CFormView)


    VideoTriggeringEventDetailView::VideoTriggeringEventDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(VideoTriggeringEventDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(VideoTriggeringEventDetailView)
	//}}AFX_DATA_INIT
    }


    VideoTriggeringEventDetailView::~VideoTriggeringEventDetailView()
    {
    }


    void VideoTriggeringEventDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(VideoTriggeringEventDetailView)
    	DDX_Control(pDX, IDC_BUTTON_APPLY,      m_applyBtn);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);

    	DDX_Control(pDX, IDC_STATIC_TYPE,       m_type);

        DDX_Control(pDX, IDC_EDIT_STATION_ID,   m_stationIdEdt);
        DDX_Control(pDX, IDC_EDIT_EVENT_ID,     m_eventIdEdt);
    	DDX_Control(pDX, IDC_EDIT_LOCATION,     m_locationEdt);
        DDX_Control(pDX, IDC_EDIT_TRIGGERING,   m_triggeringEdt);
		DDX_Control(pDX, IDC_EDIT_ALARM,		m_alarmEdt);

        DDX_Control(pDX, IDC_MANUAL_CLOSING,    m_manuallyClosedChk);
    	
        DDX_Control(pDX, IDC_STATION_ID_BITMAP, m_transparentImageId);
        DDX_Control(pDX, IDC_EVENTID_BITMAP,    m_transparentImageEvent);
        DDX_Control(pDX, IDC_LOCATION_BITMAP,   m_transparentImageLocation);
        DDX_Control(pDX, IDC_TRIGGERING_BITMAP, m_transparentImageTriggering);
		DDX_Control(pDX, IDC_ALARM_BITMAP,		m_transparentImageAlarm);
    	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(VideoTriggeringEventDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(VideoTriggeringEventDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)

    ON_BN_CLICKED(IDC_MANUAL_CLOSING, onButtonManualClosing)
    ON_BN_CLICKED(IDC_BUTTON_VIDEO_LOCATION, onButtonStation)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ENTITY, onButtonAlarmEntity)

    ON_EN_KILLFOCUS(IDC_EDIT_STATION_ID,    onKillFocusEditStationId)
	ON_EN_KILLFOCUS(IDC_EDIT_EVENT_ID,      onKillFocusEditEventId)
    ON_EN_KILLFOCUS(IDC_EDIT_LOCATION,      onKillFocusEditLocation)
	ON_EN_KILLFOCUS(IDC_EDIT_TRIGGERING,    onKillFocusEditTriggering)
	ON_EN_KILLFOCUS(IDC_EDIT_ALARM,			onKillFocusEditAlarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void VideoTriggeringEventDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

		CFormView::OnInitialUpdate();
	    
        m_eventIdEdt.SetLimitText(EVENT_ID_FIELD_LENGTH);
        m_locationEdt.SetLimitText(LOCATION_FIELD_LENGTH);
        m_triggeringEdt.SetLimitText(TRIGGERING_FIELD_LENGTH);
		m_alarmEdt.SetLimitText(ALARM_FIELD_LENGTH);
          	
        // Hide all the Video Station details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::showItemDetails(bool isShown)
    {        
        // Show or hide all the controls from the view
        CWnd* nextChild = GetWindow(GW_CHILD);

        while (0 != nextChild)
        {
            nextChild->ShowWindow(isShown);
    
            nextChild = nextChild->GetWindow(GW_HWNDNEXT);
        }
    }


    void VideoTriggeringEventDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        // This can take a while sometimes (mainly populating station list)..
        CWaitCursor wait;

        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<VideoTriggeringEventDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<VideoTriggeringEventListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper VideoTriggeringEventDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigVideoTriggeringEvent*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }


        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigVideoTriggeringEventId* object
            CString buffer;

            //
            // Populate station Id
            //

            // get the station id
            unsigned long stationId = m_configItem->getStationId();
            // get the location name for the station id
            std::string locationName = m_databaseAccessor->getLocationName(stationId);
            m_stationIdEdt.SetWindowText(locationName.c_str());
            // if it isnt found - it returns an empty string
            if (locationName == "")
            {
                // set the item to invalid
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

            //
            // Populate event id
            //
            unsigned long eventId = m_configItem->getEventId();
            if (eventId != -1)
            {
                buffer.Format("%ld", eventId);
                m_eventIdEdt.SetWindowText(buffer);
                m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_eventIdEdt.SetWindowText("");
                m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            

            //
            // Populate manually closed
            //
            m_manuallyClosedChk.SetCheck(m_configItem->getManualClosing() ? 1 : 0);


            //
            // Populate location
            //
            std::string location = m_configItem->getLocation();
            m_locationEdt.SetWindowText(location.c_str());
            if (location == "")
            {
                // set the item to invalid
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }


            //
            // Populate CCTV Triggering
            //
            std::string triggering = m_configItem->getCctvTriggering();
            m_triggeringEdt.SetWindowText(triggering.c_str());
            if (triggering == "")
            {
                // set the item to invalid
                m_triggeringEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                m_triggeringEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }

			//
			// Populate Alarm Type - TD15848
			//
			std::string alarmType = m_databaseAccessor->getAlarmTypeName(m_configItem->getEntityKey());
			m_alarmEdt.SetWindowText(alarmType.c_str());
			if (alarmType == "")
			{
				// set the item to invalid
				m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
			}
			else
			{
				m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
			}


            //
            // Validate Uniqueness
            //

            // check the (stationId, eventId) pair for duplicates
/*            unsigned int pkey = -1;
            if (!m_configItem->isNew())
            {
                // set the pkey to exclude if this item exists
                pkey = m_configItem->getKey();
            }

            if ( TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().pairExists(
                    m_configItem->getStationId(),
                    m_configItem->getEventId(),
                    pkey ) )
            {
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
*/

            //
            // Created and modified dates
            //


            if( m_configItem->isNew() )
            {
                // This is a new triggering event or at least a copy of an existing one so we indicate this in the type
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Triggering event details to display");
            
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Triggering event details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Triggering event";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164 
        }

        FUNCTION_EXIT;
    }
    

    void VideoTriggeringEventDetailView::refreshButtonAvailability()
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


    void VideoTriggeringEventDetailView::onKillFocusEditStationId() 
    {
        FUNCTION_ENTRY("onKillFocusEditStationId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        // Get the station name from the field
        CString stationString;
        m_stationIdEdt.GetWindowText(stationString);

        // get the video station id from the name
        unsigned long stationId = m_databaseAccessor->getStationId(stationString.GetBuffer(0));

        // if its not valid (-1)
        if (stationId == -1)
        {
            // set it to invalid and return
            m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        // its a valid id - so set it in the db class
        m_configItem->setStationId( stationId );
        m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

        // check the (stationId, eventId) pair for duplicates
        try
        {
            unsigned int pkey = -1;
            if (!m_configItem->isNew())
            {
                // set the pkey to exclude if this item exists
                pkey = m_configItem->getKey();
            }

            if ( TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().pairExists(
                    stationId,
                    m_configItem->getEventId(),
                    pkey ) )
            {
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                // before setting event id to valid - validate the field
                CString eventIdString;
                m_eventIdEdt.GetWindowText(eventIdString);

                TA_Base_App::FieldValidityChecker fvc;
                if ( (eventIdString.GetLength() > 0) &&
                     fvc.isValidInteger(eventIdString.GetBuffer(0)) )
                {
                    m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                }
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","Error validating station id");

            m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onKillFocusEditEventId() 
    {
        FUNCTION_ENTRY("onKillFocusEditEventId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        // Get the event id from the field
        CString eventString;
        m_eventIdEdt.GetWindowText(eventString);

        // make sure its a valid number
        TA_Base_App::FieldValidityChecker fvc;
        if (0 == eventString.GetLength() || !fvc.isValidInteger(eventString.GetBuffer(0)))
        {
            // Must enter a valid (non-empty) key value
            m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        unsigned long eventId = atol(eventString.GetBuffer(0));

        // its a valid event Id so set it in the database object
        m_configItem->setEventId( eventId );
        m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

        // check the (stationId, eventId) pair for duplicates
        try
        {
            unsigned int pkey = -1;
            if (!m_configItem->isNew())
            {
                // set the pkey to exclude if this item exists
                pkey = m_configItem->getKey();
            }

            if ( TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().pairExists(
                    m_configItem->getStationId(),
                    eventId,
                    pkey ) )
            {
                m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            }
            else
            {
                // before setting station id to valid - validate the field

                // Get the station name from the field
                CString stationString;
                m_stationIdEdt.GetWindowText(stationString);

                // get the video station id from the name
                unsigned long stationId = m_databaseAccessor->getStationId(stationString.GetBuffer(0));

                // if its not -1
                if (stationId != -1)
                {
                    // set it to valid
                    m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                }
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","Error validating station id");

            m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onKillFocusEditLocation() 
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

        if (locationString == "")
        {
            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the location string has not changed then there is no point doing an update
            if ( locationString == m_configItem->getLocation().c_str() )
            {
                FUNCTION_EXIT;
                return;
            }

            // update the location in the db object
            m_configItem->setLocation( locationString.GetBuffer(0) );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The triggering event could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

        m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onKillFocusEditTriggering() 
    {
        FUNCTION_ENTRY("onKillFocusEditTriggering");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString triggeringString;
        m_triggeringEdt.GetWindowText(triggeringString);

        if (triggeringString == "")
        {
            m_triggeringEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the string has not changed then there is no point doing an update
            if ( triggeringString == m_configItem->getCctvTriggering().c_str() )
            {
                FUNCTION_EXIT;
                return;
            }

            // update the string in the db object
            m_configItem->setCctvTriggering( triggeringString.GetBuffer(0) );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The triggering event could not be found in the database");

            m_locationEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

        m_triggeringEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onKillFocusEditAlarm() 
    {
        FUNCTION_ENTRY("onKillFocusEditAlarm");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString alarmTypeStr;
        m_alarmEdt.GetWindowText(alarmTypeStr);

        if (alarmTypeStr == "")
        {
            m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            return;
        }

        try
        {
            // If the string has not changed then there is no point doing an update
			std::string alarmS = m_databaseAccessor->getAlarmTypeName(m_configItem->getEntityKey());
            if ( alarmTypeStr == alarmS.c_str() ) 
            {
                FUNCTION_EXIT;
                return;
            }

			unsigned long entityKey = m_databaseAccessor->getAlarmEntityKey(alarmS);
            if (entityKey == -1)
            {
                // set it to invalid and return
                m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                return;
            }

            // update the entitykey in the db object
            m_configItem->setEntityKey(entityKey); 
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The alarm type could not be found in the database");

            m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

        m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onButtonManualClosing() 
    {
        FUNCTION_ENTRY("onButtonManualClosing");

        try
        {
#pragma warning(push)
#pragma warning(disable:4800)
            bool check = m_manuallyClosedChk.GetCheck();
#pragma warning(pop)

            if (check != m_configItem->getManualClosing())
            {
                m_configItem->setManualClosing(check);
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","Error loading triggering event");
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onButtonStation() 
    {
        FUNCTION_ENTRY("onButtonStation");
	    
        // Retrieve the current location this triggering event is using
        CString currentStation("");
        m_stationIdEdt.GetWindowText( currentStation );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllLocations();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_LOCATION_DIALOG_TITLE, currentStation, this);
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

            if (currentStation != dlg.getSelectedName())
            {
                // Now update the details view
                m_stationIdEdt.SetWindowText( dlg.getSelectedName() );


                // get the video station id from the name
                unsigned long stationId = m_databaseAccessor->getStationId(dlg.getSelectedName().GetBuffer(0));

                // if its not valid (-1)
                if (stationId == -1)
                {
                    // set it to invalid and return
                    m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                    return;
                }

                // its a valid id - so set it in the db class
                m_configItem->setStationId( stationId );
                m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // now check for duplicates
                unsigned int pkey = -1;
                if (!m_configItem->isNew())
                {
                    // set the pkey to exclude if this item exists
                    pkey = m_configItem->getKey();
                }

                if ( TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().pairExists(
                        stationId,
                        m_configItem->getEventId(),
                        pkey ) )
                {
                    m_stationIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                    m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                }
                else
                {
                    // before setting event id to valid - validate the field
                    CString eventIdString;
                    m_eventIdEdt.GetWindowText(eventIdString);

                    TA_Base_App::FieldValidityChecker fvc;
                    if ( (eventIdString.GetLength() > 0) &&
                         fvc.isValidInteger(eventIdString.GetBuffer(0)) )
                    {
                        m_eventIdEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                    }
                }
                
    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "The triggering event could not be found in the database");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "The triggering event could not be found in the database");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }

	// TD15848
    void VideoTriggeringEventDetailView::onButtonAlarmEntity() 
    {
        FUNCTION_ENTRY("onButtonAlarmEntity");
	    
        // Retrieve the current location this triggering event is using
        CString currentAlarm("");
        m_alarmEdt.GetWindowText( currentAlarm );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_App::ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAlarmEntities();

        // Now construct the dialog and then wait for the user to finish with it
        TA_Base_App::ChangeDialog dlg(items, CHANGE_ALARMTYPE_DIALOG_TITLE, currentAlarm, this);
        dlg.setSortItems( false );
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of an alarm type");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new alarm type which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_configItem !=  NULL, "m_configItem is NULL.");

            if (currentAlarm != dlg.getSelectedName())
            {
                // Now update the details view
                m_alarmEdt.SetWindowText( dlg.getSelectedName() );


                // get the alarm entity key from the name
                unsigned long entityKey = m_databaseAccessor->getAlarmEntityKey(dlg.getSelectedName().GetBuffer(0));

                // if its not valid (-1)
                if (entityKey == -1)
                {
                    // set it to invalid and return
                    m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
                    return;
                }

                // its a valid id - so set it in the db class
                m_configItem->setEntityKey( entityKey );
                m_alarmEdt.setTextType( TA_Base_Bus::TextColourEdit::VALID );

    	        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "The triggering event could not be found in the database");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "The triggering event could not be found in the database");
        }

        refreshButtonAvailability();
        FUNCTION_EXIT;
    }
	// TD15848


    void VideoTriggeringEventDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        if ( !m_stationIdEdt.isTextValid() ||
             !m_eventIdEdt.isTextValid() ||
             !m_locationEdt.isTextValid() ||
             !m_triggeringEdt.isTextValid() )
        {
            //UE-030060
            /*MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);*/
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "invalid names still exist in the fields";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164 
            return;
        }

		// TD15848 - add a corresponding OCC triggering event to STATION entity
		TA_IRS_Core::IConfigVideoTriggeringEvent* evt = 
			m_databaseAccessor->getOccEntity(m_configItem->getEventId(),m_configItem->getStationId());
		// TODO: add implementation if (m_configItem->isNew())
		if (evt != NULL)
		{
			TA_IRS_Core::ConfigVideoTriggeringEvent* configEvent = dynamic_cast<TA_IRS_Core::ConfigVideoTriggeringEvent*>(m_configItem);
			if (configEvent != NULL)
			{
				configEvent->setOccEntity(evt);
			}
		}
		// TD15848
        
        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // VideoTriggeringEventDetailView diagnostics

    #ifdef _DEBUG
    void VideoTriggeringEventDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void VideoTriggeringEventDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

