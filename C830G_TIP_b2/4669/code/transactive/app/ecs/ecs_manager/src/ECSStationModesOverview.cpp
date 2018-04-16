/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This is the business class that provides all the the logic and data of the ECS Station Modes 
  * Overview user interface. It manipulates the ECS Station Modes Overview user interface through 
  * IECSStationModesOverviewView.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

// TD14164 ++
#include <sstream>
#include "app/ecs/ecs_manager/src/StdAfx.h"

// ++ TD14164 
#include "app/ecs/ecs_manager/src/ECSStationModesOverview.h"
#include "app/ecs/ecs_manager/src/IECSStationModesOverviewView.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"

#include "bus/generic_gui/src/TransactiveMessage.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DatabaseKey.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
    //
    // Constructor
    //
    ECSStationModesOverview::ECSStationModesOverview( IECSStationModesOverviewView& view )
        : m_ecsStationModesOverviewView( view ), m_currentSelection( -1, PROPOSED_IS_UNKNOWN ), m_sessionId(""),
		m_masterECSAgent(0),m_proposedStationModeUpdateSubscriber(NULL)
    {
        FUNCTION_ENTRY( "Constructor" );

        m_pStationModesDataPointManager = std::auto_ptr< StationModesDataPointManager >( new StationModesDataPointManager() );
        m_pMappedECSDataCache = std::auto_ptr< MappedECSDataCache >( new MappedECSDataCache() );
		m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    ECSStationModesOverview::~ECSStationModesOverview()
    {
        FUNCTION_ENTRY( "Destructor" );

		delete m_proposedStationModeUpdateSubscriber;
		m_proposedStationModeUpdateSubscriber = NULL;

        m_mapCongestionModes.clear();

        FUNCTION_EXIT;
    }


	void ECSStationModesOverview::createStationECSAgents()
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
				m_ecsStationModesOverviewView.addStatusLine( errorInExecutionStr.str() );

				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"Unknown", "Failed to create station ECS Agent for location %d", locationKey ); 
			}
		}		
	}

    //
    // initialise
    //
    void ECSStationModesOverview::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

		TA_Base_Core::IEntityDataList  agentList;
        try 
        {
			m_pMappedECSDataCache->refreshOrderedLocationData();
            OrderedLocations orderedLocations = m_pMappedECSDataCache->getOrderedLocations();
            OrderedLocations::iterator orderedLocationsIt = orderedLocations.begin();
            while ( orderedLocationsIt != orderedLocations.end() )
            {
                m_locationKeys.push_back( ( *orderedLocationsIt ).key );
                m_locationNames.push_back( ( *orderedLocationsIt ).name );
                orderedLocationsIt++;
            }
            m_ecsStationModesOverviewView.enableApplyAllCongestedStationModesControl( false );
            m_ecsStationModesOverviewView.enableApplyStationModeControl( false );
            m_ecsStationModesOverviewView.setLocations( m_locationNames );
			m_pStationModesDataPointManager->refreshDataPointProxies( m_locationKeys, true );
            m_pMappedECSDataCache->refreshStationModeData();
            
			// acquire connection to the master ecs agent to access proposed modes
			TA_Base_Core::IEntityDataList  agentList;
			agentList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::MasterECSAgentEntityData::getStaticType(), 1);
			if (1 != agentList.size())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "There can only be one Master ECS Agent at the OCC" );
				TA_THROW( TA_Base_Core::DataException( "There can only be one Master ECS Agent at the OCC",TA_Base_Core::DataException::INVALID_VALUE,"Master ECS Agent" ) );
			}

			m_masterECSAgent = std::auto_ptr<MasterECSAgentNamedObj>(new MasterECSAgentNamedObj(agentList[0]->getName(),true));
			CORBA_CALL( (*m_masterECSAgent),
			            login,
			            ( TA_IRS_Bus::ECS_NONE_OR_INVALID, m_sessionId.c_str() ) );

			createStationECSAgents();			

			for(unsigned char j=0; j<agentList.size(); j++)
			{
				if(agentList[j] != NULL)
				{
					delete agentList[j];
					agentList[j] = 0;
				}
			}

			m_ecsStationModesOverviewView.addStatusLine("Loading station mode data...");

            loadStationModeDataPointValues();

			m_proposedStationModeUpdateSubscriber = new TA_IRS_App::ProposedStationModeUpdateSubscriber(this);

        }
		catch (TA_Base_Core::AccessDeniedException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::AccessDeniedException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to log in with ECS Master Agent");

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
			m_ecsStationModesOverviewView.addStatusLine(ostr.str());

			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"TA_Core::AccessDeniedException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Access Denied" ); 

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
			m_ecsStationModesOverviewView.addStatusLine(ostr.str());

			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"TA_Core::CORBAException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 

			FUNCTION_EXIT;
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

			TA_Base_Bus::TransActiveMessage userMsg;
			std::string reason = ex.what();
			userMsg << reason.c_str();
			userMsg.showMsgBox(IDS_UE_520011);

			std::ostringstream ostr;
			ostr << "Failed to log in with Master ECS Agent. Reason: ";
			ostr << ex.what();
			ostr << "\r\nPlease close application and try again later.";
			m_ecsStationModesOverviewView.addStatusLine(ostr.str());

			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"TA_Base_Core::ObjectResolutionException", ex.what(), "Failed to retrieve Proposed Master Mode from the Master ECS Agent" ); 

			FUNCTION_EXIT;
			return;

		}
        catch (DataException& ex)
        {
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);
		
			std::ostringstream ostr;
            ostr << "ECS Manager was unable to retrieve mandatory ECS database configuration data: ";
            ostr << ex.what() << ".\nApplication will now exit.";
			m_ecsStationModesOverviewView.addStatusLine("ECS database configuration data error. Application will exit.");

            m_ecsStationModesOverviewView.terminateApplication();

            FUNCTION_EXIT;
            return;
        }
        catch (DatabaseException& ex)
        {
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ex.what();
			userMsg.showMsgBox(IDS_UE_520012);

			 std::ostringstream ostr;
            ostr << "ECS Manager was unable to retrieve mandatory ECS database configuration data: ";
            ostr << ex.what() << ".\nApplication will now exit.";
			m_ecsStationModesOverviewView.addStatusLine("Unable to retrieve ECS database configuration. Application will exit.");

            m_ecsStationModesOverviewView.terminateApplication();

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void ECSStationModesOverview::loadStationModeDataPointValues()
    {
        FUNCTION_ENTRY( "loadStationModeDataPointValues" );

        // NOTE: Important to only acquire lock and register for datapoint updates AFTER all DataPoint 
        // proxies have been created (ie. after refreshDataPointProxies) else deadlock may occur.
        ThreadGuard guard( m_updateLock );

        m_pStationModesDataPointManager->registerStationModeStatusObserver( this );

        m_currentProposedStationModeValues = StationModeDataPointValues( m_locationKeys.size(), 0 );
		for(unsigned int i=0; i<m_locationKeys.size(); i++)
		{
			std::vector<int> emptyVector;
			m_currentOverridingProposedStationModeValues[i] = emptyVector;
		}

        
		// marvin++ acquire station and overriding station modes via master ecs agent
		int value;
		bool isValid = true;

		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedModes = NULL;
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes proposedOverridingStationModes;
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes proposedStationModes;

		std::ostringstream ostrProposedMode;
		try
		{
			CORBA_CALL_RETURN( proposedModes,
			                   (*m_masterECSAgent),
			                   getAllProposedModes,
			                   () );

			filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION,*proposedModes,proposedStationModes);
			filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION,*proposedModes,proposedOverridingStationModes);

            proposedStationModeUpdated( isValid, &proposedStationModes );
			overridingProposedStationModesUpdated(isValid, &proposedOverridingStationModes);
		}
 		catch( const CORBA::Exception& ex )
		{
			isValid = false;
            value = 0;
			ostrProposedMode << "Unable to retrieve overriding proposed station modes. ";
			ostrProposedMode << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);		

			m_ecsStationModesOverviewView.addStatusLine(ostrProposedMode.str());
		}
		catch (const TA_Base_Core::ObjectResolutionException& ex)
		{
			isValid = false;
            value = 0;
			ostrProposedMode << "Unable to retrieve overriding proposed station modes. ";
			ostrProposedMode << ex.what();
			
			m_ecsStationModesOverviewView.addStatusLine(ostrProposedMode.str());
		}
		catch( ... )
		{
			isValid = false;
            value = 0;
			ostrProposedMode << "Unable to retrieve overriding proposed station modes. ";
			ostrProposedMode << "Unknown exception";
			
			m_ecsStationModesOverviewView.addStatusLine(ostrProposedMode.str());
		} 
		// ++marvin
		
		LocationKeys::iterator locationKeysIt;
        for ( locationKeysIt = m_locationKeys.begin();
			  locationKeysIt != m_locationKeys.end();
			  ++locationKeysIt)
        {
            // Determine current Status.
            isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeStatus(*locationKeysIt);				
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }     
            stationModeStatusUpdated(*locationKeysIt, isValid, value);
           
            
            // Determine current In Progress.
            isValid = true;
            try
            {
                value = m_pStationModesDataPointManager->getStationModeInProgress(*locationKeysIt);
            }
            catch ( const ScadaProxyException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not determine Station Mode Status value." );
                isValid = false;
                value = 0;
            }
            stationModeInProgressUpdated(*locationKeysIt, isValid, value);

			// Determine Station Lock Status
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
				//zn++ 1st go thru defects list: Prob4
				//m_ecsStationModesOverviewView.addStatusLine(ostrLockStatus.str());
				//++zn
			}
			catch(...)
			{
				isValid = false;
                value = 0;

				ostrLockStatus << "Unable to retrieve lock status for station. Unknown exception";
				//zn++ 1st go thru defects list: Prob4
				//m_ecsStationModesOverviewView.addStatusLine(ostrLockStatus.str());
				//++zn
			}
			//zn++ 1st go thru defects list: Prob4
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s [%lu]", ostrLockStatus.str().c_str(), *locationKeysIt);
			//++zn
			stationVisualLockUpdated((*locationKeysIt), isValid, value);
        }
        
        enableApplyAllCongestionAsAppropriate();

        FUNCTION_EXIT;
    }
	
	void ECSStationModesOverview::SetAutoModeStatusForLocation(int locationIndex,ECurrentStationAutoModeType currentStationAutoModeType)
	{
		unsigned long locationKey = m_locationKeys[locationIndex];
		std::ostringstream status;
		std::ostringstream confirmation;

		// check station visual lock
        try
        {
            if ((m_pStationModesDataPointManager->getStationLockStatus(locationKey)) != 0)
            {
                m_ecsStationModesOverviewView.addStatusLine("Set auto mode status failed. Reason: Station visual locked.");
                m_ecsStationModesOverviewView.setCurrentStationAutoMode( locationIndex, CURRENT_STATION_AUTO_MODE_OFF );	// means set failed
                return;
            }
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception while getStationLockStatus" );
        }

        status << "Setting automode status " << currentStationAutoModeType << " of " << m_locationNames[ locationIndex ];
        m_ecsStationModesOverviewView.addStatusLine(status.str());
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting automode status %d of %s", currentStationAutoModeType, m_locationNames[locationIndex].c_str());

        try
        {
            m_ecsStationModesOverviewView.setCurrentStationAutoModeControlState(locationIndex, TA_Base_Bus::noControl);
            m_pStationModesDataPointManager->setAutoModeStatusForLocationAndType(locationKey,currentStationAutoModeType);
        }
        catch ( const ScadaProxyException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not set automode status." );	
            std::ostringstream result;
            result << ex.what();

            //zn++ 1st go thru defects list: Prob4 meaningful information
            std::string error = "Could not Set Auto Mode. Scada Exception: ";
            error += getErrorMessage(result.str());

            m_ecsStationModesOverviewView.addStatusLine(error);
            //++zn
            m_ecsStationModesOverviewView.setCurrentStationAutoMode( locationIndex, CURRENT_STATION_AUTO_MODE_OFF );	// means set failed
            return;
        }
        //zn++ CL-20847 add this exception to catch rights check failure
        catch (const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "AccessDeniedException", "Could not set automode status." );	
            std::string error = "Exception: Access Denied";
            m_ecsStationModesOverviewView.addStatusLine( error );
            m_ecsStationModesOverviewView.setCurrentStationAutoMode( locationIndex, CURRENT_STATION_AUTO_MODE_OFF );	// means set failed
            return;
        }
        //++zn
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Could not set automode status." );	
            std::string error = "Unknown Exception: Could not Set Auto Mode";
            m_ecsStationModesOverviewView.addStatusLine( error );
            m_ecsStationModesOverviewView.setCurrentStationAutoMode( locationIndex, CURRENT_STATION_AUTO_MODE_OFF );	// means set failed
            return;
        }
	}

    //
    // prepareForClose
    //
    void ECSStationModesOverview::prepareForClose()
    {
        FUNCTION_ENTRY( "prepareForClose" );

        m_pStationModesDataPointManager->deregisterStationModeStatusObserver( this );
        m_pStationModesDataPointManager->cleanUp();

        try
        {
            // marvin++ logout of master ecs agent
            CORBA_CALL( (*m_masterECSAgent), logout, ( m_sessionId.c_str() ) );
        }
        catch(...)
        {
            m_masterECSAgent->clear();
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Unknown exception caught!" );
        }

        FUNCTION_EXIT;
    }


    //
    // proposedStationModeSelected
    //
    void ECSStationModesOverview::proposedStationModeSelected( int locationIndex, EProposedStationModeType proposedStationModeType )
    {
        FUNCTION_ENTRY( "proposedStationModeSelected" );

        ThreadGuard guard( m_updateLock );

        m_currentSelection.first = locationIndex;
        m_currentSelection.second = proposedStationModeType;

        enableApplyStationModeAsAppropriate();

        FUNCTION_EXIT;
    }

	void ECSStationModesOverview::autoModeSelected()
    {
        FUNCTION_ENTRY( "proposedStationModeSelected" );

        ThreadGuard guard( m_updateLock );

		m_ecsStationModesOverviewView.enableApplyStationModeControl( true );

        FUNCTION_EXIT;
    }

	void ECSStationModesOverview::confirmApplyProposedStationMode(std::map<int, TA_IRS_App::EProposedStationModeType> allProposedIndexAndType)
	{
		std::map<int, TA_IRS_App::EProposedStationModeType>::iterator iter;
		std::ostringstream ssMessage;
		
		for (iter=allProposedIndexAndType.begin(); iter!=allProposedIndexAndType.end(); ++iter)
		{
			EProposedStationModeType overriding, proposed;
            int overridingVal, proposedVal;
			int locationIndex=iter->first;
			unsigned long locationKey = m_locationKeys[ locationIndex ];
            
			{
                ThreadGuard guard( m_updateLock ); // ThreadGuard for local snapshot of proposed values and thier types.

                proposedVal = m_currentProposedStationModeValues[ locationIndex ];
				
				if(m_currentOverridingProposedStationModeValues[locationIndex].size() > 1)
				{
					overridingVal = -1; // To indicate multiplicity.
				}
				else if(0 == m_currentOverridingProposedStationModeValues[locationIndex].size())
				{
					overridingVal = 0;
				}
				else // size is 1.
				{
					overridingVal = (m_currentOverridingProposedStationModeValues[locationIndex])[0];
				}
                
				overriding = getProposedStationModeType(locationKey, overridingVal);

                proposed = getProposedStationModeType(locationKey, proposedVal);
            }

            if (overriding == iter->second)
            {
				std::string stationName = m_locationNames[iter->first];
				ssMessage <<  overridingVal << " to station " << stationName << "\n";
            }  
			else if (proposed == iter->second)
            {
				std::string stationName = m_locationNames[iter->first];
				ssMessage <<  proposedVal << " to station " << stationName << "\n";
			}
		}

		TA_Base_Bus::TransActiveMessage userMsg;
		CString message = ssMessage.str().c_str();
		userMsg << message;
		UINT selectionButton = userMsg.showMsgBox(IDS_UW_650012);

		if (selectionButton==IDCANCEL)
		{
			return;
		}

        std::vector<int> vecEmergencyLocs;
		for (iter=allProposedIndexAndType.begin(); iter!=allProposedIndexAndType.end(); ++iter)
		{
            if (iter->second == PROPOSED_FOR_NORMAL)
            {
                applySelectedProposedStationMode(iter->first, PROPOSED_FOR_NORMAL);
            }
            else if ( iter->second == PROPOSED_FOR_CONGESTION )
            {
                if ( isStationInOverrideOfType(iter->first, PROPOSED_FOR_EMERGENCY))
                {
                    vecEmergencyLocs.push_back( iter->first );
                    continue;
                }

                applySelectedProposedStationMode(iter->first, PROPOSED_FOR_CONGESTION);
            }
        }
	}

    //
    // applySelectedProposedStationMode
    //
    void ECSStationModesOverview::applySelectedProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType )
    {
        FUNCTION_ENTRY( "applySelectedProposedStationMode" );

		//zn++ CL-17444 ITEM1
		if (isStationInOverrideOfType(locationIndex, PROPOSED_FOR_EMERGENCY) && proposedStationModeType == PROPOSED_FOR_CONGESTION)
		{
			CString msg;
			msg.Format("Can not apply station %s to congestion. Reason: Station In Emergency Proposed.", m_locationNames[locationIndex].c_str());
			AfxMessageBox(msg);
			
			return;
		}	
		//++zn

        m_currentSelection.first = locationIndex; // should be set to this already anyhow.
        m_currentSelection.second = proposedStationModeType; // should be set to this anyhow.

        if ( m_currentSelection.first > -1 && 
             m_currentSelection.first < static_cast< int >( m_locationKeys.size() ) )
        {
            unsigned long locationKey = m_locationKeys[ locationIndex ];

            EProposedStationModeType overriding, proposed;
            int overridingVal, proposedVal;
            
			{
                ThreadGuard guard( m_updateLock ); // ThreadGuard for local snapshot of proposed values and thier types.

                proposedVal = m_currentProposedStationModeValues[ locationIndex ];
				
				if(m_currentOverridingProposedStationModeValues[locationIndex].size() > 1)
				{
					overridingVal = -1; // To indicate multiplicity.
				}
				else if(0 == m_currentOverridingProposedStationModeValues[locationIndex].size())
				{
					overridingVal = 0;
				}
				else // size is 1.
				{
					overridingVal = (m_currentOverridingProposedStationModeValues[locationIndex])[0];
				}
                
				overriding = getProposedStationModeType(locationKey, overridingVal);

                proposed = getProposedStationModeType(locationKey, proposedVal);
            }

            LocationAndModeVector locationAndModeVector;

            if (overriding == proposedStationModeType)
            {
				if(overridingVal == -1)
				{
					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg.showMsgBox(IDS_UE_520016);
				
					FUNCTION_EXIT;
                    return;
				}
			
                locationAndModeVector.push_back( LocationAndMode(locationIndex, overridingVal) );
            }  
			else if (proposed == proposedStationModeType)
            {
				TA_Base_Bus::TransActiveMessage userMsg;
				std::ostringstream str;
				str << proposedVal;
				std::string stationMode = str.str();
				std::string stationName = m_locationNames[ locationIndex ];
				unsigned long pairedLocationKey;
				UINT errorCode;

				if(checkCongestionPair(locationKey, proposedVal, pairedLocationKey))
				{
					errorCode = IDS_UW_520009; // TD14164 
                    int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey(pairedLocationKey);
                    std::string pairedStationName = "[Unknown]";
                    if ( 0 <= index )
                    {
                        pairedStationName = m_locationNames[index]; 
                    }
					userMsg << stationName.c_str();
					userMsg << pairedStationName.c_str();
					userMsg << stationMode.c_str();

					UINT selectedButton = userMsg.showMsgBox(errorCode);

					if ( selectedButton == IDCANCEL )
                    {
                        FUNCTION_EXIT;
						return;
					}	
				}	
				
                locationAndModeVector.push_back( LocationAndMode(locationIndex,proposedVal) );
            }

            try
            {
                applyModes( locationAndModeVector );
            }
            catch ( TA_Base_Core::TransactiveException& expWhat )
            {
                m_ecsStationModesOverviewView.addStatusLine( expWhat.what() );
            }
            catch ( std::exception& expWhat )
            {
                m_ecsStationModesOverviewView.addStatusLine( expWhat.what() );
            }
            catch ( ... )
            {
                m_ecsStationModesOverviewView.addStatusLine( "Can not apply modes due to unknown error" );
            }
        }

        FUNCTION_EXIT;
    }


    //
    // applyAllProposedCongestionModes
    //
    void ECSStationModesOverview::applyAllProposedCongestionStationModes()
    {
        FUNCTION_ENTRY( "applyAllProposedCongestionStationModes" );

		TA_Base_Bus::TransActiveMessage userMsg;

		if ( userMsg.showMsgBox(IDS_UW_520011) == IDOK )
        {
            LocationAndModeVector locationAndModeVector;
            std::vector<int> vecEmergencyLocs;

            {
                ThreadGuard guard( m_updateLock );

                for ( unsigned int index = 0u; index < m_locationKeys.size(); index++ )
                {
                    unsigned long ulLocKey = m_locationKeys[index];
                    if ( m_currentProposedStationModeValues.size() <= index )
                    {
                        continue;
                    }

                    int nCurPropsoedMode = m_currentProposedStationModeValues[index];

                    if ( PROPOSED_FOR_CONGESTION == getProposedStationModeType( ulLocKey, nCurPropsoedMode ))
                    {
                        if ( isStationInOverrideOfType( index, PROPOSED_FOR_EMERGENCY))
                        {
                            vecEmergencyLocs.push_back( index );
                            continue;
                        }

						locationAndModeVector.push_back( LocationAndMode(index, nCurPropsoedMode) );
                    }
                }
            }

			m_ecsStationModesOverviewView.addStatusLine( "Applying all proposed congestion station modes..." );

            try
            {
                applyModes( locationAndModeVector );
            }
            catch ( TA_Base_Core::TransactiveException& expWhat )
            {
                m_ecsStationModesOverviewView.addStatusLine( expWhat.what() );
            }
            catch ( std::exception& expWhat )
            {
                m_ecsStationModesOverviewView.addStatusLine( expWhat.what() );
            }
            catch ( ... )
            {
                m_ecsStationModesOverviewView.addStatusLine( "Can not apply modes due to unknown error" );
            }
        }

        FUNCTION_EXIT;
    }


	bool ECSStationModesOverview::checkCongestionPair(unsigned long locationKey, int proposedVal, unsigned long& pairLocationKey)
	{
		std::pair<CongestionModeMapIt,CongestionModeMapIt> range;
		range = m_mapCongestionModes.equal_range(proposedVal);
		unsigned long pairedLocationkey;
		bool fWarning = false;

		for (CongestionModeMapIt iter= range.first; iter != range.second; ++iter)
		{
			pairedLocationkey = iter->second;

			if(locationKey != pairedLocationkey)
			{
				break;
			}
		}

		pairLocationKey = pairedLocationkey;

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

			EProposedStationModeType modeType = getProposedStationModeType(pairedLocationkey, value);
            
            std::ostringstream stationModeStatusOstr;
			std::string status = "Failed to retrieve station mode status for location ";
			status += "%d. Reason: ", pairedLocationkey;
            if ( false == isValid )
            {
                // Station Mode status could not be determined.
                stationModeStatusOstr << "#";
				status += "Station mode status unknown.";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, status.c_str() );
            }
            else if( modeType == PROPOSED_FOR_EMERGENCY )
            {     
                 // congestion pair is in master mode
                 fWarning = true;
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
					if(((*proposedModes)[i].locationKey == pairedLocationkey) && ((*proposedModes)[i].modeType == TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION))
			        {
				        fWarning = true;
                        break;
			        }
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

                EProposedStationModeType modeType = getProposedStationModeType(pairedLocationkey, value);
            
				std::ostringstream stationModeStatusOstr;
				std::string status = "Failed to retrieve station mode status for location ";
				status += "%d. Reason: ", pairedLocationkey;
				if ( false == isValid )
				{
					// Station Mode status could not be determined.
					stationModeStatusOstr << "#";
					status += "Station mode status unknown.";
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, status.c_str() );
				}
				else if( modeType == PROPOSED_FOR_EMERGENCY )
				{     
					 // congestion pair is in master mode
					 fWarning = true;
				}
            }

		}

		return fWarning;

	}



    //
    // applyModes
    //
    void ECSStationModesOverview::applyModes( const LocationAndModeVector& locationAndModeVector )
    {
        FUNCTION_ENTRY( "applyModes" );

		LocationAndModeVector::const_iterator locationAndModeVectorIt; 
        for (locationAndModeVectorIt = locationAndModeVector.begin();
			 locationAndModeVectorIt != locationAndModeVector.end();
			 ++locationAndModeVectorIt)
        {
			std::ostringstream status;
			bool tryAgain = true;
			bool canOverride = false;

			while(tryAgain)
			{
				int locationKey = m_locationKeys[(*locationAndModeVectorIt).first];

                if ( NULL == m_stationECSAgents[locationKey] )
                {
                    TA_THROW( TA_Base_Core::TransactiveException( "No Agent configured" ));
                }

				try
				{
					CORBA_CALL( (*m_stationECSAgents[locationKey]),
					            executeStationMode,
					            ( (*locationAndModeVectorIt ).second, canOverride, m_sessionId.c_str() ) );

					tryAgain = false;
					status << "Setting mode ";
					status << (*locationAndModeVectorIt ).second;
					status << " to location ";
					status << m_locationNames[(*locationAndModeVectorIt).first];
					m_ecsStationModesOverviewView.addStatusLine(status.str());
				}
				catch( const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationModeInProgress& )
				{	

					TA_Base_Bus::TransActiveMessage userMsg; 

					std::ostringstream ssMode;
					ssMode << (*locationAndModeVectorIt).second;
					CString param0 = ssMode.str().c_str();
					userMsg << param0;

					std::ostringstream ssLocation;
					ssLocation << m_locationNames[(*locationAndModeVectorIt).first];
					CString param1 = ssLocation.str().c_str();
					userMsg << param1;

					CString reason = "A Station Mode is in progress";
					userMsg << reason;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_520006);

					if ( selectedButton == IDOK )
					{		
						canOverride = true;
					}
				}
				catch(const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationLocked& ex )
				{
					tryAgain = false;

                    TA_Base_Bus::TransActiveMessage userMsg;

                    std::ostringstream ssMode;
                    ssMode << (*locationAndModeVectorIt).second;
                    CString param0 = ssMode.str().c_str();
                    userMsg << param0;
                    std::ostringstream ssLocation;
                    ssLocation << m_locationNames[(*locationAndModeVectorIt).first];
                    CString param1 = ssLocation.str().c_str();
                    userMsg << param1;
					CString param2 = (TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)).c_str();
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);
				}
				// CL-17274++
				catch (const TA_Base_Core::AccessDeniedException& )
				{
					tryAgain = false;

					TA_Base_Bus::TransActiveMessage userMsg;
					std::ostringstream str;
                    str << (*locationAndModeVectorIt).second;
					std::string param0 = str.str();
					userMsg << param0.c_str();
                    CString param1 = m_locationNames[(*locationAndModeVectorIt).first].c_str();
					userMsg << param1;
					CString temp = "No rights to write datapoint";
					CString param2 = temp;
					userMsg << param2;
					userMsg.showMsgBox(IDS_UW_520001);
				}
                // ++CL-17274
 				catch( const CORBA::Exception& ex )
				{
					tryAgain = false;

					TA_Base_Bus::TransActiveMessage userMsg; 

					std::ostringstream ssMode;
					ssMode << (*locationAndModeVectorIt).second;
					CString param0 = ssMode.str().c_str();
					userMsg << param0;

					std::ostringstream ssLocation;
					ssLocation << m_locationNames[(*locationAndModeVectorIt).first];
					CString param1 = ssLocation.str().c_str();
					userMsg << param1;

					CString param2 = (TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)).c_str();
					userMsg << param2;

                    userMsg.showMsgBox(IDS_UW_520001);
				}
				catch (const TA_Base_Core::ObjectResolutionException& )
				{
					tryAgain = false;
				
					TA_Base_Bus::TransActiveMessage userMsg; 

					std::ostringstream ssMode;
					ssMode << (*locationAndModeVectorIt).second;
					CString param0 = ssMode.str().c_str();
					userMsg << param0;

					std::ostringstream ssLocation;
					ssLocation << m_locationNames[(*locationAndModeVectorIt).first];
					CString param1 = ssLocation.str().c_str();
					userMsg << param1;

					CString temp = "Failure to connect to ";
					CString param2 = temp + ((*m_stationECSAgents[locationKey]).getObjectName()).c_str();
					userMsg << param2;

                    userMsg.showMsgBox(IDS_UW_520001);
				}
				catch( ... )
				{
					tryAgain = false;

					TA_Base_Bus::TransActiveMessage userMsg; 

					std::ostringstream ssMode;
					ssMode << (*locationAndModeVectorIt).second;
					CString param0 = ssMode.str().c_str();
					userMsg << param0;

					std::ostringstream ssLocation;
					ssLocation << m_locationNames[(*locationAndModeVectorIt).first];
					CString param1 = ssLocation.str().c_str();
					userMsg << param1;

					CString param2 = "Unknown exception";
					userMsg << param2;

                    userMsg.showMsgBox(IDS_UW_520001);
				} 
			}
        }

        FUNCTION_EXIT;
    }


    //
    // enableApplyStationModeAsAppropriate
    //
    void ECSStationModesOverview::enableApplyStationModeAsAppropriate()
    {
        FUNCTION_ENTRY( "enableApplyStationModeAsApropriate" );

		int ilocationKeySize = m_locationKeys.size();

        if((m_currentSelection.first > -1) && (m_currentSelection.first < ilocationKeySize))
        {
            unsigned long locationKey = m_locationKeys[m_currentSelection.first];
			EProposedStationModeType proposedStationModeType = 
				getProposedStationModeType(locationKey, m_currentProposedStationModeValues[m_currentSelection.first]);

            std::vector<int>& vecModes = m_currentOverridingProposedStationModeValues[m_currentSelection.first];
            EProposedStationModeType proposedOverridingModeType = PROPOSED_IS_UNKNOWN;
            if ( 0u < vecModes.size() )
            {
                getProposedStationModeType(locationKey, vecModes[0]);
            }

            if ((m_currentSelection.second != PROPOSED_IS_UNKNOWN) && 
                ((m_currentSelection.second == proposedStationModeType) || 
                (m_currentSelection.second == proposedOverridingModeType)))
            {
                m_ecsStationModesOverviewView.enableApplyStationModeControl(true);
                FUNCTION_EXIT;
                return;
            }
        }
        
        m_ecsStationModesOverviewView.enableApplyStationModeControl(false);
        FUNCTION_EXIT;
    }


    //
    // enableApplyAllCongestionAsAppropriate
    //
    void ECSStationModesOverview::enableApplyAllCongestionAsAppropriate()
    {
        FUNCTION_ENTRY( "enableApplyAllCongestionAsAppropriate" );

        unsigned int index = 0;
        for ( index; index < m_locationKeys.size(); index++ )
        {
            if ( PROPOSED_FOR_CONGESTION == getProposedStationModeType( m_locationKeys[ index ],
                m_currentProposedStationModeValues[ index ] ) )
            {
                m_ecsStationModesOverviewView.enableApplyAllCongestedStationModesControl( true );
                FUNCTION_EXIT;
                return;
            }
        }
        m_ecsStationModesOverviewView.enableApplyAllCongestedStationModesControl( false );

        FUNCTION_EXIT;
    }


    //
    // getProposedStationModeType
    //
    EProposedStationModeType ECSStationModesOverview::getProposedStationModeType( unsigned long locationKey,
            int proposedStationMode )
    {
        FUNCTION_ENTRY( "getProposedStationModeType" );

        int modeKey = m_pMappedECSDataCache->getStationModeKeyFromLocationKeyAndMode( locationKey, proposedStationMode );
        if ( modeKey != 0 )
        {
            StationModeDetail modeDetail = m_pMappedECSDataCache->getStationModeDetailFromKey( modeKey );
			if (modeDetail.condition.empty())
			{
				FUNCTION_EXIT;
				return PROPOSED_IS_UNKNOWN;
			}
            else if ( modeDetail.condition == EMERGENCY_COMPARE_STRING )
            {
                FUNCTION_EXIT;
                return PROPOSED_FOR_EMERGENCY;
            }
            else if ( modeDetail.condition == CONGESTION_COMPARE_STRING )
            {
                FUNCTION_EXIT;
                return PROPOSED_FOR_CONGESTION;
            }
            else
            {
                FUNCTION_EXIT;
                return PROPOSED_FOR_NORMAL;
            }
        }

        FUNCTION_EXIT;
        return PROPOSED_IS_UNKNOWN;
    }


    //
    // stationModeStatusUpdated
    //
    void ECSStationModesOverview::stationModeStatusUpdated( unsigned long locationKey, bool isValid, int value )
    {
        FUNCTION_ENTRY( "stationModeStatusUpdated" );

        ThreadGuard guard( m_updateLock );

        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
        std::ostringstream ostr;

		std::string status = "Failed to update station mode status for location ";
		status += m_locationNames[index];
		status += ". Reason: ";
        if ( false == isValid )
        {
            // Unknown StationMode status value
            ostr << "#";
            m_ecsStationModesOverviewView.setCurrentStationMode( index, ostr.str(), 
                CURRENT_STATION_MODE_FAILED );
			status += "Station mode status unknown.";
			//zn++ 1st go thru defects list: Prob4
			//m_ecsStationModesOverviewView.addStatusLine(status);
			//++zn
        }
        else if ( value  < 0 )
        {
            // Failed Station Mode.
            ostr << value;
            m_ecsStationModesOverviewView.setCurrentStationMode( index, ostr.str(), CURRENT_STATION_MODE_FAILED );
            status  += "Failed station mode.";
			//zn++ 1st go thru defects list: Prob4
			//m_ecsStationModesOverviewView.addStatusLine(status);
            //FUNCTION_EXIT;
            //return;
			//++zn
        }
        else
        {
            unsigned long stationModeKey = m_pMappedECSDataCache->getStationModeKeyFromLocationKeyAndMode( locationKey, value );
            if ( stationModeKey != 0 )
            {
                ostr << value;
                StationModeDetail stationModeDetail = m_pMappedECSDataCache->getStationModeDetailFromKey
                    ( stationModeKey );
				status.erase();
				status = "Station mode has been updated to ";
				status += ostr.str();
				status += " for location ";
                if ( 0 <= index )
                {
                    status += m_locationNames[index];
                }
                else
                {
                    status += "[Unknown]";
                }

                if (stationModeDetail.condition.empty())
                {
					m_ecsStationModesOverviewView.setCurrentStationMode( index, ostr.str(), 
						CURRENT_STATION_MODE_FAILED );
					status  += "Failed station mode.";
                }
                else if ( stationModeDetail.condition == EMERGENCY_COMPARE_STRING )
                {
                    m_ecsStationModesOverviewView.setCurrentStationMode(index, ostr.str(), 
																	CURRENT_STATION_MODE_EMERGENCY );               
					//zn++ 1st go thru defects list: Prob4
					//m_ecsStationModesOverviewView.addStatusLine(status);
					//++zn
                }
                else if ( stationModeDetail.condition == CONGESTION_COMPARE_STRING )
                {
                    m_ecsStationModesOverviewView.setCurrentStationMode(index, ostr.str(), 
																	CURRENT_STATION_MODE_CONGESTION );
					//zn++ 1st go thru defects list: Prob4
					//m_ecsStationModesOverviewView.addStatusLine(status);
					//++zn
                }
                else
                {
                    m_ecsStationModesOverviewView.setCurrentStationMode(index, ostr.str(), 
																	CURRENT_STATION_MODE_NORMAL );
					//zn++ 1st go thru defects list: Prob4
					//m_ecsStationModesOverviewView.addStatusLine(status);
					//++zn
                }
				//zn++ 1st go thru defects list: Prob4
//                 FUNCTION_EXIT;
//                 return;
				//++zn
            }
            else
            {
                // Station Mode status Value does not map to any configured station mode.
                ostr << value;
                m_ecsStationModesOverviewView.setCurrentStationMode( index, ostr.str(), 
                    CURRENT_STATION_MODE_FAILED );
				status  += "Failed station mode.";
				//zn++ 1st go thru defects list: Prob4
				//m_ecsStationModesOverviewView.addStatusLine(status);
				//++zn
            }
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", status.c_str());
        FUNCTION_EXIT;
    }


    //
    // stationModeInProgressUpdated
    //
    void ECSStationModesOverview::stationModeInProgressUpdated( unsigned long locationKey, bool isValid, int value )
    {
        FUNCTION_ENTRY( "stationModeInProgressUpdated" );

        ThreadGuard guard( m_updateLock );
        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
        m_ecsStationModesOverviewView.setCurrentStationModeInProgress( index, ( value > 0 ) );

        FUNCTION_EXIT;
    }


	void ECSStationModesOverview::stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value )
	{
        FUNCTION_ENTRY( "stationVisualLockUpdated" );

        ThreadGuard guard( m_updateLock );
        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
		bool isStationLocked = (value != 0);
        m_ecsStationModesOverviewView.setCurrentStationVisualLock( index, isStationLocked);

        FUNCTION_EXIT;
	}


	void ECSStationModesOverview::stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value )
	{
		FUNCTION_ENTRY( "stationAutoModeStatusUpdated" );

        ThreadGuard guard( m_updateLock );
        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
		bool isStationLocked = (value != 0);
        m_ecsStationModesOverviewView.setCurrentStationAutoMode( index, isStationLocked);

        FUNCTION_EXIT;
	}

	void ECSStationModesOverview::stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState)
	{
		FUNCTION_ENTRY( "stationAutoModeControlStateUpdated" );

		ThreadGuard guard( m_updateLock );
        int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );

        m_ecsStationModesOverviewView.setCurrentStationAutoModeControlState(index, controlState);

        std::string strLocName = "[Unknown]";

        if ( 0 <= index )
        {
            strLocName = m_locationNames[index];
        }

		// Add status message for the auto mode on control status
		switch ( controlState )
        {
        case TA_Base_Bus::success:
			{
				std::ostringstream ssControl;
				ssControl << "Auto mode on command for station " 
						  << strLocName
						  << " succeeded";
				//zn++ 1st go thru defects list: Prob4
				//m_ecsStationModesOverviewView.addStatusLine(ssControl.str());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", ssControl.str().c_str());
				//++zn
			}
			break;
        case TA_Base_Bus::failed:
		case TA_Base_Bus::LCC_failed:
		case TA_Base_Bus::RCC_failed:
		case TA_Base_Bus::comms_failed:  
			{
				std::ostringstream ssControl;
				ssControl << "Auto mode on command for station " 
						  << strLocName
						  << " encountered "
						  << getControlStatusString(controlState);
				//zn++ 1st go thru defects list: Prob4
				//m_ecsStationModesOverviewView.addStatusLine(ssControl.str());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", ssControl.str().c_str());
				//++zn
			}
            break;
		case TA_Base_Bus::conditionalSuccess:
		case TA_Base_Bus::pending:
		case TA_Base_Bus::written:
        case TA_Base_Bus::noControl:
        default:          
            break;
        }


		FUNCTION_EXIT;
	}


	void ECSStationModesOverview::stationModeExecutionUpdated(bool isValid, TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeExecutionInfo)
	{
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
		ostr << " for Master mode";
		ostr << stationModeExecutionInfo->forMasterMode;

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

		m_ecsStationModesOverviewView.addStatusLine(ostr.str());
	}


	void ECSStationModesOverview::proposedStationModeUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedStationMode)
    {
        FUNCTION_ENTRY( "proposedStationModeUpdated" );

        ThreadGuard guard( m_updateLock );

		//interface modified to accept multiple station mode proposals
        m_mapCongestionModes.clear();

		//zn++ CL-17444 cancel the mode that is not proposed 
		std::vector<int> currentProposedIndexes;
		//++zn
	
		for (unsigned int i=0; i<proposedStationMode->length(); i++)
		{	
			EProposedStationModeType proposedType = PROPOSED_IS_UNKNOWN;
            EProposedStationModeType proposedTypePrev = PROPOSED_IS_UNKNOWN;
            EProposedStationModeType overridingType = PROPOSED_IS_UNKNOWN;

			int value = (*proposedStationMode)[i].mode;
			unsigned long locationKey = (*proposedStationMode)[i].locationKey;
			unsigned long locationKeyOfCongestionPaire = (*proposedStationMode)[i].locationKeyOfCongestionPair;	

			int index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey(locationKey);
            if ( (0 > index) || (m_currentOverridingProposedStationModeValues.end() == m_currentOverridingProposedStationModeValues.find(index)) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "locationKey %d is not cached", locationKey );
                continue;
            }

            std::vector<int>& refModeValues = m_currentOverridingProposedStationModeValues[index];

			int numberOfCurrentOverridingModes = refModeValues.size();

			//zn++ CL-17444 cancel the mode that is not proposed
			currentProposedIndexes.push_back(index);
			//++zn

			if (true == isValid)
			{
				proposedType = getProposedStationModeType(locationKey, value);
				m_mapCongestionModes.insert(CongestionModeMap::value_type(value, locationKeyOfCongestionPaire));
			}
			else
			{
				proposedType = PROPOSED_IS_UNKNOWN;
			}

			// check previously proposed modes
			proposedTypePrev = getProposedStationModeType(locationKey, m_currentProposedStationModeValues[index]);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[zn check] proposedStationModeUpdated: location = %lu, index = %d, mode = %d, proposed type = %d, prev proposed type = %d", 
				locationKey, index, value, proposedType, proposedTypePrev);
        
			bool areModesTheSame = false;
			for(int i=0; i<numberOfCurrentOverridingModes; i++)
			{
				overridingType = getProposedStationModeType(locationKey, 
						refModeValues[numberOfCurrentOverridingModes - (i+1)]);
				if(proposedTypePrev == overridingType)
				{
					areModesTheSame = true;
					break;
				}
			}

			if (proposedTypePrev != PROPOSED_IS_UNKNOWN && 
				proposedTypePrev != proposedType && 
				false == areModesTheSame )
			{
				m_ecsStationModesOverviewView.setCurrentProposedStationMode(index, proposedTypePrev,
					std::string(""), NOT_CURRENTLY_PROPOSED);
			}

			// check proposed mode
			if (true == isValid)
			{
				areModesTheSame = false;
				for(int i=0; i<numberOfCurrentOverridingModes; i++)
				{
					overridingType = getProposedStationModeType( locationKey, 
						refModeValues[numberOfCurrentOverridingModes - (i+1)]);
					if(proposedType == overridingType)
					{
						areModesTheSame = true;
						break;
					}
				}			

				if (proposedType != PROPOSED_IS_UNKNOWN && false == areModesTheSame)
				{
					std::ostringstream ostr;
					ostr << value;
					m_ecsStationModesOverviewView.setCurrentProposedStationMode(index, proposedType,
						ostr.str(), CURRENTLY_PROPOSED);
				}

				m_currentProposedStationModeValues[index] = value;
			}
			else
			{
				m_currentProposedStationModeValues[index] = 0;
			}

			//enableApplyStationModeAsAppropriate();	//CL-20790
			//enableApplyAllCongestionAsAppropriate();	//CL-20790
		}

		//zn++ CL-17444 cancel the mode that is not proposed except for override proposed mode
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_currentProposedStationModeValues size is %d", m_currentProposedStationModeValues.size());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "currentProposedIndexes size is %d", currentProposedIndexes.size());

		for (int k = 0; k < m_currentProposedStationModeValues.size(); ++k)
		{
			bool foundEmergency = false;
			bool foundNormal = false;
			bool foundCongestion = false;

			if (currentProposedIndexes.end() == std::find(currentProposedIndexes.begin(), currentProposedIndexes.end(), k)) // not current propose for this station
			{
				unsigned long stationKey = m_pMappedECSDataCache->getOrderedLocationFromIndex(k).key;
				if (TA_Base_Core::DatabaseKey::isInvalidKey(stationKey))
				{
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid station key");
					return;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] station %lu have no propose station mode", stationKey);

				if (m_currentOverridingProposedStationModeValues[k].size() == 0)	// this station have no override propose mode
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] there is not override proposed station mode for location: %lu, remove normal and congestion", stationKey);
					m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_NORMAL,
						std::string(""), NOT_CURRENTLY_PROPOSED);
					m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_CONGESTION,
					std::string(""), NOT_CURRENTLY_PROPOSED);
				}
				else
				{
					//TODO: case may not end
					if (isStationInOverrideOfType(k, PROPOSED_FOR_EMERGENCY) && !isStationInOverrideOfType(k, PROPOSED_FOR_NORMAL))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] there is at least one override emergency mode proposed for location: %lu, remove normal and congestion", stationKey);
						m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_NORMAL,
							std::string(""), NOT_CURRENTLY_PROPOSED);
						m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_CONGESTION,
						std::string(""), NOT_CURRENTLY_PROPOSED);
					}
					else if (isStationInOverrideOfType(k, PROPOSED_FOR_NORMAL))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] there is at least one override normal mode proposed for location: %lu, remove normal and congestion", stationKey);
						m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_CONGESTION,
						std::string(""), NOT_CURRENTLY_PROPOSED);

						//zn++ CL-20842 AtsTrain 1 FIRE + AtsTrain 4 FIRE - AtsTrain 4 FIRE - AtsTrain 1 FIRE
						if (!isStationInOverrideOfType(k, PROPOSED_FOR_EMERGENCY))
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] there is at least one override normal mode proposed for location: %lu, remove emergency", stationKey);
							m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_EMERGENCY,
							std::string(""), NOT_CURRENTLY_PROPOSED);
						}
						//++zn
				    }
				}
				m_currentProposedStationModeValues[k] = 0;
			}
			//zn++ CL-20842 AtsTrain 1 FIRE + AtsTrain 4 FIRE + AtsTrain 4 Congestion - AtsTrain 4 Congestion - AtsTrain 4 FIRE - AtsTrain 1 FIRE
			else // station is in current proposed, before set propose, should check override proposed	
			{
				unsigned long stationKey = m_pMappedECSDataCache->getOrderedLocationFromIndex(k).key;
				if (TA_Base_Core::DatabaseKey::isInvalidKey(stationKey))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid station key");
					return;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] station %lu have propose station mode", stationKey);

				if (isStationInOverrideOfType(k, PROPOSED_FOR_EMERGENCY))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[zn check] there is at least one override emergency mode proposed for location: %lu, remove normal and congestion", stationKey);
					m_ecsStationModesOverviewView.setCurrentProposedStationMode(k, PROPOSED_FOR_NORMAL,
						std::string(""), NOT_CURRENTLY_PROPOSED);
				}
			}
			//++zn
		}
		//++zn

		enableApplyAllCongestionAsAppropriate();	//CL-20790

        FUNCTION_EXIT;
    }


	void ECSStationModesOverview::overridingProposedStationModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes)
	{
        FUNCTION_ENTRY( "overridingProposedStationModesUpdated" );

        ThreadGuard guard( m_updateLock );

		EProposedStationModeType proposedType;
		EProposedStationModeType overridingType;
		EProposedStationModeType overridingTypePrev;
		unsigned long locationKey = 0;
		int index = 0;

		//interface modified to accept multiple station mode proposals
		if(stationModes != NULL)
		{
			//no overriding so refresh the screen
			if (stationModes->length() == 0)
			{
				std::map< int, std::vector<int> >::iterator itr; 
				
				for (itr = m_currentOverridingProposedStationModeValues.begin(); 
				     itr != m_currentOverridingProposedStationModeValues.end(); 
					 ++itr)
				{
                    // jian.huang m_currentOverridingProposedStationModeValues has been indexed by OrderedLocationIndex,
                    // so just need to get through it
                    index = (*itr).first;
                    locationKey = m_pMappedECSDataCache->getOrderedLocationFromIndex(index).key;
					if (TA_Base_Core::DatabaseKey::isInvalidKey(locationKey))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid station key");
						return;
					}
                    
                    if (index != -1 && 
                        (*itr).second.size() > 0)
                    {
                        overridingTypePrev = getProposedStationModeType(locationKey, (*itr).second[0]);
                        
                        if (overridingTypePrev != PROPOSED_IS_UNKNOWN)
                        {
                            m_ecsStationModesOverviewView.setCurrentProposedStationMode(index, overridingTypePrev,
                                std::string(""), NOT_CURRENTLY_PROPOSED);    
                        }
                        (*itr).second.clear();
					}
				}
			}

			for(unsigned int j=0; j<stationModes->length(); j++)
			{
				std::vector<int> newOverridingValues;

				locationKey = (*stationModes)[j].locationKey;
                index = m_pMappedECSDataCache->getOrderedLocationIndexFromKey( locationKey );
                if ( ( 0 > index ) || (m_currentOverridingProposedStationModeValues.end() == m_currentOverridingProposedStationModeValues.find(index)) )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "locationKey %d is not cached", locationKey );
                    continue;
                }

				if ( isValid )
				{
					overridingType = getProposedStationModeType( locationKey, (*stationModes)[j].mode );
				}
				else
				{
					overridingType = PROPOSED_IS_UNKNOWN;
				}

                std::vector<int>& refModeValues = m_currentOverridingProposedStationModeValues[index];

				int numberOfCurrentOverridingModes = refModeValues.size();
				
                if (0 == numberOfCurrentOverridingModes)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"m_currentOverridingProposedStationModeValues is empty: location = %lu, index = %d", locationKey, index);
					overridingTypePrev = PROPOSED_IS_UNKNOWN;
                }
				else
				{
					overridingTypePrev = getProposedStationModeType(locationKey, refModeValues[0]);
				}

				proposedType = getProposedStationModeType(locationKey, m_currentProposedStationModeValues[index]);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[zn check] overridingProposedStationModesUpdated: location = %lu, index = %d, mode = %d, proposed type = %d, prev overriding type = %d", 
				locationKey, index, (*stationModes)[j].mode, proposedType, overridingTypePrev);

				// Now we want to compare the new overidding station mode with the current overriding proposed station modes
				// in m_currentOverridingProposedStationModeValues for the given station (location)

				if (overridingTypePrev != PROPOSED_IS_UNKNOWN && overridingTypePrev != overridingType)
				{
					for(int i=0; i<numberOfCurrentOverridingModes; i++)
					{
						if (overridingTypePrev == getProposedStationModeType(locationKey, refModeValues[numberOfCurrentOverridingModes - (i+1)]) && 
							refModeValues[0] != refModeValues[numberOfCurrentOverridingModes - (i+1)] )
						{
							std::ostringstream ostr;
							ostr << refModeValues[numberOfCurrentOverridingModes - (i+1)];
							
							m_ecsStationModesOverviewView.setCurrentProposedStationMode(index, overridingTypePrev, ostr.str(), 
								CURRENTLY_OVERRIDING_PROPOSED);
							break;
						}
						else if (overridingTypePrev == proposedType && 
								 overridingTypePrev != getProposedStationModeType(locationKey, refModeValues[numberOfCurrentOverridingModes - (i+1)]))
						{
							std::ostringstream ostr;
							ostr << m_currentProposedStationModeValues[ index ];
							m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, proposedType, ostr.str(),
								CURRENTLY_PROPOSED );     
							break;
						}
						else if (overridingTypePrev != getProposedStationModeType(locationKey, refModeValues[numberOfCurrentOverridingModes - (i+1)]))
						{
							m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, overridingTypePrev,
							std::string(""), NOT_CURRENTLY_PROPOSED );    
							break;
						}
					}
					
				}

				if (true == isValid)
				{
					if (overridingType != PROPOSED_IS_UNKNOWN)
					{
						std::ostringstream ostr;

						// marvin++ refresh the screen to reflect the absence of overriding proposals
						if (numberOfCurrentOverridingModes==0)
						{
							ostr << (*stationModes)[j].mode;
							m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, overridingType, ostr.str(),
									CURRENTLY_OVERRIDING_PROPOSED );  
						}
						// ++marvin

						for (int i=0; i<numberOfCurrentOverridingModes; i++)
						{
							if ( overridingType != getProposedStationModeType(locationKey, refModeValues[numberOfCurrentOverridingModes - (i+1)]) )
							{
								if(numberOfCurrentOverridingModes > 1)
								{
									//ostr << (*stationModes)[numberOfCurrentOverridingModes-1];
									//ostr << "+";
									ostr <<""; // display nothing for multiple proposed modes
								}
								else
								{
									ostr << (*stationModes)[j].mode;
								}
								m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, overridingType, ostr.str(),
									CURRENTLY_OVERRIDING_PROPOSED );  
								break;
							}
							else
							{
								if ( (*stationModes)[j].mode != refModeValues[numberOfCurrentOverridingModes - (i+1)] ) 
								{
									if(numberOfCurrentOverridingModes > 1)
									{
										ostr << (*stationModes)[j].mode << "/" << refModeValues[numberOfCurrentOverridingModes - 1];
										ostr << "+";
									}
									else
									{
										ostr << (*stationModes)[j].mode << "/" << refModeValues[numberOfCurrentOverridingModes - (i+1)];
									}
									m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, overridingType, ostr.str(),
										CURRENTLY_OVERRIDING_PROPOSED );
									break;
								}
								else
								{
									if(numberOfCurrentOverridingModes > 1)
									{
										ostr << (*stationModes)[numberOfCurrentOverridingModes-1].mode;
										ostr << "+";
									}
									else
									{
										ostr << (*stationModes)[j].mode;
									}
									m_ecsStationModesOverviewView.setCurrentProposedStationMode( index, overridingType, ostr.str(),
										CURRENTLY_OVERRIDING_PROPOSED );    
								}
							}
						}
					}

					newOverridingValues.push_back((*stationModes)[j].mode);

				}
				else
				{
					newOverridingValues.push_back(0);
				}

				refModeValues = newOverridingValues;

				//enableApplyStationModeAsAppropriate();	//CL-20790
			}
			
		}

        FUNCTION_EXIT;
	}
	

	void ECSStationModesOverview::refreshHealthPollInterval()
	{
		FUNCTION_ENTRY( "refreshHealthPollInterval" );
		m_pStationModesDataPointManager->refreshHealthPollInterval();
		FUNCTION_EXIT;
	}


	// marvin++
	void ECSStationModesOverview::filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
		const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes)
	{
		//TA_ASSERT(filteredModes.length()==0,"Corba sequence not initialized");
		int length = 0;
		for(unsigned int ctr=0; ctr<proposedModes.length(); ctr++)
		{
			if(proposedModes[ctr].modeType == type)
			{
				length++;
			}
		}

		filteredModes.length(length);

		int toCtr=0;
		for(unsigned int frmCtr=0; frmCtr<proposedModes.length(); frmCtr++)
		{
			if(proposedModes[frmCtr].modeType == type)
			{
				filteredModes[toCtr] = proposedModes[frmCtr];
				toCtr++;
			}
		}
	}
	// ++marvin

	std::string ECSStationModesOverview::getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState)
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

    std::string ECSStationModesOverview::getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError)
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

	//zn++ CL-17444 ITEM1
	bool ECSStationModesOverview::isStationInOverrideOfType(int index, EProposedStationModeType type)
	{
        if (( 0 > index ) || ( m_locationKeys.size() <= index ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "index %d out of range [0, %d]", index, m_locationKeys.size() );
            TA_THROW( std::out_of_range("Vector out of range") );
        }

		if (m_currentOverridingProposedStationModeValues[index].size() == 0)
		{
			return false;
		}

		bool foundType = false;
		unsigned long locationKey = m_locationKeys[index];

        std::vector<int>& refModeValues = m_currentOverridingProposedStationModeValues[index];
		for (int i = 0; i < refModeValues.size(); ++i)
		{
			if (getProposedStationModeType(locationKey, refModeValues[i]) == type)
			{
				foundType = true;
				break;
			}
		}

		return foundType;
	}
	//++zn

} // namespace TA_IRS_App
