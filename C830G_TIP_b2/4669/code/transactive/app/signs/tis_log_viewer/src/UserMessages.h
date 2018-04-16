/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/UserMessages.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * A single way to display messages,
  * Provides a way to queue up messages during initialisation.
  */

#if !defined(TTIS_MESSAGE_BOXES_H)
#define TTIS_MESSAGE_BOXES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#include <map>
#include <vector>


namespace TA_IRS_App
{
    class UserMessages
    {

    public:

        /**
         * ~UserMessages
         *
         * Standard destructor.
         */
        virtual ~UserMessages();

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
        void displayInfo(const char* message, UINT nType = MB_OK | MB_ICONINFORMATION | MB_TOPMOST);


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
        int displayMessage(const char* message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST ) const
        {
            // TD14164 ++
            TA_Base_Bus::TransActiveMessage userMsg;
            CString reason = message;
            userMsg << reason;
            CString errMsg = userMsg.constructMessage(IDS_UE_020071);
            /*return AfxMessageBox(message, nType);*/
            return AfxMessageBox(errMsg, nType);
            // ++ TD14164
        }


        // class instance
        static UserMessages* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // a vector of queued messages
        std::vector<std::string> m_queuedMessages;

        // whether messages are being suppressed
        bool m_suppressMessages;

    };

} // end namespace TA_App

#endif
