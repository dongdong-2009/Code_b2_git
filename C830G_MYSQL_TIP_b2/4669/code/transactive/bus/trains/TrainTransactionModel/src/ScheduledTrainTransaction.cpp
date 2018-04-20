/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/trains/TrainTransactionModel/src/ScheduledTrainTransaction.h"
#include "bus/trains/TrainTransactionModel/src/IScheduledEventProcessor.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    ScheduledTrainTransaction::ScheduledTrainTransaction( TrainCommandPtr command,
                                                          ICommandSender& commandSender,
                                                          TrainStatusManager& statusManager,
                                                          unsigned long timeout,
                                                          IScheduledEventProcessor& callback,
                                                          bool recurring,
                                                          bool ignoreValidity,
                                                          const std::string& radioId )
            : NonBlockingTrainTransaction( command,
                                           commandSender,
                                           statusManager,
                                           timeout,
                                           0,
                                           ignoreValidity,
                                           radioId ),
              m_callback( callback ),
              m_recurring( recurring ),
              m_timerId( 0 ),
              m_queued( false )
    {
        FUNCTION_ENTRY( "ScheduledTrainTransaction" );
        FUNCTION_EXIT;
    }


    ScheduledTrainTransaction::~ScheduledTrainTransaction()
    {
        FUNCTION_ENTRY( "~ScheduledTrainTransaction" );
        FUNCTION_EXIT;
    }


    void ScheduledTrainTransaction::setTimerId(long timerId)
    {
        FUNCTION_ENTRY( "setTimerId" );
        
        m_timerId = timerId;
        
        FUNCTION_EXIT;
    }


    long ScheduledTrainTransaction::getTimerId() const 
    {
        FUNCTION_ENTRY( "getTimerId" );
        FUNCTION_EXIT;
    	return m_timerId;
    }


    void ScheduledTrainTransaction::setQueued()
    {
        FUNCTION_ENTRY( "setQueued" );
        
        m_queued = true;
        
        FUNCTION_EXIT;
    }


    bool ScheduledTrainTransaction::isQueued() const 
    {
        FUNCTION_ENTRY( "isQueued" );
        FUNCTION_EXIT;
    	return m_queued;
    }


    bool ScheduledTrainTransaction::isRecurring() const 
    {
        FUNCTION_ENTRY( "isRecurring" );
        FUNCTION_EXIT;
    	return m_recurring;
    }


    void ScheduledTrainTransaction::cancelRecurrence() 
    {
        FUNCTION_ENTRY( "cancelRecurrence" );
        
        m_recurring = false;
        
        FUNCTION_EXIT;
    }


    void ScheduledTrainTransaction::notifyObserversOfCompletion()
    {
        FUNCTION_ENTRY( "notifyObserversOfCompletion" );

        // the result should be handled before this command is considered as complete
        if ( NoError == getStatus() )
        {
            m_callback.receiveResponse( m_timerId, m_command );
        }
        else
        {
            m_callback.receiveErrorResponse(  m_timerId, m_command, getStatus() );
        }

        // this is now complete, it is free to be re-queued if required
        m_queued = false;

        FUNCTION_EXIT;
    }
}
