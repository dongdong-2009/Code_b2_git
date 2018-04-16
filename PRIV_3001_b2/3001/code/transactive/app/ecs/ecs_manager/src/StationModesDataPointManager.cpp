/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/StationModesDataPointManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/ecs/ecs_manager/src/stdafx.h"

#include <algorithm>
#include <string>


#include "app/ecs/ecs_manager/src/IStationModeStatusObserver.h"
#include "app/ecs/ecs_manager/src/IProposedStationModeObserver.h"
#include "app/ecs/ecs_manager/src/StationModesDataPointManager.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/common_library/src/DpValue.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"		//CL-20847

namespace TA_IRS_App
{

    //
    // Constructor
    //
	StationModesDataPointManager::StationModesDataPointManager()
		: m_isStatusAndControlInitialised( false ), m_isProposedInitialised( false )
	{
        FUNCTION_ENTRY( "Constructor" );

        m_pollTime = 0;
		
		refreshHealthPollInterval();

		// get a handle to the ScadaProxyFactory
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

        //TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
		if (NULL == m_proxyFactory)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SCADA Proxy Factory not created" );
			TA_THROW( TA_Base_Core::TransactiveException("SCADA Proxy Factory not created" ));
		}

		m_proxyFactory->setProxiesToControlMode();

