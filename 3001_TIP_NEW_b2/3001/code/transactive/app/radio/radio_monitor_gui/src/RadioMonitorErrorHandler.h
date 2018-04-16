/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorErrorHandler.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides main entry display handling function for the Radio Monitor
  * typically (default behaviour) messages are sent to message queue 
  * (if possible) and displayed after the code leaves its point of origin
  * to alleviate reentrantthreadlockable issues
  *
  */

#pragma once

#include <set>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    class RadioMonitorErrorHandler
    {
    public:
    
        enum EErrorId
        {
            UNKNOWN_ERROR,
            UNKNOWN_EXCEPTION,
            UNKNOWN_TRANSACTIVE_EXCEPTION,
            GENERIC_COMMS_ERROR,
            RESOLUTION_COMMS_ERROR,
            GENERIC_NONFATAL_ERROR,
            GENERIC_REQUEST_FAILURE,
            INTERNAL_FAILURE,
            INSUFFICIENT_RIGHTS,
            RADIO_API_ERROR,
            RADIO_CALL_NOT_FOUND,
            INVALID_MONITOR_DATA_RECEIVED,
            INSUFFICIENT_RESOURCES_TO_MONITOR,

            /// Do not add anything after this point
            ERROR_MAX_ID_SPECIFIED,
            ERROR_MAX
        };
        
        /**
         * RadioMonitorErrorHandler (constructor)
         */
        RadioMonitorErrorHandler();

        static RadioMonitorErrorHandler& getInstance();
        static void removeInstance();
        
        /**
         * defaultCatchHandler
         *
         * Provides a catch handler than handles all known exception
         *  types, converting each exception into a suitable error message
         *
         * @param logMessage (optional) User can provide a string message to be
         *          logged
         */
        //void defaultCatchHandler(const char* logMessage = 0);

        /**
         * displayModalError
         *
         * Displays an error in a modal frame.  Typically user should not call
         *  this directly, unless they know for a fact the current point in code
         *  will be OK if blocked (if the current point in code is acknowledgement
         *  of a network message, it's typically not safe to display a modal error dialog)
         * Since the modal error dialog doesn't ever require a yes/no answer
         *  there is no need to have it pop up at the instance where the problem
         *  occurred, this function shouldn't need to be used (ever), and always
         *  safer to use the default implementation (displayError)
         *
         * @param errorId the identifier of the error message to display
         *
         */
        void displayModalError(EErrorId errorId);
    
        /**
         * displayError
         *
         * default error display handler for PA Agent.  The error will be displayed
         *  immediately if there is a primary window attached to error handler, 
         *  otherwise the error will be posted to said window (assumed the window
         *  will receive the notification and display the error at that point)
         *
         * @param errorId the identifier of the error message to display
         *
         */
        void displayError(EErrorId errorId);
    
        /**
         * confirmReceiptOfRequest
         *
         * Called to confirm a notification sent out to the primary window
         * has been received (for internal checking purposes).
         *
         * @return false if the error handler doesn't recognize the input
         *          (which means the error has already been displayed to user)
         *
         */
        bool confirmReceiptOfRequest(DWORD id);    
    
        /**
         * setPrimaryWindowHandle
         * 
         * Sets the primary error dialog notification window, 0 to unset
         *  the window (in which case errors displayed immediately)
         * Whenever set, if there are errors remaining in queue, they
         *  will be displayed (immediately), to ensure they're not lost
         *
         * @pre handle is a valid window (if non zero)
         *
         */
        void setPrimaryWindowHandle(HWND hWnd);
    
        /**
         * isDisplayingModalError
         *
         * @return true if displayModalError function is actively 
         *  displaying an AfxMessageBox (from any thread) - use
         *  reference counting to handle nested or concurrent requests
         *
         */
        bool isDisplayingModalError();
    
    private:
        
        /**
         * @see defaultCatchHandler (functions split up to allow additional catch layer)
         */
        //void internalCatchHandler(const char* logMessage = 0);

        RadioMonitorErrorHandler(const RadioMonitorErrorHandler&);
        RadioMonitorErrorHandler& operator=(const RadioMonitorErrorHandler&);

        /**
         * internalDisplayMessageBox
         *
         * Pops up a message box
         *
         * @param errorStr the string to display in the message box
         *
         */
        void internalDisplayMessageBox(const std::string& errorStr);
    
        TA_Base_Core::ReEntrantThreadLockable m_lock;
        std::set<DWORD> m_queuedErrors;
        HWND    m_primaryWindowHwnd;
        
        TA_Base_Core::ReEntrantThreadLockable m_modalErrorDisplayCountLock;
        int    m_modalErrorDisplayCount;
        
        static char* s_errorMessages[];
    };

}   // end namespace TA_IRS_App
 

// Conversion of defaultCatchHandler function..
#define DEFAULT_RM_CATCH_HANDLER(logMessage)\
    catch (const TA_Base_Core::ObjectResolutionException& e)\
    {\
        std::ostringstream ostr;\
        ostr << "ObjectResolutionException: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::RESOLUTION_COMMS_ERROR);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "radioAPIException", logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::RADIO_API_ERROR);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException&)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "invalidCallIDException", logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::RADIO_CALL_NOT_FOUND);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException&)\
    {\
         LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "accessDeniedException", logMessage);\
         RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::INSUFFICIENT_RIGHTS);\
    }\
    catch (const CORBA::Exception& e)\
    {\
        std::ostringstream ostr;\
        ostr << "CORBA Exception: " << e._name();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::GENERIC_COMMS_ERROR);\
    }\
    catch (const TA_Base_Core::TransactiveException& e)\
    {\
        std::ostringstream ostr;\
        ostr << "TransactiveException: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::UNKNOWN_TRANSACTIVE_EXCEPTION);\
    }\
    catch (const std::exception& e)\
    {\
        std::ostringstream ostr;\
        ostr << "std::exception: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::UNKNOWN_EXCEPTION);\
    }\
    catch (...)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", logMessage);\
        RadioMonitorErrorHandler::getInstance().displayError(RadioMonitorErrorHandler::UNKNOWN_EXCEPTION);\
    }
