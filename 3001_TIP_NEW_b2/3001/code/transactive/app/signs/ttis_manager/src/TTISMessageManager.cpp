/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISMessageManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * A single way to display messages,
  * and a single place to store common error messages.
  *
  */

#include "stdafx.h"
#include "TTISMessageManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/signs/stis_manager/src/UserMessages.h"

#include <sstream>

namespace TA_IRS_App
{

    const char* TTISMessageManager::NO_TIS_AGENT_PREDEFINED_UNAVAILABLE    = "Error connecting to the TIS Agent. TTIS Predefined messages are unavailable.";
    const char* TTISMessageManager::NO_TRAIN_AGENT_TRAIN_LISTS_UNAVAILABLE = "Error connecting to Train Agent. Train lists will be unavailable";

    const char* TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE         = "Error loading time schedules from the database.";
    const char* TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY    = "Error loading the time schedule: ";
    const char* TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_ENTRY   = "Error loading the time schedule entry with pkey: ";
        const char* TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_ENTRY_END = " it will be deleted, save the schedule to apply this deletion.";
    const char* TTISMessageManager::ERROR_SAVING_SCHEDULE               = "There was an error saving the time schedule.\n The following items were incorrectly specified:\n";
    const char* TTISMessageManager::ERROR_SAVING_SCHEDULE_SHORT         = "There was an error saving the time schedule.";
    const char* TTISMessageManager::ERROR_DELETING_SCHEDULE             = "There was an error deleting the time schedule.";
    const char* TTISMessageManager::CURRENT_SCHEDULE_DELETED            = "The currently loaded time schedule has been deleted.";
    const char* TTISMessageManager::CURRENT_SCHEDULE_MODIFIED           = "The currently loaded time schedule has been modified. Do you wish to load the new version?.";

    const char* TTISMessageManager::START_TIME_SAME_AS_END_TIME         = "Start time is the same as End time";
    // xufeng++ 2006/10/14 TD14367
    const char* TTISMessageManager::START_TIME_AFTER_END_TIME            = "Start time is after End time";
    const char* TTISMessageManager::START_TIME_BEFORE_CURRENT_TIME        = "Start time is before Current time";
    // xufeng++ 2006/11/07 TD17546
    const char* TTISMessageManager::OUT_OF_TIME_RANGE                    = "No predefined and/or ad-hoc msg can be displayed between 03:00 and 03:01";
    // ++xufeng 2006/11/07 TD17546
    // ++xufeng 2006/10/14 TD14367

    const char* TTISMessageManager::ERROR_LOADING_PREDEFINED            = "Error loading the TTIS predefined message library from the database.";
    const char* TTISMessageManager::NO_PREDEFINED_LIBRARY               = "The current TTIS predefined message library was not found in the database.";

    const char* TTISMessageManager::FOLLOWING_ERRORS                    = "The following errors were encountered while sending the request:";

    const char* TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN         = "Error sending display request to train ";
    const char* TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN           = "Error sending clear request to train ";
    const char* TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN        = "Error sending download request to train. ";
    const char* TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN         = "Error sending upgrade request to train. ";

    const char* TTISMessageManager::TRAIN_TIME_OUT                      = "Train time out.";
    const char* TTISMessageManager::TRAIN_BUSY                          = "The train is busy.";
    // defectID(17924) AVteam++
    // const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR          = "Error connecting to the TIS Agent. Display Request not sent.";
    const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE= "Error connecting to the TIS Agent. Predefined Display Request not sent.";
    const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT    = "Error connecting to the TIS Agent. Free text Display Request not sent.";
    // defectID(17924) ++AVteam
    const char* TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR            = "Error connecting to the TIS Agent. Clear request not sent.";
    const char* TTISMessageManager::NO_TIS_AGENT_DOWNLOAD_ERROR         = "Error connecting to the TIS Agent. Download request not sent.";
    const char* TTISMessageManager::NO_TIS_AGENT_UPGRADE_ERROR          = "Error connecting to the TIS Agent. Upgrade request not sent.";

    const char* TTISMessageManager::NOT_STARTING_ALREADY_CURRENT        = "Train already has the current version. Not initiating download.";
    const char* TTISMessageManager::TRAIN_CANT_UPGRADE_NOT_CURRENT      = "Train does not have the current version. Download the current version before upgrading.";
    const char* TTISMessageManager::NOT_STARTING_ALREADY_NEXT           = "Train already has the next version. Not initiating download.";
    const char* TTISMessageManager::TRAIN_CANT_UPGRADE_NOT_NEXT         = "Train does not have the next version. Download the next version before upgrading.";
    const char* TTISMessageManager::TRAIN_CANT_UPGRADE_ALREADY_UPGRADED = "Train is already upgraded. Not initiating upgrade.";

    const char* TTISMessageManager::NO_TIS_AGENT                        = "Error connecting to the TIS Agent.";

    const char* TTISMessageManager::CANT_SET_CURRENT_SCHEDULE           = "Error setting current time schedule.";
    const char* TTISMessageManager::CANT_UPGRADE_VERSION                = "Error upgrading ISCS version.";

    const char* TTISMessageManager::ERROR_SAVING_FREE_TEXT              = "Error saving free text message. ";

    const char* TTISMessageManager::SCHEDULE_EDIT_PERMISSIONS_LOST      = "The current time schedule has unsaved changes and you have just lost permission to edit train time schedules.\nYour changes cannot be saved until you regain these permissions.";

