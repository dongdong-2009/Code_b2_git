/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// PAManager.cpp : Defines the class behaviors for the application.
//

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PAManager.h"
#include "app/pa/PAManager/src/PAManagerGUI.h"
#include "bus/pa/CachedMaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PAManager/src/GraphworxComms.h"
#include "app/pa/PAManager/src/PaManagerDlg.h"
#include "core/data_access_interface/pa_4669/src/PaAtsTriggeredBroadcastAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageVersionAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaZoneGroupAccessFactory.h"
#include "bus/generic_gui/src/IGUIApplication.h"
#include "app/pa/PAManager/src/PaStatusNameChangeSubscriber.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PAManagerApp

BEGIN_MESSAGE_MAP(PAManagerApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(PAManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PAManagerApp construction

PAManagerApp::PAManagerApp()
: TA_Base_Bus::TransActiveWinApp( new PAManagerGUI(), "PA Manager" )
{
	// Place all significant initialization in InitInstance
}

PAManagerApp::~PAManagerApp()
{
}


int PAManagerApp::ExitInstance()
{
    int result = TransActiveWinApp::ExitInstance(); 

    // Application de-registered, so may go and start up another instance (if restarting)
    if (PAManagerDlg::isApplicationRestartInProgress())
    {   
        attemptRestart();
    }

    // Clean up all static instances that we may (or may not have) used
    // It is expected this will already have been called (before servants 
    // de-activated etc) - call again just in case
    cleanupSingletons();

    return result;
}


void PAManagerApp::cleanupSingletons()
{
    TA_IRS_Bus::CachedMaps::removeInstance();
    CachedConfig::removeInstance();

    TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::removeInstance();
    TA_Base_Core::PaDvaMessageAccessFactory::removeInstance();
    TA_Base_Core::PaDvaMessageVersionAccessFactory::removeInstance();
    TA_Base_Core::PaTrainDvaMessageAccessFactory::removeInstance();
    TA_Base_Core::PaZoneAccessFactory::removeInstance();
    TA_Base_Core::PaZoneGroupAccessFactory::removeInstance();    
}


void PAManagerApp::attemptRestart()
{
    const std::string commandLine = RunParamListener::getInstance().getCommandLine();

    LOG(SourceInfo, 
            DebugUtil::GenericLog,  
            DebugUtil::DebugInfo,  
            "Launching PA Manager with command line: %s", 
            commandLine.c_str());
	if (commandLine.empty())
	{
		LOG(SourceInfo,	DebugUtil::GenericLog,DebugUtil::DebugError, "Launching PA Manager with invalid command line.");
		return;
	}
            
    try
    {
        // These default parameters taken from MainNavigator.gdf
        unsigned long posType = TA_Base_Bus::TA_GenericGui::POS_BOUNDED | 
                                TA_Base_Bus::TA_GenericGui::POS_RELATIVE | 
                                TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;
        unsigned long alignType = TA_Base_Bus::TA_GenericGui::ALIGN_FIT;

        RECT* objectDim = 0;
        
        // Determine left position of the schematic the new instance is going to be running for
        int leftPos = PAManagerDlg::getAppplicationRestartTargetPosition();

        // Obtain dimensions we need to place PA Manager at
        RECT screenDim = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                     TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, 
                                                                     leftPos);

        TA_Base_Bus::AppLauncher::getInstance().launchApplication(PA_MANAGER_GUI_APPTYPE, 
                                                             commandLine,
                                                             posType,
                                                             alignType,
                                                             objectDim,
                                                             &screenDim);
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught in PAManagerApp::attemptRestart");
    }
}

/////////////////////////////////////////////////////////////////////////////
// The one and only PAManagerApp object

PAManagerApp theApp;
