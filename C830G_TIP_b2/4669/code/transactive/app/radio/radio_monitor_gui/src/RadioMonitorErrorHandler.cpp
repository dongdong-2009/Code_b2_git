/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/RadioMonitorErrorHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides main entry display handling function for PA Manager
  * typically (default behaviour) messages are sent to message queue 
  * (if possible) and displayed after the code leaves its point of origin
  * to alleviate reentrantthreadlockable issues
  *
  */

#pragma warning(disable:4284)   // For std::set warnings

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorErrorHandler.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{   
   
    char* RadioMonitorErrorHandler::s_errorMessages[] =    
               {          
               "Unknown error.",
               "Unknown exception caught.",
               "Unidentified Transactive exception caught.",
               "CORBA Communications error.",
               "CORBA Object Resolution error.",
               "Generic Error.",
               "The operation did not complete successfully.",
               "Internal Error.",
               "Operator has insufficient privilege to execute this request.",      // UE-010021
               "Radio API Exception.",
               "Radio Exception: Call not found.",
               "Invalid call monitor update received.",
               "Insufficient resources to perform monitoring.",     // UE-631000

    
               // Do not change this last message
               "Invalid error specification - error ID out of range.",   // Internal RadioMonitorErrorHandler error
               };
    
    RadioMonitorErrorHandler::RadioMonitorErrorHandler()
    :
    m_primaryWindowHwnd(0),
    m_modalErrorDisplayCount(0)
    {
    }

    RadioMonitorErrorHandler& RadioMonitorErrorHandler::getInstance()
    {
        static RadioMonitorErrorHandler s;

        return s;
    }


    void RadioMonitorErrorHandler::removeInstance()
    {
        // Nothing to do
    }


    void RadioMonitorErrorHandler::displayModalError(EErrorId errorId)
    {
        // Check for out of range
        if (errorId >= ERROR_MAX)
        {
            TA_ASSERT(FALSE, "Invalid error display condition");
            errorId = ERROR_MAX_ID_SPECIFIED;
        }
    
        try
        {
            char* error = s_errorMessages[errorId];    
            internalDisplayMessageBox(error);
        }
        catch (...)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			std::string errMsg = userMsg.constructMessage(IDS_UE_630017);

			/*internalDisplayMessageBox("Internal error displaying dialog");*/
			internalDisplayMessageBox(errMsg.c_str());
			// ++ TD14164 
            
            return;
        }
    }
    
    
    void RadioMonitorErrorHandler::internalDisplayMessageBox(const std::string& errorStr)
    {
        {
            TA_Base_Core::ThreadGuard sync(m_modalErrorDisplayCountLock);
            m_modalErrorDisplayCount ++;
        }       
         
        TA_Base_Core::ThreadGuard guard(m_lock);

        DWORD flags = MB_OK;
    
        if (0 == m_primaryWindowHwnd)
        {
            // Prior to window creation, ensure all error messages displayed in foreground
            flags |= MB_TOPMOST;
        }

        AfxMessageBox(errorStr.c_str());
    
        {
            TA_Base_Core::ThreadGuard sync(m_modalErrorDisplayCountLock);
            m_modalErrorDisplayCount --;
        }
    }
    
    void RadioMonitorErrorHandler::displayError(EErrorId errorId)
    {
         TA_Base_Core::ThreadGuard guard(m_lock);
    
         std::set<DWORD>::iterator itr = m_queuedErrors.find(errorId);
    
         if (!m_queuedErrors.insert(std::set<DWORD>::value_type(errorId)).second)
         {
             // This message is already queued for display
             // Don't display it a second time (ie send out notification)
             return;
         }
            
         if (0 == m_primaryWindowHwnd || !::IsWindow(m_primaryWindowHwnd))
         {
            return;
         }

         // Post out the error notification for window to process
         ::PostMessage(m_primaryWindowHwnd, WM_RM_ERROR_MESSAGE, errorId, 0);     
    }
    
    
    bool RadioMonitorErrorHandler::confirmReceiptOfRequest(DWORD id)
    {
         TA_Base_Core::ThreadGuard guard(m_lock);
        
         std::set<DWORD>::iterator itr = m_queuedErrors.find(id);
         
         if (itr == m_queuedErrors.end())
         {
            // This id not found
            return false;
         }
         else
         {
             m_queuedErrors.erase(itr);
             return true;
         }
    }
    
    
    void RadioMonitorErrorHandler::setPrimaryWindowHandle(HWND hWnd)
    {
        if (0 != hWnd)
        {
            TA_ASSERT(::IsWindow(hWnd), "Invalid window handle");
        }
    
        TA_Base_Core::ThreadGuard guard(m_lock);
    
        // Display all un-acknowledged errors in queue whenever changing handle        
        for (std::set<DWORD>::iterator itr = m_queuedErrors.begin();
                                       itr != m_queuedErrors.end();
                                       itr ++)
        {
            displayModalError(EErrorId(*itr));
        }
        
        // Set the window handle AFTERWARDS, to ensure all errors receive foreground status
        m_primaryWindowHwnd = hWnd;

        // Clear existing items in queue
        m_queuedErrors.clear();
    }
    
    
    bool RadioMonitorErrorHandler::isDisplayingModalError()
    {
        TA_Base_Core::ThreadGuard sync(m_modalErrorDisplayCountLock);
        return m_modalErrorDisplayCount > 0;
    }


    
    // This was turned into a macro, to avoid maintenance confusion (the code itself works fine)
    // (the exception is not deleted twice)..
    /*
    void RadioMonitorErrorHandler::defaultCatchHandler(const char* logMessage)
    {
        __try
        {
            internalCatchHandler(logMessage);
        }
        __finally
        {
            // This suppresses stack cleanup (so exception is not deleted)
            return;
        }
    }
    
    void RadioMonitorErrorHandler::internalCatchHandler(const char* logMessage)
    {        
        if (0 == logMessage || strlen(logMessage) == 0)
        {
            logMessage = "defaultCatchHandler";
        }

        try
        {   
            // There should be an exception on the stack we can rethrow and catch
            throw;
        }
        catch (const TA_Base_Core::ObjectResolutionException& e)
        {
            std::ostringstream ostr;
            ostr << "ObjectResolutionException: " << e.what();

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);
            displayError(RESOLUTION_COMMS_ERROR);
            throw;
        }
        catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "radioAPIException", logMessage);
            displayError(RADIO_API_ERROR);
            throw;
        }
        catch (const TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "invalidCallIDException", logMessage);
            displayError(RADIO_CALL_NOT_FOUND);
            throw;
        }
        catch (const TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException&)
        {
             LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "accessDeniedException", logMessage);  
             displayError(INSUFFICIENT_RIGHTS);         
            throw;
        }
        catch (const CORBA::Exception& e)
        {
            std::ostringstream ostr;
            ostr << "CORBA Exception: " << e._name();

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);
            displayError(GENERIC_COMMS_ERROR);
            throw;
        }
        catch (const TA_Base_Core::TransactiveException& e)
        {
            std::ostringstream ostr;
            ostr << "TransactiveException: " << e.what();

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);
            displayError(UNKNOWN_TRANSACTIVE_EXCEPTION);
            throw;
        }
        catch (const std::exception& e)
        {
            std::ostringstream ostr;
            ostr << "std::exception: " << e.what();

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, ostr.str().c_str(), logMessage);
            displayError(UNKNOWN_EXCEPTION);
            throw;
        }
        catch (...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", logMessage);
            displayError(UNKNOWN_EXCEPTION);
            throw;
        }

        // If we didn't throw it again, exception will be deleted (and then be
        // deleted for a second time in outer catch handler - not what we want)
        TA_ASSERT(FALSE, "Exception must be thrown from catch handler");
    }
    */
    

} // end namespace TA_IRS_App
