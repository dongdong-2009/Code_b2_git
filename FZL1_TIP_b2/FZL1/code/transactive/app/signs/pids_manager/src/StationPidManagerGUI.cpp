/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/spidmanager/src/StationPidManagerGUI.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "stdafx.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "app/signs/pids_manager/src/StationPidManagerGUI.h"
#include "app/signs/pids_manager/src/PidManagerDialog.h"
#include "app/signs/pids_manager/src/SchematicDecideModel.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::UserSettingsException;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

StationPidManagerGUI::StationPidManagerGUI():AbstractDialogGUI( IDD_PID_MANAGER_DIALOG)
{
}

StationPidManagerGUI::~StationPidManagerGUI()
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


void StationPidManagerGUI::createApplicationObject()
{
	///@thomas_jiang_try_modify
	
	/*HANDLE hProcessSnap = NULL;
	
	PROCESSENTRY32 pe =  {0};
	unsigned char find_num = 0;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap != NULL)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if(Process32First(hProcessSnap, &pe))
		{
			do
			{
				CString strName = pe.szExeFile;
				if(strName == "STISManager.exe")
				{
					if(find_num > 0)
					{
						//CloseHandle(hProcessSnap);
						HANDLE handle = GetCurrentProcess();
						TerminateProcess(handle, 0);
						//return;
					}
					else find_num ++;
				}
			}while(Process32Next(hProcessSnap, &pe));
		}
	}
	CloseHandle(hProcessSnap);
	*/
	CPidManagerDialog *pDlg = new CPidManagerDialog(this);
	//pDlg->ShowWindow(SW_SHOW);
    setApplicationWnd( *pDlg );
}


unsigned long StationPidManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );
    FUNCTION_EXIT;    
    return STIS_MANAGER_GUI_APPTYPE;
}


void StationPidManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY( "checkEntity" );

    setGuiEntity( *guiEntity );

    // TODO: Check the configuration of the entity is valid. Throw
    //       the specified exception if an error is found.
	try
	{
		SchematicDecideModel::setSubsystemKey(guiEntity->getSubsystem());
	}
	catch (...)
	{
	}

    FUNCTION_EXIT;
}


void StationPidManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );

    // TODO: Check any command line arguments specific to this application
    //       are available and correct. Throw the specified exception if they
    //       are not.

    FUNCTION_EXIT;
}


void StationPidManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );

    // TODO: Check the changes passed and pass any relevant information on.

    FUNCTION_EXIT;
}


void StationPidManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );

//    RightsManager::getInstance().setServerStatus(false);

    FUNCTION_EXIT;
}


void StationPidManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );

 //   RightsManager::getInstance().setServerStatus(true);

    FUNCTION_EXIT;
}


void StationPidManagerGUI::loadAllUserSettings()
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

    //if (getUserSetting() == NULL)
    //{
    //    setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType() )) );
    //}
    loadPositionSettings();
    // TODO: Restore any other settings here

    FUNCTION_EXIT;
}


void StationPidManagerGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY( "saveAllUserSettings" );

    // First check if we should be saving the user settings.
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
    {
	    return;
    }

    //if (getUserSetting() == NULL)
    //{
	   // setUserSetting( *(new SettingsMgr(getSessionId(), getApplicationType() )) );
    //}

    savePositionSettings();

    // TODO: Perform any shutdown tasks here
    
    FUNCTION_EXIT;
}


void StationPidManagerGUI::prepareForClose()
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


//TD18095, jianghp
void StationPidManagerGUI::onInitGenericGUICompleted()
{
	CWaitCursor cos;
	CWnd *pWnd = getApplicationWnd();
	TA_ASSERT(pWnd != NULL, "the dialog windows should not be null!");
	CPidManagerDialog * pDialog =  dynamic_cast<CPidManagerDialog * >(pWnd);
	pDialog->LockWindowUpdate();
	
	pDialog->init();
	
	pDialog->UpdateWindow();
	pDialog->UnlockWindowUpdate();
}

