/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
///////////////////////////////////////////////////////////
//  DataPointProxyManager.cpp
//  Implementation of the Class DataPointProxyManager
//  Created on:      28-Jun-2004 09:39:37 AM
///////////////////////////////////////////////////////////

#include <sys/timeb.h>

#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h"
#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataPointProxyManager::DataPointProxyManager( DataPointSyncManager& syncManager,
	                    						  ObserverUpdateQueueProcessor& observerUpdateQueueProcessor)
        : m_syncManager(syncManager),
          m_observerUpdateQueueProcessor(observerUpdateQueueProcessor)
	{
		//start();
	}


    DataPointProxyManager::~DataPointProxyManager()
    {
		terminateAndWait();
		
		ProxyMap::MapVector proxyVector = m_proxyMap.getAll();

		ProxyMap::MapVector::iterator proxyIt;
	    for ( proxyIt = proxyVector.begin(); proxyIt != proxyVector.end(); ++proxyIt )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "dataPoint(entityKey=%d) not deleted.", (*proxyIt)->getEntityKey());
		}

	    m_proxyMap.clear();
    }


    void DataPointProxyManager::registerDataPointEntityData( TA_Base_Core::IEntityDataPtr EntityData )
    {
        TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = m_entityMap.get(EntityData->getKey());
        if (sharedDpEntityData.get() == NULL)
        {
            if (EntityData.get() != NULL)
            {
                try
                {
                    sharedDpEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(EntityData);
                    m_entityMap.insert(EntityData->getKey(), sharedDpEntityData);
                }
			    catch (...)
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error casting DataPointEntityData %s (%lu)",
                        EntityData->getName().c_str(), EntityData->getKey() );
			    }
            }
        }
    }
	boost::shared_ptr<DataPointProxy> DataPointProxyManager::addProxy (unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData)
	{
		// Check if proxy is already present
		boost::shared_ptr<DataPointProxy> proxy = m_proxyMap.get(entityKey);

		if (NULL != proxy.get())
		{
			return proxy;
		}

        TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = m_entityMap.get(entityKey);
        
        if (sharedDpEntityData.get() == NULL)
        {
            if (entityData.get() == NULL)
            {
                // load entity information from database
                TA_Base_Core::IEntityData* loadedEntityData = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
        
                TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(loadedEntityData);
        
                if (dpEntityData == NULL)
                {
                    delete loadedEntityData;
                    std::ostringstream message;
                    message << "Error: entity [" << entityKey << "] is not a datapoint.";
                    TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
                }

                sharedDpEntityData = TA_Base_Core::DataPointEntityDataPtr(dpEntityData);
            }
            else
            {
                // valid data passed in
                sharedDpEntityData = entityData;
            }

            m_entityMap.insert(entityKey, sharedDpEntityData);
        }
        
        unsigned long locationKey = sharedDpEntityData->getLocation();
        if (locationKey == 0)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid location.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        std::string agentName = sharedDpEntityData->getAgentName();
        if (agentName.length() < 1)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid agent key.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
                
		// Add to map
		proxy = boost::shared_ptr<DataPointProxy>(new DataPointProxy(sharedDpEntityData, m_syncManager, m_observerUpdateQueueProcessor));

		TA_ASSERT( NULL != proxy.get(), "DataPointProxyManager::DataPointProxyManager() - Failed to create the proxy");

        //if more than one thread created more proxy after safetymap::get() function
        //insert function will reserve the first one, discard others and return the first one
		proxy = m_proxyMap.insert(entityKey, proxy);

		return proxy;
	}


	void DataPointProxyManager::getProxySmartPointer(unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData,
																	   IEntityUpdateEventProcessor& observer,
																	   DataPointProxySmartPtr & dataPointProxySmartPtr)
	{
		boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
		
		proxy = m_proxyMap.get(entityKey);

		if (NULL == proxy.get())
		{
			proxy = addProxy(entityKey, entityData);

		    // Ensure synchronisation of this node
            m_syncManager.add(m_entityMap.get(entityKey));		
			dataPointProxySmartPtr.setProxyInfo(proxy, observer, *this);
		}
		else
		{			
			dataPointProxySmartPtr.setProxyInfo(proxy, observer, *this);
		}
		//]]
	}


    void DataPointProxyManager::onCreateProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);

        // if the proxy is created and configured already, notify observer
        DataPointProxy* dataPointProxy = dynamic_cast<DataPointProxy*>(observerNotifier);
        TA_ASSERT(dataPointProxy != NULL, "non datapoint proxy pointer is passed into datapoint proxy manager");

        if (dataPointProxy->isConfigValid())
        {
            dataPointProxy->queueNotification(ConfigAvailable);
        }
    }


	void DataPointProxyManager::onCopyProxy( IProxyObserverNotifier* observerNotifier,
                                                 IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);
    }


	void DataPointProxyManager::onDeleteProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        observerNotifier->removeObserver(observer);
        
        // after proxy is created, it will be cached and never be deleted
    }


    /**
     * DataPoint Configuration
     */
    void DataPointProxyManager::onLoadDataPointConfiguration(unsigned long entityKey)
    {
		boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
		
		proxy = m_proxyMap.get(entityKey);

		if( NULL == proxy.get() )
		{
			//if we haven't found it, this just means the proxy has been deleted so the 
			//update is invalid
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onLoadDataPointConfiguration() Proxy for entity - %lu does not exist",
				entityKey );

			return;
		}

		proxy->loadConfiguration();
        //notify config available until get complete update or fail to connect to agent
        //proxy->queueNotification(ConfigAvailable);
    }


    /**
     * DataPoint Agent Interface not available
     */
    void DataPointProxyManager::onDataPointAgentInterfaceNotAvailable (unsigned long entityKey)
    {
        boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
        proxy = m_proxyMap.get(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataPointAgentInterfaceNotAvailable() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }
        
        bool isAlreadyConfigValid = proxy->isConfigValid();
        //this function will set config valid to 'true'
        proxy->updateAgentNotAvailableStatus();
        if (isAlreadyConfigValid)
        {
			//liuyu++
            //proxy->queueNotification(ValueStateUpdate);
			proxy->queueNotification(QualitySummaryUpdate);
        }
        else
        {
            proxy->queueNotification(ConfigAvailable);
        }
    }


	//TD15939
	void DataPointProxyManager::insertUpdateQueue(boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate)
	{
		insert(dataPointUpdate);
	}
	
	//TD15939, 
	void DataPointProxyManager::processEvent(boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate)
	{
		try
		{
			unsigned long  entityKey = dataPointUpdate->entityKey;
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"DataPointProxyManager::processEvent: ProxyManager process update [%d] for entity %d",
				dataPointUpdate->detail.updateType, entityKey);

            boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
            proxy = m_proxyMap.get(entityKey);
            
            if( NULL == proxy.get() )
            {
                //if we haven't found it, this just means the proxy has been deleted so the 
                //update is invalid
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "processEvent() Proxy for entity - %lu does not exist",
                    entityKey );
                
                return;
            }
                
            switch (dataPointUpdate->detail.updateType)
			{
            case TA_Base_Bus::CompleteDynamicSyncStateUpdate:
                {
                    bool isAlreadyConfigValid = proxy->isConfigValid();
                    //this function will set config valid to 'true'
                    proxy->updateInternalState(dataPointUpdate->detail.syncCompleteState);

                    if (isAlreadyConfigValid)
                    {
                        proxy->queueNotification(ValueStateUpdate);
                        proxy->queueNotification(ControlStateUpdate);
                        proxy->queueNotification(InhibitStateUpdate);
                        proxy->queueNotification(ConfigStateLastOperatorUpdate);
						proxy->queueNotification(AckSummaryUpdate);
                    }
                    else
                    {
                        proxy->queueNotification(ConfigAvailable);
                    }
                    
                    break;
                }
            case TA_Base_Bus::ValueUpdate:
                    proxy->updateValueStatus(dataPointUpdate->detail.value);
                    proxy->queueNotification(ValueStateUpdate);
                    break;

            case TA_Base_Bus::AlarmStateUpdate:
                    proxy->updateAlarmStatus(dataPointUpdate->detail.alarmState);
					//liuyu++
                    //proxy->queueNotification(ValueStateUpdate);
					proxy->queueNotification(AlarmSummaryUpdate);
                    break;

            case TA_Base_Bus::AlarmAckStateUpdate:
                    proxy->updateAlarmAckStatus(dataPointUpdate->detail.alarmAckState);
					//liuyu++
                    //proxy->queueNotification(ValueStateUpdate);
					proxy->queueNotification(AckSummaryUpdate);
                    break;
                
            case TA_Base_Bus::AlarmPropertiesUpdate:
                    proxy->updateAlarmProperties(dataPointUpdate->detail.alarmProperties);
                    proxy->queueNotification(ValueStateUpdate);
                    break;

            case TA_Base_Bus::ControlUpdate:
                    proxy->updateControlStatus(dataPointUpdate->detail.controlState);
                    proxy->queueNotification(ControlStateUpdate);
                    break;

            case TA_Base_Bus::InhibitUpdate:
                    proxy->updateInhibitedStatus(dataPointUpdate->detail.inhibitState);
                    proxy->queueNotification(InhibitStateUpdate);
                    break;

            case TA_Base_Bus::LastOperatorUpdate:
                    proxy->updateLastOperatorInfo(dataPointUpdate->detail.lastOperatorInfo);
                    proxy->queueNotification(ConfigStateLastOperatorUpdate);
                    break;

            default:
					TA_ASSERT( false, "Proxy manager received an unknown update type" );
					break;

            }
		}
		catch ( const TA_Base_Core::ScadaProxyException & e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException", e.what() );
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Catched when processing update");
		}

	}

}
