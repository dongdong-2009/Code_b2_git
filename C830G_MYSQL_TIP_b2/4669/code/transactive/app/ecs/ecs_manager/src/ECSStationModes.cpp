/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/ECSStationModes.cpp $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/09/23 15:08:35 $
  * Last modified by:  $Author: builder $
  *
  * This is the business class that provides all the the logic and data of the ECS Station Modes
  * user interface. It manipulates the ECS Station Modes user interface through IECSStationModesView.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

// TD14164 ++
#include "stdafx.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
// ++ TD14164 

#include "app/ecs/ecs_manager/src/ECSStationModes.h"
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "app\ecs\ecs_manager\src\ECSColourAccessor.h"

#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"

#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h" // CL-17274

#include <algorithm>

using namespace TA_Base_Core;

namespace TA_IRS_App
{

    //
    // Constructor
    //
    ECSStationModes::ECSStationModes( IECSStationModesView& view ) : 
	m_ecsStationModesView( view ), 
	m_pStationModesDataPointManager( 0 ), 
	m_pMappedECSDataCache( 0 ),
    m_currentUsersLocationKey( 0 ), 
	m_currentLocationSelectionIndex( -1 ), 
	m_currentStationModeSelectionIndex( -1 ),
    m_currentProposed( 0 ), 
	m_sessionId(""),
	m_proposedStationModeUpdateSubscriber(NULL),
	m_masterECSAgent(0),
	//zn++ - for beautiful code
	m_isCentralLocation(false),
	//++zn
	m_rightsMgr(NULL) 	// CL-17274
    {   
        FUNCTION_ENTRY( "Constructor" );

        m_pStationModesDataPointManager = std::auto_ptr< StationModesDataPointManager >( new StationModesDataPointManager() );
        m_pMappedECSDataCache = std::auto_ptr< MappedECSDataCache >( new MappedECSDataCache() );
		m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        FUNCTION_EXIT;
    }


    //
    // compareStationModeDetailsByMode
    //
    bool compareStationModeDetailsByMode( StationModeDetail stationModeDetail1, StationModeDetail stationModeDetail2 )
    {
        return ( stationModeDetail1.stationMode < stationModeDetail2.stationMode );
    }

    
    //
    // Destructor
    //
    ECSStationModes::~ECSStationModes()
    {
        FUNCTION_ENTRY( "Destructor" );

		if (m_proposedStationModeUpdateSubscriber != NULL)
		{
			delete m_proposedStationModeUpdateSubscriber;
			m_proposedStationModeUpdateSubscriber = NULL;
		}

        FUNCTION_EXIT;
    }


    //
    // stationModeStatusUpdated
    //
    void ECSStationModes::stationModeStatusUpdated( unsigned long locationKey, bool isValid, int nModeValue )
    {
        FUNCTION_ENTRY( "stationModeStatusUpdated" );

        ThreadGuard guard( m_updateLock );
        
        if (( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < m_locationKeys.size() ))
        {
            if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
            {
                std::ostringstream stationModeStatusOstr;
                ECurrentStationModeType currentStationModeType;
                StationModeDetail searchStationModeDetail = { nModeValue, "", "", "", "", locationKey };
                bool matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
                    m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );

				std::string status = "Failed to update station mode status for location ";
				status += m_locationNames[m_currentLocationSelectionIndex];
				status += ". Reason: ";
                if ( !isValid )
                {
                    // Station Mode status could not be determined.
                    stationModeStatusOstr << "#";
                    currentStationModeType = CURRENT_STATION_MODE_FAILED;
					status += "Station mode status unknown.";
					m_ecsStationModesView.addStatusLine(status);
                }
                else if ( matchesCurrentStationModeDetail )
                {
                    stationModeStatusOstr << nModeValue;
                    StationModeDetails::iterator stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
                    m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
                    if ( ( *stationModeDetailsIt ).condition == EMERGENCY_COMPARE_STRING )
                    {
                        currentStationModeType = CURRENT_STATION_MODE_EMERGENCY;
                    }
                    else if ( ( *stationModeDetailsIt ).condition == CONGESTION_COMPARE_STRING )
                    {
                        currentStationModeType = CURRENT_STATION_MODE_CONGESTION;
                    }
                    else
                    {
                        currentStationModeType = CURRENT_STATION_MODE_NORMAL;
                    }
                }
                else
                {
                    // Failed (negative), or unknown (did not match any configured Station Mode) mode.
                    stationModeStatusOstr << nModeValue;
                    currentStationModeType = CURRENT_STATION_MODE_FAILED;
					status  += "Failed station mode.";
					m_ecsStationModesView.addStatusLine(status);
                }

				status.erase();
                m_ecsStationModesView.setCurrentStationMode( stationModeStatusOstr.str(), currentStationModeType );
				status = "Station mode has been updated to ";
				status += stationModeStatusOstr.str();
				status += " for location ";
				status += m_locationNames[ m_currentLocationSelectionIndex ];
				m_ecsStationModesView.addStatusLine(status);
            }
        }

