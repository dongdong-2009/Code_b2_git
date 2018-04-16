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

#include "app/trains/train_agent/src/DriverCallTasks.h"
#include "app/trains/train_agent/src/ITimsDriverCallManagerTaskCallback.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/utilities/src/DebugUtil.h"

#include <ace/Synch.h>


namespace TA_IRS_App
{

    CallResetTask::CallResetTask( ITimsDriverCallManagerTaskCallback& callback,
                                  TA_IRS_Bus::CommonTypes::TrainIdType trainId )
        : m_callback( callback ),
          m_trainId( trainId )
    {
        FUNCTION_ENTRY( "CallResetTask" );
        FUNCTION_EXIT;
    }


    CallResetTask::~CallResetTask()
    {
        FUNCTION_ENTRY( "~CallResetTask" );
        FUNCTION_EXIT;
    }


    void CallResetTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // calls performTimsCallReset on the callback to do the actual work
        m_callback.performTimsCallReset( m_trainId );

        FUNCTION_EXIT;
    }


    CallSetupTask::CallSetupTask( ITimsDriverCallManagerTaskCallback& callback,
                                  TrainVoiceCallRequestPtr request,
                                  TA_Base_Core::SingleThreadBarrier& barrier )
        : m_callback( callback ),
          m_request( request ),
          m_barrier( barrier )
    {
        FUNCTION_ENTRY( "CallSetupTask" );
        FUNCTION_EXIT;
    }


    CallSetupTask::~CallSetupTask()
    {
        FUNCTION_ENTRY( "~CallSetupTask" );
        FUNCTION_EXIT;
    }


    void CallSetupTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // calls performTimsCallSetup on the callback to do the actual work
        m_callback.performTimsCallSetup( m_request );

        // post the barrier
        m_barrier.post();

        FUNCTION_EXIT;
    }

} // TA_IRS_App