    const char* TTISMessageManager::ASK_SUBMIT_CLEAR_ALL        = "Are you sure you wish to submit a 'Clear All' message?";
    const char* TTISMessageManager::ASK_SUBMIT_CLEAR_6          = "Are you sure you wish to submit a 'Clear Priority 6' message?";
    const char* TTISMessageManager::ASK_SUBMIT_CLEAR_NORMAL     = "Are you sure you wish to submit a 'Clear Normal' message?";
    const char* TTISMessageManager::ASK_SUBMIT_CLEAR_EMERGENCY  = "Are you sure you wish to submit a 'Clear Emergency' message?";

    const char* TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE   = "Do you wish to lose the unsaved changes to the current schedule?";
    const char* TTISMessageManager::ASK_SAVE_SCHEDULE           = "Are you sure you wish to save the current schedule?";
    const char* TTISMessageManager::ASK_DELETE_SCHEDULE         = "Are you sure you wish to delete the schedule?";
    const char* TTISMessageManager::ASK_REMOVE_ENTRY            = "Are you sure you wish to remove the selected entry?";
    const char* TTISMessageManager::ASK_REMOVE_ALL_ENTRIES      = "Are you sure you wish to remove all entries?";

    const char* TTISMessageManager::ASK_DISPLAY_PREDEFINED  = "Are you sure you want to display the following predefined message:";
    const char* TTISMessageManager::ASK_DISPLAY_ADHOC       = "Are you sure you want to display the following ad-hoc message:";

    const char* TTISMessageManager::ASK_DOWNLOAD_MESSAGE_LIBRARY    = "Are you sure you wish to download the current TTIS message library?";
    const char* TTISMessageManager::ASK_UPGRADE_MESSAGE_LIBRARY     = "Are you sure you wish to upgrade to the current TTIS message library?";
    const char* TTISMessageManager::ASK_DOWNLOAD_TIME_SCHEDULE      = "Are you sure you wish to download the current Train Time Schedule?";
    const char* TTISMessageManager::ASK_UPGRADE_TIME_SCHEDULE       = "Are you sure you wish to upgrade to the current Train Time Schedule?";
    const char* TTISMessageManager::ASK_SET_TIME_CURRENT_TIME_SCHEDULE  = "Are you sure you wish to set the current train time schedule to:";

	const char* TTISMessageManager::MAX_SCHEDULE_MESSAGE_REACHED  = "Maximum schedule messages reached. Please delete some before add new";    

    TTISMessageManager* TTISMessageManager::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable TTISMessageManager::m_singletonLock;

    TTISMessageManager& TTISMessageManager::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.
        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new TTISMessageManager();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    TTISMessageManager::TTISMessageManager()
        : m_alreadyDisplayed(),
          m_queuedMessages(),
          m_suppressMessages(true)
    {
        FUNCTION_ENTRY( "TTISMessageManager" );
        FUNCTION_EXIT;
    }


    TTISMessageManager::~TTISMessageManager()
    {
        FUNCTION_ENTRY( "~TTISMessageManager" );
        FUNCTION_EXIT;
    }


    void TTISMessageManager::setMessageSuppression(bool suppressionEnabled)
    {
        FUNCTION_ENTRY( "setMessageSuppression" );

        m_suppressMessages = suppressionEnabled;

        // if suppression has been turned off
        if ( (!m_suppressMessages) && (m_queuedMessages.size() > 0) )
        {
            // display all queued messages
            std::stringstream bigErrorMessage;

            for( std::vector<std::string>::iterator iter = m_queuedMessages.begin();
            iter != m_queuedMessages.end(); iter++)
            {
                bigErrorMessage << *iter << std::endl;
            }

            displayMessage(bigErrorMessage.str().c_str());

            // reset queued errors
            m_queuedMessages.clear();
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayErrorOnce(ErrorType errorType, const char* message)
    {
        FUNCTION_ENTRY( "displayErrorOnce" );

        // if the message hasnt already bee displayed
        if ( !hasBeenDisplayed(errorType) )
        {
            // display the message, and set it to displayed
            m_alreadyDisplayed[errorType] = true;
            displayError(message);
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayError(const char* message, UINT nType /* = MB_OK | MB_ICONSTOP */ )
    {
        FUNCTION_ENTRY( "displayError" );

        // if suppression is enabled
        if (m_suppressMessages)
        {
            // queue the message
            m_queuedMessages.push_back(message);
        }
        else
        {
            // otherwise display it
            displayMessage(message, nType);
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayWarning(const char* message, UINT nType /* = MB_OK | MB_ICONEXCLAMATION */ )
    {
        FUNCTION_ENTRY( "displayWarning" );

        // if suppression is enabled
        if (m_suppressMessages)
        {
            // queue the message
            m_queuedMessages.push_back(message);
        }
        else
        {
            // otherwise display it
            displayMessage(message, nType);
        }

        FUNCTION_EXIT;
    }


    int TTISMessageManager::askQuestion(const char* message, UINT nType /* = MB_YESNO | MB_ICONQUESTION */ )
    {
        FUNCTION_ENTRY( "askQuestion" );
        FUNCTION_EXIT;
        return displayMessage(TA_IRS_App::UserMessages::breakMessage(std::string(message)).c_str(), nType);
    }


    bool TTISMessageManager::hasBeenDisplayed(ErrorType errorType)
    {
        FUNCTION_ENTRY( "hasBeenDisplayed" );

        std::map<ErrorType, bool>::iterator findIter = m_alreadyDisplayed.find(errorType);

        // if it hasnt already been displayed
        if (findIter == m_alreadyDisplayed.end())
        {
            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


} // end namespace TA_IRS_App

