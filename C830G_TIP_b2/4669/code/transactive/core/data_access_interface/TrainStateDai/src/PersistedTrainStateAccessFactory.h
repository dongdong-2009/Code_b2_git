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
#if !defined(PersistedTrainStateAccessFactory_H)
#define PersistedTrainStateAccessFactory_H

#include "core/data_access_interface/TrainStateDai/src/IPersistedTrainState.h"

namespace TA_IRS_Core
{
    /**
     * This loads and saves persisted train states.
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:39 PM
     */
    class PersistedTrainStateAccessFactory
    {

    public:

        
        /**
         * This will load all persisted train states from the database.
         * 
         * @return the persisted state of all known trains
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        static IPersistedTrainStateMap loadAllTrains();
        

        /**
         * This save the persisted states of a single train
         * (whether it previously existed or not)
         * 
         * @param trainId               The Train ID
         * @param primaryTsi            The primary TSI
         * @param secondaryTsi          The secondary TSI
         * @param paLibraryVersion      The PA library version
         * @param ttisLibraryVersion    The TTIS library version
         * @param ttisTimeSchedule      The TTIS time schedule version
         * @param timestamp             The modification time
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        static void saveTrainState( unsigned char trainId,
                                    const std::string& primaryTsi,
                                    const std::string& secondaryTsi,
                                    unsigned short paLibraryVersion,
                                    unsigned short ttisLibraryVersion,
                                    unsigned short ttisTimeSchedule,
									unsigned short nextttisLibraryVersion,
                                    unsigned short nextttisTimeSchedule,
                                    time_t timestamp );
        

    private:
        PersistedTrainStateAccessFactory();
        ~PersistedTrainStateAccessFactory();
    };

}
#endif // !defined(PersistedTrainStateAccessFactory_H)
