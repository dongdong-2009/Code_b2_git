/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/TTISMessageManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
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
#include <string>
#include "boost/format.hpp"
#include "boost/tokenizer.hpp"

namespace
{
    std::string breakMessage( std::string theString )
    {
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        CWnd* appWindow = AfxGetMainWnd();
        TA_ASSERT( appWindow, "Top level window is null" );
        CRect windowRect;
        appWindow->GetWindowRect( &windowRect );
        // 1280 is the minimum size for this application
        int maximumLineLength = 640;
        if ( windowRect.Width() > 1280 )
        {
            maximumLineLength = windowRect.Width() / 2;
        }


        std::string newString;
        CDC deviceContext;
        deviceContext.CreateDC( _T( "DISPLAY" ), NULL, NULL, NULL );

        // break on newlines
        boost::char_separator<char> lineSep( "\n" );
        tokenizer lineTokens( theString, lineSep );

        // for each line
        for ( tokenizer::iterator line_iter = lineTokens.begin(); line_iter != lineTokens.end(); ++line_iter )
        {
            // calculate the length
            CSize lineSize = deviceContext.GetTextExtent( line_iter->c_str() );

            // if the line is too long
            if ( lineSize.cx > maximumLineLength )
            {
                // build it word by word
                std::string newLine;
                int newLineLength = 0;

                // break on spaces
                boost::char_separator<char> wordSep( " " );
                tokenizer wordTokens( *line_iter, wordSep );

                for ( tokenizer::iterator word_iter = wordTokens.begin(); word_iter != wordTokens.end(); ++word_iter )
                {
                    // for each word
                    std::string word( *word_iter );
                    word.append( " " );

                    // calculate the length
                    CSize wordSize = deviceContext.GetTextExtent( word.c_str() );

                    // if it is too long
                    if ( wordSize.cx > maximumLineLength )
                    {
                        // end the line and start a new one
                        if ( newLine.length() > 0 )
                        {
                            newLine.append( "\n" );
                            newString.append( newLine );
                            newLine = "";
                            newLineLength = 0;
                        }

                        while ( wordSize.cx > maximumLineLength )
                        {
                            // put a space in at the maximum length (or thereabouts)
                            long spacePosition = ( maximumLineLength * word.length() ) / wordSize.cx;

                            // put in some of the word, and a newline
                            newString.append( word.substr( 0, spacePosition ) );
                            newString.append( "\n" );
                            // remove it from the word
                            word.erase( 0, spacePosition );

                            // calcualte the length of the leftovers
                            wordSize = deviceContext.GetTextExtent( word.c_str() );
                        }

                        if ( word.size() > 0 )
                        {
                            newLine.append( word );
                            newLineLength = newLineLength + wordSize.cx;
                        }
                    }
                    else
                    {
                        // if it fits on the line - add it
                        if ( ( newLineLength + wordSize.cx ) <= maximumLineLength )
                        {
                            newLine.append( word );
                            newLineLength = newLineLength + wordSize.cx;
                        }
                        else
                        {
                            // end the line and start a new one
                            newLine.append( "\n" );
                            newString.append( newLine );
                            newLine = "";
                            newLineLength = 0;
                        }
                    }
                }

                if ( newLine.size() > 0 )
                {
                    newLine.append( "\n" );
                    newString.append( newLine );
                    newLine = "";
                    newLineLength = 0;
                }
            }
            else
            {
                newString.append( *line_iter );
                newString.append( "\n" );
            }
        }


        return newString;
    }
}

namespace TA_IRS_App
{

	// Warning message
	const char* TTISMessageManager::ASK_DISPLAY_PREDEFINED  = "Are you sure you wish to display the following TTIS predefined message:";
	const char* TTISMessageManager::ASK_DISPLAY_FREETEXT = "Are you sure you wish to display the following TTIS Ad-Hoc message:";
	const char* TTISMessageManager::ASK_UPGRADE_ISCS_MESSAGE_LIBRARY = "Are you sure you wish to upgrade the current ISCS TTIS Predefined message Library?";
	const char* TTISMessageManager::ASK_DOWNLOAD_MESSAGE_LIBRARY    = "Are you sure you wish to download the next TTIS message library to train?";
	const char* TTISMessageManager::ASK_UPGRADE_MESSAGE_LIBRARY     = "Are you sure you wish to upgrade to the next TTIS message library on train?";

	// Error handle
	const char* TTISMessageManager::ERROR_CONTACTED_AGENT = "Database error occurs when the application is trying to access the database.";
	const char* TTISMessageManager::ERROR_ACCESS_DATABASE = "The application is unable to contact the Agent to execute the request.";

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
    const char* TTISMessageManager::START_TIME_AFTER_END_TIME            = "Start time is after End time";
    const char* TTISMessageManager::START_TIME_BEFORE_CURRENT_TIME        = "Start time is before Current time";    
    const char* TTISMessageManager::OUT_OF_TIME_RANGE                    = "No predefined and/or ad-hoc msg can be displayed between 03:00 and 03:01";    
    const char* TTISMessageManager::ERROR_LOADING_PREDEFINED            = "Error loading the TTIS predefined message library from the database.";
    const char* TTISMessageManager::NO_PREDEFINED_LIBRARY               = "The current TTIS predefined message library was not found in the database.";

