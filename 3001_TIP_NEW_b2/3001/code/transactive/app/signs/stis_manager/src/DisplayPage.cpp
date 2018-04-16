/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/DisplayPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/05/12 16:16:15 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * The Station display page
  */

#include "stdafx.h"
#include "STISManager.h"
#include "STISManagerDlg.h"
#include "DisplayPage.h"
#include "UserMessages.h"
#include "STISPredefinedMessages.h"
#include "RightsManager.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"
#include "core\data_access_interface\entity_access\src\TISAgentEntityData.h"
#include "core\data_access_interface\entity_access\src\STISEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "RunThreadedFunc.h"
#include "boost/bind.hpp"
#include "boost/tokenizer.hpp"
#include <iomanip>
#include "helperfun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;


namespace TA_IRS_App
{    
    static const DWORD COLOR_RED = RGB(255, 0, 0);

    DisplayPage::DisplayPage(CWnd* pParent /*=NULL*/)
        : CDialog(DisplayPage::IDD, pParent),
          m_validMessageSelected(false),
          m_canDisplay(false),
          m_canClear(false),
          m_canModifyPidGroups(false),
          m_pidSelectionManager(NULL),
          m_timeControlManager(NULL),
		  m_stisPredefinedMessages(NULL),
		  m_canDisplayAdhoc(false),
		  m_canDisplayPredefined(false)
    {
        FUNCTION_ENTRY( "DisplayPage" );

        // {{AFX_DATA_INIT(DisplayPage)
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    DisplayPage::~DisplayPage()
    {
        FUNCTION_ENTRY( "~DisplayPage" );

		if(NULL != m_stisPredefinedMessages)
		{
			m_stisPredefinedMessages->removeInstance();
			m_stisPredefinedMessages = NULL;
		}
        FUNCTION_EXIT;
    }


    void DisplayPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(DisplayPage)
        DDX_Control(pDX, IDC_DISPLAY, m_displayButton);
        DDX_Control(pDX, IDC_CLEAR_NORMAL, m_clearNormalButton);
        DDX_Control(pDX, IDC_CLEAR_EMERGENCY, m_clearEmergencyButton);
        DDX_Control(pDX, IDC_CLEAR_ALL, m_clearAllButton);
        DDX_Control(pDX, IDC_MESSAGE_TYPE_TAB, m_messageTypeTab);
        DDX_Control(pDX, IDC_PRIORITY, m_priority);
        DDX_Control(pDX, IDC_RADIO_TIMED, m_radioTimed);
        DDX_Control(pDX, IDC_RADIO_CONTINUOUS, m_radioContinuous);
        DDX_Control(pDX, IDC_START_DATE, m_startDate);
        DDX_Control(pDX, IDC_START_TIME, m_startTime);
        DDX_Control(pDX, IDC_END_DATE, m_endDate);
        DDX_Control(pDX, IDC_END_TIME, m_endTime);
        DDX_Control(pDX, IDC_EDIT_REPEAT_INTERVAL, m_repeatInterval);
        DDX_Control(pDX, IDC_SPIN_REPEAT_INTERVAL, m_repeatIntervalSpin);
        DDX_Control(pDX, IDC_START_TIME_LABEL, m_startTimeLabel);
        DDX_Control(pDX, IDC_REPEAT_LABEL, m_repeatLabel);
        DDX_Control(pDX, IDC_END_TIME_LABEL, m_endTimeLabel);
        DDX_Control(pDX, IDC_PID_LIST, m_PIDList);
        DDX_Control(pDX, IDC_STN_PID_GROUP_COMBO, m_pidGroupCombo);
        DDX_Control(pDX, IDC_DELETE_GROUP, m_deleteGroupButton);
        DDX_Control(pDX, IDC_SAVE_GROUP, m_saveGroupButton);		
    // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(DisplayPage, CDialog)
        // {{AFX_MSG_MAP(DisplayPage)
        ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
        ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
        ON_BN_CLICKED(IDC_CLEAR_NORMAL, OnClearNormal)
        ON_BN_CLICKED(IDC_CLEAR_EMERGENCY, OnClearEmergency)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
		ON_EN_KILLFOCUS(IDC_EDIT_REPEAT_INTERVAL, OnKillFocusRepeatInterval)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL DisplayPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();
		m_repeatInterval.setBunndySpinCtrl(&m_repeatIntervalSpin);
        // set up PID selection
        m_pidSelectionManager = new PidSelectionManager(m_PIDList,
                                                        m_pidGroupCombo,
                                                        m_saveGroupButton,
                                                        m_deleteGroupButton,
                                                        *this);
		// Init unsermessages in gui main thread, guard unexpected exception
		UserMessages::getInstance();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


	void DisplayPage::initRightsCheck()
	{
        m_canClear = RightsManager::getInstance().canClear();
		m_canDisplayPredefined = RightsManager::getInstance().canDisplayPredefined();
		m_canDisplayAdhoc = RightsManager::getInstance().canDisplayFreeText();
        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();
		this->PostMessage(WM_UPDATE_RIGHTS);
        RightsManager::getInstance().registerForRightsChanges(this);
	}

    void DisplayPage::InitAll(/*REBProgressManager & mgr*/)
    {
        FUNCTION_ENTRY( "InitAll" );
		runThreadedFunc(boost::bind(&DisplayPage::initRightsCheck, this));
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();// TD11310
        m_pidSelectionManager->Init();
        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);
        m_timeControlManager = new TimeControlManager(m_priority,
                                                      m_radioContinuous,
                                                      m_radioTimed,
                                                      m_startDate,
                                                      m_startTime,
                                                      m_endDate,
                                                      m_endTime,
                                                      m_repeatInterval,
                                                      m_startTimeLabel,
                                                      m_endTimeLabel,
                                                      m_repeatLabel,
                                                      m_messageTypeTab.getTimeTypeLisener());
        m_messageTypeTab.setMessageSelectionListener(this);
        m_messageTypeTab.InitAll();
		// Force dialog repaint
        Invalidate();
		// For test pid select issue
		//TA_Base_Core::RunParams::getInstance().set("Select", "LIN,0,DT18.TIS.ENT.LED01");
        FUNCTION_EXIT;
    }