        FUNCTION_EXIT;
    }


	void ECSStationModes::stationModeExecutionUpdated(bool isValid, TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeExecutionInfo)
	{
        FUNCTION_ENTRY( "stationModeExecutionUpdated" );
		std::string locName("");
		try
		{
			std::auto_ptr<TA_Base_Core::ILocation> locationData(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(stationModeExecutionInfo->locationKey));
			locName = (locationData->getName());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Could not load location information for given key." );
		}

		std::ostringstream ostr;
		ostr << "Station Mode execution at ";
		ostr << locName;
		ostr <<	" has changed from ";
		ostr << stationModeExecutionInfo->fromStationMode;
		ostr << " to ";
		ostr << stationModeExecutionInfo->toStationMode;
		ostr << " for Master mode ";
		ostr << stationModeExecutionInfo->forMasterMode;

        //TD13744, to display excution state
        ostr << ", Execution state: ";
        ostr << getExecutionStateString(stationModeExecutionInfo->executionState);

        if (stationModeExecutionInfo->executionError != TA_IRS_Bus::ECS_ERROR_NONE)
        {
            ostr << ", Execution error: ";
            ostr << getExecutionErrorString(stationModeExecutionInfo->executionError);

            std::string addInfo(stationModeExecutionInfo->additionalInfo.in());
            if (addInfo.length() > 0)
            {
                ostr << ", Additional Infomation: ";
                ostr << getErrorMessage(stationModeExecutionInfo->additionalInfo.in());
            }
        }
        
		m_ecsStationModesView.addStatusLine(ostr.str());
	}

    //
    // stationModeInProgressUpdated
    //
    void ECSStationModes::stationModeInProgressUpdated( unsigned long locationKey, bool isValid, int value )
    {
        FUNCTION_ENTRY( "stationModeInProgressUpdated" );

        ThreadGuard guard( m_updateLock );

        if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {
            if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
            {
                if ( value > 0 )
                {
                    m_ecsStationModesView.setCurrentStationModeInProgress( true );
                }
                else
                {
                    m_ecsStationModesView.setCurrentStationModeInProgress( false );
                }
            }
        }

        FUNCTION_EXIT;
    }


	void ECSStationModes::stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value )
	{
        FUNCTION_ENTRY( "stationVisualLockUpdated" );

        ThreadGuard guard( m_updateLock );

        if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {
			bool isStationLocked = (value != 0);
			m_stationLockStatusMap[locationKey] = isStationLocked;
			// The following will ensure that the combo box colour map is updated as well. 
			int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey(locationKey);
			if(index != -1)
			{
				COLORREF colour;
				if(isStationLocked)
				{
					colour = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK);
				}
				else
				{
					colour = RGB(255, 255, 255);
				}
				m_ecsStationModesView.setColourMap(index, colour);
			}
		
			if(locationKey == m_locationKeys[ m_currentLocationSelectionIndex ])
			{
				m_ecsStationModesView.setCurrentStationVisualLock( isStationLocked, m_currentLocationSelectionIndex);
			}
            
        }

        FUNCTION_EXIT;
	}


	void ECSStationModes::stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value )
	{

	}

	void ECSStationModes::stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState)
	{

	}


	bool ECSStationModes::isProposedModeInOverridingMap(int proposedMode)
	{		
		CurrentProposedModeIt itr;

		if(m_currentProposed.size()>0){
			for(unsigned int i=0; i<m_currentOverridingStationModes.length(); i++)
			{
	// Code modified to build.  
	// ToDo - get modes by filtering.
				// marvin++
				itr = std::find(m_currentProposed.begin(), 
					m_currentProposed.end(), m_currentOverridingStationModes[i].mode);
				if(itr != m_currentProposed.end())
				{
					// Should not get in here at all.. 
					return true;
				}
				// ++marvin
			}
		}

		return false;
	}

	// marvin++
	void ECSStationModes::proposedStationModeUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedStationMode )
    {
        FUNCTION_ENTRY( "proposedStationModeUpdated" );

        ThreadGuard guard( m_updateLock );

		StationModeDetail searchStationModeDetail;
		StationModeDetails::iterator stationModeDetailsIt;
		bool matchesCurrentStationModeDetail=false;

		//zn++ CL-20842 ITEM2: if no proposed mode, should remove the one not current propose
		// perform check for absence of previously proposed modes and refresh the proposal
		//if(proposedStationMode->length()>0)
		if(proposedStationMode->length() >= 0)
		//++zn
		{
			if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
			{

				CurrentProposedModeIt itr = m_currentProposed.begin();

				if(itr != m_currentProposed.end())
				{
					for (;itr != m_currentProposed.end(); itr ++)
					{
						searchStationModeDetail.stationMode = *itr;
						matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );

						if (matchesCurrentStationModeDetail &&  !isProposedModeInOverridingMap((*itr)) )
						{
							stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
								m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
							m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
								NOT_CURRENTLY_PROPOSED );
						}
					}
					m_currentProposed.clear();
                    // liuyu++ #134
                    m_CongestionModePair.clear();
                    // ++liuyu #134
				}
			}
		}

		//highlight all the proposals for station mode
		for(unsigned int i=0; i<proposedStationMode->length(); i++)
		{
			unsigned long locationKey = (*proposedStationMode)[i].locationKey;
			int value = (*proposedStationMode)[i].mode;
            // liuyu++ #134
            unsigned long locationKeyOfCongestionPaire = (*proposedStationMode)[i].locationKeyOfCongestionPair;
            // ++liuyu #134
			unsigned long type = (*proposedStationMode)[i].modeType;

			if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) )
				&& type == TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION)
			{
				if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
				{		
					if ( isValid )
					{
                        m_currentProposed.push_back(value);
                        // liuyu++ #134
                        m_CongestionModePair.insert(CongestionModePairLocationKey::value_type(value, locationKeyOfCongestionPaire));
                        // ++liuyu #134
						searchStationModeDetail.stationMode = value;
						matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );


						if ( ( matchesCurrentStationModeDetail ) && !isProposedModeInOverridingMap(value))
						{
							stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
							m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
								CURRENTLY_PROPOSED );
						}
					}
				}
			}
		}

        FUNCTION_EXIT;
    }
	// ++marvin


	// marvin++
	void ECSStationModes::overridingProposedStationModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes)
	{
		FUNCTION_ENTRY( "overridingProposedStationModesUpdated" );

        ThreadGuard guard( m_updateLock );

		StationModeDetails::iterator stationModeDetailsIt;
		bool matchesCurrentStationModeDetail =false;
		StationModeDetail searchStationModeDetail;

	
        if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {
			//No overriding proposals, update the view
			if(stationModes->length() ==0)
			{
				for(unsigned int ctr=0; ctr<m_currentOverridingStationModes.length(); ctr++)
				{
					searchStationModeDetail.stationMode = m_currentOverridingStationModes[ctr].mode;
					matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
					if ( matchesCurrentStationModeDetail )
					{
						stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );

						m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
							NOT_CURRENTLY_PROPOSED );
					}
				}
				
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes emptySequence;
				// Making sure the current proposed station mode sequence is re-initialised with no data.
				m_currentOverridingStationModes = emptySequence; 
				return;
			}
            
			//check for all overriding proposals
			for(unsigned int j =0; j< stationModes->length(); j++)
			{
				for(unsigned int i=0; i<m_currentOverridingStationModes.length(); i++)
				{
					unsigned long locationKey = (*stationModes)[j].locationKey;

					if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
					{
						searchStationModeDetail.stationMode = m_currentOverridingStationModes[i].mode;
						matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );

						if ( matchesCurrentStationModeDetail )
						{
							stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
							m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );

							CurrentProposedModeIt itr;

							itr = std::find(m_currentProposed.begin(), 
								m_currentProposed.end(), m_currentOverridingStationModes[i].mode);

							if (itr != m_currentProposed.end())
							{
								m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
									CURRENTLY_PROPOSED );
							}
							else
							{
								m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
									NOT_CURRENTLY_PROPOSED );
							}
						}
					}
				}
			}

			TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes emptySequence;
			// Making sure the current proposed station mode sequence is re-initialised with no data.
			m_currentOverridingStationModes = emptySequence; 

			if ( isValid && stationModes != NULL )
			{
				// wenching++ 
				// bug 134 (TD)
				unsigned int length = 0;
			
				for(unsigned int i=0; i<stationModes->length(); i++)
				{
					unsigned long locationKey = (*stationModes)[i].locationKey;

					if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
						length++;
				}

				if(length>0)
				{
					m_currentOverridingStationModes.length(length);
					unsigned int index=0;
					for(unsigned int i=0; i<stationModes->length(); i++)
					{
						unsigned long locationKey = (*stationModes)[i].locationKey;
							
						if ( locationKey == m_locationKeys[ m_currentLocationSelectionIndex ] )
						{
							m_currentOverridingStationModes[index] = (*stationModes)[i];
						
							// Code modified to build.  
							// ToDo - get modes by filtering.

							searchStationModeDetail.stationMode = m_currentOverridingStationModes[index].mode;
							matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
								m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
							if ( matchesCurrentStationModeDetail )
							{
								stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
								m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
								m_ecsStationModesView.setStationModeProposalState( ( stationModeDetailsIt - m_currentStationModeDetails.begin() ), 
									CURRENTLY_OVERRIDING_PROPOSED );
							}

							index++;
						}
					}
				}
				// ++wenching
				// bug 134 (TD)
			} 
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Stationmodes is NULL or valid flag is false." );
			}
        }

        FUNCTION_EXIT;
	}
	// ++marvin


	void ECSStationModes::createStationECSAgents()
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
				m_ecsStationModesView.addStatusLine( errorInExecutionStr.str() );
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"Unknown", "Failed to create station ECS Agent for location %d", locationKey ); 
			}
		}
	}

    //
    // initialise
    //
    void ECSStationModes::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

		TA_Base_Core::IEntityDataList  agentList;
        try 
        {
			// Location info:
            determineUserLocation();
            loadLocations();
			// Create datapoints' proxies

			if (isCentralLocation())
            {
                m_pStationModesDataPointManager->refreshDataPointProxies( m_locationKeys, true );
            }
            else
            {
                if ( ( m_currentLocationSelectionIndex > -1 ) && 
                    ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
                {
                    LocationKeys locationKeys;
                    locationKeys.push_back( m_locationKeys[ m_currentLocationSelectionIndex ] );
                    m_pStationModesDataPointManager->refreshDataPointProxies( locationKeys, true);
                }
            }
			// Reference to master ecs agent for retrieval of proposed station modes
			agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::MasterECSAgentEntityData::getStaticType(), 1);
			//TA_ASSERT(agentList.size() == 1, "There can only be one Master ECS Agent at the OCC");
			if (agentList.size() != 1)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There can only be one Master ECS Agent at the OCC");
				TA_THROW( TA_Base_Core::DataException( "There can only be one Master ECS Agent at the OCC",TA_Base_Core::DataException::INVALID_VALUE,"Master ECS Agent" ) );
			}

			m_masterECSAgent = std::auto_ptr<MasterECSAgentNamedObj>(new MasterECSAgentNamedObj(agentList[0]->getName(),true));

            try
            {
			    CORBA_CALL( (*m_masterECSAgent), login, ( TA_IRS_Bus::ECS_NONE_OR_INVALID, m_sessionId.c_str() ) );
            }
		    // TD19322 - if can not resolve OccEcsAgent, enter the degrade mode, should continue launching the UI
		    catch (const TA_Base_Core::ObjectResolutionException& ex)
		    {
			    m_masterECSAgent->clear();
			    LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent, will do the degrade mode thing" ); 	
		    }// ++TD19322

            for(unsigned char j=0; j<agentList.size(); j++)
            {
                if(agentList[j] != NULL)
                {
                    delete agentList[j];
                    agentList[j] = 0;
                }
            }
			// ++marvin

			// Create Station ECS agents
			createStationECSAgents();
			
			// Station Mode data
			m_ecsStationModesView.addStatusLine("Loading station mode data...");

			if (isCentralLocation())
            {
                m_pMappedECSDataCache->refreshStationModeData();
            }
            else
            {
                m_pMappedECSDataCache->refreshStationModeData( m_currentUsersLocationKey );
            }  
	
            loadDetailsForLocation();


            // NOTE: Important to only acquire lock and register for datapoint updates AFTER all DataPoint 
            // proxies have been created (ie. after refreshDataPointProxies) else deadlock may occur.
            ThreadGuard guard( m_updateLock );
            
            m_pStationModesDataPointManager->registerStationModeStatusObserver( this );
			
			m_ecsStationModesView.addStatusLine("Finished loading station mode data.");

			// marvin++
			m_proposedStationModeUpdateSubscriber = new TA_IRS_App::ProposedStationModeUpdateSubscriber(this);
			// ++marvin

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ECSStationModes::initialise() - start color map" );

			// Initialise the Combo box colour map. This is once off only
