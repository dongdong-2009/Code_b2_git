/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/IStateValidationItem.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
    const unsigned long StateValidationCycle::TERMINATE_DELAY = 10;

    //
    // StateValidationCycle
    //
    StateValidationCycle::StateValidationCycle()
        : m_doTerminate( false ), 
          m_lowPriorityPeriod( 0 ), 
          m_highPriorityPeriod( 0 ),
          m_lowPriorityTimer(),
          m_highPriorityTimer()
    {
    }


    //
    // ~StateValidationCycle
    //
    StateValidationCycle::~StateValidationCycle()
    {
        // Ensure cycle has been stopped.
        stopValidationCycle();
    }


    //
    // startValidationCycle
    //
    void StateValidationCycle::startValidationCycle()
    {
        TA_Base_Core::ThreadGuard stateGuard( m_threadStateLock );

        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN == getCurrentState() )
        {
            start();
        }
    }


    //
    // stopValidationCycle
    //
    void StateValidationCycle::stopValidationCycle()
    {
        TA_Base_Core::ThreadGuard stateGuard( m_threadStateLock );

        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() )
        {
            terminateAndWait();
        }

        m_doTerminate = false;
    }


    //
    // setValidationCyclePeriods
    //
    void StateValidationCycle::setValidationCyclePeriods( unsigned long lowPriority, unsigned long highPriority )
    {
        FUNCTION_ENTRY( "setValidationCyclePeriods" );

        TA_Base_Core::ThreadGuard stateGuard( m_threadStateLock );

        // If it is currently running we stop, then modify periods, and restart.
        // else, just modify the periods. This ensures thread safety without using
        // having to lock access to the periods themselves (and hence acquire locks
        // during main run method).
        bool isCurrentlyRunning = ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() );

        if ( isCurrentlyRunning )
        {
            stopValidationCycle();
        }

        m_lowPriorityPeriod = lowPriority;
        m_highPriorityPeriod = highPriority;

        if ( isCurrentlyRunning )
        {
            startValidationCycle();
        }

        FUNCTION_EXIT;
    }


    //
    // addValidationItem
    //
    void StateValidationCycle::addValidationItem( IStateValidationItem& item, bool isHighPriorityItem  )
    {
        FUNCTION_ENTRY( "addValidationItem" );

        TA_Base_Core::ThreadGuard stateGuard( m_threadStateLock );

        // If it is currently running we stop, then add item, and restart.
        // Else, just add it. When the cycle is restarted it will ensure 
        // the lists are unique so no item can be updated multiple times 
        // on same cycle.
        bool isCurrentlyRunning = ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() );

        if ( isCurrentlyRunning )
        {
            stopValidationCycle();
        }

        if ( isHighPriorityItem )
        {
            m_highPriorityItems.push_back( &item );
        }
        else
        {
            m_lowPriorityItems.push_back( &item );
        }

        if ( isCurrentlyRunning )
        {
            startValidationCycle();
        }

        FUNCTION_EXIT;        
    }


    // 
    // removeValidationItem
    //
    void StateValidationCycle::removeValidationItem( IStateValidationItem& item, bool isHighPriorityItem )
    {
        FUNCTION_ENTRY( "addValidationItem" );

        TA_Base_Core::ThreadGuard stateGuard( m_threadStateLock );

        // If it is currently running we stop, then remove item, and restart.
        // Else, just remove it.
        bool isCurrentlyRunning = ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() );

        if ( isCurrentlyRunning )
        {
            stopValidationCycle();
        }

        // Note - std::list::remove is not a potential memory leak,
        // so no need to use erase(remove) idiom as with std::algorithm::remove
        // (refer to "Effective C++" <Meyers> Item #32).
        if ( isHighPriorityItem )
        {
            m_highPriorityItems.remove( &item );
        }
        else
        {
            m_lowPriorityItems.remove( &item );
        }

        if ( isCurrentlyRunning )
        {
            startValidationCycle();
        }

        FUNCTION_EXIT; 
    }


    //
    // run
    //
    void StateValidationCycle::run()
    {
        try
        {
		    FUNCTION_ENTRY( "run" );

            // Ensure there are no duplicate
            // validation items in lists.
            m_highPriorityItems.sort();
            m_lowPriorityItems.sort();
            m_highPriorityItems.unique();
            m_lowPriorityItems.unique();

            // Perform initial cycle (don't wait for first period as want
            // to determine initial state as soon as agent connects to hardware).
            validateHighPriorityItems();
            validateLowPriorityItems();

            while ( false == m_doTerminate )
            {
                if ( hasHighPriorityPeriodExpired() )
                {
                    validateHighPriorityItems();
                }

                if ( hasLowPriorityPeriodExpired() )
                {
                    validateLowPriorityItems();
                }

                unsigned waitPeriod = calculateWaitPeriod();

                // Acquire linked lock before checking terminate flag to ensure access to it
                // is mutually exclusive to setting it and broadcasting condition.
                TA_Base_Core::ThreadGuard cycleConditionGuard( m_cycleCondition.getLinkedThreadLockable() );

                if ( false == m_doTerminate )
                {
                    // Linked lockable intrinsically released immediately before wait.
                    m_cycleCondition.timedWait( waitPeriod );
                    // Linked lockable intrinsically reacquired immediately after wait.
                }
            }

		    FUNCTION_EXIT;
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }


    }


    //
    // terminate
    //
	void StateValidationCycle::terminate()
	{
        // Acquire linked lock before setting terminate flag and broadcasting to ensure thread is not
        // currently checking flag and entering wait simultaneously.
        TA_Base_Core::ThreadGuard cycleConditionGuard( m_cycleCondition.getLinkedThreadLockable() );

		m_doTerminate = true;

        // Now wake up immediately if in timedWait( <waitperiod> ).
        m_cycleCondition.broadcast();
	}


    //
    // validateLowPriorityItems
    //
    void StateValidationCycle::validateLowPriorityItems()
    {
        std::list< IStateValidationItem* >::iterator itemIt = m_lowPriorityItems.begin();
        while ( ( false == m_doTerminate ) && ( itemIt != m_lowPriorityItems.end() ) )
        {
            ( *itemIt )->validateState();
            itemIt++;
        }

		COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventCommand());
		COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, false);
		
        m_lowPriorityTimer.restart();
    }


    // 
    // validateHighPriorityItems
    //
    void StateValidationCycle::validateHighPriorityItems()
    {
        std::list< IStateValidationItem* >::iterator itemIt = m_highPriorityItems.begin();
        while ( ( false == m_doTerminate ) && ( itemIt != m_highPriorityItems.end() ) )
        {
            ( *itemIt )->validateState();
            itemIt++;
        }

        m_highPriorityTimer.restart();
    }


    //
    // calculateWaitPeriod
    //
    unsigned long StateValidationCycle::calculateWaitPeriod()
    {
        unsigned long lowPriorityElapsed = static_cast< unsigned long >( m_lowPriorityTimer.elapsedTime() * 1000 );
        unsigned long highPriorityElapsed = static_cast< unsigned long >( m_highPriorityTimer.elapsedTime() * 1000 );
        unsigned long returnVal( 0 );
        if ( lowPriorityElapsed < m_lowPriorityPeriod )
        {
            returnVal = ( m_lowPriorityPeriod - lowPriorityElapsed );
        }
        else
        {
            return 0;
        }

        if ( highPriorityElapsed < m_highPriorityPeriod )
        {
            unsigned long timeTillHigh = ( m_highPriorityPeriod - highPriorityElapsed );
            if ( timeTillHigh < returnVal )
            {
                returnVal = timeTillHigh;
            }
        }
        else
        {
            return 0;
        }

        return returnVal;
    }


    //
    // hasLowPriorityPeriodExpired
    //
    bool StateValidationCycle::hasLowPriorityPeriodExpired()
    {
        return ( m_lowPriorityPeriod <= static_cast< unsigned long >( m_lowPriorityTimer.elapsedTime() * 1000 ) );
    }


    //
    // hasHighPriorityPeriodExpired
    //
    bool StateValidationCycle::hasHighPriorityPeriodExpired()
    {
        return ( m_highPriorityPeriod <= static_cast< unsigned long >( m_highPriorityTimer.elapsedTime() * 1000 ) );
    }

} // namespace TA_IRS_App
