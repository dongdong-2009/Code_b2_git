/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentSynHandler.h $
  * @author  Wu Mintao
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *	DataPointAgentCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */

#ifndef DATAPOINT_AGENT_SYN_HANDLER_H
#define DATAPOINT_AGENT_SYN_HANDLER_H

#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/synchronisation/src/Condition.h"
#include "core/naming/src/NamedObject.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"

namespace TA_Base_Bus
{
    
	//TD15618, use timer to trigger if the observers are timeout
    class DataPointAgentSynHandler :public virtual TA_Base_Core::Thread, 
                                    public virtual TA_Base_Core::ITimeoutCallback
	{
	public:


        DataPointAgentSynHandler (std::string& agentname, TA_Base_Bus::IDataPointAgent * specificDataPointAgent );


		virtual ~DataPointAgentSynHandler();

        virtual void run();
        virtual void terminate();
        virtual void timerExpired(long timerId, void* userData);
        virtual void stopSynchronize();
        virtual void startSynchronize();


    private:
        volatile bool m_toBeTerminated;
        volatile bool m_isSynchronizing;
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataPointAgentCorbaDef,
                                           TA_Base_Bus::IDataPointAgentCorbaDef_ptr,
                                           TA_Base_Bus::IDataPointAgentCorbaDef_var >   DataPointAgentInterface;	
        
        DataPointAgentInterface * m_agentInterface;
		std::map < unsigned long, DataPoint * > m_listOfLocalDataPoints;
        unsigned long m_observerId;        
        int m_corbaErrorPollSkipCount;
        TA_Base_Core::Condition					m_timerCondition;
        TA_Base_Core::SingletonTimerUtil&		m_timerUtility;
        static const unsigned short BULK_POLL_AGENT_SYN_PERIOD_MSECS;
        static const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS;
	};
}

#endif

