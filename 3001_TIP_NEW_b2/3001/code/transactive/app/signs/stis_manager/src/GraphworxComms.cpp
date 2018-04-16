/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/GraphworxComms.cpp $
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
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>


// Reset the state of the graphworx displays on startup to become inactive PaManagerGUI::checkCommandLine()
//resetGraphworxDisplaysToInactive();
         
// Make all graphworx displays inactive on shutdown, PAManagerApp::cleanupSingletons
//resetGraphworxDisplaysToInactive


// Activate graphworx display we're operating for once started up (PAManagerDlg::OnInitDialog)
//activateGraphworxDisplay(...);

using namespace TA_IRS_App;

GraphworxComms GraphworxComms::s_instance;

GraphworxComms GraphworxComms::getInstance()
{
    return s_instance;
}


bool GraphworxComms::dispatchCommandToGraphworx(const std::string& command, const long displayIdentifier /*= -1*/)
{
    
    bool launched = false;

    // filepath (command) will need the string "#TIS,<command>" 
    // (GWX will interpret the # as a command for this display)
    std::ostringstream graphworxCommand;    
    graphworxCommand << "#TIS," << command;
     
    // asset would be an empty string (that is not used)
    const std::string asset("");

    try
    {  
        TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(graphworxCommand.str(), asset, displayIdentifier);
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

    return launched;
}


bool GraphworxComms::resetGraphworxDisplaysToInactive() 
{
    // Send through the de-activate keyword
    return dispatchCommandToGraphworx("NONACTIVE");
}


bool GraphworxComms::activateGraphworxDisplay(const long displayId) 
{    
    return dispatchCommandToGraphworx("ACTIVE", displayId);
}


bool GraphworxComms::displayCurrentMessage(const std::string& cmd, const std::string& message, const std::string& priority, const long displayIdentifier, const std::string& startTime, const std::string& endTime) 
{    
    // give the graphworx the message in the form of:
    //      VIEWMESSAGE,<the value of GetMessage run param>,<message>,<priority level>
    std::ostringstream tempCommand;
    tempCommand << "VIEWMESSAGE," << cmd << "," << message << "," << startTime << "," << endTime << "," << priority;
    return dispatchCommandToGraphworx(tempCommand.str(), displayIdentifier);
}




