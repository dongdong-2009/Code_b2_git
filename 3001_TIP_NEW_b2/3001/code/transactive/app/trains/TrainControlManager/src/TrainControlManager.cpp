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


#include "app/trains/TrainControlManager/src/StdAfx.h"
#include "app/trains/TrainControlManager/src/TrainControlManager.h"
#include "app/trains/TrainControlManager/src/TrainControlManagerGui.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

    BEGIN_MESSAGE_MAP( TrainControlManagerApp, CWinApp )
        //{{AFX_MSG_MAP(TrainControlManagerApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
    END_MESSAGE_MAP()



    TrainControlManagerApp::TrainControlManagerApp()
        : TransActiveWinApp( new TrainControlManagerGui(), 
                             "Trn Update Manager" )
    {
        FUNCTION_ENTRY( "TrainControlManagerApp" );
        FUNCTION_EXIT;
    }

    
    TrainControlManagerApp::~TrainControlManagerApp()
    {
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // The one and only TrainControlManagerApp object

    TrainControlManagerApp theApp;
}
