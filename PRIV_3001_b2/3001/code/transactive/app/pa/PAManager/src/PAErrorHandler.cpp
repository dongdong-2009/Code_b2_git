/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAErrorHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Provides main entry display handling function for PA Manager
  * typically (default behaviour) messages are sent to message queue 
  * (if possible) and displayed after the code leaves its point of origin
  * to alleviate reentrantthreadlockable issues
  *
  */

#pragma warning(disable:4284)   // For std::set warnings

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TA_Base_Core::ReEntrantThreadLockable PaErrorHandler::s_lock;
std::list<DWORD> PaErrorHandler::s_queuedErrors;
HWND PaErrorHandler::s_primaryWindowHwnd = 0;

TA_Base_Core::ReEntrantThreadLockable PaErrorHandler::s_modalErrorDisplayCountLock;
int PaErrorHandler::s_modalErrorDisplayCount = 0;

// Map the internal error id to error message resource id, may display the same pop up message as defined in SwDS
const UINT PaErrorHandler::s_arrErrorMessages[] = 
{
    IDS_UE_670001, // DATABASE_ERROR
    IDS_UE_670003, // UNSPECIFIED_ERROR
    IDS_UE_670002, // PA_AGENT_FAIL
    IDS_UE_670003, // INVALID_REQUEST_PARAMS
    // IDS_UW_610006, // BROADCAST_ID_NOT_FOUND
    IDS_UI_670003, // BROADCAST_ID_NOT_FOUND
    IDS_UE_670003, // BROADCAST_STILL_ACTIVE
    IDS_UE_670000, // FEPC_HARDWARE_FAILURE
    IDS_UI_670003, // THIS_DVA_BROADCAST_DELETED
    IDS_UE_610010, // INSUFFICIENT_RIGHTS
    IDS_UE_670003, // NO_EXTERNAL_PA_INTERACTION_ALLOWED
    IDS_UE_670005, // INVALID_BROADCAST_NAME
    IDS_UE_670003, // MAXIMUM_BROADCAST_COUNT_REACHED
    IDS_UE_670003, // STATION_CANNOT_RUN_FOR_OVERALL
    IDS_UE_670004, // EMPTY_DESCRIPTION_NOT_ALLOWED
    IDS_UE_670003, // PA_PROTOCOL_FAILURE
    IDS_UE_670003, // UNABLE_TO_PERFORM_REQUEST
    IDS_UE_610018, // AD_HOC_TYPE_NOT_CHANGED
    IDS_UW_610009, // ZONE_GROUP_NAME_RESERVED
    IDS_UE_670006, // INVALID_ZONE_GROUP_NAME
	IDS_UE_670019, //ZONE_NOT_MAPPED

    IDS_UW_050018 // ERROR_MAX_ID_SPECIFIED
};

void PaErrorHandler::displayModalError(EErrorId errorId)
{
    // Check for out of range
    if (errorId >= ERROR_MAX)
    {
        errorId = ERROR_MAX_ID_SPECIFIED;
    }

    try
    {
		internalDisplayMessageBox(s_arrErrorMessages[errorId]);
    }
    catch (...)
    {
        internalDisplayMessageBox(IDS_UE_630017);
    }
}


void PaErrorHandler::internalDisplayMessageBox(const UINT unErrorId)
{
    // Ensure don't hold onto any locks while actually displaying the message
    // box, which is a blocking call (via TransActiveMessage)
    {
        TA_Base_Core::ThreadGuard sync(s_modalErrorDisplayCountLock);
        s_modalErrorDisplayCount ++;
    }

    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg.showMsgBox(unErrorId);

    {
        TA_Base_Core::ThreadGuard sync(s_modalErrorDisplayCountLock);
        s_modalErrorDisplayCount --;
    }
}

void PaErrorHandler::displayError(EErrorId errorId)
{
     TA_Base_Core::ThreadGuard guard(s_lock);

     std::list<DWORD>::iterator itr = std::find( s_queuedErrors.begin(), s_queuedErrors.end(), errorId );

     if (itr != s_queuedErrors.end())
     {
         // This message is already queued for display
         // Don't display it a second time (ie send out notification)
         return;
     }

     s_queuedErrors.push_back( errorId );

     if (0 == s_primaryWindowHwnd || !::IsWindow(s_primaryWindowHwnd))
     {
        // Queue to display when m_primaryWindowHwnd is available
        return;
     }

     // Post out the error notification for window to process
     ::PostMessage(s_primaryWindowHwnd, WM_PA_ERROR_MESSAGE, errorId, 0);
}


bool PaErrorHandler::confirmReceiptOfRequest(DWORD errorId)
{
     TA_Base_Core::ThreadGuard guard(s_lock);

     std::list<DWORD>::iterator itr = std::find( s_queuedErrors.begin(), s_queuedErrors.end(), errorId );
     
     if (itr == s_queuedErrors.end())
     {
        // This id not found
        return false;
     }
     else
     {
         s_queuedErrors.erase(itr);
         return true;
     }
}


void PaErrorHandler::setPrimaryWindowHandle(HWND hWnd)
{
    if (0 != hWnd)
    {
        if( !::IsWindow(hWnd) )
        {
            return;
        }
    }

    TA_Base_Core::ThreadGuard guard(s_lock);

    // Set the window handle AFTERWARDS, to ensure all errors receive foreground status
    s_primaryWindowHwnd = hWnd;
}


bool PaErrorHandler::isDisplayingModalError()
{
    TA_Base_Core::ThreadGuard sync(s_modalErrorDisplayCountLock);
    return s_modalErrorDisplayCount > 0;
}
