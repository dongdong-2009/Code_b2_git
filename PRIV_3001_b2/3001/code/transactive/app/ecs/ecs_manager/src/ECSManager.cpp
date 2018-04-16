/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ECSManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Derived from the TransActiveWinApp, this is the main application class for the ECSManager.
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/ECSManagerGUI.h"

#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// ECSManagerApp

namespace TA_IRS_App
{
    BEGIN_MESSAGE_MAP(ECSManagerApp, TransActiveWinApp)
        //{{AFX_MSG_MAP(ECSManagerApp)
            // NOTE - the ClassWizard will add and remove mapping macros here.
            //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
    END_MESSAGE_MAP()


    //
    // Constructor
    //
    ECSManagerApp::ECSManagerApp() 
    : TransActiveWinApp( new ECSManagerGUI(), "ECS Manager" )
    {
    }
    
    
    //
    // InitInstance
    //
    BOOL ECSManagerApp::InitInstance()
    {
        // Ensures that the mandatory command-line parameter that determines the ECS dialog type 
        // for this application instance (AND hence the corresponding application type)
        // should be set as a runparam before the framework demands the application type from 
        // ECSManagerGUI
        std::string cmdLine(m_lpCmdLine);
        TA_Base_Core::RunParams::getInstance().parseCmdLine( cmdLine.c_str() );

        // now let the TransActive framework kick in.
        return TransActiveWinApp::InitInstance();
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only ECSManagerApp object

    ECSManagerApp theApp;

} // namespace TA_IRS_App
