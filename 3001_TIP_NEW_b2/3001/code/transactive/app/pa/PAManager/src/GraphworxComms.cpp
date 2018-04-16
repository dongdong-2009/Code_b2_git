/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/GraphworxComms.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides interface to graphworx displays
  *
  */

#include "StdAfx.h"
#include "app/pa/PAManager/src/GraphworxComms.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/RunParams.h"
#include <sstream>

#define RPARAM_NO_GRAPHWORX "NoGraphworx"

// Reset the state of the graphworx displays on startup to become inactive PaManagerGUI::checkCommandLine()
//resetGraphworxDisplaysToInactive();
         
// Make all graphworx displays inactive on shutdown, PAManagerApp::cleanupSingletons
//resetGraphworxDisplaysToInactive


// Activate graphworx display we're operating for once started up (PAManagerDlg::OnInitDialog)
//activateGraphworxDisplay(...);

using namespace TA_App;

GraphworxComms GraphworxComms::s_instance;

GraphworxComms GraphworxComms::getInstance()
{
    return s_instance;
}


bool GraphworxComms::dispatchCommandToGraphworx(const std::string displayIdentifier, const std::string command)
{
    FUNCTION_ENTRY("dispatchCommandToGraphworx");

    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_NO_GRAPHWORX))
    {
        FUNCTION_EXIT;
        return true;
    }

    bool launched = false;

    // leftPosition will define the display this message is going to
    // -1 sends command to all displays
    long leftPosition = -1;
    
    if (displayIdentifier.length() > 0)
    {
        leftPosition = atol(displayIdentifier.c_str());
    }
    
    // filepath (command) will need the string "#PA,<command>" 
    // (GWX will interpret the # as a command for this display)
    std::ostringstream graphworxCommand;    
    graphworxCommand << "#PA," << command;
     
    // asset would be and empty string (that is not used)
    const std::string asset("");

    try
    {  
        TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(graphworxCommand.str(), asset, leftPosition);
        launched = true;
    }
    catch(TA_Base_Core::ApplicationException&)
	{
		LOG(SourceInfo,TA_Base_Core::DebugUtil::ExceptionCatch, "ApplicationException", 
			"Could not launch graphworx display.");
	}
	catch(...)
    {
		LOG(SourceInfo,TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", 
			"Could not launch graphworx display.");        
    }

    FUNCTION_EXIT;

    return launched;
}


bool GraphworxComms::resetGraphworxDisplaysToInactive() 
{
    // Send through the de-activate keyword
    return dispatchCommandToGraphworx("-1", "NONACTIVE");
}



bool GraphworxComms::activateGraphworxDisplay(const std::string& displayId) 
{    
    return dispatchCommandToGraphworx(displayId, "ACTIVE");
}


