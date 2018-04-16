/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/IscsAliveWorker.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//
//  IscsAliveWorker.h
//  Implementation of the Class IscsAliveWorker
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(IscsAliveWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define IscsAliveWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_


#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/IAliveDataPointAccess.h"

#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"

#include "core/naming/src/NamedObject.h"

#include "bus/scada/proxy_library/src/IDataPointAccess.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"


#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"	// TD19822
#include "app/ecs/ECSAgent/src/EcsGlobals.h"				// TD19822
class ItaEcsCachedConfig;
class ItaEcsDatapoints;
class IAliveDataPointAccess;

//class IDataPointAccess;
namespace TA_Base_Bus
{
	class DataPointProxySmartPtr;
}


//TD4303, to use timer utility instead of use sleep
class IscsAliveWorker : public TA_Base_Core::Thread,
                        public virtual AbstractFailoverable,
                        public TA_Base_Core::ITimeoutCallback,
						public IAliveDataPointAccess
						//public TA_Base_Bus::IDataPointAccess
{

public:
    IscsAliveWorker(ItaEcsCachedConfig& theConfig, ItaEcsDatapoints& theDatapoints);

    //Implement for ITimeoutCallback;
    void timerExpired(long timerId, void* userData);

	/**
	 *
	 * WriteDataPointValueWithoutLoggingEvent
	 *
	 * Change the value associated with the point
	 * (audit message NOT to be logged)
	 *
	 */
    virtual void writeDataPointValueWithoutLoggingEvent ( const std::string& sessionId, unsigned long value, unsigned long triggeringEntityKey, const char * triggeringEntityValue ); 

    virtual ~IscsAliveWorker();

protected:

    virtual void run();
    virtual void terminate()
    {
        m_isRunning = false;
    }
    void process();

	/**
	 *
	 *  TD19822 - Need to check both RTU1 and RTU2`s polling status, if all are bad quality, then 
	 *	no need to send ISCS alive message, if any one of them is available, then do the wirte thing
	 *
	 */
	bool checkBothRTUPollingStatus();

    
    volatile bool                                   m_isRunning;
    ItaEcsCachedConfig&                             m_theConfig;
	ItaEcsDatapoints&                               m_theDatapoints;
    
    TA_Base_Core::SingletonTimerUtil&		        m_timerUtility;

private:

    IscsAliveWorker( const IscsAliveWorker& theIscsAliveWorker);
	void initProxy();

private:
	typedef TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef, 
                                            TA_Base_Bus::IDataPointAccessCorbaDef_ptr, 
                                            TA_Base_Bus::IDataPointAccessCorbaDef_var> DpAccessInterface;

    DpAccessInterface*						m_dpAccessInterface;
	unsigned long							m_entityKey;

	TA_Base_Bus::ScadaProxyFactory*			m_proxyFactory;				// TD19822
	NullEventProcessor                      m_nullEventProcessor;		// TD19822

	//xinsong++
	TA_Base_Bus::DataPointProxySmartPtr m_rtu1proxy;
	TA_Base_Bus::DataPointProxySmartPtr m_rtu2proxy;

};

#endif // !defined(IscsAliveWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
