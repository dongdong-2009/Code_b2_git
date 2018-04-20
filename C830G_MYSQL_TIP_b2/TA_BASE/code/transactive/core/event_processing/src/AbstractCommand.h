/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/event_processing/src/AbstractCommand.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * AbstractCommand implements functionality that is required of all scheduled commands,
  * namely the ability to retrieve the priority of the command, the delay before executing the command,
  * and the time at which the command was added to the scheduler.
  *
  * It also defines two function objects used to decide the order of the command objects in the
  * priority queues.
  *
  */

#if !defined(_ABSTRACT_SCHEDULED_COMMAND_H_)
#define _ABSTRACT_SCHEDULED_COMMAND_H_

#include <functional>
#include <time.h>

#include "ace/Time_Value.h"

namespace TA_Base_Core
{
    // Forward declaration.
    class CommandScheduler;

    class AbstractCommand
    {
    public:
        /**
         * Constructor
         *
         * Constructs the command with the specified priority.
         *
         * @param delay The amount of time to wait, in seconds, before executing the command.
         */
        AbstractCommand( const unsigned long delay );


        /**
         * Destructor
         */
        virtual ~AbstractCommand();


        /**
         * getDelay
         *
         * Returns the number of milliseconds to wait before executing the command.
         *
         * @return unsigned long The number of milliseconds to wait.
         */
        virtual unsigned long getDelay() const;


        /**
         * getScheduledTime
         *
         * Gets the time at which the command should be executed.
         *
         * @return ACE_Time_Value The time at which the command should be executed.
         */
         virtual const ACE_Time_Value& getScheduledTime() const;


        /**
         * execute
         *
         * Performs whatever processing is required of the command object.
         *
         * @param scheduler The scheduler that is executing the command.
         */
        virtual void execute( CommandScheduler& scheduler ) =0;


        /**
         * operator <
         *
         * Used by the priority_queue to determine the next command to execute.
         */
        bool operator<( const AbstractCommand* rhs );
        bool operator<( const AbstractCommand& rhs );


        /**
         * operator >
         *
         * Used by the priority_queue to determine the next command to execute.
         */
        bool operator>( const AbstractCommand* rhs );
        bool operator>( const AbstractCommand& rhs );

    private:
        // Hidden methods.
        AbstractCommand();
        AbstractCommand( const AbstractCommand& );
        const AbstractCommand& operator=( const AbstractCommand& );


        // The number of milliseconds to wait before executing the command.
        unsigned long m_delay;

        // The time at which the command will be executed.
        ACE_Time_Value m_scheduledTime;
    };
};

#endif // !defined(_ABSTRACT_SCHEDULED_COMMAND_H_)