        FUNCTION_EXIT;
	}


    //
    // Destructor
    //
	StationModesDataPointManager::~StationModesDataPointManager()
	{
        FUNCTION_ENTRY( "Destructor" );

		//zn++ CL-20847
		CRightsMgrSingleton::getInstance()->DeleteInstance();
		//++zn

		cleanUpAllDataPointProxies();

		// remove handle to ScadaProxyFactory
		m_proxyFactory->setProxiesToMonitorMode();
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_proxyFactory = NULL;

        FUNCTION_EXIT;
	}


    //
    // cleanUp
    //
	void StationModesDataPointManager::cleanUp()
	{
        FUNCTION_ENTRY( "cleanUp" );

		cleanUpAllDataPointProxies();

        FUNCTION_EXIT;
	}


    //
    // registerStationModeStatusObserver
    //
	void StationModesDataPointManager::registerStationModeStatusObserver( IStationModeStatusObserver* observer )
	{
		FUNCTION_ENTRY( "registerStationModeStatusObserver" );
	
		ThreadGuard guard( m_observersLock );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "registerStationModeStatusObserver() obtained ThreadLockable." );

		m_registeredStationModeStatusObservers.push_back( observer );
		std::sort( m_registeredStationModeStatusObservers.begin(), m_registeredStationModeStatusObservers.end() );

		FUNCTION_EXIT;
	}


    //
    // deregisterStationModeStatusObserver
    //
	void StationModesDataPointManager::deregisterStationModeStatusObserver( IStationModeStatusObserver* observer )
	{
		FUNCTION_ENTRY( "deregisterStationModeStatusObserver" );

		ThreadGuard guard( m_observersLock );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "deregisterStationModeStatusObserver() obtained ThreadLockable." );

        StationModeStatusObservers::iterator itFound = std::find( m_registeredStationModeStatusObservers.begin(), m_registeredStationModeStatusObservers.end(),  observer );
		if( itFound != m_registeredStationModeStatusObservers.end() )
        {
		    m_registeredStationModeStatusObservers.erase( itFound );
        }
        else
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "StationModesDataPointManager::deregisterProposedMasterModeObserver called with pointer to non registered observer." );
        }

		FUNCTION_EXIT;
	}


    //
    // refreshDataPointProxies
    //
	void StationModesDataPointManager::refreshDataPointProxies( const LocationKeys& locationKeys, 
			bool doStatusAndControl, bool doClearAllExisting /*=true*/ )
	{
        FUNCTION_ENTRY( "refreshDataPointProxies" );

		LocationKeys sortedLocationKeys = locationKeys;
		std::sort( sortedLocationKeys.begin(), sortedLocationKeys.end() );
		//std::unique( sortedLocationKeys.begin(), sortedLocationKeys.end() );
		sortedLocationKeys.erase(std::unique( sortedLocationKeys.begin(), sortedLocationKeys.end() ),sortedLocationKeys.end());

		if ( doClearAllExisting )
		{
			cleanUpAllDataPointProxies();
			m_locationKeys = sortedLocationKeys;
		}
		else // retain existing and only refresh as specified.
		{
			cleanUpSpecifiedDataPointProxies( sortedLocationKeys, doStatusAndControl );
			m_locationKeys.insert( m_locationKeys.end(), sortedLocationKeys.begin(), sortedLocationKeys.end() );
			std::sort( m_locationKeys.begin(), m_locationKeys.end() );
			//std::unique( m_locationKeys.begin(), m_locationKeys.end() );
			m_locationKeys.erase(std::unique( m_locationKeys.begin(), m_locationKeys.end() ),m_locationKeys.end());
		}

		StationModeDataPointKeySet stationModeDataPointKeySet( NUMBER_OF_STATIONMODE_DATAPOINT_TYPES, 0 );
		LocationKeyToStationModeDataPointKeySet::iterator locationKeyToStationModeDataPointKeySetIt;
		for (LocationKeys::iterator locationKeysIt = sortedLocationKeys.begin();
			 locationKeysIt != sortedLocationKeys.end();
			 ++locationKeysIt)
		{
			locationKeyToStationModeDataPointKeySetIt = 
				m_locationKeyToStationModeDataPointKeySet.find(*locationKeysIt);

			if (locationKeyToStationModeDataPointKeySetIt == m_locationKeyToStationModeDataPointKeySet.end())
			{
				m_locationKeyToStationModeDataPointKeySet.insert( LocationKeyToStationModeDataPointKeySet::value_type
					( ( *locationKeysIt ), stationModeDataPointKeySet ) );
			}	
		}

		if ( doStatusAndControl )
		{
			populateStatusAndControlDataPointProxies( sortedLocationKeys );
			m_isStatusAndControlInitialised = true;
		}

        FUNCTION_EXIT;
	}


    //
    // populateStatusAndControlDataPointProxies
    //
	void StationModesDataPointManager::populateStatusAndControlDataPointProxies( const LocationKeys& locationKeys )
	{
        FUNCTION_ENTRY( "populateStatusAndControlDataPointProxies" );

		// get data point entities
		DataPointEntityDataVector modeStatusDataPointEntities;
		DataPointEntityDataVector modeInProgressDataPointEntities;
		DataPointEntityDataVector modeVisualLockDataPointEntities;
		DataPointEntityDataVector modeAutoStatusDataPointEntities;

		DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken
				( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getStationModeStatusToken(),
					modeStatusDataPointEntities );

		DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken
				( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getStationModeInProgressToken(),
					modeInProgressDataPointEntities );

		DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken
				( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getStationVisualLockToken(),
					modeVisualLockDataPointEntities );			

		try
		{
			// Create proxies
			populateProxiesFromDataPointEntityData(locationKeys, modeStatusDataPointEntities, STATUS);
			populateProxiesFromDataPointEntityData(locationKeys, modeInProgressDataPointEntities, IN_PROGRESS);
			populateProxiesFromDataPointEntityData(locationKeys, modeVisualLockDataPointEntities, VISUAL_LOCK);
			//populateProxiesFromDataPointEntityData(locationKeys, modeAutoStatusDataPointEntities, AUTOMODE_STATUS);
			
			populateProxiesForAutoModeControl(locationKeys);
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknow exception catched." );
		}

        FUNCTION_EXIT;
	}

    // populateProxiesFromDataPointEntityData
    //
	void StationModesDataPointManager::populateProxiesFromDataPointEntityData( 
		const LocationKeys& locationKeys, 
		const DataPointEntityDataVector& dataPointEntities, 
		EStationModeDataPointType stationModeDataPointType )
	{
        FUNCTION_ENTRY( "populateProxiesFromDataPointEntityData" );

        if (( 0 > stationModeDataPointType ) || ( NUMBER_OF_STATIONMODE_DATAPOINT_TYPES <= stationModeDataPointType ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "invalid EStationModeDataPointType" );
            return;
        }

		DataPointEntityDataVector::const_iterator DataPointEntityDataIt;
		for (DataPointEntityDataIt = dataPointEntities.begin();
			 DataPointEntityDataIt != dataPointEntities.end();
			 ++DataPointEntityDataIt)
		{
			//zn++ - CL-20670
			//DataPointProxyAndIdentifiers * dataPointProxyAndIdentifiers = new DataPointProxyAndIdentifiers();
			//dataPointProxyAndIdentifiers->stationModeDataPointType = stationModeDataPointType;
			boost::shared_ptr<DataPointProxyAndIdentifiers> dataPointProxyAndIdentifiers(new DataPointProxyAndIdentifiers());
			dataPointProxyAndIdentifiers->stationModeDataPointType = stationModeDataPointType;
			//++zn

			unsigned long locationKey = (*DataPointEntityDataIt)->getLocation();

			if ( locationKeys.end() != std::find(locationKeys.begin(), locationKeys.end(), locationKey) )
			{
				unsigned long entityKey = (*DataPointEntityDataIt)->getKey();
				
				dataPointProxyAndIdentifiers->locationKey = locationKey;
				m_proxyFactory->createDataPointProxy(entityKey, *this, dataPointProxyAndIdentifiers->p_IDataPointProxy);

				m_entityKeyToDataPointProxyAndIdentifiers[entityKey] = dataPointProxyAndIdentifiers;
                StationModeDataPointKeySet& setStationModes = m_locationKeyToStationModeDataPointKeySet[locationKey];
                if (( setStationModes.size() > stationModeDataPointType ) && ( 0 <= stationModeDataPointType ))
                {
                    setStationModes[stationModeDataPointType] = entityKey;
                }
			}	
		}

		for (DataPointEntityDataIt = dataPointEntities.begin();
			 DataPointEntityDataIt != dataPointEntities.end();
			 ++DataPointEntityDataIt)
		{
			 delete ( *DataPointEntityDataIt );
		}

        FUNCTION_EXIT;
	}


	void StationModesDataPointManager::populateProxiesForAutoModeControl(const LocationKeys& locationKeys)
	{
		FUNCTION_ENTRY( "populateProxiesForAutoModeControl" );	

		LocationKeys::const_iterator LocationKeyIt;
		try
		{		
			for (LocationKeyIt=locationKeys.begin(); LocationKeyIt!=locationKeys.end(); ++LocationKeyIt)
			{
				std::string sqlToken = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration()
								.getAutoModeOnToken();
				
				TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance()
								.getEntitiesOfTypeAtLocationWithNameLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(), 
											*LocationKeyIt, 
											sqlToken);
        
				if ( entities.size() != 1 )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid number of datapoints for %s", sqlToken.c_str() );  //TD14337 yezh++      
				}
				else
				{
					TA_Base_Core::IEntityData* entityData = entities[0];
					
					boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData( dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData) );
            
					TA_Base_Bus::DataPointProxySmartPtr * dp = new TA_Base_Bus::DataPointProxySmartPtr();
					m_proxyFactory->createDataPointProxy(dpEntityData, *this, *dp);
					m_autoModeControlDataPoint[*LocationKeyIt] = dp;
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknow exception catched." );
		}

		FUNCTION_EXIT;
	}


    //
    // cleanUpSpecifiedDataPointProxies
    //
	void StationModesDataPointManager::cleanUpSpecifiedDataPointProxies( const LocationKeys& locationKeys, 
						bool doStatusAndControl)
	{
        FUNCTION_ENTRY( "cleanUpSpecifiedDataPointProxies" );

		LocationKeyToStationModeDataPointKeySet::iterator locationKeyToStationModeDataPointKeySetIt;
		for( locationKeyToStationModeDataPointKeySetIt = m_locationKeyToStationModeDataPointKeySet.begin();
			 locationKeyToStationModeDataPointKeySetIt != m_locationKeyToStationModeDataPointKeySet.end(); 
			 ++locationKeyToStationModeDataPointKeySetIt)
		{
			if ((locationKeys.end() != std::find(locationKeys.begin(), locationKeys.end(), (*locationKeyToStationModeDataPointKeySetIt).first)) &&
				 doStatusAndControl)
			{
				unsigned long entityKey = 0;

				entityKey = (*locationKeyToStationModeDataPointKeySetIt).second[CONTROL];
				if ( entityKey != 0 )
				{
					m_entityKeyToDataPointProxyAndIdentifiers.erase( entityKey );
					(*locationKeyToStationModeDataPointKeySetIt).second[ CONTROL ] = 0;
				}

				entityKey = (*locationKeyToStationModeDataPointKeySetIt).second[STATUS];
				if ( entityKey != 0 )
				{
					m_entityKeyToDataPointProxyAndIdentifiers.erase( entityKey );
					(*locationKeyToStationModeDataPointKeySetIt).second[ STATUS ] = 0;
				}

				entityKey = (*locationKeyToStationModeDataPointKeySetIt).second[IN_PROGRESS];
				if ( entityKey != 0 )
				{
					m_entityKeyToDataPointProxyAndIdentifiers.erase( entityKey );
					(*locationKeyToStationModeDataPointKeySetIt).second[IN_PROGRESS] = 0;
				}

				entityKey = (*locationKeyToStationModeDataPointKeySetIt).second[VISUAL_LOCK];
				if ( entityKey != 0 )
				{
					m_entityKeyToDataPointProxyAndIdentifiers.erase( entityKey );
					(*locationKeyToStationModeDataPointKeySetIt).second[VISUAL_LOCK] = 0;
				}
			}	
		}
        
        FUNCTION_EXIT;
	}


    //
    // cleanUpAllDataPointProxies
    //
	void StationModesDataPointManager::cleanUpAllDataPointProxies()
	{
        FUNCTION_ENTRY( "cleanUpAllDataPointProxies" );

		EntityKeyToDataPointProxyAndIdentifiers::iterator entityKeyToDataPointProxyAndIdentifierstIt
			= m_entityKeyToDataPointProxyAndIdentifiers.begin();

		m_entityKeyToDataPointProxyAndIdentifiers.clear();
		m_locationKeyToStationModeDataPointKeySet.clear();
		m_isStatusAndControlInitialised = false;
		m_isProposedInitialised = false;
		m_autoModeControlDataPoint.clear();

        FUNCTION_EXIT;
	}


    //
    // processEntityUpdateEvent
    //
	void StationModesDataPointManager::processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType )
	{
		FUNCTION_ENTRY( "processEntityUpdateEvent" );

		ThreadGuard guard( m_observersLock );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processEntityUpdateEvent() obtained ThreadLockable." );

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[DP] %lu, update type: %d", entityKey, updateType);

		switch ( updateType )
		{
        //TD13744, also need to process ConfigAvailable event to display valid value
        case TA_Base_Bus::ConfigAvailable:
        case TA_Base_Bus::ValueStateUpdate:
        case TA_Base_Bus::AlarmSummaryUpdate://limin++, TD20740
        case TA_Base_Bus::AckSummaryUpdate:  //limin++, TD20740
			{
                EntityKeyToDataPointProxyAndIdentifiers::iterator entityKeyToDataPointProxyAndIdentifiersIt
				    = m_entityKeyToDataPointProxyAndIdentifiers.find( entityKey );
                if ( entityKeyToDataPointProxyAndIdentifiersIt != m_entityKeyToDataPointProxyAndIdentifiers.end() )
                {
                    updateObserversWithDataPointValue( entityKeyToDataPointProxyAndIdentifiersIt, true );
                }
            }
            break;
		//display the control status for select datapoints
        case TA_Base_Bus::ControlStateUpdate:
			{
				try
				{
					std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator iter;
					for (iter = m_autoModeControlDataPoint.begin();
					     iter != m_autoModeControlDataPoint.end();
						 ++iter)
					{
						TA_Base_Bus::DataPointProxySmartPtr* dp = (iter->second);
						if ((*dp)->getEntityKey() == entityKey)
						{
							TA_Base_Bus::EDataPointControlState state = (*dp)->getControlState();

							StationModeStatusObservers::iterator stationModeStatusObserversIt;
							for ( stationModeStatusObserversIt = m_registeredStationModeStatusObservers.begin();
								  stationModeStatusObserversIt != m_registeredStationModeStatusObservers.end();
					              ++stationModeStatusObserversIt)
							{
								(*stationModeStatusObserversIt)->stationAutoModeControlStateUpdated(iter->first, state);	
							}	
							break;
						}
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ScadaProxyException caught: %s", e.what());
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Exception caught during processEntityUpdateEvent, entitykey [%u], updateType [%d].", entityKey, updateType);
				}
			}

        default:
            break;
        }

		FUNCTION_EXIT;
	}


    //
    // updateObserversWithDataPointValue
    //
    void StationModesDataPointManager::updateObserversWithDataPointValue( EntityKeyToDataPointProxyAndIdentifiers::iterator entityKeyToDataPointProxyAndIdentifiersIt, bool isValid )
    {
        FUNCTION_ENTRY( "updateObserversWithDataPointValue" );
		
		int newVal = 0;
		unsigned long locationKey;
		StationModeStatusObservers::iterator stationModeStatusObserversIt;

		switch( ( *entityKeyToDataPointProxyAndIdentifiersIt ).second->stationModeDataPointType )
		{
		case STATUS:
			if ( false == m_registeredStationModeStatusObservers.empty() )
			{
                locationKey = (*entityKeyToDataPointProxyAndIdentifiersIt).second->locationKey;

                if ( true == isValid )
                {
                    try
                    {
                        newVal = getValueFromLocationAndType( locationKey, STATUS );
                    }
                    catch ( const ScadaProxyException& )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Station Mode Status." );
                        isValid = false;
                    }
                }
				
				for ( stationModeStatusObserversIt = m_registeredStationModeStatusObservers.begin();
				      stationModeStatusObserversIt != m_registeredStationModeStatusObservers.end();
					  ++stationModeStatusObserversIt)
				{
					(*stationModeStatusObserversIt)->stationModeStatusUpdated(locationKey, isValid, newVal);	
				}			
			}
			break;

		case IN_PROGRESS:
			if ( false == m_registeredStationModeStatusObservers.empty() )
			{
				locationKey = (*entityKeyToDataPointProxyAndIdentifiersIt).second->locationKey;
                if ( true == isValid )
                {
                    try
                    {
                        newVal = getValueFromLocationAndType( locationKey, IN_PROGRESS );
                    }
                    catch ( const ScadaProxyException& )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Station Mode In Progress." );
                        isValid = false;
                    }
                }
				
				for ( stationModeStatusObserversIt = m_registeredStationModeStatusObservers.begin();
				      stationModeStatusObserversIt != m_registeredStationModeStatusObservers.end();
					  ++stationModeStatusObserversIt)
				{
					(*stationModeStatusObserversIt)->stationModeInProgressUpdated(locationKey, isValid, newVal);
				}			
			}
			break;

		case VISUAL_LOCK:
			if ( false == m_registeredStationModeStatusObservers.empty() )
			{
				locationKey = ( *entityKeyToDataPointProxyAndIdentifiersIt ).second->locationKey;
                if ( true == isValid )
                {
                    try
                    {
                        newVal = getValueFromLocationAndType( locationKey, VISUAL_LOCK );
                    }
                    catch ( const ScadaProxyException& )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Station Mode In Progress." );
                        isValid = false;
                    }
                }
				
				for ( stationModeStatusObserversIt = m_registeredStationModeStatusObservers.begin();
				      stationModeStatusObserversIt != m_registeredStationModeStatusObservers.end();
					  ++stationModeStatusObserversIt)
				{
					(*stationModeStatusObserversIt)->stationVisualLockUpdated(locationKey, isValid, newVal);
				}			
			}
			break;

		case AUTOMODE_STATUS:
			if ( false == m_registeredStationModeStatusObservers.empty() )
			{
				locationKey = ( *entityKeyToDataPointProxyAndIdentifiersIt ).second->locationKey;
                if ( true == isValid )
                {
                    try
                    {
                        newVal = getValueFromLocationAndType( locationKey, AUTOMODE_STATUS );
                    }
                    catch ( const ScadaProxyException& )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Station Mode In Progress." );
                        isValid = false;
                    }
                }

				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Test::AutoMode status is %d", newVal);
				
				for ( stationModeStatusObserversIt = m_registeredStationModeStatusObservers.begin();
				      stationModeStatusObserversIt != m_registeredStationModeStatusObservers.end();
					  ++stationModeStatusObserversIt)
				{
					(*stationModeStatusObserversIt)->stationAutoModeStatusUpdated(locationKey, isValid, newVal);
				}			
			}
			break;

		default:
			break; // Do nothing for CONTROL.
		}

        FUNCTION_EXIT;   
    }


    //
    // getValueFromLocationAndType
    //
	int StationModesDataPointManager::getValueFromLocationAndType( unsigned long locationKey, 
		                                         EStationModeDataPointType stationModeDataPointType )
	{
        FUNCTION_ENTRY( "getValueFromLocationAndType" );

		LocationKeyToStationModeDataPointKeySet::iterator locationKeyToStationModeDataPointKeySetIt
			= m_locationKeyToStationModeDataPointKeySet.find(locationKey);

		if (locationKeyToStationModeDataPointKeySetIt != m_locationKeyToStationModeDataPointKeySet.end())
		{
			unsigned long entityKey = (*locationKeyToStationModeDataPointKeySetIt).second[stationModeDataPointType];
            
            //TD13744, only try to access datapoint when they are valid
            if ( 0 != entityKey && m_entityKeyToDataPointProxyAndIdentifiers[entityKey]->p_IDataPointProxy->isConfigValid())
            {
				if (stationModeDataPointType == VISUAL_LOCK ||
					stationModeDataPointType == AUTOMODE_STATUS)
				{
					int returnValue = static_cast<int>(m_entityKeyToDataPointProxyAndIdentifiers[entityKey]
										->p_IDataPointProxy->getValue().getBoolean() );
					FUNCTION_EXIT;
					return returnValue;
				}
				if (stationModeDataPointType == IN_PROGRESS)
				{
					int returnValue = static_cast<int>(m_entityKeyToDataPointProxyAndIdentifiers[entityKey]
										->p_IDataPointProxy->getValue().getEnum() );
					FUNCTION_EXIT;
					return returnValue;
				}
				else
				{
					int returnValue = static_cast<int>(m_entityKeyToDataPointProxyAndIdentifiers[entityKey]
										->p_IDataPointProxy->getValue().getFloat() );
					FUNCTION_EXIT;
					return returnValue;
				}
			}
            else
            {
                FUNCTION_EXIT;
                TA_THROW( ScadaProxyException( "The Corresponding ECS Station Mode DataPoint Proxy is not ready yet." ) );
			}
		}

        FUNCTION_EXIT;
        TA_THROW( ScadaProxyException( "No Corresponding ECS Station Mode DataPoint could be located from currently configured Token." ) );
	}

	
	void StationModesDataPointManager::setAutoModeStatusForLocationAndType( unsigned long locationKey, 
		                                           ECurrentStationAutoModeType currentStationAutoModeType )
	{
        FUNCTION_ENTRY( "setAutoModeStatusForLocation" );

		std::string sAutoModeStatus = "";
		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator AutoModeProxyIt = 
						m_autoModeControlDataPoint.find(locationKey);

		if (AutoModeProxyIt != m_autoModeControlDataPoint.end())
		{
			
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

			if(currentStationAutoModeType == CURRENT_STATION_AUTO_MODE_ON)
			{
				sAutoModeStatus = (*m_autoModeControlDataPoint[locationKey])->getBooleanLabel(true);
			}
			else if (currentStationAutoModeType == CURRENT_STATION_AUTO_MODE_OFF)
			{
				sAutoModeStatus = (*m_autoModeControlDataPoint[locationKey])->getBooleanLabel(false);
			}

			if( false == sAutoModeStatus.empty())
			{
				//zn++ CL-20847: before set timetable, should check if operator do have the right
				TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
				unsigned long resourceId = (*m_autoModeControlDataPoint[locationKey])->getEntityKey();
				TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;	
				std::string reason;
				bool result = false;
				
				try
				{
					result = rightsMgr->isActionPermittedOnResource(
						sessionId, 
						resourceId, 
						TA_Base_Bus::aca_DATA_POINT_WRITE, 
						reason, 
						decisionModule);													
				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Check rights before set timetable meet exception");
					result = false;
				}
				
				if (!result)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No rights to set auto mode to %s", (*m_autoModeControlDataPoint[locationKey])->getEntityName().c_str());
					throw TA_Base_Core::AccessDeniedException("No rights to set auto mode");
				}
				//++zn

				(*m_autoModeControlDataPoint[locationKey])->setValue(sAutoModeStatus,sessionId);								
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Cannot find proxy, auto mode on control for location %d failed.",
				locationKey);
		}
		
        FUNCTION_EXIT;        
	}

