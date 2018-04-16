/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSMasterModes.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the business class that provides all the the logic and data of the ECS Master Modes
  * user interface. It manipulates the ECS Master Modes user interface through IECSMasterModesView.
  *
  */

#include "stdafx.h"
#include <algorithm>



#include "app/ecs/ecs_manager/src/ECSMasterModes.h"
#include "app/ecs/ecs_manager/src/IECSMasterModesView.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"

#include "bus/generic_gui/src/TransactiveMessage.h"

#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"




using TA_IRS_App::ECSManagerConfigurationAccess;
// using TA_IRS_App::ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES;
// using TA_IRS_App::ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE;
// using TA_IRS_App::ECSManagerConfigurationAccess::NORMAL_MASTER_MODES;
#define EMERGENCY_MASTER_MODES			TA_IRS_App::ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES
#define EMERGENCY_MASTER_MODES_FOR_ZONE	TA_IRS_App::ECSManagerConfigurationAccess::EMERGENCY_MASTER_MODES_FOR_ZONE
#define NORMAL_MASTER_MODES				TA_IRS_App::ECSManagerConfigurationAccess::NORMAL_MASTER_MODES
// using TA_IRS_App::MappedECSDataCache::OrderedLocations;
// using TA_IRS_App::MappedECSDataCache::MasterModeKeys;
// using TA_IRS_App::TA_ECSManagerCommonDefs::MasterModeDetail;
// using TA_IRS_App::TA_ECSManagerCommonDefs::StationModeDetail;
// using TA_IRS_App::TA_ECSManagerCommonDefs::LocationKeys;
// using TA_IRS_App::TA_ECSManagerCommonDefs::ZoneDirectionKeys;
typedef TA_IRS_App::MappedECSDataCache::OrderedLocations OrderedLocations;
typedef TA_IRS_App::MappedECSDataCache::MasterModeKeys MasterModeKeys;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::MasterModeDetail MasterModeDetail;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::StationModeDetail StationModeDetail;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::LocationKeys LocationKeys;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::ZoneDirectionKeys ZoneDirectionKeys;

#define CURRENT_STATION_MODE_NORMAL TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_NORMAL
#define CURRENT_STATION_MODE_EMERGENCY TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_EMERGENCY
#define CURRENT_STATION_MODE_CONGESTION TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_CONGESTION
#define CURRENT_STATION_MODE_FAILED TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_FAILED
#define NOT_CURRENTLY_PROPOSED TA_IRS_App::TA_ECSManagerCommonDefs::NOT_CURRENTLY_PROPOSED
#define CURRENTLY_OVERRIDING_PROPOSED TA_IRS_App::TA_ECSManagerCommonDefs::CURRENTLY_OVERRIDING_PROPOSED

// using TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_NORMAL;
// using TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_EMERGENCY;
// using TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_CONGESTION;
// using TA_IRS_App::TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_FAILED;
// using TA_IRS_App::TA_ECSManagerCommonDefs::NOT_CURRENTLY_PROPOSED;
// using TA_IRS_App::TA_ECSManagerCommonDefs::CURRENTLY_OVERRIDING_PROPOSED;

// using TA_IRS_App::TA_ECSManagerCommonDefs::StationModes;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::StationModes StationModes;
// using TA_IRS_App::TA_ECSManagerCommonDefs::ZONE_EQUIPMENT_RPARAM;
// using TA_IRS_App::TA_ECSManagerCommonDefs::EMERGENCY_COMPARE_STRING;
// using TA_IRS_App::TA_ECSManagerCommonDefs::CONGESTION_COMPARE_STRING;

