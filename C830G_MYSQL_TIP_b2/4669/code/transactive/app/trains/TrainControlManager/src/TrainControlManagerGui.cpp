/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "app/trains/TrainControlManager/src/StdAfx.h"
#include "app/trains/TrainControlManager/src/TrainControlManagerGui.h"
#include "app/trains/TrainControlManager/src/TrainControlDialog.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/security/rights_library/src/CtaRights.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_App
{
    TrainControlManagerGui::TrainControlManagerGui()
        : AbstractDialogGUI( IDD_TRAIN_CONTROL_MANAGER_DIALOG ),
          m_taRights( NULL )
    {
        FUNCTION_ENTRY( "TrainControlManagerGui" );


        FUNCTION_EXIT;
    }


    TrainControlManagerGui::~TrainControlManagerGui()
    {
        FUNCTION_ENTRY( "~TrainControlManagerGui" );

        // cleanup application
        try
        {
            if ( getApplicationWnd() != NULL )
            {
                delete getApplicationWnd();
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor" );
        }

        // cleanup m_taRights
        delete m_taRights;
        m_taRights = NULL;

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::checkCommandLine()
    {
        FUNCTION_ENTRY( "checkCommandLine" );

        // do nothing

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::checkEntity( TA_Base_Core::IEntityData* guiEntity )
    {
        FUNCTION_ENTRY( "checkEntity" );

        TA_ASSERT( guiEntity != NULL, "The IEntityData pointer passed in was NULL" );

        setGuiEntity( *guiEntity );

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::createApplicationObject()
    {
        FUNCTION_ENTRY( "createApplicationObject" );

        m_taRights = new TA_Base_Bus::CtaRights();

        TrainControlDialog* dlg = new TrainControlDialog( *this, *m_taRights );
        setApplicationWnd( *dlg );

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::entityChanged( const std::vector<std::string>& changes )
    {
        FUNCTION_ENTRY( "entityChanged" );

        // do nothing

        FUNCTION_EXIT;
    }


    unsigned long TrainControlManagerGui::getApplicationType()
    {
        FUNCTION_ENTRY( "getApplicationType" );

        FUNCTION_EXIT;
        return TRAIN_MANAGER_GUI_APPTYPE;
    }


    void TrainControlManagerGui::loadAllUserSettings()
    {
        FUNCTION_ENTRY( "loadAllUserSettings" );

        // First check if we should be loading the user settings.

        if ( false == TA_Base_Core::RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
        {
            setWindowPosition();
            
            FUNCTION_EXIT;
            return;
        }

        loadPositionSettings();

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );

        // do nothing

        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException& )
        {
            // Handle here with appropriate errror message
        }

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::saveAllUserSettings()
    {
        FUNCTION_ENTRY( "saveAllUserSettings" );

        // First check if we should be saving the user settings.

        if ( TA_Base_Core::RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
        {
            return;
        }

        savePositionSettings();

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::serverIsDown()
    {
        FUNCTION_ENTRY( "serverIsDown" );

        // do nothing

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::serverIsUp()
    {
        FUNCTION_ENTRY( "serverIsUp" );

        // do nothing

        FUNCTION_EXIT;
    }


    void TrainControlManagerGui::onInitGenericGUICompleted()
    {
        CWaitCursor cur;
	    
        TrainControlDialog* pDlg = dynamic_cast< TrainControlDialog* >( getApplicationWnd() );
        TA_ASSERT(pDlg != NULL, "the main dialog is NULL!");
	    
        pDlg->LockWindowUpdate();
	    
        pDlg->initialise();

	    pDlg->UnlockWindowUpdate();
	    
        pDlg->UpdateWindow();
    }
}