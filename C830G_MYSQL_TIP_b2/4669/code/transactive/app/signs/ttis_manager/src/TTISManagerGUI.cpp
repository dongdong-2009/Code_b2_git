/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/4669/transactive/app/signs/ttis_manager/src/TTISManagerGUI.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/05/27 10:48:20 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  *
  *
  */

#include "stdafx.h"

#include "app/signs/ttis_manager/src/TTISManager.h"
#include "app/signs/ttis_manager/src/TTISManagerGUI.h"
#include "app/signs/ttis_manager/src/TTISManagerDlg.h"
#include "app/signs/ttis_manager/src/RightsManager.h"

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

/////////////////////////////////////////////////////////////////////////////
// TTISManagerGUI

namespace TA_IRS_App
{
	bool guiInitCompleted = false;
    TTISManagerGUI::TTISManagerGUI()
    :
    AbstractDialogGUI( IDD_TISMANAGER_DIALOG )
    {
        FUNCTION_ENTRY( "TTISManagerGUI" );
        FUNCTION_EXIT;
    }


    TTISManagerGUI::~TTISManagerGUI()
    {
        FUNCTION_ENTRY( "~TTISManagerGUI" );

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

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::createApplicationObject()
    {
        FUNCTION_ENTRY( "createApplicationObject" );

        // TD18095, jianghp, to fix the performance of showing manager application
        TTISManagerDlg *pDlg = new TTISManagerDlg(*this);
        // TD18095, jianghp, to fix the performance of showing manager application
        setApplicationWnd( *pDlg );

        FUNCTION_EXIT;
    }


    unsigned long TTISManagerGUI::getApplicationType()
    {
        FUNCTION_ENTRY( "getApplicationType" );
        FUNCTION_EXIT;
        return TTIS_MANAGER_GUI_APPTYPE;
    }


    void TTISManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY( "checkEntity" );

        setGuiEntity( *guiEntity );

        // TODO: Check the configuration of the entity is valid. Throw
        // the specified exception if an error is found.

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::checkCommandLine()
    {
        FUNCTION_ENTRY( "checkCommandLine" );

        // TODO: Check any command line arguments specific to this application
        // are available and correct. Throw the specified exception if they
        // are not.

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY( "entityChanged" );

        // TODO: Check the changes passed and pass any relevant information on.

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::serverIsDown()
    {
        FUNCTION_ENTRY( "serverIsDown" );

        RightsManager::getInstance().setServerStatus(false);

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::serverIsUp()
    {
        FUNCTION_ENTRY( "serverIsUp" );

        RightsManager::getInstance().setServerStatus(true);

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY( "loadAllUserSettings" );

        // First check if we should be loading the user settings.

        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
           setWindowPosition();

           FUNCTION_EXIT;
           return;
        }

        if (getUserSetting() == NULL)
        {
            setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType())) );
        }
        loadPositionSettings();
        // TODO: Restore any other settings here

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY( "saveAllUserSettings" );

        // First check if we should be saving the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            FUNCTION_EXIT;
            return;
        }

        if (getUserSetting() == NULL)
        {
            setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType())) );
        }

        savePositionSettings();

        // TODO: Perform any shutdown tasks here

        FUNCTION_EXIT;
    }


    void TTISManagerGUI::prepareForClose()
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


    void TTISManagerGUI::onInitGenericGUICompleted()
    {
        FUNCTION_ENTRY( "onInitGenericGUICompleted" );

        // TODO:

        FUNCTION_EXIT;
    }


	void TTISManagerGUI::setPosition(const unsigned long posFlag,
		                     const unsigned long alignFlag,
		                     const RECT& objectDim,
                             const RECT& boundaryDim)
	{
		if(!guiInitCompleted)
		{
			return;
		}
		AbstractGUIApplication::setPosition(posFlag, alignFlag, objectDim, boundaryDim);
	}


/////////////////////////////////////////////////////////////////////////////
// TTISManagerGUI message handlers

} // TA_IRS_App