#define ZONE_EQUIPMENT_RPARAM TA_IRS_App::TA_ECSManagerCommonDefs::ZONE_EQUIPMENT_RPARAM
#define EMERGENCY_COMPARE_STRING TA_IRS_App::TA_ECSManagerCommonDefs::EMERGENCY_COMPARE_STRING
#define CONGESTION_COMPARE_STRING TA_IRS_App::TA_ECSManagerCommonDefs::CONGESTION_COMPARE_STRING

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ScadaProxyException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{
    const int BLINK_EVACUATION_ARROWS_BITSET_INT =  0x00000001;
    const int BLINK_AIRFLOW_ARROWS_BITSET_INT =     0x00000100;
	const unsigned short CORBA_TIMEOUT = 120;

    // specify constant static Parameter Value array used to map String Direction codes to Enumerated Direction
    // code.
    const ECSMasterModes::DirectionTypeValuePair ECSMasterModes::DIRECTION_MAPPING_ARRAY[] =
    {
        { "U", ECSMasterModes::UP },
        { "D", ECSMasterModes::DOWN },
        { "L", ECSMasterModes::LEFT },
        { "R", ECSMasterModes::RIGHT },
        { NULL, ECSMasterModes::UNKNOWN }
    };


    //
    // Constructor
    //
    ECSMasterModes::ECSMasterModes( IECSMasterModesView& view ): 
	m_ecsMasterModesView( view ), m_pStationModesDataPointManager( 0 ), m_pMappedECSDataCache( 0 ),
    m_currentMasterMode( 0 ), m_currentlySelectedMasterModeIndex( -1 ),
    m_currentMasterModeDirectionKey( -1 ), m_currentMasterModeDisplayDirection(""), 
	m_zoneID( -1 ), m_masterECSAgent( 0 ), m_sessionId(""), m_rightsMgr (NULL),
	m_proposedMasterModeUpdateSubscriber(NULL)
    {
        m_pStationModesDataPointManager = std::auto_ptr< StationModesDataPointManager >( new StationModesDataPointManager() );
        m_pMappedECSDataCache = std::auto_ptr< MappedECSDataCache >( new MappedECSDataCache() );
		m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    }


    //
    // Destructor
    //
    ECSMasterModes::~ECSMasterModes()
    {	
		delete m_proposedMasterModeUpdateSubscriber;
		m_proposedMasterModeUpdateSubscriber = NULL;	
    }


    //
    // compareMasterModeDetailsByMode
    //
    bool compareMasterModeDetailsByMode( MasterModeDetail masterModeDetail1, MasterModeDetail masterModeDetail2 )
    {
        return ( masterModeDetail1.masterMode < masterModeDetail2.masterMode );
    }


    //
    // masterModeSelectionChanged
    // 
    void ECSMasterModes::masterModeSelectionChanged( int masterModeIndex )
    {
        FUNCTION_ENTRY( "masterModeSelectionChanged" );

		// ZN: 17274++ : check permission, if fails, no need perform write datapoint value
		if (checkPermission() == false)
		{
			std::ostringstream status;
			status << "No rights to perform master mode selection changed";
			m_ecsMasterModesView.addStatusLine(status.str());
			
			return;
		}
		// ZN: ++17274

        ThreadGuard guard( m_updateLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "masterModeSelectionChanged() obtained ThreadLockable." );

        TA_ASSERT( masterModeIndex < static_cast< int >( m_currentMasterModeDetails.size() ),
            "ECSMasterModes::masterModeSelectionChanged called with index beyond range of MasterModes that should be currently displayed to user." );
        try
        {
			CORBA_CALL( (*m_masterECSAgent),
			            selectMasterMode,
			            ( m_currentMasterModeDetails[ masterModeIndex ].masterMode, m_sessionId.c_str() ) );

			std::ostringstream status;
			status << "Master mode  ";
			status << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			status << " currently selected. ";
			m_ecsMasterModesView.addStatusLine(status.str());
        }
		catch( const TA_Base_Core::BadParameterException& ex )
		{
			m_ecsMasterModesView.enableApplyMasterModeControl( false );
			std::ostringstream status;
			status << "Unable to Select ECS Master Mode ";
			status << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			status << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			m_ecsMasterModesView.addStatusLine(status.str());			
		}
		catch( const TA_Base_Core::AccessDeniedException& ex )
		{
			m_ecsMasterModesView.enableApplyMasterModeControl( false );
			std::ostringstream status;
			status << "Unable to Select ECS Master Mode ";
			status << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			status << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			m_ecsMasterModesView.addStatusLine(status.str());
		}
 		catch( const CORBA::Exception& ex )
		{
			m_ecsMasterModesView.enableApplyMasterModeControl( false );
            
			TA_Base_Bus::TransActiveMessage userMsg;
			std::ostringstream mode;
			mode << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			std::string description = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			userMsg << mode.str().c_str();
			userMsg << description.c_str();
			userMsg.showMsgBox(IDS_UE_520014);
		
			std::ostringstream ostr;
            ostr << "Unable to Select ECS Master Mode ";
            ostr << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			ostr << " ";
            ostr << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			m_ecsMasterModesView.addStatusLine(ostr.str());
		}
		catch (const TA_Base_Core::ObjectResolutionException& )
		{
			m_ecsMasterModesView.enableApplyMasterModeControl( false );
			
			TA_Base_Bus::TransActiveMessage userMsg;
			std::ostringstream mode;
			mode << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			std::string description = "Failed to resolve master ECS agent";
			userMsg << mode.str().c_str();
			userMsg << description.c_str();
			userMsg.showMsgBox(IDS_UE_520014);
		
			std::ostringstream ostr;
            ostr << "Unable to Select ECS Master Mode ";
            ostr << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			ostr << " ";
			ostr << "Failure to connect to " << (*m_masterECSAgent).getObjectName();
			m_ecsMasterModesView.addStatusLine(ostr.str());

		}
		catch( ... )
		{
			m_ecsMasterModesView.enableApplyMasterModeControl( false );
           
			TA_Base_Bus::TransActiveMessage userMsg;
			std::ostringstream mode;
			mode << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			std::string description = "Unknown exception";

			userMsg << mode.str().c_str();
			userMsg << description.c_str();
			userMsg.showMsgBox(IDS_UE_520014);
		
			std::ostringstream ostr;
            ostr << "Unable to Select ECS Master Mode ";
            ostr << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
           	ostr << " ";
            ostr << "Unknown exception";
			m_ecsMasterModesView.addStatusLine(ostr.str());
		} 

        m_ecsMasterModesView.enableApplyMasterModeControl( true );
        m_currentlySelectedMasterModeIndex = masterModeIndex;

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "masterModeSelectionChanged(): m_currentlySelectedMasterModeIndex=%d",
					 m_currentlySelectedMasterModeIndex);

        FUNCTION_EXIT;
    }


    //
    // applySelectedMasterMode
    //
    void ECSMasterModes::applySelectedMasterMode( int masterModeIndex )
    {
        FUNCTION_ENTRY( "applySelectedMasterMode" );

		if (checkPermission() == false)
		{
			
			TA_Base_Bus::TransActiveMessage userMsg;	
			std::ostringstream masterMode;
			masterMode << m_currentMasterModeDetails[ masterModeIndex ].masterMode;
			CString mode = masterMode.str().c_str();
			userMsg << mode;
			std::string reason = "No rights to perform master mode";
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UW_650013);

			return;
		}
		

        if ( masterModeIndex != m_currentlySelectedMasterModeIndex )
        {
            masterModeSelectionChanged( masterModeIndex );
        }

		TA_Base_Bus::TransActiveMessage userMsg;
		std::string description;
		UINT errorCode;
		long selectedMasterMode = m_currentMasterModeDetails[ masterModeIndex ].masterMode;

        if ( ( masterModeIndex > -1 ) && ( masterModeIndex < static_cast< int >( m_currentMasterModeDetails.size() ) ) )
        {
			if((m_currentMasterModeDetails[masterModeIndex].verificationMessage).empty())
			{
				std::ostringstream str;
				str << selectedMasterMode;
				description = str.str();
				userMsg << description.c_str();
				errorCode = IDS_UW_520002;
			}
			else
			{
				description = m_currentMasterModeDetails[masterModeIndex].verificationMessage;
				userMsg << description.c_str();
				errorCode = IDS_UW_650007;
			}

			UINT selectedButton = userMsg.showMsgBox(errorCode);
			if ( selectedButton == IDOK)
            {
                ThreadGuard guard( m_updateLock );
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "applySelectedMasterMode() obtained ThreadLockable." );
											

                std::ostringstream errorInExecutionStr;

				bool tryAgain = true;
				bool canOverride = false;

				while(tryAgain)
				{
                    std::ostringstream confirmStr;
					tryAgain = false;

					try
					{
						CORBA_CALL( (*m_masterECSAgent),
						            executeMasterMode,
						            ( selectedMasterMode, canOverride, m_sessionId.c_str() ) );
					}
					catch( const TA_Base_Core::BadParameterException& ex )
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Base_Core::BadParameterException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" ); 
					}
					catch( const TA_Base_Core::AccessDeniedException& ex )
					{
						errorInExecutionStr << "Unable to execute Master Mode. Reason: Access denied.";
						m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Base_Core::AccessDeniedException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" ); 
					}
					catch( const TA_IRS_Bus::IMasterEcsAgentCorbaDef::MasterModeInProgress& ex )
					{
						// Require to try again, this time with Overriding flag set.
			
						TA_Base_Bus::TransActiveMessage userMsg;	
						std::ostringstream masterMode;
						masterMode << ex.masterMode;
						CString mode = masterMode.str().c_str();
						userMsg << mode;
						UINT selectedButton = userMsg.showMsgBox(IDS_UW_520003);
						if ( selectedButton == IDOK )
						{
							canOverride = true;
							tryAgain = true;
						}
	
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Core::MasterModeInProgress", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" ); 
					}
					catch( const TA_IRS_Bus::IMasterEcsAgentCorbaDef::MasterModeActive& ex )
					{
						//errorInExecutionStr << "Unable to execute Master Mode. Reason: Master Mode Active";
						//m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
						TA_Base_Bus::TransActiveMessage userMsg;	
						std::ostringstream masterMode;
						masterMode << ex.masterMode;
						CString mode = masterMode.str().c_str();
						userMsg << mode;
						UINT selectedButton = userMsg.showMsgBox(IDS_UW_650008);
						if ( selectedButton == IDOK )
						{
							canOverride = true;
							tryAgain = true;
						}

						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Core::MasterModeActive", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" );
					}
					catch( const TA_IRS_Bus::IMasterEcsAgentCorbaDef::StationsInLocalMode& ex )
					{
						// TD16884++
						std::ostringstream stationsInLocal;
						if (ex.locations.length() > 0)
						{
							int index=0;
							for (int i=0; i<ex.locations.length(); i++)
							{
								stationsInLocal << m_locationNames[m_pMappedECSDataCache->getOrderedLocationIndexFromKey(ex.locations[i])];
								if (i != ex.locations.length()-1)
								{
									stationsInLocal << ", ";
								}
							}
						}

						TA_Base_Bus::TransActiveMessage userMsg;
						CString param = stationsInLocal.str().c_str();
						userMsg << param;
						UINT selectedButton = userMsg.showMsgBox(IDS_UE_803012);
						// ++TD16884

						errorInExecutionStr << "Unable to execute Master Mode. Reason: One or more stations are in Local mode: ";
						errorInExecutionStr << stationsInLocal.str();
						m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
					
						
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Core::StationsInLocalMode", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" );
					}
					catch( const TA_IRS_Bus::IMasterEcsAgentCorbaDef::NormalMasterModeFailure& ex )
					{
						errorInExecutionStr << "Unable to execute Master Mode. Reason: Normal Master Mode Failure.";
						m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
							"TA_Core::NormalMasterModeFailure", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to execute Master Mode" );
					}							
 					catch( const CORBA::Exception& ex )
					{
						errorInExecutionStr << "Unable to execute Master Mode. Reason: CORBA Exception";
						m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
					}
					catch (const TA_Base_Core::ObjectResolutionException& ex)
					{
						errorInExecutionStr << "Unable to execute Master Mode. Reason: Unable to resolve Master ECS Agent.";
						m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
					}
					catch( ... )
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Unknown exception caught!" ); 
					} 
                } //end of while
            }
        }

        FUNCTION_EXIT;
    }


	void ECSMasterModes::createStationECSAgents()
	{
		TA_Base_Core::CorbaNameList ecsStationAgentCorbaNames;
		ecsStationAgentCorbaNames = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType(TA_Base_Core::StationECSAgentEntityData::getStaticType(),true);

		unsigned long locationKey = 0;
		std::ostringstream errorInExecutionStr;
		for(unsigned int i=0; i<ecsStationAgentCorbaNames.size(); i++)
		{
			locationKey = ecsStationAgentCorbaNames[i].getLocationKey();
			try
			{
				StationECSAgentNamedObj* agent = new StationECSAgentNamedObj(ecsStationAgentCorbaNames[i]);
				m_stationECSAgents[locationKey] = agent;
			}
			catch(...)
			{
				errorInExecutionStr << "Unable to execute Station Mode. Reason: Access denied.";
				m_ecsMasterModesView.addStatusLine( errorInExecutionStr.str() );
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"Unknown", "Failed to create station ECS Agent for location %d", locationKey ); 
			}
		}
	}

    //
    // initialise
    //
    void ECSMasterModes::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

		TA_Base_Core::IEntityDataList  agentList;
        try 
        {
			loadLocations();
			 m_pStationModesDataPointManager->refreshDataPointProxies( m_locationKeys, true );
			// Retrieve Master ECS Agent at the OCC, which has a location key of 1.
			agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::MasterECSAgentEntityData::getStaticType(), 1);
			TA_ASSERT(agentList.size() == 1, "There can only be one Master ECS Agent at the OCC");

            if ( EMERGENCY_MASTER_MODES_FOR_ZONE == ECSManagerConfigurationAccess::getInstance().getDialogType() )
            {
                determineZoneToLoadFor();
            }

            m_masterECSAgent = std::auto_ptr<MasterECSAgentNamedObj>(new MasterECSAgentNamedObj(agentList[0]->getName(),true));
			m_masterECSAgentEntityKey = agentList[0]->getKey();
			m_masterECSAgentLocation = agentList[0]->getLocation();
			m_masterECSAgentSubsystem = agentList[0]->getPhysicalSubsystem();
			CORBA_CALL( (*m_masterECSAgent),
			            login,
			            ( m_zoneID, m_sessionId.c_str() ) );

			createStationECSAgents();

			// We're now done with the agentList object(s), so delete it.
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}

            
            
			m_ecsMasterModesView.addStatusLine("Loading master mode data...");

            loadMasterModeDetails(); 
            
           

            // NOTE: Important only acquire lock and register for datapoint updates AFTER all DataPoint 
            // proxies have been created (ie. after all calls to refreshDataPointProxies) else deadlock may occur.
            ThreadGuard guard( m_updateLock );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "initialise() obtained ThreadLockable." );

			m_proposedMasterModeUpdateSubscriber = new TA_IRS_App::ProposedMasterModeUpdateSubscriber(this);
			
            loadDataPointValues();

			TA_Base_Bus::RightsLibraryFactory rightsFactory;

			if ( TA_Base_Core::RunParams::getInstance().isSet("DebugAlwaysTrueRightsLib") )
			{
				m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
			}
			else
			{
				m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
			}

			m_ecsMasterModesView.addStatusLine("Finished loading master mode data.");

			
        }
		catch (const TA_Base_Core::ObjectResolutionException& ex)
		{
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}

			TA_Base_Bus::TransActiveMessage userMsg;
			std::string reason = "Failure to connect to " + (*m_masterECSAgent).getObjectName();
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UE_520011);
		
			std::ostringstream ostr;
			ostr << "Failed to log in with Master ECS Agent. Reason: ";
			ostr << "Failure to connect to " << (*m_masterECSAgent).getObjectName();
			ostr << "\r\nPlease close application and try again later.";
			m_ecsMasterModesView.addStatusLine(ostr.str());

			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent"); 

			FUNCTION_EXIT;
			return;

		}
		catch (TA_Base_Core::AccessDeniedException& ex)
		{
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}	

			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "Access denied";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_520011);
		
			std::ostringstream ostr;
			ostr << "Failed to log in with Master ECS Agent. Reason: Access denied. \r\n";
			ostr << "Please close application and try again later.";
			m_ecsMasterModesView.addStatusLine(ostr.str());

			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::AccessDeniedException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to log in with ECS Master Agent" );

			FUNCTION_EXIT;
            return;
		}
		catch( const CORBA::Exception& ex )
		{
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}
			
			TA_Base_Bus::TransActiveMessage userMsg;
			std::string reason = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UE_520011);
		
			std::ostringstream ostr;
			ostr << "Failed to log in with Master ECS Agent. Reason: ";
			ostr << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			ostr << "\r\nPlease close application and try again later.";
			m_ecsMasterModesView.addStatusLine(ostr.str());

			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 

			FUNCTION_EXIT;
			return;
		}
        catch (DataException& ex)
        {
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}

			m_ecsMasterModesView.addStatusLine("ECS database configuration data error. Application will exit.");
            
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);
	
            m_ecsMasterModesView.terminateApplication();

            FUNCTION_EXIT;
            return;
        }
        catch (DatabaseException& ex)
        {
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}

			m_ecsMasterModesView.addStatusLine("Unable to retrieve ECS database configuration. Application will exit.");
		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);
		
            m_ecsMasterModesView.terminateApplication();

            FUNCTION_EXIT;
            return;
        }
		catch (...)
		{
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}

			m_ecsMasterModesView.addStatusLine("Unknown exception during initialisation. Application will exit.");
		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520013);
		
            m_ecsMasterModesView.terminateApplication();

			FUNCTION_EXIT;
            return;
		}

        FUNCTION_EXIT;
    }


    //
    // loadLocations
    //
    void ECSMasterModes::loadLocations()
    {
        FUNCTION_ENTRY( "loadLocations" );

        m_pMappedECSDataCache->refreshOrderedLocationData();
        OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
        
//		m_ecsMasterModesView.addStatusLine("Loading locations...");

		OrderedLocations::iterator orderedLocationsIt = orderedLocations.begin();
        for (;orderedLocationsIt != orderedLocations.end(); ++orderedLocationsIt)
        {
            m_locationKeys.push_back( (*orderedLocationsIt).key );
            m_locationNames.push_back( (*orderedLocationsIt).name );
        }

        m_ecsMasterModesView.setLocations( m_locationNames );

		m_ecsMasterModesView.addStatusLine("Locations loaded.");
        
        FUNCTION_EXIT;
    }


    //
    // determineZoneToLoadFor
    //
    void ECSMasterModes::determineZoneToLoadFor()
    {
        FUNCTION_ENTRY( "determineZoneToLoadFor" );
        
        if ( RunParams::getInstance().isSet( ZONE_EQUIPMENT_RPARAM ) )
        {
            //m_zoneID = m_pMappedECSDataCache->getZoneIdFromZoneDirectionName( 
            //     RunParams::getInstance().get( ZONE_DIRECTION_RPARAM ) );
            std::string equipmentName = RunParams::getInstance().get( ZONE_EQUIPMENT_RPARAM );
            if(equipmentName.empty())
            {
                m_zoneID = TA_IRS_Bus::ECS_NONE_OR_INVALID;
            }
            else
            {
                m_zoneID = m_pMappedECSDataCache->getZoneIdFromZoneEquipmentName(equipmentName);
            }
        }
        else
        {
            try
            {
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* ret = NULL;
				CORBA_CALL_RETURN( ret,
				                   (*m_masterECSAgent),
				                   getAllProposedModes,
				                   () );
				m_currentProposedMasterModes = *ret;
			}
			catch( const CORBA::Exception& ex )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
			}
			catch (const TA_Base_Core::ObjectResolutionException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 

			}
			catch( ... )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"Unknown", "Unknown exception caught!" ); 
			} 
           
			for(unsigned int i=0; i<m_currentProposedMasterModes.length(); i++)
			{
                // Code modified to build.  
                // ToDo - need to filter the m_currentProposedMasterModes list of proposed modes to only have 
                // master modes.  
                
				m_pMappedECSDataCache->refreshMasterModeData( m_currentProposedMasterModes[i].mode );

				unsigned long proposedKey = m_pMappedECSDataCache->getMasterModeKeyFromMode(m_currentProposedMasterModes[i].mode);
				if (0 != proposedKey)
				{
					m_zoneID = m_pMappedECSDataCache->getMasterModeDetailFromKey( proposedKey ).zoneID;
				}
			}
        }

        FUNCTION_EXIT;
    }


    //
    // loadMasterModeDetails
    //
    void ECSMasterModes::loadMasterModeDetails()
    {
        FUNCTION_ENTRY( "loadMasterModeDetails" );

        m_currentMasterModeDetails.clear();

		MasterModeKeys masterModeKeys;

        switch ( ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
        case NORMAL_MASTER_MODES:
            {
                m_pMappedECSDataCache->refreshStationModeData();
                m_pMappedECSDataCache->refreshNormalMasterModeData();
                masterModeKeys = m_pMappedECSDataCache->getNormalMasterModeKeys();
            }
            break;
        case EMERGENCY_MASTER_MODES:
            {
                m_pMappedECSDataCache->refreshStationModeData();
                m_pMappedECSDataCache->refreshEmergencyMasterModeData();
                masterModeKeys = m_pMappedECSDataCache->getEmergencyMasterModeKeys();
            }
            break;
        case EMERGENCY_MASTER_MODES_FOR_ZONE:
            {
                if (  m_zoneID != -1 )
                {
                    m_pMappedECSDataCache->refreshStationModeData();
                    m_pMappedECSDataCache->refreshEmergencyMasterModeData( m_zoneID );
                }
                masterModeKeys = m_pMappedECSDataCache->getEmergencyMasterModeKeys();
            }
            break;
        default:
            TA_ASSERT( false, "Unexpected dialog type during initialisation of Master Modes Dialog." );
        }

        MasterModeKeys::iterator masterModeKeysIt = masterModeKeys.begin();
        for (;masterModeKeysIt != masterModeKeys.end(); ++masterModeKeysIt)
        {
            m_currentMasterModeDetails.push_back(m_pMappedECSDataCache->
													getMasterModeDetailFromKey( (*masterModeKeysIt) ));     
        }

        std::sort(m_currentMasterModeDetails.begin(), m_currentMasterModeDetails.end(), 
				  compareMasterModeDetailsByMode );

        m_ecsMasterModesView.enableApplyMasterModeControl( false );
        m_ecsMasterModesView.setMasterModeDetails( m_currentMasterModeDetails );

        FUNCTION_EXIT;
    }


    //
    // loadDataPointValues
    //
    void ECSMasterModes::loadDataPointValues()
    {
        FUNCTION_ENTRY( "loadDataPointValues" );

        m_pStationModesDataPointManager->registerStationModeStatusObserver( this );
        
        bool isValid;
        int value;
		LocationKeys::iterator locationKeysIt; 
        for (locationKeysIt = m_locationKeys.begin();
		     locationKeysIt != m_locationKeys.end();
			 ++locationKeysIt)
        {
			// Mode status
            isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeStatus( ( *locationKeysIt ) );
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }
            stationModeStatusUpdated( ( *locationKeysIt ), isValid, value );

			// Mode in progress
            isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeInProgress( ( *locationKeysIt ) );
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode In Prgress value." );
                isValid = false;
                value = 0;
            }
            stationModeInProgressUpdated( ( *locationKeysIt ), isValid, value );

			// Station lock status
			std::ostringstream ostrLockStatus;
			isValid = true;
			try
			{
				value = m_pStationModesDataPointManager->getStationLockStatus(( *locationKeysIt ));
			}
			catch( const ScadaProxyException& ex)
			{
				ostrLockStatus << "Unable to retrieve lock status for station. ";
				ostrLockStatus << ex.what();

				isValid = false;
                value = 0;
				m_ecsMasterModesView.addStatusLine(ostrLockStatus.str());
			}
			catch(...)
			{
				isValid = false;
                value = 0;

				ostrLockStatus << "Unable to retrieve lock status for station. Unknown exception";
				m_ecsMasterModesView.addStatusLine(ostrLockStatus.str());
			}
			stationVisualLockUpdated((*locationKeysIt), isValid, value);     
        }      

		// TO DO: Replace with Agent Call.
		// No need current Master mode anymore, this bit is all handled by the ECS Agent now.

        isValid = true;

		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* masterModes = 0;

		try
		{
			CORBA_CALL_RETURN( masterModes,
			                   (*m_masterECSAgent),
			                   getAllProposedModes,
			                   () );
		}
		catch( const CORBA::Exception& ex )
		{
			isValid = false;
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Core::CORBAException", 
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
		}
		catch (const TA_Base_Core::ObjectResolutionException& ex)
		{
			isValid = false;
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", 
				ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 

		}
		catch( ... )
		{
			isValid = false;
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Unknown exception caught!" ); 
		}

		// TO DO Implement the multiple proposed mode function
		proposedMasterModesUpdated(isValid, masterModes);
        
        FUNCTION_EXIT;
    }


    //
    // prepareForClose
    //
    void ECSMasterModes::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );

        m_pStationModesDataPointManager->deregisterStationModeStatusObserver( this );
        m_pStationModesDataPointManager->cleanUp();
		
		try{
			CORBA_CALL( (*m_masterECSAgent),
			            logout,
			            ( m_sessionId.c_str() ) );
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Unknown exception caught!" ); 
			throw;
		}
        FUNCTION_EXIT;
    }


    //
    // stationModeStatusUpdated
    //
    void ECSMasterModes::stationModeStatusUpdated( unsigned long locationKey, bool isValid, int value )
    {
        FUNCTION_ENTRY( "stationModeStatusUpdated" );

        ThreadGuard guard( m_updateLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "stationModeStatusUpdated() obtained ThreadLockable." );
        
        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
        std::ostringstream ostr;
		std::string status = "Failed to update station mode status for location ";
			status += m_locationNames[index];
			status += ". Reason: ";

        if ( false == isValid )
        {
            // Unknown StationMode status value
            ostr << "#";
            m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), 
                CURRENT_STATION_MODE_FAILED );
			status += "Station mode status unknown.";
			m_ecsMasterModesView.addStatusLine(status);
        }
        else if ( value  < 0 )
        {
            // Failed Station Mode.
            ostr << value;
            m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), CURRENT_STATION_MODE_FAILED );
			status += "Failed station mode.";
			m_ecsMasterModesView.addStatusLine(status);
            FUNCTION_EXIT;
            return;
        }
        else
        {
            unsigned long stationModeKey = m_pMappedECSDataCache->getStationModeKeyFromLocationKeyAndMode( locationKey, value );
            if ( stationModeKey != 0 )
            {
                ostr << value;
                StationModeDetail stationModeDetail = m_pMappedECSDataCache->getStationModeDetailFromKey
                    ( stationModeKey );
                if ( stationModeDetail.condition == EMERGENCY_COMPARE_STRING )
                {
                    m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), 
                        CURRENT_STATION_MODE_EMERGENCY );               
                }
                else if ( stationModeDetail.condition == CONGESTION_COMPARE_STRING )
                {
                    m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), 
                        CURRENT_STATION_MODE_CONGESTION );
                }
                else
                {
                    m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), 
                        CURRENT_STATION_MODE_NORMAL );
                }
                FUNCTION_EXIT;
                return;
            }
            else
            {
                // Station Mode status Value does not map to any configured station mode.
                ostr << value;
                m_ecsMasterModesView.setCurrentStationMode( index, ostr.str(), 
                    CURRENT_STATION_MODE_FAILED );
				status += "Station mode failed mapping to any configured station mode.";
				m_ecsMasterModesView.addStatusLine(status);
            }
        }

    }

	void ECSMasterModes::stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value )
	{
        FUNCTION_ENTRY( "stationVisualLockUpdated" );

        ThreadGuard guard( m_updateLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "stationVisualLockUpdated obtained ThreadLockable." );

        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );

		bool isStationLocked = (value != 0);
        m_ecsMasterModesView.setCurrentStationVisualLock( index, isStationLocked );

        FUNCTION_EXIT;
	}


    //
    // stationModeInProgressUpdated
    //
    void ECSMasterModes::stationModeInProgressUpdated( unsigned long locationKey, bool isValid, int value )
    {
        FUNCTION_ENTRY( "stationModeInProgressUpdated" );

        ThreadGuard guard( m_updateLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "stationModeInProgress obtained ThreadLockable." );

        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
        m_ecsMasterModesView.setCurrentStationModeInProgress( index, ( value > 0 ) );

        FUNCTION_EXIT;
    }

	void ECSMasterModes::stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value )
	{

	}

	void ECSMasterModes::stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState)
	{

	}


	void ECSMasterModes::proposedMasterModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* masterModes)
	{
		FUNCTION_ENTRY( "proposedMasterModesUpdated" );
		ThreadGuard guard( m_updateLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proposedMasterModesUpdated() obtained ThreadLockable." );

		int index;
		MasterModeDetail searchMasterModeDetail;
		for(unsigned int i=0; i<m_currentProposedMasterModes.length(); i++)
		{
            // Get master modes by filtering.
			searchMasterModeDetail.masterMode = m_currentProposedMasterModes[i].mode;
			if (std::binary_search( m_currentMasterModeDetails.begin(), m_currentMasterModeDetails.end(),
				searchMasterModeDetail, compareMasterModeDetailsByMode ))
			{
				index = (std::lower_bound( m_currentMasterModeDetails.begin(), m_currentMasterModeDetails.end(),
				searchMasterModeDetail, compareMasterModeDetailsByMode ) - m_currentMasterModeDetails.begin());

				m_ecsMasterModesView.setCurrentProposedMasterMode(index, NOT_CURRENTLY_PROPOSED);
			}
		}

		// Making sure the current proposed master mode sequence is re-initialised with no data.
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes emptySequence;
		m_currentProposedMasterModes = emptySequence; 
		
		if (isValid && masterModes != NULL)
		{
			m_currentProposedMasterModes.length(masterModes->length());

			for(unsigned int i=0; i<masterModes->length(); i++)
			{
                // Get master modes by filtering.
				m_currentProposedMasterModes[i] = (*masterModes)[i];
				searchMasterModeDetail.masterMode  = m_currentProposedMasterModes[i].mode;

				if (std::binary_search( m_currentMasterModeDetails.begin(), m_currentMasterModeDetails.end(),
                    searchMasterModeDetail, compareMasterModeDetailsByMode))
				{
					index = (std::lower_bound( m_currentMasterModeDetails.begin(), m_currentMasterModeDetails.end(),
					searchMasterModeDetail, compareMasterModeDetailsByMode ) - m_currentMasterModeDetails.begin());

					m_ecsMasterModesView.setCurrentProposedMasterMode(index, CURRENTLY_OVERRIDING_PROPOSED);
				}
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Mastermodes is NULL or valid flag is false." );
		}
		
		// else m_currentProposedMasterModes will be empty 

        FUNCTION_EXIT;
	}


	void ECSMasterModes::masterModeExecutionUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* masterModeExecutionInfo)
	{
		if(isValid && masterModeExecutionInfo != NULL)
		{
			// Master mode execution details
			std::ostringstream ssMasterMode;
			ssMasterMode << "Master mode execution state: ";
			ssMasterMode << getExecutionStateString(masterModeExecutionInfo->executionState);
			ssMasterMode << ", mode has been set from ";
			ssMasterMode << masterModeExecutionInfo->fromMasterMode;
			ssMasterMode << " to ";
			ssMasterMode << masterModeExecutionInfo->toMasterMode;
			
			if (masterModeExecutionInfo->executionError != TA_IRS_Bus::ECS_ERROR_NONE)
			{
				ssMasterMode << ", Execution error: ";
				ssMasterMode << getExecutionErrorString(masterModeExecutionInfo->executionError);

				std::string addInfo(masterModeExecutionInfo->additionalInfo.in());
				if (addInfo.length() > 0)
				{
					ssMasterMode << ", Additional Infomation: ";
					ssMasterMode << getErrorMessage(masterModeExecutionInfo->additionalInfo.in());
				}
			}
			
			m_ecsMasterModesView.addStatusLine(ssMasterMode.str());

			// Stations' mode execution details
			for (int i=0; i<masterModeExecutionInfo->stationUpdateDatas.length(); i++)
			{
				if (masterModeExecutionInfo->stationUpdateDatas[i].executionError != 
															TA_IRS_Bus::ECS_ERROR_NONE)
				{
					std::ostringstream ssStationMode;
					int locationKey = masterModeExecutionInfo->stationUpdateDatas[i].locationKey;
					ssStationMode << "Station Mode Execution error for Location " << m_locationNames[m_pMappedECSDataCache->getOrderedLocationIndexFromKey(locationKey)];
					ssStationMode << ": ";
					ssStationMode << getExecutionErrorString(masterModeExecutionInfo->stationUpdateDatas[i].executionError);

					std::string addInfo(masterModeExecutionInfo->stationUpdateDatas[i].additionalInfo.in());
					if (addInfo.length() > 0)
					{
						ssStationMode << ", Additional Infomation: ";
						ssStationMode << getErrorMessage(masterModeExecutionInfo->stationUpdateDatas[i].additionalInfo.in());
					}

					m_ecsMasterModesView.addStatusLine(ssStationMode.str());
				}
			}

		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "masterModeExecutionInfo is NULL or valid flag is false." );
		}
	}

    //
    // getBitSetIntegerFromDirection (Static helper to map display string code)
    //
    int ECSMasterModes::getBitSetIntegerFromDirection( EDirectionSettingIndex directionSettingIndex, const std::string& directionString )
    {
        const DirectionTypeValuePair* pDirectionMappingArray = DIRECTION_MAPPING_ARRAY;

        TA_ASSERT(  pDirectionMappingArray != NULL, "Object is NULL" );

        while (pDirectionMappingArray->directionMapping != UNKNOWN)
        {
            if (directionString.compare(pDirectionMappingArray->directionString) == 0)
            {
                return  ( 0x1 << ( directionSettingIndex + pDirectionMappingArray->directionMapping ) );
            }

            pDirectionMappingArray++;
        }

        return 0x0;
    }

	std::string ECSMasterModes::getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState)
    {
        std::string resultString;

        switch(executionState) 
        {
        case TA_IRS_Bus::ECS_EXECUTION_READY:
            resultString = "Ready";
        	break;
        case TA_IRS_Bus::ECS_EXECUTION_CONTROL:
            resultString = "Pending";
            break;
        case TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS:
            resultString = "In Progress";
            break;
        case TA_IRS_Bus::ECS_EXECUTION_COMPLETED:
            resultString = "Completed";
            break;
        default:
            break;
        }

        return resultString;
    }

    std::string ECSMasterModes::getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError)
    {
        std::string resultString;

        switch(executionError) 
        {
        case TA_IRS_Bus::ECS_ERROR_NONE:
            resultString = "No Error";
            break;
        case TA_IRS_Bus::ECS_ERROR_ABORTED:
            resultString = "Aborted";
            break;
        case TA_IRS_Bus::ECS_ERROR_TIME_LIMIT_EXCEEDED:
            resultString = "Time Limit Execeeded";
            break;
        case TA_IRS_Bus::ECS_ERROR_CORRUPTED_STATE:
            resultString = "Corrupted State";
            break;
        case TA_IRS_Bus::ECS_ERROR_WRONG_MODE_RESULT:
            resultString = "Unable to Set Station Mode";
            break;
        case TA_IRS_Bus::ECS_ERROR_MM_LOCAL_MODES_P:
            resultString = "Master Mode to Local Modes";
            break;
        case TA_IRS_Bus::ECS_ERROR_MM_IN_PROGRESS_P:
            resultString = "Master Mode In Progress";
            break;
        case TA_IRS_Bus::ECS_ERROR_MM_ACTIVE_P:
            resultString = "Master Mode Active";
            break;
        case TA_IRS_Bus::ECS_ERROR_MM_NORMAL_MM_FAILURE_P:
            resultString = "Master Mode Failure";
            break;
        case TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P:
            resultString = "Station Mode Write Failure";
            break;
        case TA_IRS_Bus::ECS_ERROR_SM_LOCAL_MODE:
            resultString = "Station In Local Mode";
            break;
        case TA_IRS_Bus::ECS_ERROR_SM_MASTER_MODE_LOCKED:
            resultString = "Master Mode Locked";
            break;
        case TA_IRS_Bus::ECS_ERROR_SM_IN_PROGRESS_P:
            resultString = "Station Mode In Progress";
            break;
        case TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P:
            resultString = "Station ECS Agent Failure";
            break;
        default:
            break;
        }

        return resultString;
    }


	bool ECSMasterModes::checkPermission()
	{
		bool actionAllowed = false;
		std::string reason;
		std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

		try
		{
			actionAllowed = m_rightsMgr->isActionPermittedOnResourceAndLocSub(
								sessionId, m_masterECSAgentEntityKey, 
								m_masterECSAgentLocation, m_masterECSAgentSubsystem,
								TA_Base_Bus::aca_DATA_POINT_WRITE, 
								reason, decisionModule);	
			
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown exception caught when checking the action permission for masterEcsAgent" );

			actionAllowed = false;
		}  
		
		return actionAllowed;

	}

} // namespace TA_IRS_App


