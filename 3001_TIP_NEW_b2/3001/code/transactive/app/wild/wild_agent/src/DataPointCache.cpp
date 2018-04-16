/* DataPointCache
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/DataPointCache.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Implementation file
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/wild/wild_agent/src/DataPointCache.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "app/wild/wild_agent/src/DataCorrelation.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/wild/wild_agent/src/WILDConfiguration.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/message/types/WILDAgentAlarms_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/exceptions/src/WILDAgentExceptions.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/types/WILDAgentStateUpdate_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/WILDAgentExceptions.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace TA_IRS_App
{
	DataPointCache* DataPointCache::m_instance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable DataPointCache::m_virtualDataPointLock;

	DataPointCache::DataPointCache()
		:m_dataCorrelator(NULL),
		//TD18093, m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
		m_stateUpdateSender(NULL),
		m_isControlMode(false),
		m_isInitialStartUp(true),
		m_raisedInvalidWILDConfigurationAlarm(false) //TD15198 azenitha
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "DataPointCache");

		try
		{
			if(0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL))
			{
				m_isControlMode = true;
			}
			
			m_dataCorrelator = new DataCorrelation();
			m_virtualDataPointList.clear();	
				
			if(!m_stateUpdateSender)
			{
				m_stateUpdateSender = 
					TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::WILDAgentStateUpdate::Context );
			}
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", 
				"WILD Agent error. Rethrow as Generic Agent exception");

			cleanUp();
			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::AGENT_ERROR, "Unknown error with WILD Agent"));
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "DataPointCache");
	}


	DataPointCache::~DataPointCache()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~DataPointCache");

		cleanUp();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~DataPointCache");
	}


	void DataPointCache::cleanUp()
	{
        // ss++ TD16650
        m_alarmInhitedProxyVirtualDpMap.clear();
        m_indexToProxyEntityMap.clear();
        m_entityNameToEntityKeyMap.clear();
        // ++ss
        
		// Deactivate the DataPoints' servant.  The GenericAgent
		// will delete the objects from memory space

		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(dynamic_cast<TA_Base_Core::AlarmReceiver*>(this));

		if(m_stateUpdateSender != NULL)
		{
			delete m_stateUpdateSender;
			m_stateUpdateSender = 0;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		std::map<unsigned long, bool> noDupMap;
		for(std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator dpitr = m_virtualDataPointList.begin(); dpitr != m_virtualDataPointList.end(); dpitr++)
		{
			std::map<unsigned long, bool>::iterator itr = noDupMap.find(((dpitr)->second)->getEntityKey());
			if(itr != noDupMap.end())
			{
				// Do absolutely nothing.. means a duplicate dp has been found, no need to resubscribe.
			}
			else
			{
				noDupMap[((dpitr)->second)->getEntityKey()] = true;
			}
		}

		noDupMap.clear();
		m_virtualDataPointList.clear();

		if(m_dataCorrelator != NULL)
		{
			delete m_dataCorrelator;
			m_dataCorrelator = 0;
		}

        m_dpProxyInhibitMonitorMap.clear();
		m_dpProxyMap.clear();
	}

	
	DataPointCache* DataPointCache::getInstance()
	{
		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is okay at this stage.
			TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );

			if ( 0 == m_instance )
			{
				m_instance = new DataPointCache();
    		}
		}

		return m_instance;		
	}


	void DataPointCache::removeInstance()
	{
		// 
		// Guard this to prevent multiple threads atempting
		// to delete/create simultaneously
		//
		TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );  // Any lock will do
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}
	}


	void DataPointCache::insertIntoVirtualDataPointList(unsigned long entityKey, unsigned long wheelID, TA_Base_Bus::DataPoint& dataPoint)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "insertIntoVirtualDataPointList");

		TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );

		// Insert the data point into map. The map will have 2 unique keys. Note this will
		// increase the size of the map but pointers are not expensive and the speedup we observe
		// from finding a data point by wheelID in the map has greater advantages.
		m_virtualDataPointList[entityKey] = &dataPoint;

		if(wheelID != 0) // Only insert if it is a valid ID. It is 0 if convertAddressIntoID() found something wrong.
		{
			m_virtualDataPointList[wheelID] = &dataPoint;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
			"Added %s to map. Size of virtual DataPoint Map: %d", (dataPoint.getDataPointName()).c_str(), m_virtualDataPointList.size());

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "insertIntoVirtualDataPointList");
	}


	void DataPointCache::updateDataPointConfiguration(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "updateDataPointConfiguration");

		TA_Base_Bus::DataPoint* theDataPoint = 0;
		TA_Base_Core::IEntityData* theEntity = 0;

		TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );

		bool updateFound = false;
		std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator itr = m_virtualDataPointList.find(updateEvent.getKey());
		if(itr != m_virtualDataPointList.end())
		{
			TA_ASSERT(itr->second, "DataPoint* in m_dataPoints is NULL");
			theDataPoint = itr->second;
			updateFound = true;
		}
		
		if (theDataPoint)
        {
            // get the type of updating event
		    switch ( updateEvent.getModifications() )
		    {
			    case TA_Base_Core::Update:
			    {
				    // pass the config update to the data point for updating
					// TO DO: need to do more here if we're hard coding/making assumptions
					// on data point configuration
				    theDataPoint->update ( updateEvent );

				    // exit switch statement
				    break;
			    }
			    case TA_Base_Core::Delete:
					// Do nothing
					break;
		    }
        }

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateDataPointConfiguration");
	}

	void DataPointCache::subscribeForAlarmUpdates()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "subscribeForAlarmUpdates");

		TA_Base_Core::ThreadGuard  guard(m_virtualDataPointLock);

		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(dynamic_cast<TA_Base_Core::AlarmReceiver*>(this));

		TA_Base_Core::AlarmSubscriptionManager::subscribeToAlarmsFromLocation(
					TA_Base_Core::DataPointAlarms::DpTrue, // All WILD alarms
					dynamic_cast<TA_Base_Core::AlarmReceiver*>(this),
					WILDConfiguration::getInstance()->getAgentLocationKey()); // specified location

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "subscribeForAlarmUpdates");
	}


	void DataPointCache::unsubscribeForAlarmUpdates()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "unsubscribeForAlarmUpdates");

		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(dynamic_cast<TA_Base_Core::AlarmReceiver*>(this));

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "unsubscribeForAlarmUpdates");		
	}


	void DataPointCache::receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& notificationMessage )
	{
 		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "receiveAlarmMessage");

		// Loack the data point map so we can clear the alarm state consistently.
		TA_Base_Core::ThreadGuard  guard(m_virtualDataPointLock);

		switch ( notificationMessage._d() )
		{
			// Only care about updates.
			case TA_Base_Core::UpdateAlarm: // Updated alarm
				try
				{
					if(notificationMessage.update().alarmHasBeenClosed)
					{
						// Here is the only place where we normalise our wheel alarms. The operator
						// has manually closed the alarm through the Alarm Manager/Banner. 
						std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator itr = m_virtualDataPointList.find(notificationMessage.update().assocEntityKey);
						if(itr != m_virtualDataPointList.end())	
						{
							TA_Base_Bus::DpValue currentDPVal = (itr->second)->getCurrentValue();
							if(currentDPVal.getBoolean() != false) // Only update if value is different.
							{
								// Now update the virtual data point we're responsible for..
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
										TA_Base_Core::DebugUtil::DebugNorm, "Updating data point state for %s", (itr->second)->getDataPointName().c_str());

								TA_Base_Bus::DpValue dpValue((itr->second)->getDataPointDataType(),
															 (itr->second)->getEnumLabelsMap(),
															 (itr->second)->getBooleanLabels());
								dpValue.setBoolean ( false );  //Reset the alarm condition
    
								TA_Base_Bus::DataPointState newState( dpValue, notificationMessage.update().closeTime, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
									(itr->second)->updateFieldState( newState );
							}
						}
					}
				
				}
				catch (...)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					 "Exception handling update alarm notification.");
				}
				break;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "receiveAlarmMessage");
	}	


	TA_Base_Bus::DataPoint* DataPointCache::updateAndRetrieveDataPoint(unsigned long wildDpKey, unsigned long wheelID, bool value, long timeStamp)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "updateAndRetrieveDataPoint");

		TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );

		std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator itr = m_virtualDataPointList.find(wheelID);
		if(itr != m_virtualDataPointList.end())
		{
            // ss++ TD16650
            bool toInhibit = false;
            if(wildDpKey == 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Respective WILD Equipment Datapoint EntityKey is invalid (0)");
            }
            else
            {
                // find if the wild wheel has alarm inhibited
                std::map<unsigned long, std::vector<TA_Base_Bus::DataPoint*> >::iterator inhibitedIter = m_alarmInhitedProxyVirtualDpMap.find(wildDpKey);
                if(inhibitedIter != m_alarmInhitedProxyVirtualDpMap.end())
                {
                    toInhibit = true;
                    bool isPresent = false;
                    std::vector<TA_Base_Bus::DataPoint*>& trainDpInhibitedList = inhibitedIter->second;
                    for(unsigned int i = 0; i < trainDpInhibitedList.size(); i++)
                    {
                        if(trainDpInhibitedList[i] == itr->second)
                        {
                            isPresent = true;
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found an existing DP that is already inhibited");
                            break;
                        }
                    }

                    if(isPresent == false) // No existing record
                    {
                        (itr->second)->setInputInhibitState(TA_Base_Bus::DPT_INTERNAL_SESSION_ID, TA_Base_Bus::InhibitAlarm);
                        trainDpInhibitedList.push_back(itr->second);
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                            "%s set to alarm inhibit by wheel %d", (itr->second)->getDataPointName().c_str(), wildDpKey);
                    }
                }
            }

            if(toInhibit == false)
            {
                TA_Base_Bus::DpValue currentDPVal = (itr->second)->getCurrentValue();
			    if(currentDPVal.getBoolean() != value) // Only update if value is different.
			    {
				    // Now update the virtual data point we're responsible for..
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
						    TA_Base_Core::DebugUtil::DebugNorm, "Updating data point state for %s", (itr->second)->getDataPointName().c_str());

				    TA_Base_Bus::DpValue dpValue((itr->second)->getDataPointDataType(),
											     (itr->second)->getEnumLabelsMap(),
											     (itr->second)->getBooleanLabels());
				    dpValue.setBoolean ( value );  
    
					TA_Base_Bus::DataPointState newState( dpValue, timeStamp, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
					    (itr->second)->updateFieldState( newState );
			    }
            }

			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateAndRetrieveDataPoint");
			return itr->second;
            // ++ss
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
				TA_Base_Core::DebugUtil::DebugWarn, "Unable to locate data point for wheel ID %d", wheelID);
			// The data point cannot be found given the wheel ID. Could be a configuration problem 
			// or the createWheelID() not returning what we expected.

			std::ostringstream os;
			os << "Unable to update wheel data point. Unrecognised Wheel ID: ";
			os << wheelID;

			submitWILDErrorAlarm(os.str());
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateAndRetrieveDataPoint");
		return NULL;
	}

	
	void DataPointCache::setDataPointsMode(bool isControl)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setDataPointsMode");

		TA_Base_Core::ThreadGuard guard( m_virtualDataPointLock );
		// For each of the data points created for this agent, set the data point to monitor mode.
		std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator it = m_virtualDataPointList.begin();
		std::map<unsigned long, bool> noDupMap;
		for ( ; it!=m_virtualDataPointList.end(); ++it)
		{
			TA_ASSERT(it->second, "DataPoint* in m_virtualDataPointList is NULL");
			std::map<unsigned long, bool>::iterator itr = noDupMap.find((it->second)->getEntityKey());
			if(itr != noDupMap.end())
			{
				// Do absolutely nothing.. means a duplicate dp has been found, no need to resubscribe.
			}
			else
			{
				noDupMap[(it->second)->getEntityKey()] = true;

				if(isControl)
				{
					(it->second)->setToControlMode();
					m_isControlMode = true;

					// Initialise datapoints here.  This must be done after the datapoints
					// have been set to control mode otherwise the updateFieldState()
					// call will be ignored.

					// Initialise the virtual datapoints to some known status.
					TA_Base_Bus::DpValue dpValue((it->second)->getDataPointDataType(),
												 (it->second)->getEnumLabelsMap(),
												 (it->second)->getBooleanLabels());
					dpValue.setBoolean( false );  
    
					// Only initialise the data point state if the Agent is starting up for the first time.
					// If the Agent has been switched over from monitor to control, we don't want to 
					// override the data point states.
					if(m_isInitialStartUp)
					{
						TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
						(it->second)->updateFieldState( newState );
					}

				}
				else // Monitor
				{
					(it->second)->setToMonitorMode();
					m_isControlMode = false;
				}
			}
		}

		m_isInitialStartUp = false;
		noDupMap.clear();
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setDataPointsMode");
	}


	void DataPointCache::initialiseDataPointProxies()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "initialiseDataPointProxies");
		// Ignore Critical alarm, Status alarms data points as TEXT dp covers all of this.
		// i.e. we're reading the whole WILD table in one hit and bringing it across in the 
		// form of a TEXT data point.
		
		std::vector<TA_Base_Core::DataPointEntityData*> matchingDataPoints;

		try
		{		
			std::string token("%");
			token += WILDConfiguration::getInstance()->getAgentSubsystemName();
			token += WILDConfiguration::getInstance()->getWILDDetailsToken();
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Token to retrieve real data points is %s", token.c_str());

			TA_Base_Core::DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken(token,matchingDataPoints);

			//TD15198 azenitha++
			//On initialization, there's no need to check if alarm was raised because
			//m_raisedInvalidWILDConfigurationAlarm will always be false at startup
			//closeAlarm will never be called in this case to close previously raised alarm
			//Just send closeAlarm on startup (accdg to adam radics)
			//if (m_raisedInvalidWILDConfigurationAlarm == true)
			//{
				closeAlarm(TA_Base_Core::WILDAgentAlarms::InvalidWILDConfiguration);
				m_raisedInvalidWILDConfigurationAlarm = false;
			//}
			//TD15198 ++azenitha
	
			if(matchingDataPoints.empty())
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Configuration error. There is no TRS_SEND Data Node or child entities for this node!");
				std::string alarmDesc("There is no such Data Point with the label: WILDDetails. Wheel defects cannot be detected untill this is configured properly.");
				submitInvalidConfigAlarm(alarmDesc);
				m_raisedInvalidWILDConfigurationAlarm = true; //TD15198 azenitha
				return;
			}

			for(std::vector<TA_Base_Core::DataPointEntityData*>::iterator realDPItr = matchingDataPoints.begin(); realDPItr != matchingDataPoints.end(); realDPItr++)
			{
				LOGBEFORECOTS("boost::shared_ptr");
				boost::shared_ptr<TA_Base_Core::DataPointEntityData> dataPointEntityData(*realDPItr);
				LOGAFTERCOTS("boost::shared_ptr");
				TA_Base_Bus::DataPointProxySmartPtr *dpProxy = new TA_Base_Bus::DataPointProxySmartPtr();
				TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dataPointEntityData, *this, *dpProxy); 
				m_dpProxyMap[dataPointEntityData->getKey()] = dpProxy;

				//the shard_prt will delete it
				//delete *realDPItr; // Must delete after we've finished using it.
				
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugDebug, "Adding entity %s to data point proxy map", dataPointEntityData->getName().c_str());
			}
			matchingDataPoints.clear();

            // ss++ TD16650
            std::string substring = "%TRS_SEND%Alarm%";
			TA_Base_Core::DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken(substring,matchingDataPoints);
            
			if(matchingDataPoints.empty())
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Wild Inhibit monitor Configuration error. There is no TRS_SEND Data Node or child entities for this node!");
				std::string alarmDesc("There is no such Data Point with the label containing: %s. Wheel defects cannot be detected untill this is configured properly.", substring.c_str());
				submitInvalidConfigAlarm(alarmDesc);
				m_raisedInvalidWILDConfigurationAlarm = true; //TD15198 azenitha
				return;
			}

			for(std::vector<TA_Base_Core::DataPointEntityData*>::iterator proxyDPItr = matchingDataPoints.begin(); proxyDPItr != matchingDataPoints.end(); proxyDPItr++)
			{
				LOGBEFORECOTS("boost::shared_ptr");
				boost::shared_ptr<TA_Base_Core::DataPointEntityData> dataPointEntityData(*proxyDPItr);
				LOGAFTERCOTS("boost::shared_ptr");
				TA_Base_Bus::DataPointProxySmartPtr *dpProxy = new TA_Base_Bus::DataPointProxySmartPtr();
				TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dataPointEntityData, *this, *dpProxy); 
				m_dpProxyInhibitMonitorMap[dataPointEntityData->getKey()] = dpProxy;

                m_entityNameToEntityKeyMap[dataPointEntityData->getName()] = dataPointEntityData->getKey();
				
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugDebug, "Adding entity %s to data point proxy inhibit monitor map", dataPointEntityData->getName().c_str());
			}
			matchingDataPoints.clear();
            initialiseIndexToProxyEntityMap();
            // ++ss

		}
		catch(TA_Base_Core::DataException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to find an entity matching the one passed in");
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unable to initialise datapoint proxies: reason %s", ex.what());

			if(!matchingDataPoints.empty())
			{
				for(unsigned int i=0; i<matchingDataPoints.size(); i++)
				{
					if(matchingDataPoints[i] != NULL)
					{	
						delete matchingDataPoints[i];
						matchingDataPoints[i] = 0;
					}
				}
				matchingDataPoints.clear();
			}
			throw;
		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the entity from the database.");
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unable to initialise datapoint proxies: reason %s", ex.what());

			if(!matchingDataPoints.empty())
			{
				for(unsigned int i=0; i<matchingDataPoints.size(); i++)
				{
					if(matchingDataPoints[i] != NULL)
					{	
						delete matchingDataPoints[i];
						matchingDataPoints[i] = 0;
					}
				}
				matchingDataPoints.clear();
			}
			throw;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "initialiseDataPointProxies");
	}


	void DataPointCache::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "processEntityUpdateEvent");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"processEntityUpdateEvent triggered with %d", updateType);

		if(!m_isControlMode)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"No point processing update because Agent is in monitor mode");
			return;
		}

		if(updateType == TA_Base_Bus::ConfigStateUpdate)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				"Config State Update received for entity key = %d",entityKey);
			LOGMORE(SourceInfo, "WILD TEXT datapoint has changed and WILD Agent may require restart.");
		}
		else if(updateType == TA_Base_Bus::ValueStateUpdate)
		{
			std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator itr = m_dpProxyMap.find(entityKey);
			if(itr != m_dpProxyMap.end())
			{
				
				if((*(itr->second))->getTypeAsString() == TA_Base_Bus::DPT_TEXT_TYPE_STRING)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
						TA_Base_Core::DebugUtil::DebugInfo, "Received text data point update from PMS Agent");
					
					// Now peform the correlation function.
					try
					{
						std::string WILDDataPointValue((*(itr->second))->getValueAsString());
						if(0 == WILDDataPointValue.compare(TA_Base_Bus::QUALITY_BAD_VALUE_UNKNOWN_STRING))
						{
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
							"Error occured whilst processing Text data point! Correlation will not continue.");
							
							LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "processEntityUpdateEvent");
							return;
						}

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"DataPoint Proxy Update: Text Datapoint, new value is %s",WILDDataPointValue.c_str());
						
						m_dataCorrelator->correlateData(WILDDataPointValue);
					}
					catch(...)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", 
						"Unable to retrieve Text value from data point");
					}
				} 
			}
		}
        // ss++ TD16550
		else if(updateType == TA_Base_Bus::InhibitStateUpdate)
		{
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			    "Inhibit state update received");

			std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator itr = m_dpProxyInhibitMonitorMap.find(entityKey);
			if(itr != m_dpProxyInhibitMonitorMap.end())
			{
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Inhibit state update received for DP %d with alarm inhibited state as %d", 
                    (*(itr->second))->getEntityKey(), (*(itr->second))->isAlarmInhibited());

                std::map<unsigned long, std::vector<TA_Base_Bus::DataPoint*> >:: iterator iter = m_alarmInhitedProxyVirtualDpMap.find(entityKey);
                if((*(itr->second))->isAlarmInhibited() == true)
                {
                    if(iter == m_alarmInhitedProxyVirtualDpMap.end())  // has not been previously inhibited
                    {
                        std::vector<TA_Base_Bus::DataPoint*> tmpList;
                        m_alarmInhitedProxyVirtualDpMap[entityKey] = tmpList;
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New alarm Inhibition");
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Has previous alarm Inhibition. No need to add for this inhibition");
                    }
                }
                else if((*(itr->second))->isAlarmInhibited() == false)
                {
                    if(iter != m_alarmInhitedProxyVirtualDpMap.end())  // has been previously inhibited
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Has previous alarm Inhibition");

                        std::vector<TA_Base_Bus::DataPoint*>& stillInhibitedDpList = iter->second;
                        if(stillInhibitedDpList.empty() == false) // Got prev train wheels travel on it
                        {
                            // Go set the respective virtual datapoint in the list to un-inhibit the alarms
                            unInhibitTrainDatapoints(stillInhibitedDpList);
                        }
                        m_alarmInhitedProxyVirtualDpMap.erase(iter);
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No previous alarm Inhibition. No need to cleanup for now's un-inhibition");
                    }
                }
            }
        }
        // ++ss

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "processEntityUpdateEvent");
	}


	void DataPointCache::submitInvalidConfigAlarm(const std::string& description)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "submitInvalidConfigAlarm");
		
		TA_Base_Core::DescriptionParameters descriptionParameters;
		TA_Base_Core::NameValuePair pair1 ( "AgentName", WILDConfiguration::getInstance()->getAgentName());
		descriptionParameters.push_back ( &pair1 );
		TA_Base_Core::NameValuePair pair2 ("Detail", description);
		descriptionParameters.push_back ( &pair2 );

		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::WILDAgentAlarms::InvalidWILDConfiguration, 
			WILDConfiguration::getInstance()->getAgentKey(),
			WILDConfiguration::getInstance()->getAgentTypeKey(),
			descriptionParameters, 
			WILDConfiguration::getInstance()->getAgentName(),
			WILDConfiguration::getInstance()->getAgentLocationKey(),
			WILDConfiguration::getInstance()->getAgentSubsystemKey(),
			WILDConfiguration::getInstance()->getAgentAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "submitInvalidConfigAlarm");
	}

	//TD15198 azenitha++
	void DataPointCache::closeAlarm( const TA_Base_Core::MessageType& type )
	{
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(type, 
			WILDConfiguration::getInstance()->getAgentKey(), 
			WILDConfiguration::getInstance()->getAgentLocationKey());
	}
	//TD15198 ++azenitha

	void DataPointCache::submitWILDErrorAlarm(const std::string& description)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "submitWILDErrorAlarm");

		TA_Base_Core::DescriptionParameters descriptionParameters;
		TA_Base_Core::NameValuePair pair1 ( "Detail", description);
		descriptionParameters.push_back ( &pair1 );

		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::WILDAgentAlarms::WILDError, 
			WILDConfiguration::getInstance()->getAgentKey(),
			WILDConfiguration::getInstance()->getAgentTypeKey(),
			descriptionParameters, 
			WILDConfiguration::getInstance()->getAgentName(),
			WILDConfiguration::getInstance()->getAgentLocationKey(),
			WILDConfiguration::getInstance()->getAgentSubsystemKey(),
			WILDConfiguration::getInstance()->getAgentAssetName(),			
			TA_Base_Core::AlarmConstants::defaultTime );
								
		descriptionParameters.clear();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "submitWILDErrorAlarm");
	}


	unsigned long DataPointCache::convertAddressIntoID(std::string& address) 
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "convertAddressIntoID");	

		// The following simply removes the '.' character from the address and
		// return the numeric value.
		std::string::size_type pos = address.find('.');

		while (pos != std::string::npos)
		{
			address.replace(pos, 1, "");
			pos = address.find('.');
		}
	
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "convertAddressIntoID");	
		return atol(address.c_str());
	}

    // ss++ TD16650
    void DataPointCache::unInhibitTrainDatapoints(std::vector<TA_Base_Bus::DataPoint*>& inhibitedList)
    {
        for(unsigned int i = 0; i < inhibitedList.size(); i++)
        {
			//to do zhou yuan, need to make sure whether we need remove the other inhibit, 
			//when remove the alarm inhibit,
            inhibitedList[i]->setInputInhibitState(TA_Base_Bus::DPT_INTERNAL_SESSION_ID, TA_Base_Bus::NoInputInhibit );
        }
        inhibitedList.clear();
    }
    
    unsigned long DataPointCache::getProxyEntityKeyFromInhibitMonitorList(unsigned int index)
    {
        if(index >= m_indexToProxyEntityMap.size())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error finding proxy to be monitored. Requested Index = %d", index);
            return 0;
        }
        return m_indexToProxyEntityMap[index];
    }

    unsigned long DataPointCache::getCAEntityKeyFromInhibitMonitorList()
    {
        return m_indexToProxyEntityMap[100];
    }

    unsigned long DataPointCache::findProxyEntityKey(std::string proxyName)
    {
        std::map<std::string, unsigned long>::iterator inhibitIter = m_entityNameToEntityKeyMap.find(proxyName);
        if(inhibitIter != m_entityNameToEntityKeyMap.end())
        {
            return inhibitIter->second;
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find proxy %s", proxyName.c_str());
            return 0;
        }
    }

    void DataPointCache::initialiseIndexToProxyEntityMap()
    {
        // hardcoding here, should be changed to be configurable
        m_indexToProxyEntityMap[0] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A1NormalAlarm");
        m_indexToProxyEntityMap[1] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B1NormalAlarm");
        m_indexToProxyEntityMap[2] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A2NormalAlarm");
        m_indexToProxyEntityMap[3] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B2NormalAlarm");
        m_indexToProxyEntityMap[4] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A3NormalAlarm");
        m_indexToProxyEntityMap[5] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B3NormalAlarm");
        m_indexToProxyEntityMap[6] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A4NormalAlarm");
        m_indexToProxyEntityMap[7] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B4NormalAlarm");

        m_indexToProxyEntityMap[8] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A5NormalAlarm");
        m_indexToProxyEntityMap[9] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B5NormalAlarm");
        m_indexToProxyEntityMap[10] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A6NormalAlarm");
        m_indexToProxyEntityMap[11] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B6NormalAlarm");
        m_indexToProxyEntityMap[12] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A7NormalAlarm");
        m_indexToProxyEntityMap[13] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B7NormalAlarm");
        m_indexToProxyEntityMap[14] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A8NormalAlarm");
        m_indexToProxyEntityMap[15] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B8NormalAlarm");

        m_indexToProxyEntityMap[16] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A9NormalAlarm");
        m_indexToProxyEntityMap[17] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B9NormalAlarm");
        m_indexToProxyEntityMap[18] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A10NormalAlarm");
        m_indexToProxyEntityMap[19] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B10NormalAlarm");
        m_indexToProxyEntityMap[20] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A11NormalAlarm");
        m_indexToProxyEntityMap[21] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B11NormalAlarm");
        m_indexToProxyEntityMap[22] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-A12NormalAlarm");
        m_indexToProxyEntityMap[23] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-B12NormalAlarm");

        m_indexToProxyEntityMap[100] = findProxyEntityKey("OCC.WILD.WILD.TRS_SEND.diiTRS_SEND-CriticalNormalAlarm");
    }
    // ++ss

} //TA_App
