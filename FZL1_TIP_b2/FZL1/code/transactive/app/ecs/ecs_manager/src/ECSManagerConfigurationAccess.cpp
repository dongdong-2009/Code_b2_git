/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton to manage Access to the ECSManager Configuration.
  *
  * NOTE: TA_Base_Core::ECSManager is thread-safe.
  *
  */


#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    ECSManagerConfigurationAccess* ECSManagerConfigurationAccess::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable ECSManagerConfigurationAccess::m_singletonLock;

    // specify constant static Parameter Value array used to map String to Enumerated Type
    // for the DialogType parameter
    const ECSManagerConfigurationAccess::DialogTypeValuePair ECSManagerConfigurationAccess::DIALOG_TYPE_VALUE_ARRAY[] =
    {
        { "StationModes",           ECSManagerConfigurationAccess::STATION_MODES },
        { "StationModesOverview",   ECSManagerConfigurationAccess::STATION_MODES_OVERVIEW },
        { "NormalMasterModes",      ECSManagerConfigurationAccess::NORMAL_MASTER_MODES },
        { "EmergencyMasterModes",   ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES },
        { "EmergencyMasterModesForZone", ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE },
		{ "CongestionParameters",	ECSManagerConfigurationAccess::CONGESTION_OPERATION_SETTINGS },
		{ "StationTimetable",		ECSManagerConfigurationAccess::TIMETABLE_CHANGE },
		// 10745g-Start
		// Overall ECSTimetable Setting dialog
		{ "OverallStationTimetableSettingMode", ECSManagerConfigurationAccess::OVERALL_STATION_TIMETABLE_SETTING_MODE},
		// 10745g-End
        { NULL, ECSManagerConfigurationAccess::INVALID_VALUE } 
    };


    //
    // Destructor
    //
    ECSManagerConfigurationAccess::~ECSManagerConfigurationAccess()
    {
    }


    //
    // getInstance
    //
    ECSManagerConfigurationAccess& ECSManagerConfigurationAccess::getInstance()
    {
        //
        // Guard the method to ensure access by client code is synchronous
        //
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        if( m_theInstance == 0 )
        {
            m_theInstance = new ECSManagerConfigurationAccess();
        }

        return *m_theInstance;
    }


    //
    // removeInstance
    //
    void ECSManagerConfigurationAccess::removeInstance()
    {
        // 
        // Guard this to prevent multiple threads atempting
        // to delete/create simultaneously
        //
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        if( m_theInstance != 0 )
        {
            delete m_theInstance;
            m_theInstance = 0;
        }
    }


    //
    // Constructor
    //
    ECSManagerConfigurationAccess::ECSManagerConfigurationAccess()
        : m_entityConfiguration( 0 ), m_isDialogTypeInitialised( false ), m_dialogType( INVALID_VALUE )
    {
    }


    //
    // setEntityconfiguration
    //
    void ECSManagerConfigurationAccess::setEntityConfiguration( TA_Base_Core::ECSManager& entityConfiguration )
    {
        m_entityConfiguration = &entityConfiguration;
    }


    //
    // getEntityConfiguration
    //
    TA_Base_Core::ECSManager& ECSManagerConfigurationAccess::getEntityConfiguration()
    {
        //
        // Note, ECSManager is thread-safe, so no need to guard the method providing access to it.
        // (note - this wouldn't necessarily protect the object anyhow as doesn't guarantee two threads wouldn't 
        // actually access the object's interface simultaneously)
        //
        TA_ASSERT( m_entityConfiguration, "m_entityConfiguration should be initialised before accessed." );
        return *m_entityConfiguration;
    }


    //
    // getDialogType
    //
    ECSManagerConfigurationAccess::EECSDialogType ECSManagerConfigurationAccess::getDialogType()
    {
        //
        // No need to thread guard, as ECSManagerGUI will always access first and hence this method 
        // stores value from runParam and sets initialised flag to true. The value will not subsequently 
        // be altered (even if runParam changed). This is intended behaviour as application is not
        // designed to change dialog type during application instance. It must remain as specified by mandatory
        // command line parameter when application started.
        //
        if ( false == m_isDialogTypeInitialised ) 
        {
            m_dialogType = getDialogTypeValueFromString( 
                TA_Base_Core::RunParams::getInstance().get( TA_ECSManagerCommonDefs::DIALOG_TYPE_RPARAM ) );
            m_isDialogTypeInitialised = true;
        }

        return m_dialogType; 
    }


	unsigned int ECSManagerConfigurationAccess::getDialogIDTemplate()
	{
		UINT dlgResourceID=0;

		switch( getDialogType() )
        {
        case ECSManagerConfigurationAccess::STATION_MODES:
			dlgResourceID = IDD_ECSSTATIONMODES_DIALOG;
			break;

        case ECSManagerConfigurationAccess::STATION_MODES_OVERVIEW:
			dlgResourceID = IDD_ECSSTATIONMODESOVERVIEW_DIALOG;
			break;

        case ECSManagerConfigurationAccess::NORMAL_MASTER_MODES:
			dlgResourceID = IDD_ECSMASTERMODES_DIALOG;
			break;

        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES:
			dlgResourceID = IDD_ECSMASTERMODES_DIALOG;
			break;

        case ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE:
			dlgResourceID = IDD_ECSMASTERMODES_DIALOG;
			break;

        case ECSManagerConfigurationAccess::CONGESTION_OPERATION_SETTINGS:
			dlgResourceID = IDD_CONGESTION_PARAMS_DIALOG;
			break;

        case ECSManagerConfigurationAccess::TIMETABLE_CHANGE:
			dlgResourceID = IDD_TIMETABLE_CHANGE_DLG;
			break;

        case ECSManagerConfigurationAccess::OVERALL_STATION_TIMETABLE_SETTING_MODE:
			dlgResourceID = IDD_ECSOVERALLTIMETABLE_SELECTION_DIALOG;
			break;
			
        default:
			TA_ASSERT( false, "getDialogType() returned unexpected enumerated type value");
        }

		return dlgResourceID;
	}


    //
    // getDialogTypeValueFromString
    //
    ECSManagerConfigurationAccess::EECSDialogType ECSManagerConfigurationAccess::getDialogTypeValueFromString( const std::string& stringValue )
    {
        // can't use FUNCTION_ENTRY macro in static method
        LOG_FUNCTION_ENTRY( SourceInfo, "getDialogTypeValueFromString" );
        const DialogTypeValuePair* pDialogTypeArray = DIALOG_TYPE_VALUE_ARRAY;

        TA_ASSERT( pDialogTypeArray != NULL, "Object is NULL" );

        while ( pDialogTypeArray->typeValue != INVALID_VALUE )
        {
            if ( stringValue.compare( pDialogTypeArray->stringValue ) == 0 )
            {
                LOG_FUNCTION_EXIT( SourceInfo, "getDialogTypeValueFromString" ); // can't use FUNCTION_EXIT macro in static method
                return pDialogTypeArray->typeValue;
            }
            pDialogTypeArray++;
        }

        LOG_FUNCTION_EXIT( SourceInfo, "getDialogTypeValueFromString" ); // can't use FUNCTION_EXIT macro in static method
        return INVALID_VALUE;
    }

} // namespace TA_IRS_App
