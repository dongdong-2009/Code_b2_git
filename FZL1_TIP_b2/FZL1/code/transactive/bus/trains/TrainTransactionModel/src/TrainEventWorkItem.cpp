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


#include "bus/trains/TrainTransactionModel/src/TrainEventWorkItem.h"
#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    TrainEventWorkItem::TrainEventWorkItem( ITrainEventProcessor& eventProcessor,
                                            TrainEventPtr event )
          : m_eventProcessor( eventProcessor ),
            m_event( event )
    {
        FUNCTION_ENTRY( "TrainEventWorkItem" );
        FUNCTION_EXIT;
    }


    TrainEventWorkItem::~TrainEventWorkItem()
    {
        FUNCTION_ENTRY( "~TrainEventWorkItem" );
        FUNCTION_EXIT;
    }


    void TrainEventWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_eventProcessor.processEvent( m_event );

        FUNCTION_EXIT;
    }
}