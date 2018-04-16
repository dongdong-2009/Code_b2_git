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


#include "bus/trains/TrainTransactionModel/src/StateChangeWorkItem.h"
#include "bus/trains/TrainTransactionModel/src/ITrainStateUpdateListener.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    StateChangeWorkItem::StateChangeWorkItem( ITrainStateUpdateListener& listener,
                                              TrainStatePtr state )
        : m_listener( listener ),
          m_update( state )
    {
        FUNCTION_ENTRY( "StateChangeWorkItem" );
        FUNCTION_EXIT;
    }


    StateChangeWorkItem::~StateChangeWorkItem()
    {
        FUNCTION_ENTRY( "~StateChangeWorkItem" );
        FUNCTION_EXIT;
    }


    void StateChangeWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_listener.processTrainStateChanged( m_update );

        FUNCTION_EXIT;
    }
}
