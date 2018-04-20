/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManagerErrorHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Central point for handling of error messages displayed by the Radio Manager application
  * 
  */

#pragma warning(disable:4284 4786)   // For std::set warnings

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManagerErrorHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// namespace TA_IRS_App
//{   
   
    char* RadioManagerErrorHandler::s_errorMessages[] =    
               {              
               // Do not change this last message
               "Invalid error specification - error ID out of range.",   // Internal RadioManagerErrorHandler error
               };
    

    RadioManagerErrorHandler::RadioManagerErrorHandler()
    :
    m_primaryWindowHwnd(0),
    m_modalErrorDisplayCount(0)
    {
    }

    RadioManagerErrorHandler& RadioManagerErrorHandler::getInstance()
    {
        static RadioManagerErrorHandler s;

        return s;
    }


    void RadioManagerErrorHandler::removeInstance()
    {
        // Nothing to do
    }


    void RadioManagerErrorHandler::displayModalError(EErrorId errorId)
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
    
    
    void RadioManagerErrorHandler::internalDisplayMessageBox(const std::string& errorStr)
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
    

    void RadioManagerErrorHandler::displayError(EErrorId errorId)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UW_632026);
        /*AfxMessageBox("RadioManagerErrorHandler: displayError function not defined");*/
		AfxMessageBox(errMsg);
		// ++ TD14164 

        return;

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
        //::PostMessage(m_primaryWindowHwnd, 0WM_RM_ERROR_MESSAGE, errorId, 0);     
    }
    
    
    bool RadioManagerErrorHandler::confirmReceiptOfRequest(DWORD id)
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
    
    
    void RadioManagerErrorHandler::setPrimaryWindowHandle(HWND hWnd)
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
    
    
    bool RadioManagerErrorHandler::isDisplayingModalError()
    {
        TA_Base_Core::ThreadGuard sync(m_modalErrorDisplayCountLock);
        return m_modalErrorDisplayCount > 0;
    }


    int RadioManagerErrorHandler::AfxMessageBox( 
        LPCTSTR lpszText, 
        UINT nType, 
        UINT nIDHelp)
    {              
        TA_Base_Core::ThreadGuard guard(m_lock);
        if (0 == m_primaryWindowHwnd)
        {
            // Prior to window creation, ensure all error messages displayed in foreground
            nType |= MB_TOPMOST;
        }

        return ::AfxMessageBox(lpszText, nType, nIDHelp);
    }


    int RadioManagerErrorHandler::MessageBox(
                HWND hWnd,          
                LPCTSTR lpText,     
                LPCTSTR lpCaption,  
                UINT uType)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        if (0 == m_primaryWindowHwnd)
        {
            // Prior to window creation, ensure all error messages displayed in foreground
            uType |= MB_TOPMOST;
        }

        return ::MessageBox(hWnd, lpText, lpCaption, uType);
    }

//} // end namespace TA_IRS_App