    const char* TTISMessageManager::FOLLOWING_ERRORS                    = "The following errors were encountered while sending the request:";

    const char* TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN         = "Error sending display request to train ";
    const char* TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN           = "Error sending clear request to train ";
    const char* TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN        = "Error sending download request to train. ";
    const char* TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN         = "Error sending upgrade request to train. ";
	const char* TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN          = "Error sending cancel download request to train.";

    const char* TTISMessageManager::TRAIN_TIME_OUT                      = "Train time out.";
    const char* TTISMessageManager::TRAIN_BUSY                          = "The train is busy.";
    const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR          = "Error connecting to the TIS Agent. Display Request not sent.";
	const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORPREDEFINE = "Error connecting to the TIS Agent. Predefined Display Request not sent.";
	const char* TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORFREETEXT  = "Error connecting to the TIS Agent. Free text Display Request not sent.";
	const char* TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR            = "Error connecting to the TIS Agent. Clear request not sent.";
    const char* TTISMessageManager::NO_TRAIN_AGENT_DISPLAY_ERROR_PREDEFINE = "Error connecting to the Train Agent. Predefined Display Request not sent.";
    const char* TTISMessageManager::NO_TRAIN_AGENT_DISPLAY_ERROR_FREETEXT    = "Error connecting to the Train Agent. Free text Display Request not sent.";
    const char* TTISMessageManager::NO_TRAIN_AGENT_CLEAR_ERROR            = "Error connecting to the Train Agent. Clear request not sent.";
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

	const char* TTISMessageManager::ASK_SUBMIT_RESET = "Are you sure you wish to submit a reset command?";

    const char* TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE   = "Do you wish to lose the unsaved changes to the current schedule?";
    const char* TTISMessageManager::ASK_SAVE_SCHEDULE           = "Are you sure you wish to save the current schedule?";
    const char* TTISMessageManager::ASK_DELETE_SCHEDULE         = "Are you sure you wish to delete the schedule?";
    const char* TTISMessageManager::ASK_REMOVE_ENTRY            = "Are you sure you wish to remove the selected entry?";
    const char* TTISMessageManager::ASK_REMOVE_ALL_ENTRIES      = "Are you sure you wish to remove all entries?";

  
	
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
          m_suppressMessages( false )
    {
        FUNCTION_ENTRY( "TTISMessageManager" );
        FUNCTION_EXIT;
    }


    TTISMessageManager::~TTISMessageManager()
    {
        FUNCTION_ENTRY( "~TTISMessageManager" );
        FUNCTION_EXIT;
    }


    void TTISMessageManager::setMessageSuppression( bool suppressionEnabled )
    {
        FUNCTION_ENTRY( "setMessageSuppression" );

        m_suppressMessages = suppressionEnabled;

        // if suppression has been turned off
        if ( ( !m_suppressMessages ) && ( m_queuedMessages.size() > 0 ) )
        {
            // display all queued messages
            std::stringstream bigErrorMessage;

            for( std::vector<std::string>::iterator iter = m_queuedMessages.begin();
                    iter != m_queuedMessages.end(); iter++ )
            {
                bigErrorMessage << *iter << std::endl;
            }

            displayMessage( bigErrorMessage.str().c_str() );

            // reset queued errors
            m_queuedMessages.clear();
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayErrorOnce( ErrorType errorType, const char* message )
    {
        FUNCTION_ENTRY( "displayErrorOnce" );

        // if the message hasnt already bee displayed
        if ( !hasBeenDisplayed( errorType ) )
        {
            // display the message, and set it to displayed
            m_alreadyDisplayed[errorType] = true;
            displayError( message );
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayError( const char* message, UINT nType /* = MB_OK | MB_ICONSTOP */ )
    {
        FUNCTION_ENTRY( "displayError" );

        // if suppression is enabled
        if ( m_suppressMessages )
        {
            // queue the message
            m_queuedMessages.push_back( message );
        }
        else
        {
            // otherwise display it
            displayMessage( message, nType );
        }

        FUNCTION_EXIT;
    }


    void TTISMessageManager::displayWarning( const char* message, UINT nType /* = MB_OK | MB_ICONEXCLAMATION */ )
    {
        FUNCTION_ENTRY( "displayWarning" );

        // if suppression is enabled
        if ( m_suppressMessages )
        {
            // queue the message
            m_queuedMessages.push_back( message );
        }
        else
        {
            // otherwise display it
            displayMessage( message, nType );
        }

        FUNCTION_EXIT;
    }


    int TTISMessageManager::askQuestion( const char* message, UINT nType /* = MB_YESNO | MB_ICONQUESTION */ )
    {
        FUNCTION_ENTRY( "askQuestion" );
        FUNCTION_EXIT;
        return displayMessage( message, nType );
    }


    bool TTISMessageManager::hasBeenDisplayed( ErrorType errorType )
    {
        FUNCTION_ENTRY( "hasBeenDisplayed" );

        std::map<ErrorType, bool>::iterator findIter = m_alreadyDisplayed.find( errorType );

        // if it hasnt already been displayed
        if ( findIter == m_alreadyDisplayed.end() )
        {
            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


} // end namespace TA_IRS_App

