/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneManagerFramework.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class provides the managed process interface for the application.  This
  * handles the creation of the CtaTelephoneManagerSummary class and  the
  * monitoring of changes passed from the control station.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerFramework.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/RunParams.h"
#include "app/telephone/telephone_manager/src/TelephoneRightsInitializThread.h"
#include "core/synchronisation/src/Semaphore.h"

using namespace TA_Base_Core;


TelephoneManagerFramework::TelephoneManagerFramework()
:
AbstractDialogGUI( IDD_TELEPHONE_MANAGER_DIALOG ),
m_pctaTelephoneManagerSummary( NULL ),
m_ptelephoneManagerDlg( NULL )
{
    FUNCTION_ENTRY( "TelephoneManagerFramework" );
    FUNCTION_EXIT;
}


TelephoneManagerFramework::~TelephoneManagerFramework()
{
    FUNCTION_ENTRY( "~TelephoneManagerFramework" );

    MessageSubscriptionManager::getInstance().unsubscribeToMessages(
        static_cast<TelephoneDirectoryMessageSubscriber*>(m_pctaTelephoneManagerSummary));

    MessageSubscriptionManager::getInstance().unsubscribeToMessages(
        static_cast<TelephoneDirectLinesMessageSubscriber*>(m_pctaTelephoneManagerSummary));

    //LIMIN TODO, fore exit
    exit( 0 );

    // delete m_application - this should be done in subclass to prevent
    // memory leak
    try
    {
        CWnd* app = getApplicationWnd();
        if (app != NULL)
        {
            delete app;
        }
    }
    catch( ... )
    {
    }

    delete m_pctaTelephoneManagerSummary;
    // no need to delete m_userSettings as that is deleted in
    // AbstractGUIApplication

    FUNCTION_EXIT;
}


unsigned long TelephoneManagerFramework::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );
    FUNCTION_EXIT;
    return TELEPHONE_MANAGER_GUI_APPTYPE;
}


void TelephoneManagerFramework::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY( "checkEntity" );

    setGuiEntity(*guiEntity);

    FUNCTION_EXIT;
}


void TelephoneManagerFramework::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );
    FUNCTION_EXIT;
}


void TelephoneManagerFramework::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );
    FUNCTION_EXIT;
}


void TelephoneManagerFramework::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );
    FUNCTION_EXIT;
}


void TelephoneManagerFramework::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );
    FUNCTION_EXIT;
}


//TES230++
//void TelephoneManagerFramework::changeFocus(const TA_Base_Core::EFocusType focus)
//{
//    FUNCTION_ENTRY("changeFocus");
//    FUNCTION_EXIT;
//}
//++TES230


void TelephoneManagerFramework::prepareForClose()
{
    FUNCTION_ENTRY( "prepareForClose" );

    try
    {
        saveAllUserSettings();
    }
    catch ( UserSettingsException& )
    {
        // Handle here with appropriate log message
    }

    FUNCTION_EXIT;
}


void TelephoneManagerFramework::createApplicationObject()
{
    FUNCTION_ENTRY( "createApplicationObject" );

    /*
        m_pctaTelephoneManagerSummary = new CtaTelephoneManagerSummary(*this->getGUIEntity());
        m_pctaTelephoneManagerSummary->Subscribe();*/

    m_ptelephoneManagerDlg = new CTelephoneManagerDlg(*this);

    if (m_ptelephoneManagerDlg == NULL)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on CTelephoneManagerDlg returned NULL. Cannot create window");

        FUNCTION_EXIT;
        return;
    }

    setApplicationWnd(*m_ptelephoneManagerDlg);

    FUNCTION_EXIT;
}


void TelephoneManagerFramework::loadAllUserSettings()
{
    FUNCTION_ENTRY( "loadAllUserSettings" );

    if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
       setWindowPosition();
       LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "loadAllUserSettings" );

       FUNCTION_EXIT;
       return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting( *(new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() )));
    }

    loadPositionSettings();

    FUNCTION_EXIT;
}


void TelephoneManagerFramework::saveAllUserSettings()
{
    FUNCTION_ENTRY( "saveAllUserSettings" );

    LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "saveAllUserSettings" );

    // TODO: First check if we should be saving the user settings. Eg
    // if ( 0 == RunParams::getInstance().get(RPARAM_PREFERENCES).
    // compare("off") );
    // {
    // return;
    // }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() ));
    }

    savePositionSettings();

    // TODO: Save any other settings here

    LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "saveAllUserSettings" );

    FUNCTION_EXIT;
}


void TelephoneManagerFramework::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY( "onRunParamChange" );
    FUNCTION_EXIT;
}


//TD18095, jianghp
void TelephoneManagerFramework::onInitGenericGUICompleted()
{
    FUNCTION_ENTRY( "onInitGenericGUICompleted" );

    TA_Base_Core::IEntityDataPtr guiEntityData(this->getGUIEntity());
    m_pctaTelephoneManagerSummary = new CtaTelephoneManagerSummary(guiEntityData);
    m_pctaTelephoneManagerSummary->Subscribe();

    CWnd *pwnd = getApplicationWnd();
    CTelephoneManagerDlg *pDialog = dynamic_cast<CTelephoneManagerDlg *>(pwnd);
    TA_ASSERT(pDialog != NULL, "the apllication window is NULL!");

    m_telephoneRightsInitializThread.setSessionIdEntityKey( *pDialog, m_pctaTelephoneManagerSummary->getMySessionId(), m_pctaTelephoneManagerSummary->getMyEntityKey() );
    m_telephoneRightsInitializThread.setRightsSemaphore( *m_pctaTelephoneManagerSummary, m_pctaTelephoneManagerSummary->getSemaphore() );
    m_telephoneRightsInitializThread.start();

    pDialog->setTelephoneManagerSummary(m_pctaTelephoneManagerSummary);

    FUNCTION_EXIT;
}
