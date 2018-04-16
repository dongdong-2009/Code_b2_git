/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OpcDataNodeAgentProxy.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/12 15:49:47 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// OpcDataNodeAgentProxy.h: interface for the OpcDataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef OPC_DATANODE_AGENT_PROXY_H
#define OPC_DATANODE_AGENT_PROXY_H

#include <set>
#include <string>
#include <sys/timeb.h>

#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_App
{
 typedef enum
    {
        CORBA_EXCEPTION = 0,
        OBJECT_RESOLUTION_EXCEPTION = 1,
        BAD_PARAMETER_EXCEPTION = 2,
        OPERATION_MODE_EXCEPTION = 3,
        SCADA_PROXY_EXCEPTION = 4,
        UNKNOWN_EXCEPTION = 5
    } ExceptionType;


	class OpcServerManager;

	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataNodeAgentCorbaDef,
		TA_Base_Bus::IDataNodeAgentCorbaDef_ptr,
		TA_Base_Bus::IDataNodeAgentCorbaDef_var >   DataNodeAgentInterface;

    class OpcDataNodeAgentProxy : public TA_Base_Core::Thread
    {
    public:
	    OpcDataNodeAgentProxy(unsigned long agentKey, std::vector<unsigned long> dnMap, OpcServerManager* pManager);

	    virtual ~OpcDataNodeAgentProxy();

        virtual void run();
        
        virtual void terminate();
        
        void startSynchronise();

        void stopSynchronise();

        bool isSynchronised();

		void processUpdates(TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dnUpdates);

		void prepareAgentThreadEnd();
		void setAllToBadQuality(TA_Base_Bus::EQualityStatus eq = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);

    private:
        volatile bool	m_toBeTerminated;

        bool			m_isSynchronising;

		bool			m_isPrevSynchronising;

        unsigned long	m_observerId;
		OpcServerManager* m_pOpcServerManager;
        DataNodeAgentInterface * m_agentDNInterface;
        std::vector<unsigned long> m_dnList;

		unsigned long   m_agentKey;
        std::string		m_agentName;

        unsigned long	m_pollPeriod; // in millisecond

        unsigned long	m_retryPeriod; // in second

        bool			m_connected;
		bool			m_isAllBadQuality;
        time_t			m_timeLastConnected;

		TA_Base_Core::Condition m_conditionLoop;
        
    };
}
#endif 
