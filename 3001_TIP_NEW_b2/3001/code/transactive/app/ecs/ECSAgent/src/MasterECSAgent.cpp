/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/MasterECSAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/03/26 11:00:59 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER


#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMapsInitialiser.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfigInitialiser.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsComms.h"
#include "app/ecs/ECSAgent/src/ItaEcsSynchronisation.h"
#include "app/ecs/ECSAgent/src/ItaEcsAlarms.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentSummary.h"
#include "app/ecs/ECSAgent/src/MasterECSAgent.h"
#include "app/ecs/ECSAgent/src/EmergencyProposalEngine.h"
#include "app/ecs/ECSAgent/src/CongestionProposalEngine.h"
#include "app/ecs/ECSAgent/src/MasterModeManager.h"
#include "app/ecs/ECSAgent/src/EmergencyNormalisationMonitor.h"

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#define TA_THREADGUARD_TEST(threadlockable) TA_NAMED_THREADGUARD(threadlockable, "ThreadGuard")

//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
        typedef void ( T::* MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
    public:
        
        static ThreadedMemberFunction& alloc( T& o, MemberFunction mf = NULL )
        {
			TA_Base_Core::ThreadGuard guardName(lock());
            
            static ThreadedMemberFunction* instance = NULL;
            
            instance = new ThreadedMemberFunction( o, instance );
            
            if ( NULL != mf )
            {
                instance->add( mf );
                
                instance->start();
            }
            
            return *instance;
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
    protected:
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member function(s) to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ThreadedMemberFunction: calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            clearPrivious();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o, ThreadedMemberFunction* privious )
            : m_object( o ),
            m_privious( privious )
        {
        }
        
        ThreadedMemberFunction( const ThreadedMemberFunction& );
        ThreadedMemberFunction& operator= ( const ThreadedMemberFunction& );
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        void clearPrivious()
        {
            delete m_privious;
            m_privious = NULL;
        }
        
        static TA_Base_Core::NonReEntrantThreadLockable& lock()
        {
            static TA_Base_Core::NonReEntrantThreadLockable lock;
            return lock;
        }
        
    private:
        
        T& m_object;
        MemberFunctionList m_members;
        ThreadedMemberFunction* m_privious;
    };
    
    typedef ThreadedMemberFunction< MasterECSAgent > MasterECSAgentThreadedMemberFunction;
}
//limin++, failover


using TA_Base_Bus::IEntity;

//
// Constructor
//
MasterECSAgent::MasterECSAgent(
    int argc, 
    char* argv[],
    ItaEcsAgentSummary& theSummary)
: AbstractECSAgent(theSummary),
  m_emergencyProposalEngine(NULL),
  m_congestionProposalEngine(NULL),
  m_masterModeManager(NULL),
  m_emergencyNormalisationMonitor(NULL)
{
	FUNCTION_ENTRY("Constructor");

    setup(argc, argv);

	FUNCTION_EXIT;
}


//
// Destructor
//
MasterECSAgent::~MasterECSAgent()
{
	FUNCTION_ENTRY("Destructor");

    cleanup();

	FUNCTION_EXIT;
}

// ExceptionChecked
void MasterECSAgent::setupSpecifics()
    //throw(TA_Core::GenericAgentException, TA_Base_Core::InvalidECSConfigurationException, TA_Base_Core::CachedMappingNotFoundException)
{
    FUNCTION_ENTRY("setupSpecifics");

    AbstractECSAgent::setupSpecifics();

    if (!m_emergencyProposalEngine)
    {
        m_emergencyProposalEngine = new EmergencyProposalEngine(
            m_theSummary.getItaEcsCachedMaps(),
            m_theSummary.getItaEcsCachedConfig(),
            m_theSummary.getItaEcsAgentCompleteState(),
            m_theSummary.getItaEcsDatapoints());
    }

    if (!m_congestionProposalEngine)
    {
        m_congestionProposalEngine = new CongestionProposalEngine(
            m_theSummary.getItaEcsCachedConfig(),
            m_theSummary.getItaEcsCachedMaps(),
            m_theSummary.getItaEcsDatapoints(),
            m_theSummary.getItaEcsAgentCompleteState());
    }

    if (!m_emergencyNormalisationMonitor)
    {
        m_emergencyNormalisationMonitor = new EmergencyNormalisationMonitor(
            m_theSummary.getItaEcsDatapoints(),
			m_theSummary.getItaEcsAlarms());
    }

    if (!m_masterModeManager)
    {
        m_masterModeManager = new MasterModeManager( 
            m_theSummary.getItaEcsCachedMaps(),
            m_theSummary.getItaEcsCachedConfig(),
            m_theSummary.getItaEcsAuditing(),
            m_theSummary.getItaEcsAlarms(),
            m_theSummary.getItaEcsAgentCompleteState(),
			m_theSummary.getItaEcsDatapoints());
    }
    FUNCTION_EXIT;
}

// ExceptionChecked
void MasterECSAgent::cleanupSpecifics()
{
    FUNCTION_ENTRY("cleanupSpecifics");

    if(m_masterModeManager)
    {
        delete m_masterModeManager;
        m_masterModeManager = NULL;
    }

    if(m_emergencyNormalisationMonitor)
    {
        delete m_emergencyNormalisationMonitor;
        m_emergencyNormalisationMonitor = NULL;
    }

    if(m_congestionProposalEngine)
    {
        delete m_congestionProposalEngine;
        m_congestionProposalEngine = NULL;
    }

    if(m_emergencyProposalEngine)
    {
        delete m_emergencyProposalEngine;
        m_emergencyProposalEngine = NULL;
    }

	unsubscribeForAlarmUpdates();

    FUNCTION_EXIT;
}
void MasterECSAgent::stopSpecificThreads()
{ 
    m_masterModeManager->terminateAndWait();
}