    void DisplayPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        RightsManager::getInstance().deregisterForRightsChanges(this);

        m_messageTypeTab.setMessageSelectionListener(NULL);

        // clean up PID selection
        if (m_pidSelectionManager != NULL)
        {
            delete m_pidSelectionManager;
            m_pidSelectionManager = NULL;
        }

        // clean up time controls
        if (m_timeControlManager != NULL)
        {
            delete m_timeControlManager;
            m_timeControlManager = NULL;
        }

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    void DisplayPage::pidSelectionChanged(bool pidsInList, bool pidSelectionExists)
    {
        FUNCTION_ENTRY( "pidSelectionChanged" );

        // TES 880 - this was changed from (m_pidSelected && m_canClear)
        // because a clear request now goes to all PIDs in the list
        // you can no longer select individual items in the PID list
		updateClearButtonState();
		updateDisplayButtonState();

        FUNCTION_EXIT;
    }

	void DisplayPage::updateDisplayButtonState()
	{
		bool enable = (m_pidSelectionManager->isPidListEmpty() && m_validMessageSelected && m_canDisplay);
		m_displayButton.EnableWindow(enable);
	}

	void DisplayPage::updateClearButtonState()
	{
		bool enable = (m_pidSelectionManager->isPidListEmpty() && m_canClear);
        m_clearAllButton.EnableWindow(enable);
        m_clearEmergencyButton.EnableWindow(enable);
        m_clearNormalButton.EnableWindow(enable);
	}

