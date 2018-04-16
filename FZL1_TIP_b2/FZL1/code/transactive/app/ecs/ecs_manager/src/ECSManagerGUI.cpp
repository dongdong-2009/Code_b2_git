/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSManagerGUI.cpp $
* @author:  Robert Young
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/06/12 13:35:44 $
* Last modified by:  $Author: builder $
*
* This is the ECSManager implementation of IGUIAccess and IGUIApplication from the GenericGUI
* framework.
*
*/

#include<algorithm>

#include "app/ecs/ecs_manager/src/stdafx.h"

#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/ECSManagerGUI.h"
#include "app/ecs/ecs_manager/src/ECSStationModesDlg.h"
#include "app/ecs/ecs_manager/src/ECSStationModesOverviewDlg.h"
#include "app/ecs/ecs_manager/src/ECSMasterModesDlg.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/CongestionParameterDlg.h"
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"
#include "app/ecs/ecs_manager/src/EcsOverallTimetableSettingDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/GenericGuiException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

//Prasanna
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
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////
	
    //
    // Constructor
	//TD18413 xinsong++
	//because the commandline haven't initialized, so call getDialogIDTemplate will failed
	//we will set dlgId during winapp::initInstance by call AbstractDialogGUI::setDlgResourceID
    //
    ECSManagerGUI::ECSManagerGUI()
	:
	//AbstractDialogGUI(ECSManagerConfigurationAccess::getInstance().getDialogIDTemplate())
	AbstractDialogGUI(0)
    {   
		
		
    }
	
	
    //
    // Destructor
    //
    ECSManagerGUI::~ECSManagerGUI()
    {
        //
        // clean-up the object we created memory for during createApplication(),
        // as the superclass doesn't do it for us.
        //
        if ( NULL != getApplicationWnd() )
        {
            delete getApplicationWnd();
        }
    }
	
	
    //
    // createApplicationObject
    //
    void ECSManagerGUI::createApplicationObject()
    {
        FUNCTION_ENTRY( "createApplicationObject" );

		UINT nDlgResourceId = ECSManagerConfigurationAccess::getInstance().getDialogIDTemplate();
		setDlgResourceID( nDlgResourceId );
		
		// determine which dialog to create from mandatory run param set on command line
        switch( ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
        case ECSManagerConfigurationAccess::STATION_MODES:
			{// --dialog-type set for Station Modes Dialog
				ECSStationModesDlg *pDlg = new ECSStationModesDlg( *this );
                setApplicationWnd( * pDlg );
			}
			break;

        case ECSManagerConfigurationAccess::STATION_MODES_OVERVIEW:
			{	// --dialog-type set for for Proposed Station Modes Dialog
				ECSStationModesOverviewDlg *pDlg = new ECSStationModesOverviewDlg( *this );
                setApplicationWnd( *pDlg);
			}
			break;

        case ECSManagerConfigurationAccess::NORMAL_MASTER_MODES:
			{	// --dialog-type set for Normal Master Modes Dialog
				ECSMasterModesDlg *pDlg = new ECSMasterModesDlg( *this );
                setApplicationWnd( *pDlg );
			}
			break;

        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES:
			{	// --dialog-type set for Emergency Master Modes Dialog 
				ECSMasterModesDlg *pDlg = new ECSMasterModesDlg( *this );
                setApplicationWnd( *pDlg );
			}
			break;

        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE:
			{// --dialog-type set for Emergency Master Modes For Zone Dialog
				ECSMasterModesDlg *pDlg = new ECSMasterModesDlg( *this );
                setApplicationWnd( *pDlg );
			}
			break;

        case ECSManagerConfigurationAccess::CONGESTION_OPERATION_SETTINGS:
			{// --dialog-type set for Emergency Master Modes For Zone Dialog
				CCongestionParameterDlg *pDlg = new CCongestionParameterDlg( *this ) ;
                setApplicationWnd( *pDlg );
			}
			break;

        case ECSManagerConfigurationAccess::TIMETABLE_CHANGE:
			{// --dialog-type set for Emergency Master Modes For Zone Dialo
				CTimetableSelectionDlg *pDlg = new CTimetableSelectionDlg( *this ) ;
                setApplicationWnd( *pDlg);
			}
			break;

        case ECSManagerConfigurationAccess::OVERALL_STATION_TIMETABLE_SETTING_MODE:
			{//// 10745g-Start		 --dialog-type set for Overall ECS Timetable Setting Dialog
				COverallEcsTimetableSettingDlg *pDlg = new COverallEcsTimetableSettingDlg( *this ) ;
                setApplicationWnd( *pDlg );
			}
			break;	
			
        default:
			// // 10745g-End	already checked this parameter (getApplicationType()) so execution should never get here
			TA_ASSERT( false, "ECSManagerConfigurationAccess::getInstance().getDialogType() returned unexpected enumerated type value");
        }
		FUNCTION_EXIT;
    }
	
	
	EObjectType ECSManagerGUI::getEObjectTypeFromApplicationType()
	{
		FUNCTION_ENTRY( "getEObjectTypeFromApplicationType" );

		switch( ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
		// --dialog-type set for Station Modes Dialog
        case ECSManagerConfigurationAccess::STATION_MODES:
			return TA_Base_Core::ECS_STATION_MODE ;
			break;

		// --dialog-type set for for Proposed Station Modes Dialog
        case ECSManagerConfigurationAccess::STATION_MODES_OVERVIEW:
			return TA_Base_Core::ECS_STATION_MODE;
			break;

		// --dialog-type set for Normal Master Modes Dialog
        case ECSManagerConfigurationAccess::NORMAL_MASTER_MODES:
			return TA_Base_Core::ECS_MASTER_MODE;
			break;

		// --dialog-type set for Emergency Master Modes Dialog
        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES:
			return TA_Base_Core::ECS_MASTER_MODE;;
			break;

		// --dialog-type set for Emergency Master Modes Dialog
        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE:
			return TA_Base_Core::ECS_MASTER_MODE;
			break;	
			
		//liuyu++
		// --dialog-type set for Congestion Parameter Settings Dialog
        case ECSManagerConfigurationAccess::CONGESTION_OPERATION_SETTINGS:
			return TA_Base_Core::ECS_MASTER_MODE;
			break;

		// --dialog-type set for Timetable Change Dialog
        case ECSManagerConfigurationAccess::TIMETABLE_CHANGE:
			return TA_Base_Core::ECS_STATION_MODE;
			break;

		// --dialog-type set for Overall Station Timetable Settings Mode Dialog
        case ECSManagerConfigurationAccess::OVERALL_STATION_TIMETABLE_SETTING_MODE:
			return TA_Base_Core::ECS_STATION_MODE;
			break;

			//++liuyu
        case ECSManagerConfigurationAccess::INVALID_VALUE:
            {
                // The mandatory ECSManager runParam DialogType was not correctly 
                // specified on the command-line so throw an exception.
                std::string errorMsg = "A mandatory command-line parameter was ommited or incorrectly specified: ";
                errorMsg += TA_ECSManagerCommonDefs::DIALOG_TYPE_RPARAM;
                TA_THROW( TA_Base_Core::GenericGUIException( errorMsg.c_str(), TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID ) );
                break;
            }
        default:
            // Execution should never get here as should be set known enum value (even if invalidly specified on command-line
            // in whiuch case it should have been mapped to INVALID_VALUE above)
			TA_ASSERT( false, "ECSManagerConfigurationAccess::getInstance().getDialogType() returned unexpected enumerated type value" );
        }
		FUNCTION_EXIT;
		return TA_Base_Core::GLOBAL_PARAMETERS;
	}
	// 7196-End	
    //
    // getApplicationType
    //
    unsigned long ECSManagerGUI::getApplicationType()
    {
        // Each Dialog Type for this applciation should be treated as a seperate application type.
        // They are simply in same executable as objects are shareds and it is neater to contain in
        // one.
		
        // ECSManagerConfigurationAccess will convert the mandatory runparam DialogType to a
        // suitable enum and store for any subsequent client code to access. It is not intended
        // to change even if the actual runparam value was modified during application life.
        // We initialise it here by calling the getrDialogType() function and verify it is not
        // set to value indicating that an invalid value (or no value at all) was passed on 
        // the command-line.
		
		unsigned long applicationType = 0;

        switch( ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
		// --dialog-type set for Station Modes Dialog
        case ECSManagerConfigurationAccess::STATION_MODES:
			applicationType = ECS_STATION_MODES_GUI_APPTYPE;
			break;

		// --dialog-type set for for Proposed Station Modes Dialog
        case ECSManagerConfigurationAccess::STATION_MODES_OVERVIEW:
			applicationType = ECS_STATION_MODES_OVERVIEW_GUI_APPTYPE;
			break;

		// --dialog-type set for Normal Master Modes Dialog
        case ECSManagerConfigurationAccess::NORMAL_MASTER_MODES:
			applicationType = ECS_NORMAL_MASTER_MODES_GUI_APPTYPE;
			break;

		// --dialog-type set for Emergency Master Modes Dialog
        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES:
			applicationType = ECS_EMERGENCY_MASTER_MODES_GUI_APPTYPE;
			break;

		// --dialog-type set for Emergency Master Modes Dialog
        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE:
			applicationType = ECS_EMERGENCY_MASTER_MODES_FOR_ZONE_GUI_APPTYPE;
			break;

		// --dialog-type set for Congestion Parameters Dialog
		case ECSManagerConfigurationAccess::CONGESTION_OPERATION_SETTINGS:
			applicationType = ECS_CONGESTION_PARAMETERS_APPTYPE;
			break;

		// --dialog-type set for Station Timetable Dialog
		case ECSManagerConfigurationAccess::TIMETABLE_CHANGE:
			applicationType = ECS_STATION_TIMETABLE_APPTYPE;
			break;

		// 10745g-Start		
		// --dialog-type set for Overall ECS Timetable Setting Dialog
		case ECSManagerConfigurationAccess::OVERALL_STATION_TIMETABLE_SETTING_MODE:
			applicationType = ECS_OVERALL_TIMETABLE_SETTING_GUI_APPTYPE;
			break;			
		// 10745g-End		
			
        case ECSManagerConfigurationAccess::INVALID_VALUE:
            {
                // The mandatory ECSManager runParam DialogType was not correctly 
                // specified on the command-line so throw an exception.
                std::string errorMsg = "A mandatory command-line parameter was ommited or incorrectly specified: ";
                errorMsg += TA_ECSManagerCommonDefs::DIALOG_TYPE_RPARAM;
                TA_THROW( TA_Base_Core::GenericGUIException( errorMsg.c_str(), TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID ) );
                break;
            }
        default:
            break;
        }
		
		return applicationType;
    }
	
	
    //
    // checkEntity
    //
    void ECSManagerGUI::checkEntity( TA_Base_Core::IEntityData* guiEntity )
    {
        setGuiEntity( *guiEntity );
		
        TA_Base_Core::ECSManager* pECSManagerEntityData = dynamic_cast< TA_Base_Core::ECSManager* > ( guiEntity );
		
        if ( NULL == pECSManagerEntityData )
        {
            // Throw an exception to indicate entity name passed in waws not of valid type
            TA_THROW( TA_Base_Core::GenericGUIException( "The entity specifed to the application was not of type ECSManager",
                TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
        }
		
        ECSManagerConfigurationAccess::getInstance().setEntityConfiguration( *pECSManagerEntityData );
		
		//Prasanna
		
    }
	
	
    //
    // checkCommandLine
    //
    void ECSManagerGUI::checkCommandLine()
    {
        FUNCTION_ENTRY( "checkCommandLine" );
		
        // Nothing to do here as mandatory command line argument has been 
        // checked during getApplicationType()
		
        FUNCTION_EXIT;  
    }
	
	
    //
    // entityChanged
    //
    void ECSManagerGUI::entityChanged( const std::vector<std::string>& changes )
    {
        //This will be called by GenericGUI when it receives a callback
        //indicating that the GUI entity has been modified. GenericGUI will have
        //invalidated the entity database object and will tell the GUI which
        //items were modified. This method just has to retrieve the details out
        //of it and update accordingly. If the details in it are invalid then this
        //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
		// 7196-Start
		// Update the station mode data point point manager if there is a entity change
		// this implementation is temporary.need to change with update messages.
		bool bUpdateForDataPointHealthPollInterval = false;
        std::string strChange = TA_Base_Core::ECSManager::DATAPOINT_HEALTH_POLL_INTERVAL_PARAMETER_NAME;
        std::vector<std::string>::const_iterator iter = std::find(changes.begin(), changes.end(), strChange);
        if (iter != changes.end() )
        {
			switch(getApplicationType())
			{
			case ECS_STATION_MODES_OVERVIEW_GUI_APPTYPE:
				{
					ECSStationModesOverviewDlg *pDlg = (ECSStationModesOverviewDlg *)getApplicationWnd();
					if(pDlg != NULL)
						pDlg->refreshHealthPollInterval();
					break;
				}
			case ECS_STATION_MODES_GUI_APPTYPE:
				{
					ECSStationModesDlg *pDlg = (ECSStationModesDlg *)getApplicationWnd();
					if(pDlg != NULL)
						pDlg->refreshHealthPollInterval();
					break;
				}
			default:
				{
					break;
				}
			}
        }
		// 7196-End
    }
	
	
    //
    // serverIsDown
    //
    void ECSManagerGUI::serverIsDown()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
    }
	
	
    //
    // serverIsUp
    //
    void ECSManagerGUI::serverIsUp()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
    }
	
	
    //
    // loadAllUserSettings
    //
    void ECSManagerGUI::loadAllUserSettings()
    {
        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
        {
			setWindowPosition();
			LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
			return;
        }
		
        if ( getUserSetting() == NULL )
        {
            setUserSetting( *( new SettingsMgr( getSessionId(), getApplicationType() ) ) );
        }
		
        loadPositionSettings();
    }
	
	
    //
    // saveAllUserSettings
    //
    void ECSManagerGUI::saveAllUserSettings()
    {
        // First check if we should be saving the user settings.
        if ( RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
        {
            return;
        }
		
        if ( getUserSetting() == NULL )
        {
            setUserSetting( *( new SettingsMgr( getSessionId(), getApplicationType() ) ) );
        }
		
        savePositionSettings();
    }
	
	
    //
    // prepareForClose
    //
    void ECSManagerGUI::prepareForClose()
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
            // Handle here with appropriate error message
        }
		
        //
        // clean-up the singleton instance of ECSManagerConfigurationAccess
        //
        //ECSManagerConfigurationAccess::removeInstance();
    }
	
	//TD18095, jianghp
	void ECSManagerGUI::onInitGenericGUICompleted()
	{
		//do nothing, let the child class do something indeed.
		IBaseView *pView  = dynamic_cast<IBaseView *>(getApplicationWnd());
		TA_ASSERT(pView != NULL, "the current view is NULL!");
		pView->init();
	};
} // namespace TA_IRS_App