//			int value = 0;
//			for(unsigned int i=0; i<m_locationKeys.size(); i++)
//			{
//				try
//				{
//					value = m_pStationModesDataPointManager->getStationLockStatus( m_locationKeys[i]);
//					m_stationLockStatusMap[m_locationKeys[i]] = (value != 0);
//					COLORREF colour;
//					if(m_stationLockStatusMap[m_locationKeys[i]])
//					{
//						colour = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK);
//					}
//					else
//					{
//						colour = RGB(255, 255, 255);
//					}
//
//					m_ecsStationModesView.setColourMap(i, colour);
//				}
//				catch(const ScadaProxyException&)
//				{
//					LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Lock status." );
//				}
//				
//			}
			loadValuesForLocation();
			// CL-17274++
			TA_Base_Bus::RightsLibraryFactory rightsFactory;
			
			if ( TA_Base_Core::RunParams::getInstance().isSet("DebugAlwaysTrueRightsLib") )
			{
				m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
			}
			else
			{
				m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
			}
			// ++CL-17274
        }
		catch (TA_Base_Core::AccessDeniedException& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::AccessDeniedException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to log in with ECS Master Agent" );
			for(unsigned char i=0; i<agentList.size(); i++)
			{
				if(agentList[i] != NULL)
				{
					delete agentList[i];
					agentList[i] = 0;
				}
			}	

			std::ostringstream ostr;

			ostr << "Failed to log in with Master ECS Agent. Reason: Access denied. \r\n";
			ostr << "Please close application and try again later.";
			// TD14164 ++
			/*m_ecsStationModesView.displayErrorMessage(ostr.str());*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "Access denied";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_520011);
			// ++ TD14164 
			m_ecsStationModesView.addStatusLine(ostr.str());

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
			std::ostringstream ostr;

			ostr << "Failed to log in with Master ECS Agent. Reason: ";
			ostr << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			ostr << "\r\nPlease close application and try again later.";
			// TD14164 ++
			/*m_ecsStationModesView.displayErrorMessage(ostr.str());*/
			TA_Base_Bus::TransActiveMessage userMsg;
			std::string reason = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UE_520011);
			// ++ TD14164 
			m_ecsStationModesView.addStatusLine(ostr.str());

			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
			return;
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

			std::ostringstream ostr;

			ostr << "Failed to log in with Master ECS Agent. Reason: ";
			ostr << ex.what();
			ostr << "\r\nPlease close application and try again later.";
			// TD14164 ++
			/*m_ecsStationModesView.displayErrorMessage(ostr.str());*/
			TA_Base_Bus::TransActiveMessage userMsg;
			std::string reason = ex.what();
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UE_520011);
			// ++ TD14164 
			m_ecsStationModesView.addStatusLine(ostr.str());

			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
			return;

		}
        catch (DataException& ex)
        {
            std::ostringstream ostr;
            ostr << "ECS Manager was unable to retrieve mandatory ECS database configuration data: ";
            ostr << ex.what() << ".\r\nApplication will now exit.";
			m_ecsStationModesView.addStatusLine("ECS database configuration data error. Application will exit.");
			// TD14164 ++
            /*m_ecsStationModesView.displayErrorMessage( ostr.str() );*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);
			// ++ TD14164 
            m_ecsStationModesView.terminateApplication();
            FUNCTION_EXIT;
            return;
        }
        catch (DatabaseException& ex)
        {
            std::ostringstream ostr;
            ostr << "ECS Manager was unable to retrieve mandatory ECS database configuration data: ";
            ostr << ex.what() << ".\r\nApplication will now exit.";
			m_ecsStationModesView.addStatusLine("Unable to retrieve ECS database configuration. Application will exit.");
			// TD14164 ++
            /*m_ecsStationModesView.displayErrorMessage( ostr.str() );*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);
			// ++ TD14164 
            m_ecsStationModesView.terminateApplication();
            FUNCTION_EXIT;
            return;
        }

		//zn++
		if (isCentralLocation())
        {
            m_ecsStationModesView.enableLocationSelectionControl( true );
        }
		//++zn

        FUNCTION_EXIT;
    }


    //
    // stationModeSelectionChanged
    //
    void ECSStationModes::stationModeSelectionChanged( int stationModeSelectionIndex )
    {
		// marvin++
		m_ecsStationModesView.enableApplyStationModeControl( true );
		// ++marvin
    }


    //
    // applyStationMode
    //
    void ECSStationModes::applyStationMode( int stationModeSelectionIndex )
    {
        FUNCTION_ENTRY( "applyStationMode" );

        bool bCanAccess = false;
        try
        {
            bCanAccess = checkPermission();
        }
        catch ( TA_Base_Core::DataException& expWhat )
        {
            m_ecsStationModesView.addStatusLine( expWhat.what() );
            return;
        }
        catch ( std::exception& expWhat )
        {
            m_ecsStationModesView.addStatusLine( expWhat.what() );
            return;
        }
        catch (...)
        {
            m_ecsStationModesView.addStatusLine( "Unable to apply station mode due to unknown error" );
            return;
        }

		if ( !bCanAccess )
		{
			m_ecsStationModesView.addStatusLine( "No rights to apply station mode" );

			return;
		}

        if (( m_currentStationModeDetails.size() <= stationModeSelectionIndex ) || ( stationModeSelectionIndex < 0 ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "ECSStationModes::applyStationMode called with index beyond range of StationModes that should be currently displayed to user." );
            return;
        }
    
		// TO DO - No longer use data point proxies.. now we have an IDL call to the Station ECS Agent.

        // liuyu++ #134
        // if the congestion mode pair is in master mode issue a warning message.
        bool fWarning = false;
        if (m_currentStationModeDetails[ stationModeSelectionIndex ].condition == CONGESTION_COMPARE_STRING)
        {
            int value = m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
            std::map<int, unsigned long>::iterator it = m_CongestionModePair.find(value);
            if (it!= m_CongestionModePair.end())
            {
                unsigned long pairedLocationkey = (*it).second;
                // if the Congestion Pair is in an active master mode, issue a warning dialog
                if (pairedLocationkey != TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR)
                {
                    int value = 0;
                    bool isValid = true;
                    try
                    {
                        value = m_pStationModesDataPointManager->getStationModeStatus( pairedLocationkey );
                    }
                    catch ( const ScadaProxyException& )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                        isValid = false;
                        value = 0;
                    }
                    {
                        std::ostringstream stationModeStatusOstr;
                        StationModeDetails::iterator stationModeDetailsIt;
                        StationModeDetail searchStationModeDetail;
                        searchStationModeDetail.stationMode = value;
                        bool matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
                            m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
                
				        std::string status = "Failed to retrieve station mode status for location ";
				        status += "%d. Reason: ", pairedLocationkey;
                        if ( false == isValid )
                        {
                            // Station Mode status could not be determined.
                            stationModeStatusOstr << "#";
					        status += "Station mode status unknown.";
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, status.c_str() );
                        }
                        else if ( matchesCurrentStationModeDetail )
                        {
                            stationModeStatusOstr << value;
                            stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
                            m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
                            if ( ( *stationModeDetailsIt ).condition == EMERGENCY_COMPARE_STRING )
                            {
                                // congestion pair is in master mode
                                fWarning = true;
                            }
                        }
                    }

                    // check if congestion pair is under a proposed master mode
                    if (false == fWarning)
                    {
                        TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedModes = 0;
						CORBA_CALL_RETURN( proposedModes,
						                   (*m_masterECSAgent),
						                   getAllProposedModes,
						                   () );
		                for(unsigned int i=0; i < proposedModes->length(); i++)
		                {
							// wenching ++
							// bug 134 (TD)
							if(((*proposedModes)[i].locationKey == pairedLocationkey) && ((*proposedModes)[i].modeType == TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION))
			                {
				                fWarning = true;
                                break;
			                }
							// ++ wenching
							// bug 134 (TD)
		                }
                    }

                    // check if congestion pair is applying master mode.
                    if (false == fWarning)
                    {
                        isValid = true;
                        try
                        {
                            value = m_pStationModesDataPointManager->getStationModeInProgress( pairedLocationkey );
                        }
                        catch ( const ScadaProxyException& )
                        {
                            LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                            isValid = false;
                            value = 0;
                        }
                        std::ostringstream stationModeStatusOstr;
                        StationModeDetails::iterator stationModeDetailsIt;
                        StationModeDetail searchStationModeDetail;
                        searchStationModeDetail.stationMode = value;
                        bool matchesCurrentStationModeDetail = std::binary_search( m_currentStationModeDetails.begin(), 
                            m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
                
				        std::string status = "Failed to retrieve station mode status for location ";
				        status += "%d. Reason: ", pairedLocationkey;
                        if ( false == isValid )
                        {
                            // Station Mode status could not be determined.
                            stationModeStatusOstr << "#";
					        status += "Station mode status unknown.";
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, status.c_str() );
                        }
                        else if ( matchesCurrentStationModeDetail )
                        {
                            stationModeStatusOstr << value;
                            stationModeDetailsIt = std::lower_bound( m_currentStationModeDetails.begin(), 
                            m_currentStationModeDetails.end(), searchStationModeDetail, compareStationModeDetailsByMode );
                            if ( ( *stationModeDetailsIt ).condition == EMERGENCY_COMPARE_STRING )
                            {
                                // congestion pair is in master mode
                                fWarning = true;
                            }
                        }
                    }
               }
            }
        }

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		std::string stationMode;
		UINT errorCode;
		// ++ TD14164 

        bool tryAgain = true;
		bool canOverride = false;

		std::ostringstream confirmStr;
		confirmStr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
		std::string param0 = confirmStr.str();
		userMsg << param0.c_str();
		CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
		userMsg << param1;

        if (true == fWarning)
        {	    
			// TD14164 ++		
			errorCode = IDS_UW_520004;
			// ++ TD14164 
        }
        else
        {    
			// TD14164 ++
			errorCode = IDS_UW_520005;
			// ++ TD14164 
        }
        // ++liuyu #134

		// TD14164 ++
		/*if ( m_ecsStationModesView.aquireConfirmation( confirmStr.str() ) )*/
		UINT selectedButton = userMsg.showMsgBox(errorCode);
		if ( selectedButton == IDOK )
		// ++ TD14164 
        {

			while(tryAgain)
			{
				try
				{
					bool isEmergencyMode = ( m_currentStationModeDetails[stationModeSelectionIndex].condition.compare(EMERGENCY_COMPARE_STRING) == 0);
					m_ecsStationModesView.setCurrentAppliedMode(stationModeSelectionIndex, isEmergencyMode);

					CORBA_CALL( (*m_stationECSAgents[m_locationKeys[ m_currentLocationSelectionIndex ]]),
					            executeStationMode,
					            ( m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode, canOverride, m_sessionId.c_str() ) );

					tryAgain = false;

					std::ostringstream status;
					status << "Station mode  ";
					status << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					status << " applied to location ";
					status << m_locationNames[ m_currentLocationSelectionIndex ];
					m_ecsStationModesView.addStatusLine(status.str());
					//m_ecsStationModesView.setCurrentAppliedMode(stationModeSelectionIndex, isProposedModeInOverridingMap(m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode));
				}
				catch( const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationModeInProgress&)
				{	

					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
					str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					std::string stationMode = str.str();
					CString reason = "A Station Mode is in progress";
					userMsg << stationMode.c_str();
					userMsg << (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
					userMsg << reason;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_520006); 

					if ( selectedButton == IDOK )
					{		
						canOverride = true;
					}
                    else
                    {
                        tryAgain = false;
                    }
				}
				catch(const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationLocked& ex )
				{
					tryAgain = false;
				
					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
					str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					std::string param0 = str.str();
					userMsg << param0.c_str();
					CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
					userMsg << param1;
					CString param2 = (TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)).c_str();
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);

					std::ostringstream ostr;
					ostr << "Unable to Apply ECS Station Mode ";
					ostr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					ostr << " to Station " << m_locationNames[ m_currentLocationSelectionIndex ] << ":  \r\n";
					ostr << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
					m_ecsStationModesView.addStatusLine(ostr.str());
				}
				catch (const TA_Base_Core::ObjectResolutionException& )
				{
					tryAgain = false;
					std::ostringstream ostr;
					ostr << "Unable to Apply ECS Station Mode ";
					ostr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					ostr << " to Station " << m_locationNames[ m_currentLocationSelectionIndex ] << ":  \r\n";
					//ostr << ex.what();
					ostr << "Failure to connect to " << (*m_stationECSAgents[m_locationKeys[ m_currentLocationSelectionIndex ]]).getObjectName();
				
					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
					str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					std::string param0 = str.str();
					userMsg << param0.c_str();
					CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
					userMsg << param1;
					CString temp = "Failure to connect to";
					std::string temp2 = (*m_stationECSAgents[m_locationKeys[ m_currentLocationSelectionIndex ]]).getObjectName();
					CString param2 = temp + temp2.c_str();
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);
				
					m_ecsStationModesView.addStatusLine(ostr.str());
				}
				// CL-17274++
				catch (const TA_Base_Core::AccessDeniedException& )
				{
					tryAgain = false;
					std::ostringstream ostr;
					ostr << "No rights to perform Station Mode ";
					ostr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					ostr << " to Station " << m_locationNames[ m_currentLocationSelectionIndex ] << ":  \r\n";
					
					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
					str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					std::string param0 = str.str();
					userMsg << param0.c_str();
					CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
					userMsg << param1;
					CString temp = "No rights to write datapoint";
					CString param2 = temp;
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);
					
					m_ecsStationModesView.addStatusLine(ostr.str());
					
				}
                // ++CL-17274
                catch( const CORBA::Exception& ex )
                {
                    tryAgain = false;

                    std::ostringstream ostr;
                    ostr << "Unable to Apply ECS Station Mode ";
                    ostr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
                    ostr << " to Station " << m_locationNames[ m_currentLocationSelectionIndex ] << ":  \r\n";
                    ostr << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

                    TA_Base_Bus::TransActiveMessage userMsg;
                    std::ostringstream str;
                    str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
                    std::string param0 = str.str();
                    userMsg << param0.c_str();
                    CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
                    userMsg << param1;
                    CString param2 = (TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)).c_str();
                    userMsg << param2;
                    userMsg.showMsgBox(IDS_UW_520001);

                    m_ecsStationModesView.addStatusLine(ostr.str());
                }
				catch( ... )
				{
					tryAgain = false;
					std::ostringstream ostr;
					ostr << "Unable to Apply ECS Station Mode ";
					ostr << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					ostr << " to Station " << m_locationNames[ m_currentLocationSelectionIndex ] << ":  \r\n";
					ostr << "Unknown exception";
					
					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
					str << m_currentStationModeDetails[ stationModeSelectionIndex ].stationMode;
					std::string param0 = str.str();
					userMsg << param0.c_str();
					CString param1 = (m_locationNames[ m_currentLocationSelectionIndex ]).c_str();
					userMsg << param1;
					CString param2 = "Unknown exception";
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);
				
					m_ecsStationModesView.addStatusLine(ostr.str());
				} 

				checkModeInProgress(); //TD16730
			}
		}


        FUNCTION_EXIT;
    }


    //
    // locationSelectionChanged
    //
    void ECSStationModes::locationSelectionChanged( int locationIndex )
    {
        FUNCTION_ENTRY( "locationSelectionChanged" );

        ThreadGuard guard( m_updateLock );
		//zn++
		m_ecsStationModesView.enableLocationSelectionControl(false);
		//++zn
        m_currentLocationSelectionIndex = locationIndex;
        loadDetailsForLocation();
        loadValuesForLocation();
		//zn++
		m_ecsStationModesView.enableLocationSelectionControl(true);
		//++zn
		

        FUNCTION_EXIT;
    }


    //
    // prepareForClose
    //
    void ECSStationModes::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );

        m_pStationModesDataPointManager->deregisterStationModeStatusObserver( this );
        //TD13744, not used any more
        //m_pStationModesDataPointManager->deregisterProposedStationModeObserver( this );
        m_pStationModesDataPointManager->cleanUp();

        if ( NULL == m_masterECSAgent.get() )
        {
            return;
        }

        try
        {
		    CORBA_CALL( (*m_masterECSAgent), logout, ( m_sessionId.c_str() ) );
        }
		// TD19322 ++
		catch (TA_Base_Core::ObjectResolutionException &ex) 
		{
			m_masterECSAgent->clear();
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 
		}// ++ TD19322
        FUNCTION_EXIT;
    }


    //
    // determineUserLocation
    //
    void ECSStationModes::determineUserLocation()
    {
        FUNCTION_ENTRY( "determineUserLocation" );

        /*// Determine Physical Site of this Console Session.
        std::string sessionID = RunParams::getInstance().get( RPARAM_SESSIONID );

        IConsole* console = ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
        m_currentUsersLocationKey = console->getLocation();
        delete console;*/
		
		// TD17267 ++	use login location key instead of the console key to determine current user location
		std::string locationkey = RunParams::getInstance().get( RPARAM_LOCATIONKEY );
		m_currentUsersLocationKey = atol( locationkey.c_str() );

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "User Location Key is %lu ", m_currentUsersLocationKey);
		// ++ TD17267

		//zn++	- for beautiful code
        if ( m_currentUsersLocationKey == ECSManagerConfigurationAccess::getInstance()
            .getEntityConfiguration().getCentralControlLocationKey() )
        {
			m_isCentralLocation = true;
        }
		//++zn
	
        FUNCTION_EXIT;

    }


    //
    // loadLocations
    //
    void ECSStationModes::loadLocations()
    {
        FUNCTION_ENTRY( "loadLocations" );
        
        m_pMappedECSDataCache->refreshOrderedLocationData();
   
        OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
        OrderedLocations::const_iterator orderedLocationsIt = orderedLocations.begin();
		m_ecsStationModesView.addStatusLine("Loading locations...");
        while ( orderedLocationsIt != orderedLocations.end() )
        {
            m_locationKeys.push_back( ( *orderedLocationsIt ).key );
            m_locationNames.push_back( ( *orderedLocationsIt ).name );
            orderedLocationsIt++;
        }

        m_ecsStationModesView.setLocations( m_locationNames );
		m_ecsStationModesView.addStatusLine("Locations loaded.");

		//zn++
		m_ecsStationModesView.enableLocationSelectionControl( false );
		//++zn

		if (isCentralLocation())
        {
            m_currentLocationSelectionIndex = 0; // Set to first location by default.
			//zn++
            //m_ecsStationModesView.enableLocationSelectionControl( true );
			//++zn
        }
        else
        {
            m_currentLocationSelectionIndex = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( m_currentUsersLocationKey );
			//zn++
            //m_ecsStationModesView.enableLocationSelectionControl( false );
			//++zn
        }

        m_ecsStationModesView.setSelectedLocationIndex( m_currentLocationSelectionIndex );

        FUNCTION_EXIT;
    }


    //
    // loadDetailsForLocation
    //
    void ECSStationModes::loadDetailsForLocation()
    {
        FUNCTION_ENTRY( "loadDetailsForLocation" );

        m_ecsStationModesView.enableApplyStationModeControl( false );
        m_currentStationModeDetails.clear();
        m_currentStationModeSelectionIndex = -1;

        if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {
            if ( m_currentLocationSelectionIndex > 0 )
            {
                m_ecsStationModesView.setOutboundLocation( m_locationNames[ ( m_currentLocationSelectionIndex - 1 ) ] );
                // DTL-149
                m_ecsStationModesView.setOutboundStationLock( m_stationLockStatusMap[m_locationKeys[m_currentLocationSelectionIndex - 1]] );
            }
            else
            {
                m_ecsStationModesView.setOutboundLocation( std::string("") );
                // DTL-149
                m_ecsStationModesView.setOutboundStationLock( false );
            }
			
            // DTL-149	
			// m_ecsStationModesView.setOutboundStationLock( m_stationLockStatusMap[m_locationKeys[m_currentLocationSelectionIndex - 1]] );

            if ( ( m_currentLocationSelectionIndex + 1 ) < static_cast< int >( m_locationNames.size() ) )
            {
                m_ecsStationModesView.setInboundLocation( m_locationNames[ ( m_currentLocationSelectionIndex + 1 ) ] );
                // DTL-149
				m_ecsStationModesView.setInboundStationLock( m_stationLockStatusMap[m_locationKeys[m_currentLocationSelectionIndex + 1]] );
            }
            else
            {
                m_ecsStationModesView.setInboundLocation( std::string("") );
                // DTL-149
				m_ecsStationModesView.setInboundStationLock( false );
            }

            // DTL-149
            // m_ecsStationModesView.setInboundStationLock( m_stationLockStatusMap[m_locationKeys[m_currentLocationSelectionIndex + 1]] );

            StationModeKeys stationModeKeys = m_pMappedECSDataCache->
                getStationModeKeysFromLocationKey( m_locationKeys[ m_currentLocationSelectionIndex ] );
            StationModeKeys::const_iterator stationModeKeysIt = stationModeKeys.begin();
            while ( stationModeKeysIt != stationModeKeys.end() )
            {
				StationModeDetail stationModeDetail = m_pMappedECSDataCache->getStationModeDetailFromKey( ( *stationModeKeysIt ) );
				if (TA_Base_Core::DatabaseKey::isInvalidKey(stationModeDetail.stationMode))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid station mode");
					stationModeKeysIt++;
					continue;
				}
                m_currentStationModeDetails.push_back( stationModeDetail );
                stationModeKeysIt++;
            }
            std::sort( m_currentStationModeDetails.begin(), m_currentStationModeDetails.end(), compareStationModeDetailsByMode );
            m_ecsStationModesView.setStationModeDetails( m_currentStationModeDetails );
			m_ecsStationModesView.addStatusLine("Station mode details loaded.");
        }

        FUNCTION_EXIT;
    }


    //
    // loadValuesForLocation
    //
    void ECSStationModes::loadValuesForLocation()
    {
        FUNCTION_ENTRY( "loadValuesForLocation" );

        if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {     
            // Determine current Status.
            unsigned long locationKey =  m_locationKeys[ m_currentLocationSelectionIndex ];
            int value = 0;
            bool isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeStatus( locationKey );
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }
            stationModeStatusUpdated( locationKey, isValid, value );

            // Determine current In Progress.
            isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeInProgress( locationKey );
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }
            stationModeInProgressUpdated( locationKey, isValid, value );

			// Determine the lock status mode for each station. Must be called only when
			// initialise() has finished.
			isValid = true;

			stationVisualLockUpdated(locationKey, isValid, m_stationLockStatusMap[locationKey]);

			TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedModes = 0;
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes proposedOverridingStationModes;
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes proposedStationModes;

			// TO DO. Retrieving proposed modes is now handled by the Station ECS Agent.
			std::ostringstream errorInExecutionStr;
			try
			{
				// marvin++ perform retrieval of proposed modes
				CORBA_CALL_RETURN( proposedModes,
				                   (*m_masterECSAgent),
				                   getAllProposedModes,
				                   () );

				filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION,*proposedModes,proposedStationModes);
				filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION,*proposedModes,proposedOverridingStationModes);

				proposedStationModeUpdated( isValid, &proposedStationModes );
				overridingProposedStationModesUpdated(isValid, &proposedOverridingStationModes);
				// ++marvin
			}
 			catch( const CORBA::Exception& ex )
			{
				isValid = false;
                value = 0;

				errorInExecutionStr << "Unable to retrieve proposed Station Mode. Reason: CORBA Exception";
				m_ecsStationModesView.addStatusLine( errorInExecutionStr.str() );
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Station Mode from the Station ECS Agent" ); 
			}
			catch (const TA_Base_Core::ObjectResolutionException& ex)
			{
				isValid = false;
                value = 0;

				errorInExecutionStr << "Unable to retrieve proposed Station Mode. Reason: Unable to resolve Master ECS Agent.";
				m_ecsStationModesView.addStatusLine( errorInExecutionStr.str() );
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Station Mode from the Station ECS Agent" ); 

			}
			catch( ... )
			{
				isValid = false;
                value = 0;
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Unknown exception caught!" ); 
			} 
        }

        FUNCTION_EXIT;
    }

	//16730
	void ECSStationModes::checkModeInProgress()
	{
		if ( ( m_currentLocationSelectionIndex > -1 ) && ( m_currentLocationSelectionIndex < static_cast< int >( m_locationKeys.size() ) ) )
        {     
            // Determine current Status.
            unsigned long locationKey =  m_locationKeys[ m_currentLocationSelectionIndex ];
			int value = 0;
			bool isValid = true;

			try
            {
                value = m_pStationModesDataPointManager->getStationModeInProgress( locationKey );
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }

            stationModeInProgressUpdated( locationKey, isValid, value );
		}

	}


	// marvin++
	void ECSStationModes::filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes)
	{

		//TA_ASSERT(filteredModes.length()==0,"Corba sequence not initialized");
		//if (0 != filteredModes.length())
		//{
		//	filteredModes.length(0);
		//}

		int length = 0;
		for(unsigned int ctr=0; ctr < proposedModes.length(); ctr++)
		{
			if(proposedModes[ctr].modeType == type)
			{
				length++;
			}
		}

		filteredModes.length(length);

		int toCtr=0;
		for(unsigned int frmCtr=0; frmCtr < proposedModes.length(); frmCtr++)
		{
			if(proposedModes[frmCtr].modeType == type)
			{
				filteredModes[toCtr] = proposedModes[frmCtr];
				toCtr++;
			}
		}
	}
	// ++marvin

	// 7196-Start
	void ECSStationModes::refreshHealthPollInterval()
    {
        FUNCTION_ENTRY( "refreshHealthPollInterval" );
		m_pStationModesDataPointManager->refreshHealthPollInterval();
		FUNCTION_EXIT;
	}
	// 7196-End

    std::string ECSStationModes::getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState)
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

    std::string ECSStationModes::getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError)
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
        //TD16800
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

	// CL-17274++
	bool ECSStationModes::checkPermission()
	{
		bool actionAllowed = false;
		std::string reason;
		std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

		TA_Base_Core::IEntityDataList  agentList;
		unsigned long locationkey = m_locationKeys[ m_currentLocationSelectionIndex ];
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "17274 locationkey = %lu", locationkey);
		agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::StationECSAgentEntityData::getStaticType(), locationkey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "17274 number of agent %d", agentList.size());
        if ( 0 == agentList.size() )
        {
            TA_THROW( TA_Base_Core::TransactiveException( "No Agent configured" ))
        }
		unsigned long currentStationEntityKey = agentList[0]->getKey();

		try
		{
			actionAllowed = m_rightsMgr->isActionPermittedOnResource(
				sessionId, currentStationEntityKey, 
				TA_Base_Bus::aca_DATA_POINT_WRITE, 
				reason, decisionModule);
			
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when checking the action permission for masterEcsAgent" );
			
			actionAllowed = false;
		}  
		
		return actionAllowed;
	}
	// ++CL-17274

	//zn++ - for beautiful code
	bool ECSStationModes::isCentralLocation()
	{
		return m_isCentralLocation;
	}
	//++zn
        
} // namespace TA_IRS_App
