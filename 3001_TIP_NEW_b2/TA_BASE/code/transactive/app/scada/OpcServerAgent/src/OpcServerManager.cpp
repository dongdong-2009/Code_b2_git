/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcServerManager.cpp $
  * @author:	Dhanshri Mokashi
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2016/06/27 09:30:19 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description: The class provides a two connections,  one where it fetches updates from Client agents and second where
  *	it updates OPCBridge with appropriate data format (converted from Client agent updates).
  */
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/OpcServerAgentAccessFactory.h"

#include "OpcServerManager.h"
#include "OpcDataNodeAgentProxy.h"
#include "OpcDataPointAgentProxy.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/SimpleNode.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/SimplePoint.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	OpcServerManager::OpcServerManager()
		:
		m_threadTerminated(false),
		m_pollPeriod(500),
		m_retryPeriod(10),
		m_queueSemaphore(0)
	{
		m_dnAgentProxies.clear();
		m_dpAgentProxies.clear();

		// set m_pollPeriod to .5s if parameter is not set between 1000 to 2000 ms
        m_pollPeriod = atoi(TA_Base_Core::RunParams::getInstance().get("PollFrequencyMilliSecs").c_str());
        if ( (m_pollPeriod <= 100) || (m_pollPeriod >= 2000) )
        {
            m_pollPeriod = 500;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ignoring the input value. Setting PollFrequency to default [%u] ms.", m_pollPeriod);
        }
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting PollFrequency to [%u] ms.", m_pollPeriod);
		}
        // set retry period if parameter is set between m_pollPeriod to 1800 seconds

        unsigned long retryPeriod = atol(TA_Base_Core::RunParams::getInstance().get("RetryPeriodSecond").c_str());
        
        if ( (retryPeriod * 1000) >= m_pollPeriod &&  retryPeriod <= 1800 )
        {
            m_retryPeriod = retryPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retry period is set to [%u] seconds for all agent.", m_retryPeriod);
        }
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting Retry period to [%u] seconds.", m_retryPeriod);
		}
		m_dnMap.clear();
		m_dpMap.clear();
		m_dataNodes.empty();
		m_dataPoints.empty();
	}

	OpcServerManager::~OpcServerManager()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Destroying OpcServerManager...");		
		std::map<unsigned long, OpcDataNodeAgentProxy*>::iterator it = m_dnAgentProxies.begin();
		while (m_dnAgentProxies.end() != it)
		{
			if( it->second != NULL)
			{
				delete it->second;
			}
			++it;
		}
		m_dnAgentProxies.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_dnAgentProxies.clear()");

		std::map<unsigned long, OpcDataPointAgentProxy*>::iterator it2 = m_dpAgentProxies.begin();
		while (m_dpAgentProxies.end() != it2)
		{
			if( it2->second != NULL)
			{
				delete it2->second;
			}
			++it2;
		}
		m_dpAgentProxies.clear();

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_dpAgentProxies.clear()");
		// DN - delete locked block
		{
			TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
			
			std::map < unsigned long, TA_Base_Bus::SimpleNode*>::iterator  itr = m_dataNodes.begin();
			
			while ( itr != m_dataNodes.end() )
			{
				if ( NULL != itr->second )
				{
					delete itr->second;
					itr->second = NULL;
				}
				++itr;
			}
			m_dataNodes.clear();
		}
		// END DN - delete locked block

		// DP - delete locked block
		{
			TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );
			
			std::map < unsigned long, TA_Base_Bus::SimplePoint*>::iterator  itrDP = m_dataPoints.begin();
			
			while ( itrDP != m_dataPoints.end() )
			{
				if ( NULL != itrDP->second )
				{
					delete itrDP->second;
					itrDP->second = NULL;
				}
				++itrDP;
			}
			m_dataPoints.clear();
		}
		//END - DP - delete locked block
		m_dnMap.clear();
		m_dpMap.clear();
	}

	void OpcServerManager::terminate()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::terminate().");		
		m_threadTerminated = true;
		m_queueSemaphore.post();
		
		std::map<unsigned long, OpcDataNodeAgentProxy*>::iterator it = m_dnAgentProxies.begin();
		while (m_dnAgentProxies.end() != it)
		{
			if( it->second != NULL)
			{
				it->second->terminateAndWait();
			}
			++it;
		}

		std::map<unsigned long, OpcDataPointAgentProxy*>::iterator it2 = m_dpAgentProxies.begin();
		while (m_dpAgentProxies.end() != it2)
		{
			if( it2->second != NULL)
			{
				it2->second->terminateAndWait();
			}
			++it2;
		}
	}

	void OpcServerManager::run()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::run()" );
		unsigned long qSize = 0;
		AbstractSimpleData* asd;
		TA_Base_Bus::DisplayStateSequence* points = 0;
		TA_Base_Bus::DataNodeDisplayStateVector* nodes = 0;
		while (false == m_threadTerminated)
		{
			m_queueSemaphore.wait();
			if ( m_threadTerminated )
			{
				break;
			}
			{
				TA_Base_Core::ThreadGuard guard( m_queueLock );
				qSize = m_SimpleDataUpdates.size();
				if (qSize > 0)
				{
					asd = m_SimpleDataUpdates.front();
					m_SimpleDataUpdates.pop();
				}
			}
			if( qSize > 0 && asd->getAttributeType() == E_OPC_SDN)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcServerManager - nodes - queue size is:%d", qSize);
				SimpleNodeData *pSimpleNodeData = dynamic_cast<SimpleNodeData *>( asd );
				processData(pSimpleNodeData->m_dnUpdates);
				delete asd;
				asd = 0;			
			}
			else if(qSize > 0 && asd->getAttributeType() == E_OPC_SDP)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcServerManager - points - queue size is:%d", qSize);
				SimplePointData *pSimplePointData = dynamic_cast<SimplePointData *>( asd );
				processData(pSimplePointData->m_dpUpdates);
				delete asd;
				asd = 0;
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcServerManager - no updates!!! queue size is:%d", qSize );
			}
        }
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager run() finished as m_threadTerminated = %d", m_threadTerminated );
		m_threadTerminated = false;
	}

	void OpcServerManager::addSimpleNode(unsigned long entityKey, unsigned long agentKey)
	{
		TA_Base_Bus::SimpleNode* dn = new TA_Base_Bus::SimpleNode(entityKey);
		if(NULL == dn)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcServerManager - Failure to create SimpleNode for EK %u", entityKey);
			return;
		}
		m_dataNodes.insert( std::pair< unsigned long, TA_Base_Bus::SimpleNode* > (entityKey, dn) );

		AgentKeyMap::iterator it = m_dnMap.find(agentKey);
		if (it == 	m_dnMap.end())
		{
			std::vector<unsigned long> vecElement;
			vecElement.push_back(entityKey);
			m_dnMap.insert( std::pair< unsigned long, std::vector<unsigned long> > (agentKey, vecElement) );
		}
		else
		{
			m_dnMap[agentKey].push_back(entityKey);
		}
	}
	
	void OpcServerManager::addSimplePoint(unsigned long entityKey, unsigned long agentKey)
	{
		TA_Base_Bus::SimplePoint* dp = new TA_Base_Bus::SimplePoint(entityKey);
		if(NULL == dp)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcServerManager - Failure to create SimplePoint for EK %u", entityKey);
			return;
		}
		m_dataPoints.insert( std::map< unsigned long, TA_Base_Bus::SimplePoint* >::value_type (entityKey, dp) );

		AgentKeyMap::iterator it1 = m_dpMap.find(agentKey);
		if (it1 == 	m_dpMap.end())
		{
			std::vector<unsigned long> vecElement;
			vecElement.push_back(entityKey);
			m_dpMap.insert( std::pair< unsigned long, std::vector<unsigned long> > (agentKey, vecElement) );
		}
		else
		{
			m_dpMap[agentKey].push_back(entityKey);
		}
	}

	void OpcServerManager::createAgentProxy(std::vector<TA_Base_Core::AgentData*>& dpdnAgentList)
	{
		start();
		unsigned int i = 0;
		unsigned int nSize = dpdnAgentList.size();
		if(nSize < 1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "vAgentKeys.size is unexpected value %d, returning..", nSize);
			return;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating m_dnAgentProxies list, size= %d", nSize);		
		}
		unsigned long agentKey = 0;

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "OpcDataNodeAgentProxy creation for %d items.", m_dnMap.size());
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "OpcDataPointAgentProxy creation for %d items.", m_dpMap.size());

		for (i = 0; i < nSize; ++i)
		{
			agentKey = dpdnAgentList[i]->agentKey;
			AgentKeyMap::iterator itDN = m_dnMap.find(agentKey);
			if (itDN == m_dnMap.end())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "No datanodes for this agent %u so not creating OpcDataNodeAgentProxy", agentKey);
			}
			else
			{
				m_dnAgentProxies[agentKey] = new OpcDataNodeAgentProxy(agentKey, m_dnMap[agentKey], this);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Created OpcDataNodeAgentProxy for agentKey= %u, Total DNs = %d", agentKey, m_dnMap[agentKey].size() );
			}

			AgentKeyMap::iterator itDP = m_dpMap.find(agentKey);
			if (itDP == m_dpMap.end())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "No datapoints for this agent %u so not creating OpcDataPointAgentProxy", agentKey);
			}
			else
			{
				m_dpAgentProxies[agentKey] = new OpcDataPointAgentProxy(agentKey, m_dpMap[agentKey], this);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Created OpcDataPointAgentProxy for agentKey= %u, Total DPs = %d", agentKey, m_dpMap[agentKey].size() );
			}
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Total OpcDataNodeAgentProxy %d items.", m_dnAgentProxies.size());
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Total OpcDataPointAgentProxy %d items.", m_dpAgentProxies.size());
	}

	void OpcServerManager::startSynchronise()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::startSynchronise(), DN.size = %d: DP.size = %d", m_dataNodes.size(), m_dataPoints.size());
		std::map<unsigned long, OpcDataNodeAgentProxy*>::const_iterator it = m_dnAgentProxies.begin();
		while (m_dnAgentProxies.end() != it)
		{
			it->second->startSynchronise();
			++it;
		}
		std::map<unsigned long, OpcDataPointAgentProxy*>::const_iterator itSrv = m_dpAgentProxies.begin();
		while (m_dpAgentProxies.end() != itSrv)
		{
			itSrv->second->startSynchronise();
			++itSrv;
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_dataNodes.size = %d: m_dataPoints.size = %d", m_dataNodes.size(), m_dataPoints.size());
	}

	void OpcServerManager::stopSynchronise()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::stopSynchronise()");

		std::map<unsigned long, OpcDataNodeAgentProxy*>::const_iterator it = m_dnAgentProxies.begin();
		while (m_dnAgentProxies.end() != it)
		{
			it->second->stopSynchronise();
			++it;
		}

		std::map<unsigned long, OpcDataPointAgentProxy*>::const_iterator itSrv = m_dpAgentProxies.begin();
		while (m_dpAgentProxies.end() != itSrv)
		{
			itSrv->second->stopSynchronise();
			++itSrv;
		}
	}
	void OpcServerManager::submitUpdateData(TA_Base_Bus::DataNodeDisplayStateVector* dnUpdates)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::submitUpdateData()");
		{
			TA_Base_Core::ThreadGuard guard( m_queueLock );
			SimpleNodeData* asd = new SimpleNodeData;
			asd->m_dnUpdates = dnUpdates;
			m_SimpleDataUpdates.push(asd);
		}
		m_queueSemaphore.post();
	}

	void OpcServerManager::submitUpdateData(TA_Base_Bus::DisplayStateSequence* dpUpdates)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::submitUpdateData()");
		{
			TA_Base_Core::ThreadGuard guard( m_queueLock );
			SimplePointData* asd = new SimplePointData;
			asd->m_dpUpdates = dpUpdates;
			m_SimpleDataUpdates.push(asd);
		}
		m_queueSemaphore.post();
	}

	void OpcServerManager::processData(TA_Base_Bus::DisplayStateSequence* dpUpdates)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::processData() - DisplayStateSequence");
		unsigned int updateSize = 0;
		//Update SimplePoint
		if (0 != dpUpdates)
		{
			updateSize = dpUpdates->size();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updateSize = %u" , updateSize);
		if (NULL != dpUpdates && updateSize > 0)
		{
			for (unsigned int i = 0; i < updateSize && !m_threadTerminated; ++i)
			{
				TA_Base_Bus::DataPointDisplayState dataPointUpdate = (*dpUpdates)[i];
				TA_Base_Bus::SimplePoint* sp = getSimplePoint(dataPointUpdate.entityKey);
				if( sp != NULL)
				{
					// sp will update its value then notify all observers about the change.
					sp->setDisplayState(dataPointUpdate);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating Point data.EK %u, data.dataPointStatus %u", dataPointUpdate.entityKey , dataPointUpdate.dataPointStatus);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No simplepoint exists for this EK %u.", dataPointUpdate.entityKey);
				}
			}
		}
	}

	void OpcServerManager::processData(TA_Base_Bus::DataNodeDisplayStateVector* dnUpdates)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcServerManager::processData() - DataNodeDisplayStateVector");
		//Update SimpleNode
		unsigned int updateSize = 0;
		if (0 != dnUpdates)
		{
			updateSize = dnUpdates->size();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updateSize = %u" , updateSize);
		if (NULL != dnUpdates && updateSize > 0)
		{
			for (unsigned int i = 0; i < updateSize && !m_threadTerminated; ++i)
			{
				TA_Base_Bus::DataNodeDisplayState dataUpdate = (*dnUpdates)[i];
				TA_Base_Bus::SimpleNode* sn = getSimpleNode(dataUpdate.entityKey);
				if( sn != NULL)
				{
					// sp will update its value then notify all observers about the change.
					sn->setNodeState(dataUpdate);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating Node: data.EK %u, data.quality.value %d", dataUpdate.entityKey , dataUpdate.quality.value);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No simpleNode exists for this EK %u.", dataUpdate.entityKey);
				}
			}//End - for
		}
	}

	TA_Base_Bus::SimplePoint*  OpcServerManager::getSimplePoint ( unsigned long entityKey )
	{
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );
		
		std::map < unsigned long, TA_Base_Bus::SimplePoint*>::const_iterator  itr = m_dataPoints.find( entityKey );
		
		if ( itr != m_dataPoints.end() )
		{
            if ( NULL == itr->second )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataPoint", entityKey );
			}
        }
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not found DataPoint with EK %ul in list m_dataPoints", entityKey );
		}
		return itr->second;
	}

	void  OpcServerManager::getAllSimplePoints (std::map < unsigned long, TA_Base_Bus::SimplePoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();
		
        dataPointList = m_dataPoints;
		return;
	}


	TA_Base_Bus::SimpleNode*  OpcServerManager::getSimpleNode( unsigned long entityKey )
	{
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		std::map < unsigned long, TA_Base_Bus::SimpleNode*>::const_iterator  itr = m_dataNodes.find( entityKey );
		
		if ( itr != m_dataNodes.end() )
		{
            if ( NULL == itr->second )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not found DataNode with EK %ul in list m_dataNodes", entityKey );
		}
		return itr->second;
	}

	void  OpcServerManager::getAllSimpleNodes (std::map < unsigned long, TA_Base_Bus::SimpleNode *>& dataNodeList )
	{
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();

		dataNodeList = m_dataNodes;

		return;
	}

}
