/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManagerErrorHandler.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Central point for handling of error messages displayed by the Radio Manager application
  * 
  */

#pragma once

#include <string>
#include <set>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

//namespace TA_IRS_App
//{

    class RadioManagerErrorHandler
    {
    public:
    
        enum EErrorId
        {            
            /// Do not add anything after this point
            ERROR_MAX_ID_SPECIFIED,
            ERROR_MAX
        };
        
        /**
         * RadioManagerErrorHandler (constructor)
         */
        RadioManagerErrorHandler();

        static RadioManagerErrorHandler& getInstance();
        static void removeInstance();
        
        /** 
         * Allows app defined implementation of AfxMessageBox
         */
        int AfxMessageBox( 
                LPCTSTR lpszText, 
                UINT nType = MB_OK, 
                UINT nIDHelp = 0 );
                
        /** 
         * Allows app defined implementation of MessageBox
         */
        int MessageBox(
                HWND hWnd,          
                LPCTSTR lpText,     
                LPCTSTR lpCaption,  
                UINT uType);          
        

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

        RadioManagerErrorHandler(const RadioManagerErrorHandler&);
        RadioManagerErrorHandler& operator=(const RadioManagerErrorHandler&);

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

//}   // end namespace TA_IRS_App

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"

// Provides a catch handler that catches and logs the exception without displaying an error message
#define SILENT_RM_CATCH_HANDLER(logMessage)\
    catch (const TA_Base_Core::ObjectResolutionException& e)\
    {\
        std::ostringstream ostr;\
        ostr << "ObjectResolutionException: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "radioAPIException", logMessage);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException&)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "invalidCallIDException", logMessage);\
    }\
    catch (const TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException&)\
    {\
         LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "accessDeniedException", logMessage);\
    }\
    catch (const CORBA::Exception& e)\
    {\
        std::ostringstream ostr;\
        ostr << "CORBA Exception: " << e._name();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
    }\
    catch (const TA_Base_Core::TransactiveException& e)\
    {\
        std::ostringstream ostr;\
        ostr << "TransactiveException: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
    }\
    catch (const std::exception& e)\
    {\
        std::ostringstream ostr;\
        ostr << "std::exception: " << e.what();\
    \
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);\
    }\
    catch (...)\
    {\
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", logMessage);\
    }
