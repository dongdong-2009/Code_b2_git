/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcServerManager.h $
  * @author:	Dhanshri Mokashi
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2016/06/27 09:30:19 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description: Manages OPC Server Agent
  *
  */

#ifndef OPC_SERVER_MANAGER_H
#define OPC_SERVER_MANAGER_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"
#include <queue>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/data_access_interface/entity_access/src/OpcServerAgentEntityData.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"

#include "AbstractSimpleData.h"

namespace TA_Base_Core
{
	class AgentData;
};

namespace TA_Base_Bus
{
	class OpcDataNodeAgentCorbaDef_Impl;
	class OpcDataPointAgentCorbaDef_Impl;

	class SimplePoint;
	class SimpleNode;

	typedef std::vector<DisplayStateSequence> DisplayStateSequenceVector;
	typedef std::vector<DataNodeDisplayState> DataNodeDisplayStateVector;
};

namespace TA_Base_App
{
	class OpcDataNodeAgentProxy;
	class OpcDataPointAgentProxy;

	typedef std::map< unsigned long, std::vector<unsigned long> > AgentKeyMap;

	class OpcServerManager: public TA_Base_Core::Thread 
	{
	public:
		OpcServerManager ();
		virtual ~OpcServerManager ();
		virtual void terminate();
		virtual void run();

		void addSimpleNode(unsigned long entityKey, unsigned long agentKey);
		void addSimplePoint(unsigned long entityKey, unsigned long agentKey);

		void startSynchronise();
        void stopSynchronise();

		void createAgentProxy(std::vector<TA_Base_Core::AgentData*>&);
		void submitUpdateData(TA_Base_Bus::DisplayStateSequence* dpUpdates);
		void submitUpdateData(TA_Base_Bus::DataNodeDisplayStateVector* dnUpdates);

		unsigned long getPollFrequency() 
		{
			return m_pollPeriod;
		}

		unsigned long getRetryperiod() 
		{
			return m_retryPeriod;
		}
		//
		//Functions required by Interface - ISimplePointAgent
		//
		virtual TA_Base_Bus::SimplePoint * getSimplePoint ( unsigned long entityKey ) ;
		virtual void getAllSimplePoints (std::map < unsigned long, TA_Base_Bus::SimplePoint *>& dataPointList ) ;

		//
		//Functions required by Interface - ISimpleNodeAgent
		//
		virtual TA_Base_Bus::SimpleNode * getSimpleNode( unsigned long entityKey );
        virtual void getAllSimpleNodes (std::map < unsigned long, TA_Base_Bus::SimpleNode *>& dataNodeList );
	

	private:
		void processData(TA_Base_Bus::DisplayStateSequence* dpUpdates);
		void processData(TA_Base_Bus::DataNodeDisplayStateVector* dnUpdates);

	private:
		std::map< unsigned long, TA_Base_Bus::SimplePoint *> m_dataPoints;
		std::map< unsigned long, TA_Base_Bus::SimpleNode *> m_dataNodes;

		//Agent Key <-->Owned DataPoints
		AgentKeyMap m_dpMap;
		//Agent Key <-->Owned DataNodes 
		AgentKeyMap m_dnMap;

		TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
		
		std::queue<TA_Base_App::AbstractSimpleData*>			m_SimpleDataUpdates;
	
		volatile bool m_threadTerminated;
		TA_Base_Core::ReEntrantThreadLockable		m_queueLock;
		TA_Base_Core::Semaphore						m_queueSemaphore;
        unsigned long	m_pollPeriod; // in millisecond

        unsigned long	m_retryPeriod; // in second		//agentkey to AgentProxy.
		std::map<unsigned long, OpcDataNodeAgentProxy*>	m_dnAgentProxies;
		std::map<unsigned long, OpcDataPointAgentProxy*>	m_dpAgentProxies;
	};
}
#endif
