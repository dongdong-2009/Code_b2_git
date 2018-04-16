/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/ReadSDSMessageDialog.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides class to view and (optionally) confirm/acknowledge SDS Messages.
  * In accordance with TES #497:
  * SDS Messages should be displayed in a non-modal dialog.
  * Only 1 SDS Message can be displayed at a time.
  * If attempt to display a second SDS Message, the first is automatically cancelled
  *     (that is, NOT acknowledged, so it remains in call stack)
  *
  * The associated resource has the capacity to display 140 'W' characters within
  * the text area.  If need more than this, need to expand size of the 
  * IDD_VIEW_TEXT_MESSAGE resource
  *
  * The message is shown from a modeless dialog box, only call methods from the 
  * primary windows thread (this is not a threadsafe class, shouldn't need to be)
  *
  * We don't need to worry about this call getting acknowledged from an external
  * source (before we acknowledge it from the dialog), because for us to
  * be viewing the SDS message, we must own it (and will be the only party that
  * can acknowledge it)
  */

#include "StdAfx.h"
#include "ReadSDSMessageDialog.h"
#include "RadioManagerDlg.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

namespace TA_IRS_App
{

    ReadSDSMessageDialog::ReadSDSMessageDialog()
     : CDialog(IDD_VIEW_TEXT_MESSAGE, 0),
        m_callID(-1)
    {
        confirmInMainThread();

        // Create immediately (hidden)
        Create(IDD_VIEW_TEXT_MESSAGE);        
    }

    
    void ReadSDSMessageDialog::confirmInMainThread()
    {
        if( AfxGetApp()->m_nThreadID != GetCurrentThreadId())
        {
            TA_ASSERT(FALSE, "Attempting to interact with dialog outside main thread");
        }
    }


    void ReadSDSMessageDialog::displaySDSMessage(long callID)
    {
        confirmInMainThread();        

        if (IsWindowVisible())
        {
            if (m_callID == callID)
            {
                // Same call, just show window (to bring into focus)
                SetFocus();
                return;               
            }

            // Window currently displaying a different SDS Message, simply cancel the
            // last SDS Message
            OnCancel();
        }

        m_callID = callID;

        char *message = 0;
        try
        {
            CWaitCursor wait;

            // Extract call details from manager
            CRadioManagerDialog* pMgr = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->m_pMainWnd);

            if (0 == pMgr)
            {
                throw std::exception("Radio Manager Dialog is NULL");
            }

			// Get the message text
			CORBA_CALL_RETURN( message,
			                   pMgr->getRadioCorbaDef(),
			                   getCallText,
			                   ( callID, ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );
            SetDlgItemText(IDC_TEXT_MESSAGE, message);

		    delete message;
            message = 0;

            // Display window with text
            ShowWindow(SW_SHOW);
			//TD17300++
			CRect ParentRc,ChildRc;
			this->GetClientRect(&ChildRc);
			pMgr->GetWindowRect(&ParentRc);
			//MoveWindow(ParentRc.left,ParentRc.top,ChildRc.Width(),ChildRc.Height());
			::SetWindowPos(this->m_hWnd,HWND_TOPMOST, ParentRc.left+ (ParentRc.Width() - ChildRc.Width())/2,
													  ParentRc.top + (ParentRc.Height() - ChildRc.Height())/2,
													   ChildRc.Width(),ChildRc.Height(), SWP_NOSIZE ); 
			//++TD17300
        }
        SILENT_RM_CATCH_HANDLER("Error retrieving SDS message.");

        if (!IsWindowVisible())
        {
            // If window not shown, must have been an error

			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "retrieving";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632027);
            /*RadioManagerErrorHandler::getInstance().MessageBox(
                            m_hWnd, 
                            "Error retrieving SDS message.",
                            "Radio Manager", 
                            MB_OK | MB_ICONEXCLAMATION);*/
			RadioManagerErrorHandler::getInstance().MessageBox(
                            m_hWnd, 
                            errMsg,
                            "Radio Manager", 
                            MB_OK | MB_ICONEXCLAMATION);
			// ++ TD14164 			
        }            
    }


    void ReadSDSMessageDialog::OnOK()
    {
        // User wanting to acknowledge message
        try
        {
            CWaitCursor wait;
                    
            CRadioManagerDialog* pMgr = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->m_pMainWnd);

            if (0 == pMgr)
            {
                throw std::exception("Radio Manager Dialog is NULL");
            }

            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

            pMgr->addCallToHistory(m_callID);
			CORBA_CALL( pMgr->getRadioSessionCorbaDef(),
			            releaseCall,
			            ( m_callID, sessionId.c_str() ) );
            // Only hide window on successful acknowledgement
            ShowWindow(SW_HIDE);
        }
        SILENT_RM_CATCH_HANDLER("Failed to release SDS Message");

        if (IsWindowVisible())
        {
            // Operation failed (dialog will be left open, so operator can try again, or cancel)

			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "acknowledging";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632027);
			/*RadioManagerErrorHandler::getInstance().MessageBox(
                            m_hWnd, 
                            "Error acknowledging SDS message.",
                            "Radio Manager", 
                            MB_OK | MB_ICONEXCLAMATION);*/
			RadioManagerErrorHandler::getInstance().MessageBox(
                            m_hWnd, 
                            errMsg,
                            "Radio Manager", 
                            MB_OK | MB_ICONEXCLAMATION);
			// ++ TD14164 
        }
    }


    void ReadSDSMessageDialog::OnCancel()
    {
        // User didn't want to acknowledge, just hide window
        ShowWindow(SW_HIDE);
    }

    /*
    // No need to listen out for TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Deleted notification
    // This operator is the only person that can acknowledge the message
    void ReadSDSMessageDialog::setRadioName(std::string radioName)
    {
     	FUNCTION_ENTRY("setRadioName");
    
        try
        {        
        	// Determine entity key for the local radio agent
        	// This is used to filter the incoming notifications from the radio agents
        	std::auto_ptr<TA_Base_Core::IEntityData> entity(
                        TA_Base_Core::EntityAccessFactory::getInstance().getEntity(radioName));

        	TA_ASSERT(entity.get() != NULL,"Radio entity could not be found.");

        	unsigned long radioEntityKey = entity->getKey();
            unsigned long radioEntityLocation = entity->getLocation();
        	
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
                            TA_Base_Core::RadioComms::RadioCallStatus, 
                            this, 
                            m_radioEntityKey, 
                            0, 
                            m_radioEntityLocation, 
                            NULL );     		
    	}
        SILENT_RM_CATCH_HANDLER("Unable to initialise subscriber");
	}    
    */

}   // end namespace TA_IRS_App
