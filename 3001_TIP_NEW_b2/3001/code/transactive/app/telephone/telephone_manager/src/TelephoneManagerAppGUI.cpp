/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/TelephoneManagerAppGUI.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class provides the managed process interface for the application.  This
  * handles the run parameters and the monitoring of changes passed from the control station.
  */

#include "stdafx.h"
#include "TelephoneManagerAppGUI.h"
#include "Telephone_Manager.h"
#include "Telephone_managerdlg.h"

#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "bus\user_settings\src\SettingsMgr.h"
#include "bus\application_types\src\apptypes.h"
#include "core\exceptions\src\UserSettingsException.h"
#include "core\exceptions\src\GenericGUIException.h"

//using namespace TA_Core::TA_Utilities;
//using namespace TA_Core::TA_Exceptions;
//using namespace TA_Bus::UserSettings;


TelephoneManagerAppGUI::TelephoneManagerAppGUI()
{
    FUNCTION_ENTRY( "TelephoneManagerAppGUI" );
    FUNCTION_EXIT;
}


TelephoneManagerAppGUI::~TelephoneManagerAppGUI()
{
    FUNCTION_ENTRY( "~TelephoneManagerAppGUI" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "~TelephoneManagerAppGUI" );

    try
    {
//        if (getApplicationWnd() != NULL)
//        {
//            delete m_application;
//        }
    }
    catch( ... )
    {
    }

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "~TelephoneManagerAppGUI" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::createApplicationObject()
{
    FUNCTION_ENTRY( "createApplicationObject" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "createApplicationObject" );

    setApplicationWnd(*(new CTelephone_managerDlg(*this)));

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "createApplicationObject" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::loadAllUserSettings()
{
    FUNCTION_ENTRY( "loadAllUserSettings" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "loadAllUserSettings" );

    // First check if we should be loading the user settings.


    if ( TA_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
       setWindowPosition();
       LOG( SourceInfo, TA_Core::DebugUtil::FunctionExit, "loadAllUserSettings" );

       FUNCTION_EXIT;
       return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting( *(new TA_Bus::SettingsMgr(getSessionId(), getApplicationType() )));
    }

    loadPositionSettings();

    // TODO: Restore any other settings here


    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "loadAllUserSettings" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY( "saveAllUserSettings" );

    LOG( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "saveAllUserSettings" );

    // TODO: First check if we should be saving the user settings. Eg
    // if ( 0 == RunParams::getInstance().get(RPARAM_PREFERENCES).
    // compare("off") );
    // {
    // return;
    // }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*(new TA_Bus::SettingsMgr(getSessionId(), getApplicationType() )));
    }

    savePositionSettings();

    // TODO: Save any other settings here

    LOG( SourceInfo, TA_Core::DebugUtil::FunctionExit, "saveAllUserSettings" );

    FUNCTION_EXIT;
}


unsigned long TelephoneManagerAppGUI::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );

    LOG( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "getApplicationType" );

    LOG( SourceInfo, TA_Core::DebugUtil::FunctionExit, "getApplicationType" );

    FUNCTION_EXIT;
    return TELEPHONE_CONTROLLER_GUI_APPTYPE;
}


void TelephoneManagerAppGUI::checkEntity(TA_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY( "checkEntity" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "checkEntity" );


    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "checkEntity" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "checkCommandLine" );

/*    if (!RunParams::getInstance().isSet(EQUIPMENT_RPARAM))
    {
        std::string  details = "Parameter missing - ";
        details +=  EQUIPMENT_RPARAM;
        TA_THROW(GenericGUIException(details.c_str(),GenericGUIException::COMMAND_LINE_INVALID));
    }
*/
    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "checkCommandLine" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "entityChanged" );

    // TODO: Check the changes passed and pass any relevant information on
    // to the dialog so it can update.

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "entityChanged" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "serverIsDown" );

    // TODO: Get the GUI to update to indicate this.

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "serverIsDown" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "serverIsUp" );

    // TODO: Get the GUI to update to indicate this.

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "serverIsUp" );

    FUNCTION_EXIT;
}


void TelephoneManagerAppGUI::prepareForClose()
{
    FUNCTION_ENTRY( "prepareForClose" );

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionEntry, "prepareForClose" );

    try
    {
        saveAllUserSettings();
    }
    catch ( TA_Core::UserSettingsException& )
    {
        // Handle here with appropriate errror message
    }

    // TODO: Perform any shutdown tasks here

    LOG ( SourceInfo, TA_Core::DebugUtil::FunctionExit, "prepareForClose" );

    FUNCTION_EXIT;
}


