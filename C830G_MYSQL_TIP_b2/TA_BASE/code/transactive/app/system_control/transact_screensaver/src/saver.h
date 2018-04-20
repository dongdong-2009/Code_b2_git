/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/transact_screensaver/src/saver.h $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#if !defined(SAVER_H)
#define SAVER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/system_control/transact_screensaver/src/resource.h"		// main symbols

namespace TA_Base_App
{
    class CSaverApp : public CWinApp
    {
    public:
	    CSaverApp();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSaverApp)
	    public:
	    virtual BOOL InitInstance();
	    virtual int ExitInstance();
	    //}}AFX_VIRTUAL

    // Implementation

	    //{{AFX_MSG(CSaverApp)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
	    enum SCRMODE { SMNONE, SMCONFIG, SMPASSWORD, SMPREVIEW, SMSAVER };

	    SCRMODE GetScrMode(HWND &hWnd) const;

	    void DoConfig(HWND hWndParent) const;
    };
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(SAVER_H)
