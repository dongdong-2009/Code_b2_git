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

#include "app/trains/train_agent/src/DataCallEndWorkItem.h"
#include "app/trains/train_agent/src/IDataCallEndObserver.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    DataCallEndWorkItem::DataCallEndWorkItem( IDataCallEndObserver& observer,
                                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              bool success,
                                              const std::string& details )
            : m_observer( observer ),
              m_trainId( trainId ),
              m_success( success ),
              m_details( details )
    {
        FUNCTION_ENTRY( "DataCallEndWorkItem" );
        FUNCTION_EXIT;
    }


    DataCallEndWorkItem::~DataCallEndWorkItem()
    {
        FUNCTION_ENTRY( "~DataCallEndWorkItem" );
        FUNCTION_EXIT;
    }


    void DataCallEndWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // call m_observer with the details
        m_observer.processDataCallEndEvent( m_trainId,
                                            m_success,
                                            m_details );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
