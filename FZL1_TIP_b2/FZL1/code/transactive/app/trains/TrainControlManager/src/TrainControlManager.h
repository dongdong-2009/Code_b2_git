/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainControlManager_H)
#define TrainControlManager_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/trains/TrainControlManager/src/resource.h"

#pragma   comment(linker,   "/FORCE:MULTIPLE")


#include "bus/generic_gui/src/TransActiveWinApp.h"


namespace TA_IRS_App
{

    class TrainControlManagerApp : public TA_Base_Bus::TransActiveWinApp
    {

    public:


        /**
         * Constructor
         */
        TrainControlManagerApp();


        /**
         * Destructor
         */
        ~TrainControlManagerApp();


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TrainControlManagerApp)
        //}}AFX_VIRTUAL

        // Implementation

        //{{AFX_MSG(TrainControlManagerApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(TrainControlManager_H)
