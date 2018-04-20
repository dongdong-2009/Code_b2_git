/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/UserMessages.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * A single way to display messages,
  * Provides a way to queue up messages during initialisation.
  */

#include "UserMessages.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{


    UserMessages* UserMessages::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable UserMessages::m_singletonLock;

    UserMessages& UserMessages::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.
        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new UserMessages();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    UserMessages::UserMessages()
        : m_queuedMessages(),
          m_suppressMessages(true)
    {
        FUNCTION_ENTRY( "UserMessages" );
        FUNCTION_EXIT;
    }


    UserMessages::~UserMessages()
    {
        FUNCTION_ENTRY( "~UserMessages" );
        FUNCTION_EXIT;
    }


    void UserMessages::setMessageSuppression(bool suppressionEnabled)
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


    void UserMessages::displayError(const char* message, UINT nType /* = MB_OK | MB_ICONSTOP | MB_TOPMOST */ )
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


    void UserMessages::displayWarning(const char* message, UINT nType /* = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST */ )
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


    void UserMessages::displayInfo(const char* message, UINT nType /* = MB_OK | MB_ICONINFORMATION | MB_TOPMOST */)
    {
        FUNCTION_ENTRY( "displayInfo" );

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


    int UserMessages::askQuestion(const char* message, UINT nType /* = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST */ )
    {
        FUNCTION_ENTRY( "askQuestion" );
        FUNCTION_EXIT;
        return displayMessage(message, nType);
    }


} // end namespace TA_App

