/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TTISManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Main application
  *
  */


#if !defined(AFX_TISMANAGER_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_)
#define AFX_TISMANAGER_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// TTISManagerApp:
// See TISManager.cpp for the implementation of this class
//

namespace TA_IRS_App
{


    class TTISManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        TTISManagerApp();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(TTISManagerApp)
        public:
        // }}AFX_VIRTUAL

    // Implementation

        // {{AFX_MSG(TTISManagerApp)
            // NOTE - the ClassWizard will add and remove member functions here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TISMANAGER_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_)
