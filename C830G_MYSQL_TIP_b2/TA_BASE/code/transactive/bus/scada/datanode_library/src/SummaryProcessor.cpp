/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/scada/datanode_library/src/SummaryProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/06/02 14:56:02 $
  * Last modified by:  $Author: yong.liu $
  *
  *	The SummaryProcessor thread is responsible 
  * for generating events and submitting
  * them.
  *
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ScadaProxyException.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    SummaryProcessor*                        SummaryProcessor::m_singleton = NULL;
    int                                      SummaryProcessor::m_count = 0;
    TA_Base_Core::NonReEntrantThreadLockable SummaryProcessor::m_singletonLock;


    SummaryProcessor* SummaryProcessor::getInstance()
	{
		TA_THREADGUARD( m_singletonLock );

		if ( m_singleton == NULL )
		{
			m_singleton = new SummaryProcessor();
		}
		++m_count;
		return m_singleton;
	}	

    void SummaryProcessor::removeInstance( SummaryProcessor*& removeMe )
	{
		TA_THREADGUARD( m_singletonLock );
		
		if ( removeMe == m_singleton )
		{
			if ( --m_count <= 0 )
			{
				delete m_singleton;
				m_singleton = NULL;
				m_count = 0;
			}
			removeMe = NULL;
		}
	}


	SummaryProcessor::SummaryProcessor()
	:
    m_entityMap (NULL),
    m_summaryProcessor(NULL),
    m_logStats(false)
	{
        if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "LogStatistics" ) )
        {
            m_logStats = true;
        }
        int maxNumber = 10;
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "SummaryProcessorWorkers" ) )
		{
			maxNumber = atoi ( TA_Base_Core::RunParams::getInstance().get( "SummaryProcessorWorkers" ).c_str() );
        }
        
        if ( m_logStats )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Starting Summary Processor with %d threads", maxNumber );
        }
        
        // create the processor pool, but dont start it until we have the entity map
        m_summaryProcessor = new TA_Base_Core::QueueProcessorPool<SummaryRequest, TA_Base_Core::QueueProcessorWorker<SummaryRequest> >( maxNumber, *this, false );
	}

	SummaryProcessor::~SummaryProcessor()
	{
        delete m_summaryProcessor;
        m_summaryProcessor = NULL;
	}
    

	void SummaryProcessor::setEntityMap(const EntityMap* entityMap)
	{
        TA_ASSERT( NULL != entityMap, "entityMap cannot be NULL" );

		m_entityMap = entityMap;
        
        // start the processing of queued items
        m_summaryProcessor->startProcessing();
    }

    void SummaryProcessor::queueItem( boost::shared_ptr<SummaryRequest>& item )
    {
    	unsigned long entitykey = item->getEntityKey();
        m_summaryProcessor->queueItem(entitykey, item);

		if (TA_Base_Core::DebugUtil::DebugDebug <= TA_Base_Core::DebugUtil::getInstance().getLevel()) 
    	{ 
    		std::vector<unsigned long> queSizeVect = m_summaryProcessor->getQueueSizes();
			int index = item->getEntityKey() % queSizeVect.size();
			unsigned long qSize = queSizeVect[index];
			if( qSize >= 100 )
			{
        		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Queued Summary entitykey [%d] updateType[%d],put into Thread [%d] has [%d] items",
        			entitykey,item->getUpdateType(),index, qSize);
				return;
			}
    	} 

        if ( m_logStats )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Queued summary request %u, %d",
                        item->getEntityKey(), item->getUpdateType() );
        }
    }

    void SummaryProcessor::queueItemUnique( boost::shared_ptr<SummaryRequest>& item )
    {
    	unsigned long entitykey = item->getEntityKey();
        m_summaryProcessor->queueItemUnique(entitykey, item);
		
		if (TA_Base_Core::DebugUtil::DebugDebug <= TA_Base_Core::DebugUtil::getInstance().getLevel()) 
    	{ 
    		std::vector<unsigned long> queSizeVect = m_summaryProcessor->getQueueSizes();
			int index = item->getEntityKey() % queSizeVect.size();
			unsigned long qSize = queSizeVect[index];
			if( qSize >= 100 )
			{
        		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Queued Summary entitykey [%d] updateType[%d],put into Thread [%d] has [%d] items",
        			entitykey,item->getUpdateType(),index, qSize);
				return;
			}
    	} 
		
        if ( m_logStats )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Queued unique summary request %u, %d",
                item->getEntityKey(), item->getUpdateType() );
        }
    }

	void SummaryProcessor::queueProcessorPoolCallback( boost::shared_ptr<SummaryRequest> newRequest )
	{
		if (newRequest.get() != NULL)
		{
            TA_ASSERT( NULL != m_entityMap, "The entity map must be set before the workers are started" );
            
            unsigned long entityKey = newRequest->getEntityKey();
			EntityMap::const_iterator it = m_entityMap->find(entityKey);

			if (it == m_entityMap->end())
			{
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                    "Cannot find DataNode with entitykey==%lu", entityKey );
                return;
            }
            
            if ( true == m_logStats )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "Processing summary request %u, %d",
                    newRequest->getEntityKey(), newRequest->getUpdateType() );
            }

            DataNode* dataNode = NULL;

			try
			{
				dataNode = dynamic_cast<DataNode*>(it->second);
			}
			catch (...)
			{
			}

            TA_ASSERT(dataNode != NULL, "m_entityMap contains invalid pointer; Failed to cast into DataNode*");

            switch (newRequest->getType())
            {
            case SummaryRequest::ProxyUpdate:
                dataNode->processProxyUpdate(newRequest->getUpdateType());
                break;

            case SummaryRequest::UpdateActivation:
                dataNode->processUpdateActivation();
                break;

            default:
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled Request Type: %d",
                    newRequest->getType() );
                TA_ASSERT( false, "Unhandled request type." );
            }
		}
	}
}
