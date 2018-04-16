/**
  * The source code in this file is the property of Ripple System
  * and is not for redistribution in any form. 
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerGUI.cpp $
  * @author: Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  *
  * Implementation file for the Integrated Alarm Viewer GUI application class.
  */


#pragma warning(disable:4541)
#pragma warning(disable:4786)

#include <sstream>

#include "stdafx.h"
#include "app/ats/AtsIscsAlarmViewer/src/Resource.h"
#include "app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerGUI.h"
#include "app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	AtsIscsAlarmViewerGUI::AtsIscsAlarmViewerGUI()
    :
    AbstractDialogGUI( IDD_ATSISCSALARMVIEWER_DIALOG )
	{

	}


	AtsIscsAlarmViewerGUI::~AtsIscsAlarmViewerGUI()
    {
        FUNCTION_ENTRY( "~AtsIscsAlarmViewerGUI");

        try
        {
			CWnd* application = getApplicationWnd();
            if (application != NULL)
            {
                delete application;
            }
        }
        catch( ... )
        {

        }

        FUNCTION_EXIT;
    }


    void AtsIscsAlarmViewerGUI::createApplicationObject()
    {
		//TD18095, jianghp, to fix the performance of showing manager application
		CAtsIscsAlarmViewerDlg *pDlg = new CAtsIscsAlarmViewerDlg(this);
		/*//TD18095, jianghp, to fix the performance of showing manager application*/
        setApplicationWnd(*pDlg);
    }


	unsigned long AtsIscsAlarmViewerGUI::getApplicationType()
	{
		return INTEGRATED_ALARM_GUI_APPTYPE;
	}


	void AtsIscsAlarmViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		setGuiEntity(*guiEntity);

		//TO DO: Check the config of the entity is valid. Throw the specified 
		// exception if an error is found
	}	


	void AtsIscsAlarmViewerGUI::checkCommandLine()
	{
		//TO DO: Check any command line arguments specific to this application are 
		//available and correct. Throw specified exception if they are not.
	}


	void AtsIscsAlarmViewerGUI::entityChanged(const std::vector<std::string>& changes)
	{
		//TO DO: check the chagnes passed and pass any relevant information on to the
		// dialog so it can update
	}


	void AtsIscsAlarmViewerGUI::serverIsDown()
	{
		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverIsDown" );
        
        dynamic_cast <CAtsIscsAlarmViewerDlg*> (getApplicationWnd())->serverDown();

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverIsDown" );
	}


	void AtsIscsAlarmViewerGUI::serverIsUp()
	{
		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverIsUp" );
        
        dynamic_cast <CAtsIscsAlarmViewerDlg*> (getApplicationWnd())->serverUp();
        
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverIsUp" );
    }


	void AtsIscsAlarmViewerGUI::loadAllUserSettings()
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
			TA_Base_Bus::SettingsMgr* userSettings = 
				new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());
			if (userSettings == NULL)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
				"new on SettingsMgr returned NULL. Cannot load user settings");
				return;
			}
			setUserSetting(*userSettings);
		}

		loadPositionSettings();
	}


	void AtsIscsAlarmViewerGUI::saveAllUserSettings()
	{
		// First check if we should be saving the user settings.
		if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
			return;
		}

		if (getUserSetting() == NULL)
		{
			TA_Base_Bus::SettingsMgr* userSettings = 
				new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());
			if (userSettings == NULL)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
				"new on SettingsMgr returned NULL. Cannot load user settings");
				return;
			}
			setUserSetting(*userSettings);
		}
    
		savePositionSettings();
	}


	void AtsIscsAlarmViewerGUI::prepareForClose()
	{
		try
		{
			saveAllUserSettings();
		}
		catch ( TA_Base_Core::UserSettingsException&)
		{
			//TO DO: Handle here with appropriate error message.
		}
    }


    void AtsIscsAlarmViewerGUI::onInitGenericGUICompleted()
    {
        //TODO:
    }
}   // end namespace TA_IRS_App
