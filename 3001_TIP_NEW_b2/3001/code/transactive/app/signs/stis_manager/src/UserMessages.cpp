/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/UserMessages.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2014/05/23 15:31:56 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * A single way to display messages,
  * and a single place to store common error messages.
  * 
  */

#include "stdafx.h"
#include "UserMessages.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "boost/tokenizer.hpp"


#include <sstream>

namespace TA_IRS_App
{


    const char* UserMessages::INFO_REQUEST_SUCCESSFUL = "%s Request sent successfully"; // [type]
	//UW_670001 & UW_670002
	const char* UserMessages::QUESTION_DISPLAY = "Are you sure you wish to display the following %s message:\n\"%s\""; // [type], [message]
	const wchar_t* UserMessages::QUESTION_DISPLAY_W = L"Are you sure you wish to display the following %s message:\n\"%s\""; // [type], [message]
	//UW_670003
    const char* UserMessages::QUESTION_CLEAR = "Are you sure you wish to clear the Display?"; // [type]
    const char* UserMessages::QUESTION_DELETE_PID_GROUP = "Are you sure you wish to delete the PID group %s?"; // [group]
	//US_670004
    const char* UserMessages::QUESTION_UPGRADE_LIBRARY = "Are you sure you wish to upgrade the TIS library?";
	//US_670005
	const char* UserMessages::QUESTION_DOWNLOAD_TIME_SCHEDULE = "Are you sure you wish to download the time schedule?";

	
    const char* UserMessages::ERROR_NO_PREDEFINED_IN_DB = "The current STIS predefined message library was not found in the database.";
    const char* UserMessages::ERROR_LOADING_PREDEFINED = "Error loading the current STIS predefined message library from the database.";
    const char* UserMessages::ERROR_START_TIME_AFTER_END = "Start time cannot be after End time. Cannot submit display request.";
    const char* UserMessages::ERROR_START_TIME_EQUALS_END = "Start time is the same as End time. Cannot submit display request.";
    const char* UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME = "Start time cannot be before Current time. Cannot submit display request.";;
    const char* UserMessages::ERROR_END_TIME_PASSED = "The End time has passed. Cannot submit display request.";
    const char* UserMessages::ERROR_REQUEST_FAILED = "%s request failed. %s"; // [Type], [error]
    const char* UserMessages::ERROR_UPGRADE_FAILED = "Error upgrading ISCS version. %s"; // [error]
	
	
	const char* UserMessages::ERROR_CONNECTION_STIS = "Unable to establish Connection with the STIS System";
	const char* UserMessages::ERROR_CONNECTION_DATABASE = "Unable to connect to database";
	const char* UserMessages::ERROR_CONTACT_AGENT = "Agent cannot be contacted";
	const char* UserMessages::ERROR_PERFORM_REQUEST = "Unable to perform requested operation";

	const char* UserMessages::ERROR_REPEAT_INTERVAL_INVALID = "Repeat interval should between 1 minute to 1 hour.";


    UserMessages* UserMessages::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable UserMessages::m_singletonLock;

