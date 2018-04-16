/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISManagerGUI.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/05/23 15:31:56 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "stdafx.h"

#include "app/signs/stis_manager/src/STISManagerGUI.h"
#include "app/signs/stis_manager/src/STISManagerDlg.h"
#include "app/signs/stis_manager/src/RightsManager.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Core::UserSettingsException;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    CSTISManagerGUI::CSTISManagerGUI()
    :
    AbstractDialogGUI( IDD_STISMANAGER_DIALOG )
    {
    }


    CSTISManagerGUI::~CSTISManagerGUI()
    {
        try
        {
            if (getApplicationWnd() != NULL)
            {
                delete getApplicationWnd();
            }
        }
        catch( ... )
        {
        }
    }


    void CSTISManagerGUI::createApplicationObject()
    {
		HWND hwnd = FindWindow(NULL, "TIS Manager"); 
		if(hwnd != NULL)
		//ExitProcess(0);
			return ;
		CSTISManagerDlg *pDlg = new CSTISManagerDlg(*this);
	    setApplicationWnd( *pDlg );
    }


    unsigned long CSTISManagerGUI::getApplicationType()
    {
        FUNCTION_ENTRY( "getApplicationType" );
        FUNCTION_EXIT;    
        return STIS_MANAGER_GUI_APPTYPE;
    }


    void CSTISManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY( "checkEntity" );

        setGuiEntity( *guiEntity );

        // TODO: Check the configuration of the entity is valid. Throw
        //       the specified exception if an error is found.

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::checkCommandLine()
    {
        FUNCTION_ENTRY( "checkCommandLine" );

        // TODO: Check any command line arguments specific to this application
        //       are available and correct. Throw the specified exception if they
        //       are not.

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY( "entityChanged" );
    
        // TODO: Check the changes passed and pass any relevant information on.

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::serverIsDown()
    {
        FUNCTION_ENTRY( "serverIsDown" );

        RightsManager::getInstance().setServerStatus(false);

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::serverIsUp()
    {
        FUNCTION_ENTRY( "serverIsUp" );

        RightsManager::getInstance().setServerStatus(true);

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY( "loadAllUserSettings" );

        // First check if we should be loading the user settings.

        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).
                  empty() )
        {
           setWindowPosition();
           FUNCTION_EXIT;
           return;
        }

        if (getUserSetting() == NULL)
        {
            setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType() )) );
	    }
        loadPositionSettings();
        // TODO: Restore any other settings here

        FUNCTION_EXIT;
    }


    void CSTISManagerGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY( "saveAllUserSettings" );

	    // First check if we should be saving the user settings.
	    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
	    {
		    return;
	    }

	    if (getUserSetting() == NULL)
	    {
		    setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType() )) );
	    }
    
	    savePositionSettings();

	    // TODO: Perform any shutdown tasks here
	    
	    FUNCTION_EXIT;
    }


    void CSTISManagerGUI::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );
 
	    try
        {
            saveAllUserSettings();
        }
        catch ( UserSettingsException& )
        {
            // Handle here with appropriate errror message
        }

        // TODO: Perform any shutdown tasks here

        FUNCTION_EXIT;
    }


	void CSTISManagerGUI::onInitGenericGUICompleted()
	{
	}
} // TA_IRS_App
