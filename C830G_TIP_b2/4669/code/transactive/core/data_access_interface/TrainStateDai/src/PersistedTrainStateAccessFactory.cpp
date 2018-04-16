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

#include "core/data_access_interface/TrainStateDai/src/PersistedTrainStateAccessFactory.h"
#include "core/data_access_interface/TrainStateDai/src/PersistedTrainState.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{

    IPersistedTrainStateMap PersistedTrainStateAccessFactory::loadAllTrains() 
    {
        FUNCTION_ENTRY( "loadAllTrains" );

        FUNCTION_EXIT;
        return PersistedTrainState::loadAllTrainStates();
    }


    void PersistedTrainStateAccessFactory::saveTrainState( unsigned char trainId,
                                                           const std::string& primaryTsi,
                                                           const std::string& secondaryTsi,
                                                           unsigned short paLibraryVersion,
                                                           unsigned short ttisLibraryVersion,
                                                           unsigned short ttisTimeSchedule,
														   unsigned short nextttisLibraryVersion,
                                                           unsigned short nextttisTimeSchedule,
                                                           time_t timestamp )
    {
        FUNCTION_ENTRY("saveTrainState");    
        
        // call saveTrainState in PersistedTrainState
        PersistedTrainState::saveTrainState( trainId,
                                             primaryTsi,
                                             secondaryTsi,
                                             paLibraryVersion,
                                             ttisLibraryVersion,
                                             ttisTimeSchedule,
											 nextttisLibraryVersion,
                                             nextttisTimeSchedule,
                                             timestamp );
        
        FUNCTION_EXIT;
    }
}