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
//  DirectAgentSynchronisationInterface.cpp
//  Implementation of the Class DirectAgentSynchronisationInterface
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4284)
#pragma warning (disable:4786)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataNodeLocalSynchronizer.h"
#include "bus/scada/proxy_library/src/DataPointLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    DataNodeLocalSynchronizer::DataNodeLocalSynchronizer( const std::string& agentName,
                                                                    unsigned long locationKey,
																	DataNodeSyncManager& manager,
																	DataNodeProxyManager& callback)
    :	AgentSynchronisationInterface(agentName, locationKey),
        m_manager(manager),
        m_callback(callback)
    {
    }


    DataNodeLocalSynchronizer::~DataNodeLocalSynchronizer()
    {
    }

    void DataNodeLocalSynchronizer::processDataNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
    {
        FUNCTION_ENTRY ("processDataNodeUpdate");

        boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate = boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
        dataNodeUpdate->entityKey = pkey;
        dataNodeUpdate->detail = *(updateDetails.get());
        
        //add the updates into queue for proxy manager later processing
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "DataNodeBulkPoll %s - received an update [%d] for entity %d",
                m_agentName.c_str(), dataNodeUpdate->detail.updateType, dataNodeUpdate->entityKey);
            
            m_callback.insertUpdateQueue(dataNodeUpdate);
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Catched");
        }
        
        FUNCTION_EXIT;
    }


    void DataNodeLocalSynchronizer::run()
    {
        while ( false == isTerminating() )
        {
			try
			{
				// Wait for signal that we have a request to process
				waitForRequest();

				if ( true == isTerminating() )
				{
					break;
				}

				RequestItem currentRequest = getNextRequest();
                        
				switch (currentRequest.requestType)
				{
				case EmptyQueue:
					// No work to do simply finish
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "EmptyQueue" );
					break;
                
				case Add:
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNodeLocalSynchronizer AddDataNode %s:%u",
							getAgentName().c_str(), currentRequest.entityKey );

                        // first load configuration
                        m_callback.onloadDataNodeConfiguration(currentRequest.entityKey);
                        
                        TA_Base_Bus::IEntity* localEntity = m_manager.getLocalEntity(currentRequest.entityKey);
                        TA_Base_Bus::DataNode* localDataNode = dynamic_cast<TA_Base_Bus::DataNode*>(localEntity);
                        
                        TA_ASSERT(localDataNode != NULL, "non-local datanode is added into local datanode synchroniser");

						// Get current values
						unsigned long entityKey = currentRequest.entityKey;

						// peter low: register first then only get updates
						m_entityKeyList.insert(entityKey);
						localDataNode->registerForUpdateDetails(this);
						
						try
						{
							boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> dataNodeState = 
                                localDataNode->getCompleteDynamicStates();
                            
                            boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate = 
                                boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
                            
                            dataNodeUpdate->entityKey = entityKey;
                            dataNodeUpdate->detail.updateType = DnCompleteSyncState;
                            dataNodeUpdate->detail.completeUpdate = *(dataNodeState.get());
                            
                            m_callback.insertUpdateQueue(dataNodeUpdate);
                        }
						catch(TA_Base_Core::ScadaProxyException& ex)
						{
							LOG_GENERIC( SourceInfo, DebugUtil::DebugError,ex.what());
						}
					}
					break;
                
				case Remove:
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNodeLocalSynchronizer RemoveDataNode %s:%u",
						getAgentName().c_str(), currentRequest.entityKey );

					m_entityKeyList.erase(currentRequest.entityKey);
					break;

				default:
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled currentRequest.requestType: %d in DataNodeLocalSynchronizer::run()",
						currentRequest.requestType );
				}
			}
			catch(const TransactiveException & te)
			{
				std::ostringstream desc;
				desc << "[DataNodeLocalSynchronizer::run] "
					 << te.what();
				LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str());
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "[DataNodeLocalSynchronizer::run] Catching unknown exception to prevent run from terminating pre-maturely");
			}
        }
		
    }

};



