/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcDataPointAgentProxy.h $
  * @author:  Dhanshri Mokashi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2016/06/27 09:30:19 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// OpcDataPointAgentProxy.h: interface for the OpcDataPointAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef OPC_DATAPOINT_AGENT_PROXY_H
#define OPC_DATAPOINT_AGENT_PROXY_H

#include <string>
#include <sys/timeb.h>

#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"
#include "core/synchronisation/src/Condition.h"
#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "bus/scada/common_library/src/CommonDefs.h"

using namespace TA_Base_Bus;

namespace TA_Base_App
{
	class OpcServerManager;

	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataPointAgentCorbaDef,
		TA_Base_Bus::IDataPointAgentCorbaDef_ptr,
		TA_Base_Bus::IDataPointAgentCorbaDef_var >   DataPointAgentInterface;

    class OpcDataPointAgentProxy : public TA_Base_Core::Thread
    {
    public:
	    OpcDataPointAgentProxy(unsigned long agentKey, std::vector<unsigned long> dpMap, OpcServerManager* pointMgr);

	    virtual ~OpcDataPointAgentProxy();

        virtual void run();
        
        virtual void terminate();
        
        void startSynchronise();
        void stopSynchronise();

        bool isSynchronised();

		void processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates);
		void setAllToBadQuality(TA_Base_Bus::EQualityStatus eq = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
		void prepareAgentThreadEnd();

	private:

        volatile bool	m_toBeTerminated;
        bool	m_isSynchronising;
		bool	m_isPrevSynchronising;
		bool	m_isAllBadQuality;
        unsigned long	m_observerId;
        DataPointAgentInterface *	m_agentDPInterface;
		std::vector<unsigned long> m_dpList;
		unsigned long   m_agentKey;
        std::string		m_agentName;

        unsigned long m_pollPeriod; // in millisecond

        unsigned long m_retryPeriod; // in second

        bool m_connected;

        TA_Base_Core::Condition m_conditionLoop;

		OpcServerManager* m_pOpcServerManager;
    };
}
#endif