// liuyu++ start master/station specific threads
void MasterECSAgent::startSpecificThreads()
{
    m_masterModeManager->start();
}
// ++liuyu start master/station specific threads


// ExceptionChecked
void MasterECSAgent::processSpecificConfiguration()
    //throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException)
{
    FUNCTION_ENTRY("processSpecificConfiguration");


    FUNCTION_EXIT;
}


// ExceptionChecked
void MasterECSAgent::validateSpecificRuntimeConfiguration()
    //throw(TA_Base_Core::InvalidECSConfigurationException)
{
    FUNCTION_ENTRY("validateSpecificRuntimeConfiguration");

	FUNCTION_EXIT;
}

void MasterECSAgent::agentSetControlSpecifics()
{
    FUNCTION_ENTRY("agentSetControlSpecifics");

    //limin++--, failover
    MasterECSAgentThreadedMemberFunction& thrd = MasterECSAgentThreadedMemberFunction::alloc( *this );
    thrd.add( &MasterECSAgent::agentSetControlSpecificsInThread );
    thrd.add( &MasterECSAgent::subscribeForAlarmUpdates );
    thrd.start();
    //m_emergencyProposalEngine->setControl(m_isOnStartup);
    //m_congestionProposalEngine->setControl(m_isOnStartup);

    //m_emergencyNormalisationMonitor->setControl(m_isOnStartup);
    //m_masterModeManager->setControl(m_isOnStartup);

	//subscribeForAlarmUpdates();
    //limin++--, failover

	FUNCTION_EXIT;
}


//limin, for failover
void MasterECSAgent::agentSetControlSpecificsInThread()
{
    FUNCTION_ENTRY( "agentSetControlSpecificsInThread" );

    m_emergencyProposalEngine->setControl(m_isOnStartup);
    m_congestionProposalEngine->setControl(m_isOnStartup);
    
    m_emergencyNormalisationMonitor->setControl(m_isOnStartup);
    m_masterModeManager->setControl(m_isOnStartup);

    FUNCTION_EXIT;
}


void MasterECSAgent::agentSetMonitorSpecifics()
{
    FUNCTION_ENTRY("agentSetMonitorSpecifics");
    
    //stopSpecificThreads(); //Moved to AbstractEcsAgent
    m_masterModeManager->setMonitor(m_isOnStartup);
    m_emergencyNormalisationMonitor->setMonitor(m_isOnStartup);

    m_congestionProposalEngine->setMonitor(m_isOnStartup);
    m_emergencyProposalEngine->setMonitor(m_isOnStartup);

    //limin++--, failover
    MasterECSAgentThreadedMemberFunction& thrd = MasterECSAgentThreadedMemberFunction::alloc( *this );
    thrd.add( &MasterECSAgent::unsubscribeForAlarmUpdates );
    thrd.start();
	//unsubscribeForAlarmUpdates();
    //limin++--, failover

	FUNCTION_EXIT;

}

void MasterECSAgent::notifyGroupOffline( const std::string& group )
{
    FUNCTION_ENTRY("notifyGroupOffline");
    // nothing to do
    FUNCTION_EXIT;
}

void MasterECSAgent::notifyGroupOnline( const std::string& group )
{
    FUNCTION_ENTRY("notifyGroupOnline");
    // nothing to do
    FUNCTION_EXIT;
}


//
// Remove emegency proposal when emergency alarm closed
//

void MasterECSAgent::subscribeForAlarmUpdates()
{
	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(dynamic_cast<TA_Base_Core::AlarmReceiver*>(this));

	unsigned long location = m_theSummary.getItaEcsCachedConfig().getAgentLocationKey();

	TA_Base_Core::AlarmSubscriptionManager::subscribeToAlarmsFromLocation(
				TA_Base_Core::DataPointAlarms::DpTrue, // All ECSalarms
				dynamic_cast<TA_Base_Core::AlarmReceiver*>(this),
				m_theSummary.getItaEcsCachedConfig().getAgentLocationKey()); // specified location
}


void MasterECSAgent::unsubscribeForAlarmUpdates()
{
	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(dynamic_cast<TA_Base_Core::AlarmReceiver*>(this));
}


void MasterECSAgent::receiveAlarmMessage(const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage)
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receiveAlarmMessage");

	switch ( alarmMessage._d() )
	{
		// Only care about updates.
		case TA_Base_Core::UpdateAlarm: // Updated alarm
			try
            {
	            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "receive AlarmUpdate Message");
				if(alarmMessage.update().alarmHasBeenClosed)
				{
					std::map<unsigned long, unsigned long>::iterator iter;
					std::map<unsigned long, unsigned long> emergencyKeyToZoneId;
					emergencyKeyToZoneId = m_theSummary.getItaEcsCachedMaps().getEmergencyEntityKeyToZoneId();
					iter = emergencyKeyToZoneId.find(alarmMessage.update().assocEntityKey);
					if (iter != emergencyKeyToZoneId.end())
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"Alarm close message: Entity %d is found in zone %d", 
						alarmMessage.update().assocEntityKey, iter->second);

						m_ecsZoneDataSource->removeEmergencyIfAlarmManuallyClosed(iter->second);
					}
				}	
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				 "Exception handling update alarm notification.");
			}
			break;
	}

}
