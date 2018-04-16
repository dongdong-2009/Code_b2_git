/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/TimsModeTasks.h"
#include "app/trains/train_agent/src/ITimsModeManagerTaskCallback.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>

namespace TA_IRS_App
{

    ChangeModeChangeAreaTask::ChangeModeChangeAreaTask( ITimsModeManagerTaskCallback& callback,
                                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                        ETaskType taskType )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_taskType( taskType ),
              m_cancelled( false )

    {
        FUNCTION_ENTRY( "ChangeModeChangeAreaTask" );
        FUNCTION_EXIT;
    }


    ChangeModeChangeAreaTask::~ChangeModeChangeAreaTask()
    {
        FUNCTION_ENTRY( "~ChangeModeChangeAreaTask" );
        FUNCTION_EXIT;
    }


    void ChangeModeChangeAreaTask::setCancelled()
    {
        FUNCTION_ENTRY( "setCancelled" );

        m_cancelled = true;

        FUNCTION_EXIT;
    }


    void ChangeModeChangeAreaTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        EChangeModeCommandResult result = Success;
        std::string details = "";

        // if the task has been cancelled
        if ( true == m_cancelled )
        {
            // log a message indicating the task was cancelled before it started
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Change mode task for train %d was cancelled before it started",
                         m_trainId );

            result = TransactionCancelledError;
            std::ostringstream temp;
            temp << "The change mode for train "
            << std::setw( 2 ) << std::setfill( '0' ) << static_cast< unsigned int >( m_trainId )
            << "has been cancelled";

            details = temp.str();
        }

        // otherwise the task has not been cancelled
        else
        {
            // execute the task, populating the result and the details string
            
            switch( m_taskType )
            {
                case ChangeModeTask:
                    result = m_callback.performChangeMode( m_trainId, details );
                    break;

                case ChangeAreaTask:
                    result = m_callback.performChangeArea( m_trainId, details );
                    break;

                default:
                    TA_ASSERT( false, "Invalid task type" );
					// fall through
            }
        }

        // notify all observers of the task result:
        // get all observers
        ObserverList observers = getObservers();

        // for each observer signal the completion with the result and details string
        for ( ObserverList::iterator observerIter = observers.begin();
              observers.end() != observerIter; ++observerIter )
        {
            ( *observerIter )->signalCompletion( result, details );
        }

        // reset the observers
        clearObservers();

        FUNCTION_EXIT;
    }

} // TA_IRS_App
