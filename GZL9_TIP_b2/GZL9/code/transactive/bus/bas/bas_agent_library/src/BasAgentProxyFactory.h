/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/GZL9/transactive/bus/bas/bas_agent_library/src/BasAgentProxyFactory.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/02/27 17:12:34 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the bas agent.
  */

#if !defined(BAS_AGENT_PROXY_FACTORY_H)
#define BAS_AGENT_PROXY_FACTORY_H

#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"
#include "ace/Null_Mutex.h"
#include "ace/Singleton.h"

namespace TA_IRS_Bus
{
	class BasAgentProxyFactory
	{
		friend class ACE_Singleton<BasAgentProxyFactory, ACE_Null_Mutex>;
	public:
		typedef std::map<unsigned long, BasAgentProxySmartPtr>::iterator  BasAgentProxyMapIterator;
		virtual ~BasAgentProxyFactory();
		BasAgentProxySmartPtr& getBasAgentProxyByLocationKey(long locationKey);

	private:
		BasAgentProxyFactory();
		BasAgentProxyFactory(const BasAgentProxyFactory&);
		BasAgentProxyFactory& operator=(const BasAgentProxyFactory&);

		std::string getAgentEntityName(unsigned long locationKey);
		void getEntityDataList(unsigned long locationKey, bool loadParameter, TA_Base_Core::IEntityDataList& dataList);

		std::map<unsigned long, BasAgentProxySmartPtr> m_basAgentProxyMap;
		TA_Base_Core::NonReEntrantThreadLockable m_agentMapLock;
	};
	
	typedef ACE_Singleton<TA_IRS_Bus::BasAgentProxyFactory, ACE_Null_Mutex> BasAgentProxyFactoryInstance;
}
#endif