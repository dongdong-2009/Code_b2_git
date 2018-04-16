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

#include "app/trains/train_agent/src/ChangeModeCommandObserver.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    ChangeModeCommandObserver::ChangeModeCommandObserver()
            : m_semaphore( 0 ),
              m_result( Success ),
              m_status( "" )
    {
        FUNCTION_ENTRY( "ChangeModeCommandObserver" );
        FUNCTION_EXIT;
    }


    ChangeModeCommandObserver::~ChangeModeCommandObserver()
    {
        FUNCTION_ENTRY( "~ChangeModeCommandObserver" );

        // unblock and blocked observers
        m_semaphore.post();

        FUNCTION_EXIT;
    }


    EChangeModeCommandResult ChangeModeCommandObserver::waitForCompletion()
    {
        FUNCTION_ENTRY( "waitForCompletion" );

        // waits on m_semaphore
        m_semaphore.wait();

        // when unblocked, returns the result

        FUNCTION_EXIT;
        return m_result;
    }


    std::string ChangeModeCommandObserver::getExtraStatusInformation()
    {
        FUNCTION_ENTRY( "getExtraStatusInformation" );
        FUNCTION_EXIT;
        return m_status;
    }


    void ChangeModeCommandObserver::signalCompletion( EChangeModeCommandResult result,
                                                      const std::string& information )
    {
        FUNCTION_ENTRY( "signalCompletion" );

        // set the result and status
        m_result = result;
        m_status = information;

        // then signal the semaphore to unblock observers
        m_semaphore.post();

        FUNCTION_EXIT;
    }

} // TA_IRS_App
