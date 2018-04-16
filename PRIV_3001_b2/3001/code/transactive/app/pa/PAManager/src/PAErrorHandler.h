/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAErrorHandler.h $
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

#if !defined(AFX_PaErrorHandler_H__782A623D_5E93_48E5_9CD4_8B98B723D98A__INCLUDED_)
#define AFX_PaErrorHandler_H__782A623D_5E93_48E5_9CD4_8B98B723D98A__INCLUDED_

#include <list>

class PaErrorHandler
{
public:

    // PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
    enum EErrorId
    {
        DATABASE_ERROR = 0,                 // Database error	UE-010007
        UNSPECIFIED_ERROR,              // Unspecified errors	UE-010010
        PA_AGENT_FAIL,                  // Failure to contact PA Agent	UE-610001
        INVALID_REQUEST_PARAMS,         // Invalid request parameters.	UE-610005
        BROADCAST_ID_NOT_FOUND,         // Broadcast not found	UE-610006
        BROADCAST_STILL_ACTIVE,         // Broadcast must be terminated first.	UE-610007
        FEPC_HARDWARE_FAILURE,          // Failure to contact PAS FEPC hardware	UE-610008
        THIS_DVA_BROADCAST_DELETED,     // While DVA Broadcast tab open, it was deleted by another operator
        INSUFFICIENT_RIGHTS,            // User does not have privileges required to do something
        NO_EXTERNAL_PA_INTERACTION_ALLOWED,
        INVALID_BROADCAST_NAME,         // Invalid string for a broadcast ID
        MAXIMUM_BROADCAST_COUNT_REACHED,    // Too many active broadcasts, unable to create another
        STATION_CANNOT_RUN_FOR_OVERALL,
        EMPTY_DESCRIPTION_NOT_ALLOWED,
        PA_PROTOCOL_FAILURE,
        UNABLE_TO_PERFORM_REQUEST,
        AD_HOC_TYPE_NOT_CHANGED,
        ZONE_GROUP_NAME_RESERVED,
        INVALID_ZONE_GROUP_NAME,
		ZONE_NOT_MAPPED,

        /// Do not add anything after this point
        ERROR_MAX_ID_SPECIFIED,
        ERROR_MAX
    };


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
    static void displayModalError(EErrorId errorId);

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
    static void displayError(EErrorId errorId);

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
    static bool confirmReceiptOfRequest(DWORD id);    

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
    static void setPrimaryWindowHandle(HWND hWnd);

    /**
     * isDisplayingModalError
     *
     * @return true if displayModalError function is actively 
     *  displaying an TransActiveMessage (from any thread) - use
     *  reference counting to handle nested or concurrent requests
     *
     */
    static bool isDisplayingModalError();


private:

    /**
     * internalDisplayMessageBox
     *
     * Pops up a message box
     *
     * @param errorStr the string to display in the message box
     *
     */
    static void internalDisplayMessageBox(const UINT errorId);

    static TA_Base_Core::ReEntrantThreadLockable s_lock;
    static std::list<DWORD> s_queuedErrors;
    static HWND    s_primaryWindowHwnd;
    
    static TA_Base_Core::ReEntrantThreadLockable s_modalErrorDisplayCountLock;
    static int s_modalErrorDisplayCount;

    static const UINT s_arrErrorMessages[];
};


#endif // !defined(AFX_PaErrorHandler_H__782A623D_5E93_48E5_9CD4_8B98B723D98A__INCLUDED_)
