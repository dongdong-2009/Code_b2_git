/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISManager.h $
  * @author:  Robin Aschcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  */

#if !defined(AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_)
#define AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "bus\generic_gui\src\TransActiveWinApp.h"


namespace TA_IRS_App
{

    class CSTISManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
	    CSTISManagerApp();
        virtual BOOL InitInstance();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSTISManagerApp)
	    //}}AFX_VIRTUAL

    // Implementation

	    //{{AFX_MSG(CSTISManagerApp)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_)
