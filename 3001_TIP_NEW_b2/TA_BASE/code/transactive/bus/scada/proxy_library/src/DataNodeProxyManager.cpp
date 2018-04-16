/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/proxy_library/src/DataNodeProxyManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2014/07/11 18:24:37 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeProxyManager.cpp
//  Implementation of the Class DataNodeProxyManager
//  Created on:      28-Jun-2004 09:39:37 AM
///////////////////////////////////////////////////////////


#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/naming/src/NamedObject.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataNodeProxyManager::DataNodeProxyManager(DataNodeSyncManager& syncManager,
	                   						   ObserverUpdateQueueProcessor& observerUpdateQueueProcessor)
        : m_syncManager(syncManager),
          m_observerUpdateQueueProcessor(observerUpdateQueueProcessor)
	{
		//start();
	}


	DataNodeProxyManager::~DataNodeProxyManager()
	{
		terminateAndWait();

		ProxyMap::MapVector proxyVector = m_proxyMap.getAll();

		ProxyMap::MapVector::iterator proxyIt;
	    for ( proxyIt = proxyVector.begin(); proxyIt != proxyVector.end(); ++proxyIt )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "dataNode(entityKey=%d) not deleted.", (*proxyIt)->getEntityKey());
		}

		m_proxyMap.clear();
	}

    
    void DataNodeProxyManager::registerDataNodeEntityData( TA_Base_Core::IEntityDataPtr EntityData )
    {
        TA_Base_Core::DataNodeEntityDataPtr sharedDnEntityData = m_entityMap.get(EntityData->getKey());
        if (sharedDnEntityData.get() == NULL)
        {
            if (EntityData.get() != NULL)
            {
                try
                {
                    sharedDnEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData);
                    m_entityMap.insert(EntityData->getKey(), sharedDnEntityData);
                }
			    catch (...)
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error casting DataNodeEntityData %s (%lu)",
                        EntityData->getName().c_str(), EntityData->getKey() );
			    }
            }
        }
    }

	boost::shared_ptr<DataNodeProxy> DataNodeProxyManager::addProxy (unsigned long entityKey, TA_Base_Core::DataNodeEntityDataPtr entityData)
	{
		// Check if proxy is already present
		boost::shared_ptr<DataNodeProxy> proxy = m_proxyMap.get(entityKey);

		if (NULL != proxy.get())
		{
			return proxy;
		}

        TA_Base_Core::DataNodeEntityDataPtr sharedDnEntityData = m_entityMap.get(entityKey);
        
        if (sharedDnEntityData.get() == NULL)
        {
            if (entityData.get() == NULL)
            {
                // load entity information from database
                TA_Base_Core::IEntityData* loadedEntityData = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
        
                TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(loadedEntityData);
        
                if (dnEntityData == NULL)
                {
                    delete loadedEntityData;
					loadedEntityData = NULL;
                    std::ostringstream message;
                    message << "Error: entity [" << entityKey << "] is not a datanode.";
                    TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
                }

                sharedDnEntityData = TA_Base_Core::DataNodeEntityDataPtr(dnEntityData);
            }
            else
            {
                // valid data passed in
                sharedDnEntityData = entityData;
            }

            m_entityMap.insert(entityKey, sharedDnEntityData);
        }
        
        unsigned long locationKey = sharedDnEntityData->getLocation();
        if (locationKey == 0)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid location.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        std::string agentName = sharedDnEntityData->getAgentName();
        if (agentName.length() < 1)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid agent key.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        // Add to map
		proxy = boost::shared_ptr<DataNodeProxy>(new DataNodeProxy(sharedDnEntityData, m_syncManager, m_observerUpdateQueueProcessor));

		TA_ASSERT( NULL != proxy.get(), "DataNodeProxyManager::DataNodeProxyManager() - Failed to create the proxy");

        //if more than one thread created more proxy after safetymap::get() function
        //insert function will reserve the first one, discard others and return the first one
        proxy = m_proxyMap.insert(entityKey, proxy);

		return proxy;
	}


	void DataNodeProxyManager::getProxySmartPointer(unsigned long entityKey, boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData,
													DataNodeProxySmartPtr & dataNodeProxySmartPtr, IEntityUpdateEventProcessor& observer)
	{
		boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
		
		proxy = m_proxyMap.get(entityKey);

		if (NULL == proxy.get())
		{
			proxy = addProxy(entityKey, entityData);

		    // Ensure synchronisation of this node
            m_syncManager.add(m_entityMap.get(entityKey));
		}

		TA_ASSERT(NULL != proxy.get(),"DataNode proxy not found.");

        dataNodeProxySmartPtr.setProxyInfo(proxy, observer, *this);
	}


    void DataNodeProxyManager::onCreateProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);

        // if the proxy is created and configured already, notify observer
        DataNodeProxy* dataNodeProxy = dynamic_cast<DataNodeProxy*>(observerNotifier);
        TA_ASSERT(dataNodeProxy != NULL, "non datanode proxy pointer is passed into datanode proxy manager");
        
        if (dataNodeProxy->isConfigValid())
        {
            dataNodeProxy->queueNotification(ConfigAvailable);
        }
    }


	void DataNodeProxyManager::onCopyProxy( IProxyObserverNotifier* observerNotifier,
                                                 IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);
    }


	void DataNodeProxyManager::onDeleteProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        observerNotifier->removeObserver(observer);

        // after proxy is created, it will be cached and never be deleted
    }


	/**
	 * Data Node Agent Interface not available
	 */
	void DataNodeProxyManager::onDataNodeAgentInterfaceNotAvailable (unsigned long entityKey)
	{
        boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
        proxy = m_proxyMap.get(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataNodeAgentInterfaceNotAvailable() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }
        
        bool isAlreadyConfigValid = proxy->isConfigValid();

		// let the proxy knows that the agent is no longer available
        //this function will set config valid to 'true'
        proxy->updateAgentNotAvailableStatus();

        if (isAlreadyConfigValid)
        {
            proxy->queueNotification(QualitySummaryUpdate);
        }
        else
        {
            proxy->queueNotification(ConfigAvailable);
        }
	}

	/**
	 * Data Node config load from DB required
	 */
	void DataNodeProxyManager::onloadDataNodeConfiguration ( unsigned long entityKey )
	{
        boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
        proxy = m_proxyMap.get(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataNodeConfigDbLoadRequired() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }
        
        proxy->loadConfiguration();
        //notify config available until get complete update or fail to connect to agent
        //proxy->queueNotification(ConfigAvailable);
    }


    //TD15939
	void DataNodeProxyManager::insertUpdateQueue(boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate)
	{
		insert(dataNodeUpdate);
	}

	void DataNodeProxyManager::processEvent(boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate)
	{
		try
		{
			unsigned long  entityKey = dataNodeUpdate->entityKey;
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"ProxyManager process update [%d] for entity %d",
				dataNodeUpdate->detail.updateType, entityKey);

            boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
            proxy = m_proxyMap.get(entityKey);
            
            if( NULL == proxy.get() )
            {
                //if we haven't found it, this just means the proxy has been deleted so the 
                //update is invalid
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "processEvent() Proxy for entity - %lu does not exist",
                    entityKey );
                
                return;
            }
            
            switch (dataNodeUpdate->detail.updateType)
			{
			case DnCompleteSyncState:
				{
                    bool isAlreadyConfigValid = proxy->isConfigValid();

                    //this function will set config valid to 'true'
                    proxy->updateState(dataNodeUpdate->detail.completeUpdate);

                    if (isAlreadyConfigValid)
                    {
                        proxy->queueNotification(TagStateUpdate);
                        proxy->queueNotification(AlarmSummaryUpdate);
                        proxy->queueNotification(AckSummaryUpdate);
                        proxy->queueNotification(ScanInhibitSummaryUpdate);
                        proxy->queueNotification(AlarmInhibitSummaryUpdate);
                        proxy->queueNotification(MmsAlarmInhibitSummaryUpdate);
                        proxy->queueNotification(ControlInhibitSummaryUpdate);
                        proxy->queueNotification(OverrideSummaryUpdate);
                        proxy->queueNotification(QualitySummaryUpdate);
                        proxy->queueNotification(ControlSummaryUpdate);
						proxy->queueNotification(CombinedInhibitMMSUpdate);
                        proxy->queueNotification(NotesStateUpdate);
                        proxy->queueNotification(InhibitStateUpdate);
                        proxy->queueNotification(ConfigStateLastOperatorUpdate);
                    }
                    else
                    {
                        proxy->queueNotification(ConfigAvailable);
                    }
                    
					break;
				}
			case DnTagStateUpdate:
                proxy->updateTagState(dataNodeUpdate->detail.tagState);
                proxy->queueNotification(TagStateUpdate);
				break;
                
			case DnAlarmSummaryUpdate :
                proxy->updateAlarmSummaryState(dataNodeUpdate->detail.alarmSummary);
                proxy->queueNotification(AlarmSummaryUpdate);
                break;

			case DnAlarmAckSummaryUpdate :
                proxy->updateAlarmAckSummaryState(dataNodeUpdate->detail.ackSummary);
                proxy->queueNotification(AckSummaryUpdate);
                break;

			case DnScanInhibitSummaryUpdate :
                proxy->updateScanInhibitSummaryState(dataNodeUpdate->detail.scanInhibitSummary);
                proxy->queueNotification(ScanInhibitSummaryUpdate);
                break;

			case DnAlarmInhibitSummaryUpdate :
                proxy->updateAlarmInhibitSummaryState(dataNodeUpdate->detail.alarmInhibitSummary);
                proxy->queueNotification(AlarmInhibitSummaryUpdate);
                break;

			case DnMmsAlarmInhibitSummaryUpdate :
                proxy->updateMmsAlarmInhibitSummaryState(dataNodeUpdate->detail.mmsAlarmInhibitSummary);
                proxy->queueNotification(MmsAlarmInhibitSummaryUpdate);
                break;

			case DnControlInhibitSummaryUpdate :
                proxy->updateControlInhibitSummaryState(dataNodeUpdate->detail.controlInhibitSummary);
                proxy->queueNotification(ControlInhibitSummaryUpdate);
                break;

			case DnOverrideSummaryUpdate :
                proxy->updateOverrideSummaryState(dataNodeUpdate->detail.overrideSummary);
                proxy->queueNotification(OverrideSummaryUpdate);
                break;

			case DnQualitySummaryUpdate :
                proxy->updateQualitySummaryState(dataNodeUpdate->detail.qualitySummary);
                proxy->queueNotification(QualitySummaryUpdate);
                break;

			case DnControlPendingSummaryUpdate :
                proxy->updateControlPendingSummaryState(dataNodeUpdate->detail.controlPendingSummary);
                proxy->queueNotification(ControlSummaryUpdate);
                break;

			case DnCombinedInhibitMMSUpdate:
				proxy->updateCombinedInhibitMMSState(dataNodeUpdate->detail.combinedInhibitMMSState);
				proxy->queueNotification(CombinedInhibitMMSUpdate);
				break;

			case DnNotesUpdate:
                proxy->updateNotesState(dataNodeUpdate->detail.notes);
                proxy->queueNotification(NotesStateUpdate);
                break;

            case DnInhibitMMSUpdate:
                proxy->updateInhibitState(dataNodeUpdate->detail.inhibitMMSState);
                proxy->queueNotification(InhibitStateUpdate);
                break;

            case DnLastOperatorUpdate:
                proxy->updateLastOperator(dataNodeUpdate->detail.lastOperator);
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
