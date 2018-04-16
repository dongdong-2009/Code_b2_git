/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef TRIGGEREDBROADCASTWORKER__INCLUDED_
#define TRIGGEREDBROADCASTWORKER__INCLUDED_

#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

namespace TA_IRS_App
{
	class TriggeredDVABroadcast;

	class TriggeredBroadcastWorker : public ACE_Event_Handler
	{
	public:
	    TriggeredBroadcastWorker();
		virtual ~TriggeredBroadcastWorker();

	    // ACE_Event_Handler interface
	    virtual int handle_timeout( const ACE_Time_Value& current_time, const void* act = 0 );

	    void addSigTriggeredBroadcast( time_t tmStartTime,
	        const std::string& strBroadcastId, unsigned long ulDvaMsgKey, 
	        const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes = 0 );

		void executeSigTriggeredBroadcast( unsigned long ulSigEventType, unsigned long ulPlatFormType);
	    void executeFasTriggeredBroadcast( const bool bOnFire );
	    void executeFasTriggeredBroadcast( const std::string& strBroadcastId, unsigned long ulDvaMsgKey, 
	        const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes );

	    void setToControl();
	    void setToMonitor();

	private:
	    ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> m_thdTriggeredBroadcastTimer;
	    bool m_bIsControl;
	};
}

#endif // ifndef (TRIGGEREDBROADCASTWORKER__INCLUDED_)
