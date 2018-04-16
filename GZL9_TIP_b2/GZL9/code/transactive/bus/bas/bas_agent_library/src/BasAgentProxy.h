/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/GZL9/transactive/bus/bas/bas_agent_library/src/BasAgentProxy.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/02/27 17:12:34 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the bas agent.
  */

#if !defined(BAS_AGENT_PROXY_H)
#define BAS_AGENT_PROXY_H

#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"

#include "core/naming/src/NamedObject.h"

#include "boost/smart_ptr.hpp"

namespace TA_IRS_Bus
{
	class BasAgentProxy
	{
	public:
		typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IStationBasAgentCorbaDef,
			TA_IRS_Bus::IStationBasAgentCorbaDef_ptr,
			TA_IRS_Bus::IStationBasAgentCorbaDef_var> BASAgentObject;
		BasAgentProxy(const std::string& entityName, const bool isAgentEntity);
		virtual ~BasAgentProxy();
		void executeStationMode(unsigned long subsystemKey, unsigned long stationMode, bool canOverrideStationMode, const std::string& sessionId);
		void executeTimeTable(unsigned long tableKey, const std::string& sessionId, bool isTodayTimeTable);

	private:
		std::auto_ptr<BASAgentObject> m_basAgentPtr;
	};

	typedef boost::shared_ptr<BasAgentProxy> BasAgentProxySmartPtr;
}

#endif