//  CL-20847 : no used
// 	void StationModesDataPointManager::setAutoModeStatusForLocation(int locationIndex,
// 				ECurrentStationAutoModeType currentStationAutoModeType)
// 	{
// 		setAutoModeStatusForLocationAndType(locationIndex,currentStationAutoModeType);
// 	}

    //
    // getStationModeStatus
    //
	int StationModesDataPointManager::getStationModeStatus( unsigned long locationKey )
	{
        FUNCTION_ENTRY( "getStationModeStatus" );

        int returnVal = getValueFromLocationAndType( locationKey, STATUS );

        FUNCTION_EXIT;
		return returnVal;
	}

    //
	// getStationAutoModeStatus
	//
    int StationModesDataPointManager::getStationAutoModeStatus( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getStationAutoModeStatus" );

        int returnVal = getValueFromLocationAndType( locationKey, AUTOMODE_STATUS );

        FUNCTION_EXIT;
		return returnVal;
    }


    //
    // getStationModeInProgress
    //
	int StationModesDataPointManager::getStationModeInProgress( unsigned long locationKey )
	{
        FUNCTION_ENTRY( "getStationModeInProgress" );

        int returnVal = getValueFromLocationAndType(locationKey, IN_PROGRESS);

        FUNCTION_EXIT;
		return returnVal;
	}


	//
	// getStationLockStatus
	//
	int StationModesDataPointManager::getStationLockStatus(unsigned long locationKey)
	{
        FUNCTION_ENTRY( "getStationLockStatus" );

        int returnVal = getValueFromLocationAndType(locationKey, VISUAL_LOCK);

        FUNCTION_EXIT;
		return returnVal;		
	}


	// 7196-Start
	void StationModesDataPointManager::refreshHealthPollInterval()
	{
		FUNCTION_ENTRY( "refreshDataPollHealthPointInterval" );

		try 
        {
            
            m_pollTime = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().
                getDataPointHealthPollInterval();
		
        }
        catch ( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve DataPointHealthInterval parameter value." ); 
        }
        catch ( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve DataPointHealthInterval parameter value." );
        }

		FUNCTION_EXIT;
	}
   // 7196-End

	std::string getErrorMessage(std::string exception)
	{
		std::string errorMessage=exception;

		std::string::size_type pos = exception.find( "AccessDeniedException" );
		if ( pos != std::string::npos ) 
		{
			errorMessage = exception.substr(0, pos) + "Reason: No rights";
			return errorMessage;
		}
		
		return errorMessage;

	}


} // namespace TA_IRS_App
