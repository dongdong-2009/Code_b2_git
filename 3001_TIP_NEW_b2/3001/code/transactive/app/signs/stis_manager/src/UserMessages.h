/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/UserMessages.h $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/06 19:23:10 $
  * Last modified by:  $Author: huang.wenbo $
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
using boost::wformat;
using boost::io::group;
using boost::str;


namespace TA_IRS_App
{
    #define CATCH_ALL_EXCEPTIONS(action)\
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_CONNECTION_STIS);\
                }\
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationFailureException", "" );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_CONNECTION_STIS);\
                }\
				catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException& )\
	            {\
				    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISGeneralFailureException", "");\
					UserMessages::getInstance().displayError(UserMessages::ERROR_PERFORM_REQUEST);\
                }\
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_CONNECTION_STIS);\
                }\
                catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in() );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_PERFORM_REQUEST);\
                }\
				catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException& ex)\
				{\
                    LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", "");\
					UserMessages::getInstance().displayError(UserMessages::ERROR_PERFORM_REQUEST);\
				}\
                catch ( const TA_Base_Core::ObjectResolutionException& ore )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_CONTACT_AGENT);\
                }\
                catch ( const CORBA::Exception& ce )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );\
					UserMessages::getInstance().displayError(UserMessages::ERROR_CONTACT_AGENT);\
                }\
		        catch (const TA_Base_Core::DataException& e)\
		        {\
		            LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", e.what());\
	            }\
		        catch (const TA_Base_Core::DatabaseException& e)\
		        {\
		            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", e.what());\
					UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);\
				}\
                catch ( ... )\
                {\
                    LOG_EXCEPTION_CATCH(SourceInfo, "...", action );\
                }

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
            UNABLE_TO_LOAD_PREDEFINED,
			UNABLE_TO_CONNECT_DATABASE
        };


        // MESSAGES
        static const char* NO_TIS_AGENT_PREDEFINED_UNAVAILABLE;
        static const char* INFO_REQUEST_SUCCESSFUL;
        static const char* QUESTION_DISPLAY;
		static const wchar_t* QUESTION_DISPLAY_W; // For unicode string(predefined)
        static const char* QUESTION_CLEAR;
        static const char* QUESTION_DELETE_PID_GROUP;
        static const char* QUESTION_UPGRADE_LIBRARY;
		static const char* QUESTION_DOWNLOAD_TIME_SCHEDULE;
        static const char* ERROR_NO_AGENT_PREDEFINED;
        static const char* ERROR_NO_PREDEFINED_IN_DB;
        static const char* ERROR_LOADING_PREDEFINED;
        static const char* ERROR_START_TIME_AFTER_END;
        static const char* ERROR_START_TIME_EQUALS_END;
        static const char* ERROR_START_TIME_BEFORE_CURRENT_TIME;
        static const char* ERROR_END_TIME_PASSED;
        static const char* ERROR_REQUEST_FAILED;
        static const char* ERROR_UPGRADE_FAILED;
		static const char* ERROR_REPEAT_INTERVAL_INVALID;


		//Added error 955
		static const char* ERROR_CONNECTION_STIS;
		static const char* ERROR_CONNECTION_DATABASE;
		static const char* ERROR_CONTACT_AGENT;
		static const char* ERROR_PERFORM_REQUEST;



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


		int askQuestion(const wchar_t* message, UINT nType = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);



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
        std::string breakMessage( std::string theString );



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

		CWnd *m_parent;  //message box's parent window
    };

} // end namespace TA_IRS_App

#endif 
