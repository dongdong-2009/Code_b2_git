/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/DisplayPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
#include "REBProgressManager.h"

#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"

#include "core\data_access_interface\entity_access\src\TISAgentEntityData.h"
#include "core\data_access_interface\entity_access\src\STISEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"

#include "boost/tokenizer.hpp"

#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    // This is the biggest repeat interval
    static const unsigned short MAX_REPEAT_INTERVAL = 999;

    static const DWORD COLOR_RED = RGB(255, 0, 0);

    DisplayPage::DisplayPage(CWnd* pParent /*=NULL*/)
        : CDialog(DisplayPage::IDD, pParent),
          m_pidsInList(false),
          m_pidSelected(false),
          m_validMessageSelected(false),
          m_canDisplay(false),
          m_canClear(false),
          m_canDisplayRATIS(false),
          m_canChangeRATISPriority(false),
          m_canModifyPidGroups(false),
          m_pidSelectionManager(NULL),
          m_timeControlManager(NULL)
    {
        FUNCTION_ENTRY( "DisplayPage" );

        // {{AFX_DATA_INIT(DisplayPage)
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    DisplayPage::~DisplayPage()
    {
        FUNCTION_ENTRY( "~DisplayPage" );

		TA_Base_Core::ThreadGuard guard1( MainTab::s_dipalyThreadLock );

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

        m_stisPredefinedMessages->removeInstance();  // TD11310 ~ added

        FUNCTION_EXIT;
    }


    void DisplayPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(DisplayPage)
        DDX_Control(pDX, IDC_DISPLAY, m_displayButton);
        DDX_Control(pDX, IDC_CLEAR_PRIORITY6, m_clearPriority6Button);
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
        DDX_Control(pDX, IDC_REPEAT_INTERVAL, m_repeatInterval);
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
        ON_BN_CLICKED(IDC_CLEAR_PRIORITY6, OnClearPriority6)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL DisplayPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();
        // set up PID selection
        m_pidSelectionManager = new PidSelectionManager(m_PIDList,
                                                        m_pidGroupCombo,
                                                        m_saveGroupButton,
                                                        m_deleteGroupButton,
                                                        *this);

        // set up the time controls
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Attempting to create new TimeControlManager");

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
        }
        catch( ... )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Caught an unknown exception while creating new TimeControlManager");
        }

        // emergency priorities are red
        m_priority.mapItemDataToColour(1, COLOR_RED);
        m_priority.mapItemDataToColour(2, COLOR_RED);
        m_priority.mapItemDataToColour(3, COLOR_RED);

        m_clearAllButton.EnableWindow(FALSE);
        m_clearEmergencyButton.EnableWindow(FALSE);
        m_clearNormalButton.EnableWindow(FALSE);
        m_clearPriority6Button.EnableWindow(FALSE);

        // set the repeat interval range (0 to 255 minutes)
        CTime lowerTime(1971, 1, 1, 0, 0, 0, 0);
        // set the repeat interval
        unsigned short hours = MAX_REPEAT_INTERVAL / 60;
        unsigned short minutes = MAX_REPEAT_INTERVAL - (hours * 60);
        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        CTime upperTime(1971, 1, 1, hours, minutes, 0, 0);
        m_repeatInterval.SetRange(&lowerTime, &upperTime);

        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    void DisplayPage::InitAll(/*REBProgressManager & mgr*/)
    {
        FUNCTION_ENTRY( "InitAll" );

        //mgr.SetStaticText(0, "Initializing the Display Page: get rights");
        // access control
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();// TD11310
        //mgr.SetProgress(10);
        m_canClear = RightsManager::getInstance().canClear();
        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();
        m_canDisplayRATIS = RightsManager::getInstance().canDisplayRatis();
        m_canChangeRATISPriority = RightsManager::getInstance().canSetOutgoingRatisPriority();
        //mgr.SetProgress(20);
        RightsManager::getInstance().registerForRightsChanges(this);

        //mgr.SetProgress(30);
        m_pidSelectionManager->Init();

        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);

        //mgr.SetStaticText(0, "Initializing the Display Page: populate the message types");

        //mgr.SetProgress(40);
        m_messageTypeTab.setMessageSelectionListener(this);

        m_messageTypeTab.InitAll();

        Invalidate();

        FUNCTION_EXIT;
    }


    void DisplayPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    void DisplayPage::pidSelectionChanged(bool pidsInList, bool pidSelectionExists)
    {
        FUNCTION_ENTRY( "pidSelectionChanged" );

        // enable buttons based on PID selection
        m_pidsInList = pidsInList;
        m_pidSelected = pidSelectionExists;

        // TES 880 - this was changed from (m_pidSelected && m_canClear)
        // because a clear request now goes to all PIDs in the list
        // you can no longer select individual items in the PID list
        m_clearAllButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearEmergencyButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearNormalButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearPriority6Button.EnableWindow(m_pidsInList && m_canClear);

        m_displayButton.EnableWindow(m_pidsInList && m_validMessageSelected && m_canDisplay);

        FUNCTION_EXIT;
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
            m_canDisplay = RightsManager::getInstance().canDisplayPredefined();

            // set up the PID list
            TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");
            m_pidSelectionManager->setRatisMode(false);

            m_timeControlManager->enableTimeControls( false );
        }

        if (m_validMessageSelected)
        {
            // TD 16349
            // m_timeControlManager->setPriority( priority, true );
            m_timeControlManager->setPriority( priority, false );
            // TD 16349
            // TD12973 - For predefined messages, this value is preset in the
            // predefined message library, and therefore cannot be changed
            // m_timeControlManager->setRepeatInterval( repeatInterval, true);
        }
        else
        {
            m_timeControlManager->blankAndDisableTimeAndPriority();

            // default adhoc attributes
            m_timeControlManager->setPriority( priority, false );
            // TD12973 - For predefined messages, this value is preset in the
            // predefined message library, and therefore cannot be changed
            // m_timeControlManager->setRepeatInterval( repeatInterval, false );
        }

        // TD12973 - repeat interval value is always disabled for predefined messages
        m_timeControlManager->setRepeatInterval( repeatInterval, false );

        m_displayButton.EnableWindow(m_pidsInList && m_validMessageSelected && m_canDisplay);

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
            m_canDisplay = RightsManager::getInstance().canDisplayFreeText();

            // set up the PID list
            TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");
            m_pidSelectionManager->setRatisMode(false);

            // set up the time controls
            m_timeControlManager->enableTimeControls( true );

            // set default adhoc attributes
            m_timeControlManager->setPriority( 4, true );
            m_timeControlManager->setRepeatInterval( repeatInterval, true );
        }

        m_displayButton.EnableWindow(m_pidsInList && m_validMessageSelected && m_canDisplay);

        FUNCTION_EXIT;
    }


    void DisplayPage::ratisMessageSelected( bool tabSwitched,
                                            bool validMessageEntered,
                                            bool isClearMessage )
    {
        FUNCTION_ENTRY( "ratisMessageSelected" );

        m_validMessageSelected = validMessageEntered;
        m_displayButton.EnableWindow( m_validMessageSelected && m_canDisplayRATIS );

        TA_ASSERT( m_timeControlManager != NULL, "Time Control manager cant be null");

        if (tabSwitched)
        {
            // set up the PID list
            TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");
            m_pidSelectionManager->setRatisMode(true);
        }

        // set up the time controls
        m_timeControlManager->enableTimeControls( false );
        m_timeControlManager->setPriority( 6, m_canChangeRATISPriority );
        m_timeControlManager->setRepeatInterval( 0, false );
        m_timeControlManager->setRatisClear( isClearMessage );

        FUNCTION_EXIT;
    }


    LRESULT DisplayPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID selection Manager is NULL");

        // the rights have changed - re check them
        m_canClear = RightsManager::getInstance().canClear();
        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();
        m_canDisplayRATIS = RightsManager::getInstance().canDisplayRatis();
        m_canChangeRATISPriority = RightsManager::getInstance().canSetOutgoingRatisPriority();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Calling setPermissions() from onRightsChanged");
        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Calling setPermissions() successfully");
        // cause a state change to trigger one of the above functions
        m_messageTypeTab.setMessageSelectionListener(this);

        FUNCTION_EXIT;
        return 0;
    }


    void DisplayPage::OnDisplay()
    {
        FUNCTION_ENTRY( "OnDisplay" );

        BeginWaitCursor();

        TA_ASSERT( m_pidSelectionManager != NULL, "PID selection manager cant be null");
        TA_ASSERT( m_timeControlManager != NULL, "Time Control manager cant be null");

        // get the current tab
        int currSel = m_messageTypeTab.GetCurSel();

        // Grab the common user selections
        CTime tempStartTime = m_timeControlManager->getStartTime();
        CTime tempEndTime = m_timeControlManager->getEndTime();
        std::string startTimeString = timeToString(tempStartTime);
        std::string endTimeString = timeToString(tempEndTime);

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
            // xufeng++ 2006/10/14 TD14367
            else if (currentTime > tempStartTime) {
                UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME);

                FUNCTION_EXIT;
                return;
            }
            // ++xufeng 2006/10/14 TD14367
        }

        unsigned short selectedPriority = m_timeControlManager->getPriority();

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        // If the operator has created a free-text message then set one up, and display the appropriate
        // confirmation message
        switch(currSel)
        {
        case FREE_TEXT:
            {
                // get the destination

                TA_Base_Core::STISDestinationList destinationList;
                m_pidSelectionManager->populateDisplayDestination( destinationList );

                TA_ASSERT( destinationList.length() != 0,
                           "The display button should not be active if there are no PIDs in the list");


                // get the message
                FreeTextMessage message = m_messageTypeTab.getFreeTextMessage();

                // get the repeat interval
                message.displayAttributes.repeatInterval = m_timeControlManager->getRepeatInterval();

                TA_ASSERT( message.messageContent.length() > 0,
                           "The display button should not be active if there is no message content" );


                std::string confirmationMessage = str( format(UserMessages::QUESTION_DISPLAY) % "Free Text" % message.messageContent );

                // Display the confirmation dialog
                if( UserMessages::getInstance().askQuestion(confirmationMessage.c_str()) != IDYES )
                {
                    FUNCTION_EXIT;
                    return;
                }

                try
                {
                    /*TISAgentAccessFactory::getInstance().getSTISAgent()->
                        submitAdHocDisplayRequest( destinationList,
                                                   message.messageContent.c_str(),
                                                   startTimeString.c_str(),
                                                   endTimeString.c_str(),
                                                   selectedPriority,
                                                   message.displayAttributes,
                                                   message.plasmaAttributes,
                                                   message.ledAttributes,
                                               sessionId.c_str() );*/
					    //libo++
						try
						{

							CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitAdHocDisplayRequest,
								( destinationList,
                               message.messageContent.c_str(),
                               startTimeString.c_str(),
                               endTimeString.c_str(),
                               selectedPriority,
                               message.displayAttributes,
                               message.plasmaAttributes,
                               message.ledAttributes,
                               sessionId.c_str() ) );

						}
						catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
						{                                                                                                               
							LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
							CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitAdHocDisplayRequest,
								(destinationList,
                               message.messageContent.c_str(),
                               startTimeString.c_str(),
                               endTimeString.c_str(),
                               selectedPriority,
                               message.displayAttributes,
                               message.plasmaAttributes,
                               message.ledAttributes,
                               sessionId.c_str() ) );
						}
						//++libo
                    UserMessages::getInstance().displayInfo(
                        str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "Free Text Display" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "Timed out while waiting for STIS server response" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "STIS Communications error" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "TIS Agent not connected to STIS Server" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in() );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % ex.details ).c_str());
                }
                catch ( const TA_Base_Core::ObjectResolutionException& ore )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "Could not resolve TIS Agent" ).c_str());
                }
                catch ( const CORBA::Exception& ce )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                        TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "Could not resolve TIS Agent" ).c_str());
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "...", "While submitting Free Text Display request" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Free Text Display" % "Could not resolve TIS Agent" ).c_str());
                }
            }

        break;

        case PREDEFINED:
            {
                TA_Base_Core::STISDestinationList destinationList;
                m_pidSelectionManager->populateDisplayDestination( destinationList );

                TA_ASSERT( destinationList.length() != 0,
                           "The display button should not be active if there are no PIDs in the list");


                // get the message
                TA_Base_Core::PredefinedMessage message = m_messageTypeTab.getPredefinedMessage();


                std::string confirmationMessage = str( format(UserMessages::QUESTION_DISPLAY) % "Predefined" % message.message );

                // Display the confirmation dialog
                if( UserMessages::getInstance().askQuestion(confirmationMessage.c_str()) != IDYES )
                {
                    FUNCTION_EXIT;
                    return;
                }


                try
                {
                    int loadedLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();


                    /*TISAgentAccessFactory::getInstance().getSTISAgent()->
                        submitPredefinedDisplayRequest( destinationList,
                                                        static_cast<TA_Base_Core::ELibrarySection>(message.librarySection),
                                                        loadedLibraryVersion,
                                                        message.messageTag,
                                                        startTimeString.c_str(),
                                                        endTimeString.c_str(),
                                                        message.priority,
                                                       sessionId.c_str() );*/
					//libo++
					try
					{

						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitPredefinedDisplayRequest,
							(destinationList,
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
							(destinationList,
                            static_cast<TA_Base_Core::ELibrarySection>(message.librarySection),
                            loadedLibraryVersion,
                            message.messageTag,
                            startTimeString.c_str(),
                            endTimeString.c_str(),
                            message.priority,
                            sessionId.c_str() ));
					}
					//++libo
                    UserMessages::getInstance().displayInfo(
                        str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "Predefined Display" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "Timed out while waiting for STIS server response" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "STIS Communications error" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "TIS Agent not connected to STIS Server" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in() );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % ex.details.in() ).c_str());
                }
                catch ( const TA_Base_Core::ObjectResolutionException& ore )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

                    // TD 16346
