/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/TisLogViewerGUI.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#include "stdafx.h"

#include "app/signs/tis_log_viewer/src/TisLogViewer.h"
#include "app/signs/tis_log_viewer/src/TisLogViewerGUI.h"
#include "app/signs/tis_log_viewer/src/resource.h"
#include "app/signs/tis_log_viewer/src/TisLogViewerDlg.h"
#include "app/signs/tis_log_viewer/src/RightsManager.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/TisLogViewerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;


namespace TA_IRS_App
{
    TisLogViewerGUI::TisLogViewerGUI()
    :
    AbstractDialogGUI( IDD_TISLOGVIEWER_DIALOG )
    {
        FUNCTION_ENTRY( "TisLogViewerGUI" );
        FUNCTION_EXIT;
    }


    TisLogViewerGUI::~TisLogViewerGUI()
    {
        FUNCTION_ENTRY( "~TisLogViewerGUI" );

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


    void TisLogViewerGUI::createApplicationObject()
    {
        FUNCTION_ENTRY( "createApplicationObject" );

        // TD18095, jianghp, to fix the performance of showing manager application
        TisLogViewerDlg* dlg = new TisLogViewerDlg(*this);
        // TD18095, jianghp, to fix the performance of showing manager application
        setApplicationWnd(*dlg);

        FUNCTION_EXIT;
    }


    unsigned long TisLogViewerGUI::getApplicationType()
    {
        FUNCTION_ENTRY( "getApplicationType" );
        FUNCTION_EXIT;
        return TIS_LOG_VIEWER_GUI_APPTYPE;
    }


    void TisLogViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY( "checkEntity" );

        TA_ASSERT( guiEntity != NULL, "The IEntityData pointer passed in was NULL" );

        setGuiEntity( *guiEntity );

        // cast down to the entity type
        TA_Base_Core::TisLogViewerEntityData* entityData = dynamic_cast< TA_Base_Core::TisLogViewerEntityData* >( guiEntity );
        if ( NULL == entityData )
        {
            CString message;
            message.Format( "The %s failed to retrieve the %s from the database.", TA_Base_Core::TisLogViewerEntityData::getStaticType().c_str(), "Entity" );
            TA_THROW( TA_Base_Core::GenericGUIException( std::string( message ), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID) );
        }

        // load the default display parameter
        try
        {
            TisLogViewerApp* theApp = dynamic_cast<TisLogViewerApp*>( AfxGetApp() );
            TA_ASSERT( theApp != NULL, "The Application object cannot be NULL" );

            theApp->setRefreshIntervalInSecs(entityData->getRefreshIntervalInSecs());
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            CString message;
            message.Format( "The %s failed to retrieve the %s from the database.", TA_Base_Core::TisLogViewerEntityData::getStaticType().c_str(), "Configuration Data" );
            TA_THROW( TA_Base_Core::GenericGUIException( std::string( message ), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID) );
        }

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::checkCommandLine()
    {
        FUNCTION_ENTRY( "checkCommandLine" );
        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY( "entityChanged" );

        // TODO: This will be called by GenericGUI when it receives a callback
        // indicating that the GUI entity has been modified. GenericGUI will have
        // invalidated the entity database object and will tell the GUI which
        // items were modified. This method just has to retrieve the details out
        // of it and update accordingly. If the details in it are invalid then this
        // should print a warning message to the user and continue to use the old settings (ie do not update the GUI).

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::serverIsDown()
    {
        FUNCTION_ENTRY( "serverIsDown" );

        // TODO: This method is used to ensure the GUI can handle when the server goes
        // up or down. The GUI should be updated to indicate the network status.
        // If the server has gone down it should enter some sort of read-only mode and
        // if the server comes back up it can restore to the correct operating mode.

        RightsManager::getInstance().setServerStatus(false);

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::serverIsUp()
    {
        FUNCTION_ENTRY( "serverIsUp" );

        // TODO: This method is used to ensure the GUI can handle when the server goes
        // up or down. The GUI should be updated to indicate the network status.
        // If the server has gone down it should enter some sort of read-only mode and
        // if the server comes back up it can restore to the correct operating mode.

        RightsManager::getInstance().setServerStatus(true);

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY( "loadAllUserSettings" );

        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
           setWindowPosition();
           LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );

           FUNCTION_EXIT;
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

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::saveAllUserSettings()
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
            TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

            if (userSettings == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
            }

            setUserSetting( *userSettings );
        }

        savePositionSettings();

        FUNCTION_EXIT;
    }


    void TisLogViewerGUI::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );

        // TODO: This provides a standard method for each GUI to use to clean itself
        // up ready to exit. This will be called whenever the application exits whether
        // it is closed by the Control Station or by the user. This should perform
        // tasks such as saving user settings, stopping any threads and asking the user
        // if they want to save their settings. When this method is completed the GUI
        // should be in a safe state to be closed.

        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException&)
        {
            // Handle here with appropriate errror message
        }

        FUNCTION_EXIT;
    }


    // TD18095, jianghp
    void TisLogViewerGUI::onInitGenericGUICompleted()
    {
        FUNCTION_ENTRY( "onInitGenericGUICompleted" );

        CWaitCursor cur;
        TisLogViewerDlg * pDlg = dynamic_cast<TisLogViewerDlg * >(getApplicationWnd());
        TA_ASSERT(pDlg != NULL, "the main dialog is NULL!");
        pDlg->LockWindowUpdate();
        pDlg->init();
        pDlg->UnlockWindowUpdate();
        pDlg->UpdateWindow();

        FUNCTION_EXIT;
    }


} // TA_App
