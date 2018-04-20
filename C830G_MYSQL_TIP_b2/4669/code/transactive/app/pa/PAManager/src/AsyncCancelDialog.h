/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/AsyncCancelDialog.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Simple class that displays a message box with a cancel button, in a separate thread
  *     (to allow for an asynchronous cancel operation)
  *
  */

#pragma once

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>
#include <afxmt.h>

interface IAsyncCancelCondition;

// All functions are threadsafe
class AsyncCancelDialog : public CDialog
{
public:
    
    /**
     * AsyncCancelDialog (constructor)
     *
     * @param hWnd the handle to the parent window
     *
     * @param interval the timer interval on which to execute callback
     *
     * @param callback the interface defining the object on which
     *      to call the callback function on
     *
     */
    AsyncCancelDialog(CWnd* parentHWnd, DWORD interval, IAsyncCancelCondition* callback);

    virtual ~AsyncCancelDialog();

protected:


	// Generated message map functions
	//{{AFX_MSG(AsyncCancelDialog)
    afx_msg void OnTimer( UINT nIDEvent );
    afx_msg BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    
    /// Internal helper function to indicate progress (with ...)
    void updateProgress();

    UINT    m_hTimer;
    DWORD   m_interval;
    IAsyncCancelCondition* m_callback;

    std::string m_originalText;
};