    UserMessages& UserMessages::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.
		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
            m_theClassInstance = new UserMessages();
		}
		
		return *m_theClassInstance;
    }

    UserMessages::UserMessages()
        : m_alreadyDisplayed(),
          m_queuedMessages(),
          m_suppressMessages(true)
    {
		m_parent =  AfxGetMainWnd();
    }


    UserMessages::~UserMessages()
    {}

	void UserMessages::setParent(CWnd *parent)
	{
		m_parent = parent;
	}

    void UserMessages::setMessageSuppression(bool suppressionEnabled)
    {
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
    }


    void UserMessages::displayErrorOnce(ErrorType errorType, const char* message)
    {
        // if the message hasnt already bee displayed
        if ( !hasBeenDisplayed(errorType) )
        {
            // display the message, and set it to displayed
            m_alreadyDisplayed[errorType] = true;
            displayError(message);
        }
    }


    void UserMessages::displayError(const char* message, UINT nType /* = MB_OK | MB_ICONSTOP | MB_TOPMOST */ )
    {
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
    }


    void UserMessages::displayWarning(const char* message, UINT nType /* = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST */ )
    {
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
    }


    void UserMessages::displayInfo(const char* message, UINT nType /* = MB_OK | MB_ICONINFORMATION | MB_TOPMOST */)
    {
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
    }


	int UserMessages::askQuestion(const wchar_t* message, UINT nType /*= MB_YESNO | MB_ICONQUESTION | MB_TOPMOST*/)
	{
		if(FALSE == ::IsWindow(m_parent->m_hWnd))
		{
			return -1;
		}
		return ::MessageBoxW(m_parent->m_hWnd, message, L"TIS Manager", nType);
	}

    int UserMessages::askQuestion(const char* message, UINT nType /* = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST */ )
    {
        return displayMessage(message, nType);
    }


    bool UserMessages::hasBeenDisplayed(ErrorType errorType)
    {
        std::map<ErrorType, bool>::iterator findIter = m_alreadyDisplayed.find(errorType);

        // if it hasnt already been displayed
        if (findIter == m_alreadyDisplayed.end())
        {
            return false;
        }

        return true;
    }


    int UserMessages::displayMessage(const char* message, UINT nType/* = MB_OK | MB_ICONSTOP | MB_TOPMOST*/)
    {
		TA_Base_Bus::TransActiveMessage userMsg;
		CString reason = breakMessage(message).c_str();
		userMsg << reason;
		CString errMsg = userMsg.constructMessage(IDS_UE_020071);
		if(FALSE == ::IsWindow(m_parent->m_hWnd))
		{
			return -1;
		}
		return ::MessageBox(m_parent->m_hWnd, errMsg, "TIS Manager" , nType);
    }

    std::string UserMessages::breakMessage( std::string theString )
	{
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        CRect windowRect;
		if(FALSE == ::IsWindow(m_parent->m_hWnd))
		{
			return theString;
		}
		m_parent->GetWindowRect(&windowRect);
        // 1280 is the minimum size for this application
        int maximumLineLength = 640;
        if (windowRect.Width() > 1280)
        {
            maximumLineLength = windowRect.Width() / 2;
        }

        std::string newString;
        CDC deviceContext;
        deviceContext.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

        // break on newlines
        boost::char_separator<char> lineSep( "\n" );
        tokenizer lineTokens(theString, lineSep);

        // for each line
        for (tokenizer::iterator line_iter = lineTokens.begin(); line_iter != lineTokens.end(); ++line_iter)
        {
            // calculate the length
            CSize lineSize = deviceContext.GetTextExtent(line_iter->c_str());

            // if the line is too long
            if (lineSize.cx > maximumLineLength)
            {
                // build it word by word
                std::string newLine;
                int newLineLength = 0;

                // break on spaces
                boost::char_separator<char> wordSep( " " );
                tokenizer wordTokens(*line_iter, wordSep);

                for (tokenizer::iterator word_iter = wordTokens.begin(); word_iter != wordTokens.end(); ++word_iter)
                {
                    // for each word
                    std::string word(*word_iter);
                    word.append(" ");

                    // calculate the length
                    CSize wordSize = deviceContext.GetTextExtent(word.c_str());

                    // if it is too long
                    if (wordSize.cx > maximumLineLength)
                    {
                        // end the line and start a new one
                        if (newLine.length() > 0)
                        {
                            newLine.append("\n");
                            newString.append(newLine);
                            newLine = "";
                            newLineLength = 0;
                        }

                        while (wordSize.cx > maximumLineLength)
                        {
                            // put a space in at the maximum length (or thereabouts)
                            long spacePosition = (maximumLineLength * word.length()) / wordSize.cx;

                            // put in some of the word, and a newline
                            newString.append(word.substr(0, spacePosition));
                            newString.append("\n");
                            // remove it from the word
                            word.erase(0, spacePosition);

                            // calcualte the length of the leftovers
                            wordSize = deviceContext.GetTextExtent(word.c_str());
                        }

                        if (word.size() > 0)
                        {
                            newLine.append(word);
                            newLineLength = newLineLength + wordSize.cx;
                        }
                    }
                    else
                    {
                        // if it fits on the line - add it
                        if ( (newLineLength + wordSize.cx) <= maximumLineLength )
                        {
                            newLine.append(word);
                            newLineLength = newLineLength + wordSize.cx;
                        }
                        else
                        {
                            // end the line and start a new one
                            newLine.append("\n");
                            newString.append(newLine);
                            newLine = "";
                            newLineLength = 0;
                        }
                    }
                }

                if (newLine.size() > 0)
                {
                    newLine.append("\n");
                    newString.append(newLine);
                    newLine = "";
                    newLineLength = 0;
                }
            }
            else
            {
                newString.append(*line_iter);
                newString.append("\n");
            }
        }


        return newString;
	}

} // end namespace TA_IRS_App