    void DisplayPage::predefinedMessageSelected( bool tabSwitched,
                                                 bool validMessageSelected,
                                                 unsigned short priority /* = 0 */,
                                                 unsigned short repeatInterval /* = 0 */)
    {
        FUNCTION_ENTRY( "predefinedMessageSelected" );

        m_validMessageSelected = validMessageSelected;

        TA_ASSERT( m_timeControlManager != NULL, "Time Control manager cant be null");

        // set up the time controls
        if (tabSwitched)
        {
            // check access control
            m_canDisplay = m_canDisplayPredefined; //RightsManager::getInstance().canDisplayPredefined();

            // set up the PID list
            TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");

            m_timeControlManager->enableTimeControls( false );
        }

        if (m_validMessageSelected)
        {
            m_timeControlManager->setPriority( priority, false );
        }
        else
        {
            m_timeControlManager->blankAndDisableTimeAndPriority();

            m_timeControlManager->setPriority( priority, false );
        }

        m_timeControlManager->setRepeatInterval( repeatInterval, false );
		updateDisplayButtonState();

        FUNCTION_EXIT;
    }


    void DisplayPage::adHocMessageSelected( bool tabSwitched,
                                            bool validMessageEntered,
                                            unsigned short repeatInterval )
    {
        FUNCTION_ENTRY( "adHocMessageSelected" );

        m_validMessageSelected = validMessageEntered;

        TA_ASSERT( m_timeControlManager != NULL, "Time Control manager cant be null");

        if (tabSwitched)
        {
            // check access control
            m_canDisplay = m_canDisplayAdhoc; //RightsManager::getInstance().canDisplayFreeText();

            // set up the PID list
            TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");

            // set up the time controls
            m_timeControlManager->enableTimeControls( true );

            // set default adhoc attributes
            m_timeControlManager->setPriority( 4, true );
            m_timeControlManager->setRepeatInterval( repeatInterval, true );
        }

		updateDisplayButtonState();
        FUNCTION_EXIT;
    }



    LRESULT DisplayPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID selection Manager is NULL");

        // the rights have changed - re check them, and reset clearbutton state
        m_canClear = RightsManager::getInstance().canClear();
        updateClearButtonState();

        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Calling setPermissions() from onRightsChanged");
        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Calling setPermissions() successfully");

