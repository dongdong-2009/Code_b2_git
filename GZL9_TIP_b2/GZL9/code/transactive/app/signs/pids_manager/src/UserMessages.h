/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/stis_manager/src/UserMessages.h $
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

#if !defined(STIS_MESSAGE_BOXES_H)
#define STIS_MESSAGE_BOXES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <map>
#include <vector>

#include "boost/format.hpp"

using boost::format;
using boost::io::group;
using boost::str;


namespace TA_IRS_App
{
    class UserMessages
    {
    public:
        // This is used for the dislayError method.
        // basically, this is for initialisation errors that you only want to
        // appear once. Eg there is no tis agent, to stop this popping up multiple
        // times, use displayErrorOnce. Any subsequent calls with the same type won't be displayed.
        // This list will grow as needed.
        enum ErrorType
        {
            UNABLE_TO_LOAD_PREDEFINED
        };

        // MESSAGES
        static const char* REQUEST_SEND_SUCCESSFUL;
        static const char* REQUEST_SEND_FAIL;
        static const char* QUESTION_DISPLAY;
        static const char* ERROR_START_TIME_AFTER_END;
        static const char* ERROR_START_TIME_EQUALS_END;
        static const char* ERROR_START_TIME_BEFORE_CURRENT_TIME;
        static const char* ERROR_END_TIME_PASSED;
		static const char* REQUEST_SEND_CYCLIC_SUCCESSFUL;

        /**
         * ~UserMessages
         *
         * Standard destructor.
         */
        virtual ~UserMessages();


        /**
         * set the parent window
         *
         * Standard destructor.
         */	
		void setParent(CWnd *parent);
        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         * Initially, message suppression will be on.
         * This object is not threadlocked, it is displaying message boxes,
         * so should only ever be called from the GUI thread.
         *
         * @return      UserMessages&
         *              A reference to an instance of a UserMessages object.
         *
         */
		static UserMessages& getInstance();


         /** 
          * setMessageSuppression
          *
          * Enable or disable message suppression.
          * While suppression is on, messages will be queued. Questions cannot be queued.
          * When turning suppression off, all queued messages will be shown.
          *
          * @param suppressionEnabled
          */
        void setMessageSuppression(bool suppressionEnabled);


        /** 
          * displayErrorOnce
          *
          * This only displays each error type once.
          * Eg on initialisation, multiple attempts to contact the train agent are made.
          * the first time it fails, display a message, after that don't. This prevents each page
          * popping up an unable to contact X agent message.
          *
          * @param errorType The type of error
          * @param message  The message to display
          *
          */
        void displayErrorOnce(ErrorType errorType, const char* message);


        /** 
          * displayError
          *
          * Display an error message box.
          *
          * @param message  The message text
          * @param nType    The dialog box style (default is error, with an ok button)
          *
          */
        void displayError(const char* message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST);


        /** 
          * displayWarning
          *
          * Display a warning message box
          *
          * @param message  The message text
          * @param nType    The dialog box style (default is exclamation, with an ok button)
          *
          */
        void displayWarning(const char* message, UINT nType = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);


        /** 
          * displayInfo
          *
          * Display an info message box
          *
          * @param message  The message text
          * @param nType    The dialog box style (default is information, with an ok button)
          *
          */
        void displayInfo(const char* message, UINT nType = MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_TOPMOST);


        /** 
          * askQuestion
          *
          * Ask a question, questions cannot be suppressed, as an answer is needed.
          *
          * @param message  The message text
          * @param nType    The dialog box style (default is question, with yes/no buttons)
          *
          * @return IDYES or IDNO (or whatever is appropriate for the given style.)
          *
          */
        int askQuestion(const char* message, UINT nType = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);


        /** 
          * breakMessage
          *
          * Message boxes break on word boundaries. If an operator were to do something stupid
          * (like in an FAT maybe) by sending a message of all W's to test the limits of the system
          * then this would come out in a message box spanning multiple screens.
          *
          * This is the life story of TES 842.
          *
          * This wont affect a regular message - because there are no words in the english language,
          * not even supercalafrajalisticexpialadocious that span more than one screen.
          *
          */
        static std::string breakMessage( std::string theString );


    private:
        
        /**
         * UserMessages
         *
         * Private constructors.
         */
        UserMessages();
        UserMessages& operator=(const UserMessages &) {};
        UserMessages( const UserMessages& ) {};

        /** 
          * displayMessage
          *
          * Just a wrapper for AfxMessageBox.
          *
          * @param message
          * @param nType
          *
          * @return 
          *
          */
        int displayMessage(const char* message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST);


        /** 
          * hasBeenDisplayed
          *
          * Has the error type been displayed?
          *
          * @param errorType The error type
          *
          * @return true if the message has been displayed or queued.
          */
        bool hasBeenDisplayed(ErrorType errorType);

        // class instance
        static UserMessages* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // map of already displayed messages
        std::map<ErrorType, bool> m_alreadyDisplayed;

        // a vector of queued messages
        std::vector<std::string> m_queuedMessages;

        // whether messages are being suppressed
        bool m_suppressMessages;

		//haipeng added
		CWnd *m_parent;  //message box's parent window
		//haipeng added
    };

} // end namespace TA_IRS_App

#endif 
