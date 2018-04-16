/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISMessageManager.h $
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

#if !defined(TTIS_MESSAGE_BOXES_H)
#define TTIS_MESSAGE_BOXES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#include <map>
#include <vector>


namespace TA_IRS_App
{
    class TTISMessageManager
    {

    public:

        // This is used for the dislayError method.
        // basically, this is for initialisation errors that you only want to
        // appear once. Eg there is no train agent, to stop this popping up multiple
        // times, use displayErrorOnce. Any subsequent calls with the same type won't be displayed.
        // This list will grow as needed.
        enum ErrorType
        {
            NO_TRAIN_AGENT_NO_TRAIN_LISTS,
            UNABLE_TO_LOAD_PREDEFINED,
            UNABLE_TO_LOAD_TIME_SCHEDULES
        };

        static const char* NO_TIS_AGENT_PREDEFINED_UNAVAILABLE;
        static const char* NO_TRAIN_AGENT_TRAIN_LISTS_UNAVAILABLE;

        static const char* ERROR_LOADING_TIME_SCHEDULE;
        static const char* ERROR_LOADING_TIME_SCHEDULE_PKEY;
        static const char* ERROR_LOADING_TIME_SCHEDULE_ENTRY;
            static const char* ERROR_LOADING_TIME_SCHEDULE_ENTRY_END;
        static const char* ERROR_SAVING_SCHEDULE;
        static const char* ERROR_SAVING_SCHEDULE_SHORT;
        static const char* ERROR_DELETING_SCHEDULE;
        static const char* CURRENT_SCHEDULE_DELETED;
        static const char* CURRENT_SCHEDULE_MODIFIED;

        static const char* START_TIME_SAME_AS_END_TIME;
        // xufeng++ 2006/10/14 TD14367
        static const char* START_TIME_AFTER_END_TIME;
        static const char* START_TIME_BEFORE_CURRENT_TIME;
        // xufeng++ 2006/11/07 TD17546
        static const char* OUT_OF_TIME_RANGE;
        // ++xufeng 2006/11/07 TD17546
        // ++xufeng 2006/10/14 TD14367

        static const char* ERROR_LOADING_PREDEFINED;
        static const char* NO_PREDEFINED_LIBRARY;

        static const char* FOLLOWING_ERRORS;

        static const char* ERROR_SENDING_DISPLAY_TRAIN;
        static const char* ERROR_SENDING_CLEAR_TRAIN;
        static const char* ERROR_SENDING_DOWNLOAD_TRAIN;
        static const char* ERROR_SENDING_UPGRADE_TRAIN;

        static const char* TRAIN_TIME_OUT;
        static const char* TRAIN_BUSY;
        // defectID(17924) AVteam++
        // static const char* NO_TIS_AGENT_DISPLAY_ERROR;
        static const char* NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE;
        static const char* NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT;
        // defectID(17924) ++AVteam
        static const char* NO_TIS_AGENT_CLEAR_ERROR;
        static const char* NO_TIS_AGENT_DOWNLOAD_ERROR;
        static const char* NO_TIS_AGENT_UPGRADE_ERROR;

        static const char* NOT_STARTING_ALREADY_CURRENT;
        static const char* TRAIN_CANT_UPGRADE_NOT_CURRENT;
        static const char* NOT_STARTING_ALREADY_NEXT;
        static const char* TRAIN_CANT_UPGRADE_NOT_NEXT;
        static const char* TRAIN_CANT_UPGRADE_ALREADY_UPGRADED;

        static const char* NO_TIS_AGENT;

        static const char* CANT_SET_CURRENT_SCHEDULE;
        static const char* CANT_UPGRADE_VERSION;

        static const char* ERROR_SAVING_FREE_TEXT;

        static const char* SCHEDULE_EDIT_PERMISSIONS_LOST;

        static const char* ASK_SUBMIT_CLEAR_ALL;
        static const char* ASK_SUBMIT_CLEAR_6;
        static const char* ASK_SUBMIT_CLEAR_NORMAL;
        static const char* ASK_SUBMIT_CLEAR_EMERGENCY;

        static const char* ASK_LOSE_UNSAVED_SCHEDULE;
        static const char* ASK_SAVE_SCHEDULE;
        static const char* ASK_DELETE_SCHEDULE;
        static const char* ASK_REMOVE_ENTRY;
        static const char* ASK_REMOVE_ALL_ENTRIES;

        static const char* ASK_DISPLAY_PREDEFINED;
        static const char* ASK_DISPLAY_ADHOC;

        static const char* ASK_DOWNLOAD_MESSAGE_LIBRARY;
        static const char* ASK_UPGRADE_MESSAGE_LIBRARY;
        static const char* ASK_DOWNLOAD_TIME_SCHEDULE;
        static const char* ASK_UPGRADE_TIME_SCHEDULE;
        static const char* ASK_SET_TIME_CURRENT_TIME_SCHEDULE;
		static const char* MAX_SCHEDULE_MESSAGE_REACHED;


        /**
         * ~TTISMessageManager
         *
         * Standard destructor.
         */
        virtual ~TTISMessageManager();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         * Initially, message suppression will be on.
         * This object is not threadlocked, it is displaying message boxes,
         * so should only ever be called from the GUI thread.
         *
         * @return      TTISMessageManager&
         *              A reference to an instance of a TTISMessageManager object.
         *
         *
         */
        static TTISMessageManager& getInstance();


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
        void displayError(const char* message, UINT nType = MB_OK | MB_ICONSTOP);


        /**
          * displayWarning
          *
          * Display a warning message box
          *
          * @param message  The message text
          * @param nType    The dialog box style (default is exclamation, with an ok button)
          *
          */
        void displayWarning(const char* message, UINT nType = MB_OK | MB_ICONEXCLAMATION);


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
        int askQuestion(const char* message, UINT nType = MB_YESNO | MB_ICONQUESTION);

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
        int displayMessage(const char* message, UINT nType = MB_OK | MB_ICONSTOP ) const
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


    private:

        /**
         * TTISMessageManager
         *
         * Private constructors.
         */
        TTISMessageManager();
        TTISMessageManager& operator=(const TTISMessageManager &) {};
        TTISMessageManager( const TTISMessageManager& ) {};


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
        static TTISMessageManager* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // map of already displayed messages
        std::map<ErrorType, bool> m_alreadyDisplayed;

        // a vector of queued messages
        std::vector<std::string> m_queuedMessages;

        // whether messages are being suppressed
        bool m_suppressMessages;

    };

} // end namespace TA_IRS_App

#endif
