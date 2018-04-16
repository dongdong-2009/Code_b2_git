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

#include "app/trains/train_agent/src/CallEndWorkItem.h"
#include "app/trains/train_agent/src/ICallEndObserver.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    CallEndWorkItem::CallEndWorkItem( ICallEndObserver& observer,
                                      const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails,
                                      const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
            : m_observer( observer ),
              m_callDetails( callDetails ),
              m_trainList( trainList )
    {
        FUNCTION_ENTRY( "CallEndWorkItem" );
        FUNCTION_EXIT;
    }


    CallEndWorkItem::~CallEndWorkItem()
    {
        FUNCTION_ENTRY( "~CallEndWorkItem" );
        FUNCTION_EXIT;
    }


    void CallEndWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // call m_observer with the details
        m_observer.processCallEndEvent( m_callDetails, m_trainList );

        FUNCTION_EXIT;
    }


} // TA_IRS_App