//                    UserMessages::getInstance().displayError(
//                        str( format(UserMessages::ERROR_REQUEST_FAILED)
//                        % "Predefined Display" % "Could not resolve TIS Agent" ).c_str());
                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "Error connecting to TIS Agent" ).c_str());
                    // TD 16346
                }
                catch ( const CORBA::Exception& ce )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                        TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

                    // TD 16346
//                    UserMessages::getInstance().displayError(
//                        str( format(UserMessages::ERROR_REQUEST_FAILED)
//                        % "Predefined Display" % "Could not resolve TIS Agent" ).c_str());
                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "Error connecting to TIS Agent" ).c_str());
                    // TD 16346
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "...", "While submitting Predefined Display request" );

                    // TD 16346
//                    UserMessages::getInstance().displayError(
//                        str( format(UserMessages::ERROR_REQUEST_FAILED)
//                        % "Predefined Display" % "Could not resolve TIS Agent" ).c_str());
                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "Predefined Display" % "Error connecting to TIS Agent" ).c_str());
                    // Td 16346
                 }
            }
        break;


        case RATIS:
            {
                RatisMessage ratisMessage = m_messageTypeTab.getRatisMessage();

                // check if it is a RATIS display or clear
                // pop up the relevant confirmation dialog
                if (ratisMessage.clear)
                {
                    std::string confirmationMessage = str( format(UserMessages::QUESTION_CLEAR) % "RATIS");

                    // Display the confirmation dialog
                    if( UserMessages::getInstance().askQuestion(confirmationMessage.c_str()) != IDYES )
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                }
                else
                {
                    TA_ASSERT( ratisMessage.messageContent.length() > 0,
                           "The display button should not be active if there is no message content" );

                    std::string confirmationMessage = str( format(UserMessages::QUESTION_DISPLAY) % "RATIS" % ratisMessage.messageContent);

                    // Display the confirmation dialog
                    if( UserMessages::getInstance().askQuestion(confirmationMessage.c_str()) != IDYES )
                    {
                        FUNCTION_EXIT;
                        return;
                    }

                }

                // now send the request

                try
                {
                    if( ratisMessage.clear )
                    {
                      /*  TISAgentAccessFactory::getInstance().getSTISAgent()->
                            submitRATISDisplayRequest( "_",
                                                       selectedPriority,
                                                       ratisMessage.tag.c_str(),
                                                       ratisMessage.destination.c_str(),
                                                       "_",
                                                       "_",
                                                       TA_Base_Core::RATIS_OUT_CLEAR,
                                                       (ratisMessage.override ? 1 :0),
                                                       (ratisMessage.vetting ? 1 :0),
                                                       sessionId.c_str() );
					*/	
					//libo++
					try
					{

						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitRATISDisplayRequest,
							("_",
                           selectedPriority,
                           ratisMessage.tag.c_str(),
                           ratisMessage.destination.c_str(),
                           "_",
                           "_",
                           TA_Base_Core::RATIS_OUT_CLEAR,
                           (ratisMessage.override ? 1 :0),
                           (ratisMessage.vetting ? 1 :0),
                           sessionId.c_str()  ));

					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());


						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitRATISDisplayRequest,
							("_",
                           selectedPriority,
                           ratisMessage.tag.c_str(),
                           ratisMessage.destination.c_str(),
                           "_",
                           "_",
                           TA_Base_Core::RATIS_OUT_CLEAR,
                           (ratisMessage.override ? 1 :0),
                           (ratisMessage.vetting ? 1 :0),
                           sessionId.c_str()  ));
					}
					//++libo

                        UserMessages::getInstance().displayInfo(
                            str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "RATIS Clear" ).c_str());
                    }
                    else
                    {
                        // TODO establish whether the message is a NEW or UPDATE message

                        /*TISAgentAccessFactory::getInstance().getSTISAgent()->
                            submitRATISDisplayRequest( ratisMessage.messageContent.c_str(),
                                                       selectedPriority,
                                                       ratisMessage.tag.c_str(),
                                                       ratisMessage.destination.c_str(),
                                                       startTimeString.c_str(),
                                                       endTimeString.c_str(),
                                                       TA_Base_Core::RATIS_OUT_NEW,
                                                       (ratisMessage.override ? 1 :0),
                                                       (ratisMessage.vetting ? 1 :0),
                                                       sessionId.c_str() );*/

					//libo++
					try
					{

						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitRATISDisplayRequest,
							(ratisMessage.messageContent.c_str(),
                           selectedPriority,
                           ratisMessage.tag.c_str(),
                           ratisMessage.destination.c_str(),
                           startTimeString.c_str(),
                           endTimeString.c_str(),
                           TA_Base_Core::RATIS_OUT_NEW,
                           (ratisMessage.override ? 1 :0),
                           (ratisMessage.vetting ? 1 :0),
                           sessionId.c_str() ));

					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitRATISDisplayRequest,
							(ratisMessage.messageContent.c_str(),
                           selectedPriority,
                           ratisMessage.tag.c_str(),
                           ratisMessage.destination.c_str(),
                           startTimeString.c_str(),
                           endTimeString.c_str(),
                           TA_Base_Core::RATIS_OUT_NEW,
                           (ratisMessage.override ? 1 :0),
                           (ratisMessage.vetting ? 1 :0),
                           sessionId.c_str() ));
					}
					//++libo

                        UserMessages::getInstance().displayInfo(
                            str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "RATIS Display" ).c_str());
                    }


                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "Timed out while waiting for STIS server response" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "STIS Communications error" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "TIS Agent not connected to STIS Server" ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in() );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % ex.details ).c_str());
                }
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException& )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISFunctionalityNotAvailableException", "" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "Function not available at this location" ).c_str());
                }
                catch ( const TA_Base_Core::ObjectResolutionException& ore )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "Could not resolve TIS Agent" ).c_str());
                }
                catch ( const CORBA::Exception& ce )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                        TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "Could not resolve TIS Agent" ).c_str());
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "...", "While submitting RATIS Display request" );

                    UserMessages::getInstance().displayError(
                        str( format(UserMessages::ERROR_REQUEST_FAILED)
                        % "RATIS Display" % "Could not resolve TIS Agent" ).c_str());
                }
            }
        break;

        default :
            EndWaitCursor();
        break;
        }

        EndWaitCursor();

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

        submitClearRequest("All", 1, 8);

        FUNCTION_EXIT;
    }


    void DisplayPage::OnClearNormal()
    {
        FUNCTION_ENTRY( "OnClearNormal" );

        submitClearRequest("Normal", 4, 8);

        FUNCTION_EXIT;
    }


    void DisplayPage::OnClearEmergency()
    {
        FUNCTION_ENTRY( "OnClearEmergency" );

        submitClearRequest("Emergency", 1, 3);

        FUNCTION_EXIT;
    }


    void DisplayPage::OnClearPriority6()
    {
        FUNCTION_ENTRY( "OnClearPriority6" );

        submitClearRequest("Priority 6", 6, 6);

        FUNCTION_EXIT;
    }


    void DisplayPage::submitClearRequest( std::string clearType,
                                          unsigned short lowerPriority,
                                          unsigned short upperPriority )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

        std::string confirmationMessage = str( format(UserMessages::QUESTION_CLEAR) % clearType );

        // Display the confirmation dialog
        if( UserMessages::getInstance().askQuestion(confirmationMessage.c_str()) != IDYES )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_ASSERT(m_pidSelectionManager != NULL, "PID selection manager is NULL");

        TA_Base_Core::STISDestinationList destination;
        // TES 880 - now a destination list for all pids in list
        // m_pidSelectionManager->populateClearDestination( destination );
        m_pidSelectionManager->populateDisplayDestination( destination );

        // TD 16370
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        // TD 16370

        clearType = "Clear " + clearType;

        try
        {
            // TD 16370
            // TISAgentAccessFactory::getInstance().getSTISAgent()->
            // submitClearRequest( destination, lowerPriority, upperPriority, "");
            /*TISAgentAccessFactory::getInstance().getSTISAgent()->
                submitClearRequest( destination, lowerPriority, upperPriority, sessionId.c_str());
            // TD 16370
			*/
					//libo++
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
					//++libo

            UserMessages::getInstance().displayInfo(
                str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clearType ).c_str());
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "Timed out while waiting for STIS server response" ).c_str());
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", "" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "STIS Communications error" ).c_str());
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "TIS Agent not connected to STIS Server" ).c_str());
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in() );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % ex.details ).c_str());
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISFunctionalityNotAvailableException", "" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "Function not available at this location" ).c_str());
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "Could not resolve TIS Agent" ).c_str());
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "Could not resolve TIS Agent" ).c_str());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "While submitting RATIS Display request" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % clearType % "Could not resolve TIS Agent" ).c_str());
        }

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


    // ++zhou yuan

} // TA_IRS_App