		// enable or disable 
        // cause a state change to trigger one of the above functions
		HandledDlg* dialog = dynamic_cast<HandledDlg*>(m_messageTypeTab.getActiveDialog());
		UINT dialogResouceId = dialog->getResourceId();
		if(IDD_PREDEFINED == dialogResouceId)
		{
			m_canDisplayPredefined = RightsManager::getInstance().canDisplayPredefined();
            m_canDisplay = m_canDisplayPredefined;
		}
		else if(IDD_FREE_TEXT == dialogResouceId)
		{
			m_canDisplayAdhoc = RightsManager::getInstance().canDisplayFreeText();
			m_canDisplay = m_canDisplayAdhoc;
		}
		updateDisplayButtonState();
        FUNCTION_EXIT;
        return 0;
    }


    void DisplayPage::OnDisplay()
    {
        FUNCTION_ENTRY( "OnDisplay" );

		CWaitCursor wait;
        TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");
        TA_ASSERT( m_timeControlManager != NULL, "Time Control manager cant be null");

        // get the current tab
        int currSel = m_messageTypeTab.GetCurSel();

        // Grab the common user selections
        CTime tempStartTime = m_timeControlManager->getStartTime();
        CTime tempEndTime = m_timeControlManager->getEndTime();
        std::string startTimeString = timeToString(tempStartTime);
        std::string endTimeString = timeToString(tempEndTime);
		unsigned short repeatInterval = 0;

		// get the destination
		TA_Base_Core::STISDestinationList destinationList;
		m_pidSelectionManager->populateDisplayDestination( destinationList );

        // If the message is not an emergency message (ie continuous)
        // ensure that the start and end time are valid
        if( m_timeControlManager->getMessageType() == TimeControlManager::MESSAGE_CYCLIC )
        {
            if( tempStartTime > tempEndTime )
            {
                UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_AFTER_END);

                FUNCTION_EXIT;
                return;
            }
            else if( tempStartTime == tempEndTime )
            {
                UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_EQUALS_END);

                FUNCTION_EXIT;
                return;
            }

            CTime currentTime = CTime::GetCurrentTime();
            if ( currentTime > tempEndTime )
            {
                UserMessages::getInstance().displayError(UserMessages::ERROR_END_TIME_PASSED);

                FUNCTION_EXIT;
                return;
            }
            else if (currentTime > tempStartTime) {
                UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME);

                FUNCTION_EXIT;
                return;
            }

			// check if repeat interval valid
			repeatInterval = m_timeControlManager->getRepeatInterval();
			if(m_timeControlManager->getValidMinRepeatInterval() > repeatInterval || m_timeControlManager->getValidMaxRepeatInterval() < repeatInterval)
			{
	            UserMessages::getInstance().displayError(UserMessages::ERROR_REPEAT_INTERVAL_INVALID);
	            FUNCTION_EXIT;
	            return;
			}
        }


        unsigned short selectedPriority = m_timeControlManager->getPriority();
        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        // If the operator has created a free-text message then set one up, and display the appropriate
        // confirmation message
        switch(currSel)
        {
        case FREE_TEXT:
            {
                

                TA_ASSERT( destinationList.length() != 0,
                           "The display button should not be active if there are no PIDs in the list");


                // get the message
                FreeTextMessage message = m_messageTypeTab.getFreeTextMessage();

                // set repeat interval
                message.displayAttributes.repeatInterval = repeatInterval;
				// for compatible with ttis
				message.ledAttributes.intensity = TA_Base_Core::INTENSITY_LOW;

                TA_ASSERT( message.messageContent.length() > 0,
                           "The display button should not be active if there is no message content" );


                std::string confirmationMessage = str( format(UserMessages::QUESTION_DISPLAY) % "ad-hoc" % message.messageContent );

                // Display the confirmation dialog
				if( IDYES != UserMessages::getInstance().askQuestion(confirmationMessage.c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST))
                {
                    FUNCTION_EXIT;
                    return;
                }

                try
                {
					try
					{
						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitAdHocDisplayRequest,
						 ( destinationList,
                           message.messageContent.c_str(),
                           startTimeString.c_str(),
                           endTimeString.c_str(),
                           selectedPriority,
                           message.displayAttributes,
                           message.lcdAttributes,
                           message.ledAttributes,
                           sessionId.c_str() ) );

					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitAdHocDisplayRequest,
					     ( destinationList,
                           message.messageContent.c_str(),
                           startTimeString.c_str(),
                           endTimeString.c_str(),
                           selectedPriority,
                           message.displayAttributes,
                           message.lcdAttributes,
                           message.ledAttributes,
                           sessionId.c_str() ) );
					}
					
                    UserMessages::getInstance().displayInfo(
                        str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "Free Text Display" ).c_str());
					
                }
				CATCH_ALL_EXCEPTIONS("While submitting Free Text Display request");
            }

	        break;

        case PREDEFINED:
            {
                TA_ASSERT( destinationList.length() != 0,
                           "The display button should not be active if there are no PIDs in the list");


                // get the message
                TA_Base_Core::PredefinedMessage message = m_messageTypeTab.getPredefinedMessage();
                //std::string confirmationMessage = str( format(UserMessages::QUESTION_DISPLAY) % "predefined" % message.message );
				std::wstring confirmationMessage = str( wformat(UserMessages::QUESTION_DISPLAY_W) % L"predefined" % convertUtf8ToUtf16le(message.message) );

                // Display the confirmation dialog
				if( IDYES != UserMessages::getInstance().askQuestion(confirmationMessage.c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST))
                {
                    FUNCTION_EXIT;
                    return;
                }


                try
                {
                    unsigned long loadedLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();
					try
					{

						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitPredefinedDisplayRequest,
						  ( destinationList,
                            static_cast<TA_Base_Core::ELibrarySection>(message.librarySection),
                            loadedLibraryVersion,
                            message.messageTag,
                            startTimeString.c_str(),
                            endTimeString.c_str(),
                            message.priority,
                            sessionId.c_str() ));

					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());

						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitPredefinedDisplayRequest,
						  ( destinationList,
                            static_cast<TA_Base_Core::ELibrarySection>(message.librarySection),
                            loadedLibraryVersion,
                            message.messageTag,
                            startTimeString.c_str(),
                            endTimeString.c_str(),
                            message.priority,
                            sessionId.c_str() ));
					}
                    UserMessages::getInstance().displayInfo(
                        str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "Predefined Display" ).c_str());
                }
				CATCH_ALL_EXCEPTIONS("While submitting Predefined Display request");
            }
	        break;

        default :
	        break;
        }
        FUNCTION_EXIT;
    }


    std::string DisplayPage::timeToString( const CTime& time )
    {
        FUNCTION_ENTRY( "timeToString" );

        std::stringstream timeStamp;

        timeStamp << time.GetYear();

        timeStamp << std::setw(2) << std::setfill('0') << time.GetMonth();

        timeStamp << std::setw(2) << std::setfill('0') << time.GetDay();

        timeStamp << std::setw(2) << std::setfill('0') << time.GetHour();

        timeStamp << std::setw(2) << std::setfill('0') << time.GetMinute();

        timeStamp << std::setw(2) << std::setfill('0') << time.GetSecond();

        FUNCTION_EXIT;
        return timeStamp.str();
    }


    void DisplayPage::OnClearAll()
    {
        FUNCTION_ENTRY( "OnClearAll" );

        submitClearRequest("All", 1, 6);

        FUNCTION_EXIT;
    }


    void DisplayPage::OnClearNormal()
    {
        FUNCTION_ENTRY( "OnClearNormal" );

        submitClearRequest("Normal", 4, 6);

        FUNCTION_EXIT;
    }


    void DisplayPage::OnClearEmergency()
    {
        FUNCTION_ENTRY( "OnClearEmergency" );

        submitClearRequest("Emergency", 1, 3);

        FUNCTION_EXIT;
    }


    void DisplayPage::submitClearRequest( std::string clearType,
                                          unsigned short lowerPriority,
                                          unsigned short upperPriority )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

		std::string confirmationMessage = UserMessages::QUESTION_CLEAR;

        // Display the confirmation dialog
        if( IDYES != UserMessages::getInstance().askQuestion(confirmationMessage.c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST))
        {
            FUNCTION_EXIT;
            return;
        }

        TA_ASSERT(m_pidSelectionManager != NULL, "PID selection manager is NULL");

        TA_Base_Core::STISDestinationList destination;
        // TES 880 - now a destination list for all pids in list
        // m_pidSelectionManager->populateClearDestination( destination );
        m_pidSelectionManager->populateDisplayDestination( destination );

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        clearType = "Clear " + clearType;

        try
        {
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitClearRequest,
					(destination, lowerPriority, upperPriority, sessionId.c_str()));

			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitClearRequest,
					(destination, lowerPriority, upperPriority, sessionId.c_str()));
			}
            UserMessages::getInstance().displayInfo(str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clearType ).c_str());
        }
		CATCH_ALL_EXCEPTIONS("While submitting clear request");
        FUNCTION_EXIT;
    }


    void DisplayPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void DisplayPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }


    // TD 15349
    // zhou yuan++
    bool DisplayPage::findAndSelectStationMessage(const std::string& messageName)
    {
        FUNCTION_ENTRY( "findAndSelectStationMessage" );
        FUNCTION_EXIT;
        return m_messageTypeTab.findAndSelectStationMessage(messageName);
    }

	void DisplayPage::OnKillFocusRepeatInterval()
	{
		m_repeatInterval.focusKilled();
	}


    // ++zhou yuan

} // TA_IRS_App
