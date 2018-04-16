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

#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainTransactionModel/src/TrainStatusManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{

    TrainStateLock::TrainStateLock( TrainStatePtr trainState )
            : m_state( trainState ),
              m_guard( new TA_Base_Core::ThreadGuard( m_state->getLock() ) )
    {
        FUNCTION_ENTRY( "TrainStateLock" );

        // Sets m_state and creates a new Threadguard,
        // with m_state.getLock() asthe lockable object.

        FUNCTION_EXIT;
    }


    TrainStateLock::~TrainStateLock()
    {
        FUNCTION_ENTRY( "~TrainStateLock" );

        // checks for data changes (dataChanged = m_state.hasDataChanged)
        bool dataChanged = m_state->hasDataChanged();

        // resets the change flag m_state.resetDataChanged()
        m_state->resetDataChanged();

        // Deletes the lock.
        delete m_guard;
        m_guard = NULL;

        // If dataChanged, it gets the train status manager from the train state,
        // and notifies it of the state change (notifyStateChanged)

        if ( dataChanged == true )
        {
            // Inform all observers of the change in state
            TrainStatusManager& statusManager = m_state->getTrainStatusManager();
            statusManager.notifyStateChanged( m_state->getTrainId(), m_state->getResourceCategory() );
        }

        FUNCTION_EXIT;
    }

}
