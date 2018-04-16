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

#include "app/trains/train_agent/src/CctvTasks.h"
#include "app/trains/train_agent/src/ITimsCctvManagerTaskCallback.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    CarrierOffTask::CarrierOffTask( ITimsCctvManagerTaskCallback& callback,
                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId )
            : m_callback( callback ),
              m_trainId( trainId ),
              m_owningTrainId( owningTrainId ),
              m_scheduled( false )
    {
        FUNCTION_ENTRY( "CarrierOffTask" );
        FUNCTION_EXIT;
    }


    CarrierOffTask::~CarrierOffTask()
    {
        FUNCTION_ENTRY( "~CarrierOffTask" );
        FUNCTION_EXIT;
    }


    bool CarrierOffTask::isScheduled()
    {
        FUNCTION_ENTRY( "isScheduled" );
        FUNCTION_EXIT;
        return m_scheduled;
    }


    void CarrierOffTask::setScheduled()
    {
        FUNCTION_ENTRY( "setScheduled" );

        m_scheduled = true;

        FUNCTION_EXIT;
    }


    void CarrierOffTask::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback.performCarrierOff( m_trainId, m_owningTrainId );

        FUNCTION_EXIT;
    }
}
