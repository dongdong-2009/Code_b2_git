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
//  DataPointLocalSynchronizer.cpp
//  Implementation of the Class DataPointLocalSynchronizer
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4284 4786 4146 4018 4250 4290 4503)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataPointLocalSynchronizer.h"
#include "bus/scada/proxy_library/src/DataPointLocalAccess.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    DataPointLocalSynchronizer::DataPointLocalSynchronizer( const std::string& agentName,
																	  unsigned long locationKey,
																	  DataPointSyncManager& manager,
																	  DataPointProxyManager& callback )
    :	AgentSynchronisationInterface(agentName, locationKey),
        m_manager(manager),
        m_callback(callback)
    {
    }


    DataPointLocalSynchronizer::~DataPointLocalSynchronizer()
    {
    }


    void DataPointLocalSynchronizer::processDataPointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
    {
        FUNCTION_ENTRY ("processDataPointUpdate");

        //deal with update directly
        boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
        dataPointUpdate->entityKey = pkey;
        dataPointUpdate->detail = *(updateDetails.get());
        
        // add the updates into queue for proxy manager later processing
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "DataPointBulkPoll %s - received an update [%d] for entity %d",
                m_agentName.c_str(), dataPointUpdate->detail.updateType, dataPointUpdate->entityKey);
            
            m_callback.insertUpdateQueue(dataPointUpdate);
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Caught during processDataPointUpdate");
        }
        
        FUNCTION_EXIT;
    }


    void DataPointLocalSynchronizer::run()
    {
        while ( false == isTerminating() )
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
                
            case Add :
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPointLocalSynchronizer AddDataPoint %s:%u",
                        getAgentName().c_str(), currentRequest.entityKey );

                    // first load configuration
                    m_callback.onLoadDataPointConfiguration(currentRequest.entityKey);

                    // then get dynamic status
                    TA_Base_Bus::IEntity* localEntity = m_manager.getLocalEntity(currentRequest.entityKey);
                    TA_Base_Bus::DataPoint* localDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>(localEntity);
                    
                    TA_ASSERT(localDataPoint != NULL, "non-local datapoint is added into local datapoint synchroniser");

                    // Get current values
                    unsigned long entityKey = currentRequest.entityKey;

					// peter low: register first then only get updates
					m_entityKeyList.insert(entityKey);
					localDataPoint->registerForUpdateDetails(this);

                    try
                    {
                        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> internalState = 
                            localDataPoint->getCompleteDynamicUpdate();
                            
                        boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = 
                            boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
                        
                        dataPointUpdate->entityKey = entityKey;
                        dataPointUpdate->detail.updateType = CompleteDynamicSyncStateUpdate;
                        dataPointUpdate->detail.syncCompleteState = *(internalState.get());

                        m_callback.insertUpdateQueue(dataPointUpdate);
                        //internal state deletes itself when it falls out of context
                    }
                    catch(TA_Base_Core::ScadaProxyException& ex)
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,ex.what());
                    }
                }
                break;

            case Remove:
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPointLocalSynchronizer RemoveDataPoint %s:%u",
                    getAgentName().c_str(), currentRequest.entityKey );

                m_entityKeyList.erase(currentRequest.entityKey);
                break;
                
            default:
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled currentRequest.requestType: %d in DataPointLocalSynchronizer::run()",
                    currentRequest.requestType );
                break;
            }
        }
		
    }
};



