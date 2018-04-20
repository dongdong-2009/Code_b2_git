

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class provides the managed process interface for the application.  This
  * handles the run parameters and the monitoring of changes passed from the control station.
  */

#include "stdafx.h"

#include "app/radio/radio_manager_gui/src/RadioManagerGUI.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

RadioManagerGUI::RadioManagerGUI()
:
AbstractDialogGUI( IDD_RADIO_MANAGER_DIALOG ),
m_mainWindow(NULL)
{

}


RadioManagerGUI::~RadioManagerGUI()
{
    FUNCTION_ENTRY( "~RadioManagerGUI" );

    try
    {
        if (m_mainWindow != NULL)
        {
			m_mainWindow->DestroyWindow();
            delete m_mainWindow;
			m_mainWindow = NULL;
        }
    }
    catch( ... )
    {
    }

    FUNCTION_EXIT;
}


void RadioManagerGUI::createApplicationObject()
{
    FUNCTION_ENTRY( "createApplicationObject" );

    m_mainWindow = new CRadioManagerDialog(*this);
	setApplicationWnd(*m_mainWindow);

    FUNCTION_EXIT;
}


void RadioManagerGUI::loadAllUserSettings()
{
    FUNCTION_ENTRY( "loadAllUserSettings" );

	// First check if we should be loading the user settings.
	if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
	{
	   setWindowPosition();
	   FUNCTION_EXIT;
	   return;
	}

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() ));
    }

    loadPositionSettings();

    FUNCTION_EXIT;
}


void RadioManagerGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY( "saveAllUserSettings" );

	//TODO: First check if we should be saving the user settings. Eg
    //if ( 0 == RunParams::getInstance().get(RPARAM_PREFERENCES).
    //          compare("off") );
    //{
    //    return;
    //}

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() ));
    }

    savePositionSettings();

    FUNCTION_EXIT;
}


unsigned long RadioManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );
    FUNCTION_EXIT;
    return RADIO_CONTROLLER_GUI_APPTYPE;
}

void RadioManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY( "checkEntity" );
    FUNCTION_EXIT;
}


void RadioManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );
	
/*    if (!RunParams::getInstance().isSet(EQUIPMENT_RPARAM))
	{
		std::string  details = "Parameter missing - ";
		details +=  EQUIPMENT_RPARAM;				
		TA_THROW(GenericGUIException(details.c_str(),GenericGUIException::COMMAND_LINE_INVALID));
	}
*/
    FUNCTION_EXIT;
}


void RadioManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );

    // TODO: Check the changes passed and pass any relevant information on
    //        to the dialog so it can update.

    FUNCTION_EXIT;
}


void RadioManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );

    // TODO: Get the GUI to update to indicate this.

    FUNCTION_EXIT;
}


void RadioManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );

    // TODO: Get the GUI to update to indicate this.

    FUNCTION_EXIT;
}


void RadioManagerGUI::prepareForClose()
{
   FUNCTION_ENTRY( "prepareForClose" );

    try
    {
        saveAllUserSettings();
    }
    catch ( TA_Base_Core::UserSettingsException& )
    {
        // Handle here with appropriate errror message
    }

    // TODO: Perform any shutdown tasks here

    FUNCTION_EXIT;
}


//TD18095, jianghp
void RadioManagerGUI::onInitGenericGUICompleted()
{
	FUNCTION_ENTRY("RadioManagerGUI::onInitGenericGUICompleted");
	CWaitCursor cur;
	CRadioManagerDialog * pDlg = dynamic_cast<CRadioManagerDialog * >(getApplicationWnd());
	TA_ASSERT(pDlg != NULL, "the main dialog is NULL!");
	pDlg->LockWindowUpdate();
	pDlg->init();
	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
	FUNCTION_EXIT;
}
