/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/templates/Dialog/src/TemplateDialogGUI.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#include "stdafx.h"
#include "TemplateDialogGUI.h"
#include "resource.h"
#include "TemplateDialogDlg.h"

#include "bus\application_types\src\apptypes.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"

#include "bus\user_settings\src\SettingsMgr.h"
#include "core\exceptions\src\UserSettingsException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TemplateDialogGUI::TemplateDialogGUI()
{   
}


TemplateDialogGUI::~TemplateDialogGUI()
{
}


void TemplateDialogGUI::createApplicationObject()
{
    CTemplateDialogDlg* dlg = new CTemplateDialogDlg(*this);
    setApplicationWnd(*dlg);
}


unsigned long TemplateDialogGUI::getApplicationType()
{
	//TODO: return a value from apptypes.h
	return 0;
}


void TemplateDialogGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
	setGuiEntity(*guiEntity);
}


void TemplateDialogGUI::checkCommandLine()
{  
}


void TemplateDialogGUI::entityChanged(const std::vector<std::string>& changes)
{
	//TODO: This will be called by GenericGUI when it receives a callback
	//indicating that the GUI entity has been modified. GenericGUI will have
	//invalidated the entity database object and will tell the GUI which
	//items were modified. This method just has to retrieve the details out
	//of it and update accordingly. If the details in it are invalid then this
	//should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
}


void TemplateDialogGUI::serverIsDown()
{
    // TODO: This method is used to ensure the GUI can handle when the server goes
	//up or down. The GUI should be updated to indicate the network status.
	//If the server has gone down it should enter some sort of read-only mode and
	//if the server comes back up it can restore to the correct operating mode.
}


void TemplateDialogGUI::serverIsUp()
{
    // TODO: This method is used to ensure the GUI can handle when the server goes
	//up or down. The GUI should be updated to indicate the network status.
	//If the server has gone down it should enter some sort of read-only mode and
	//if the server comes back up it can restore to the correct operating mode.
}


void TemplateDialogGUI::loadAllUserSettings()
{
    // First check if we should be loading the user settings.
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
    {
       setWindowPosition();
       LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
       return;
    }

    if (getUserSetting() == NULL)
    {
        TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

        if (userSettings == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
        }

        setUserSetting( *userSettings );            
    }

    loadPositionSettings();
}


void TemplateDialogGUI::saveAllUserSettings()
{
    // First check if we should be saving the user settings.
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
    {
        return;
    }

    if (getUserSetting() == NULL)
    {
        TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

        if (userSettings == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
        }

        setUserSetting( *userSettings );            
    }
    
    savePositionSettings();
}


void TemplateDialogGUI::prepareForClose()
{
	//TODO: This provides a standard method for each GUI to use to clean itself 
	//up ready to exit. This will be called whenever the application exits whether 
	//it is closed by the Control Station or by the user. This should perform 
	//tasks such as saving user settings, stopping any threads and asking the user 
	//if they want to save their settings. When this method is completed the GUI 
	//should be in a safe state to be closed.

    try
    {
        saveAllUserSettings();
    }
    catch ( TA_Base_Core::UserSettingsException&)
    {
        // Handle here with appropriate errror message
    }

    // TODO: Perform any shutdown tasks here
}

