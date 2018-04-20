/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/tis_agent_access/src/TrainDataVersions.h $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Process and data encapsulation to store the following versions for a train:
  *     1. Predefined TTIS Library Version
  *     2. Next Predefined TTIS Library Version
  *     3. Schedule Version
  *     4. Next Schedule Version
  *
  * This is necessary due to the fact that the TrainAgent does not send an
  * TA_Base_Bus::TISNotification with ALL the fileds set correctly.
  *
  * NOTE: This container class *IS* nonreentrant thread safe.
  */

#ifndef TRAINDATAVERSIONS_H
#define TRAINDATAVERSIONS_H

#include <vector>

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    struct TrainDoesNotExistException
    {
    };

    /**
     * Process and data encapsulation to store the following versions for a
     * train:
     *     1. Predefined TTIS Library Version
     *     2. Next Predefined TTIS Library Version
     *     3. Schedule Version
     *     4. Next Schedule Version
     *
     * This is necessary due to the fact that the TrainAgent does not send an
     * TA_Base_Bus::TISNotification with ALL the fields set correctly.
     * 
     * WARNING: This class inherits from ITrainSelectionGUI because this is has
     * implemented the logic to be informed when a train is *dead*.  The super
     * class is unfortunately named.  The TrainSelectionComponent should be
     * separated functionally, one part to deal with notifications from the
     * TrainAgent and the other to visually represent it.
     */
    class TrainDataVersions
    {
    public:
        /** Constructor. */
        TrainDataVersions();
        /** Destructor. */
        virtual ~TrainDataVersions();

        /**
         * Returns the library versions for a particular train.
         *
         * @param trainID The train ID of the train of interest.
         *
         * @return TA_IRS_Core::TrainDataVersion object containing the version
         *         numbers.
         * @throws TrainDoesNotExistException
         */
        TA_Base_Core::TrainDataVersion getTrainDataVersion( unsigned short trainID ) const;

        /**
         * Returns all the libray versions.
         *
         * @return A vector of TA_IRS_Core::TrainDataVersion objects
         *         containing the version numbers. 
         */
        std::vector<TA_Base_Core::TrainDataVersion> getTrainDataVersions() const;

        /**
         * Updates the Predefined TTIS Library version and Next Predefined
         * TTIS Library version.
         *
         * @param trainID The train ID to update.
         * @param libraryVersion The Predefined TTIS library version number
         * @param nextLibraryVersion The next Predefined TTIS library version
         *                           number
         *
         * @return boolean indicating whether a new record was added
         */
        bool updateTTISLibraryVersions( unsigned short trainID,
                                        unsigned short libraryVersion,
                                        unsigned short nextLibraryVersion );

        /**
         * Updates the schedule version and the next schedule version.
         *
         * @param trainID The train ID to update.
         * @param libraryVersion The schedule version number
         * @param nextLibraryVersion The next schedule version number
         *
         * @return boolean indicating whether a new record was added
         */
        bool updateScheduleVersions( unsigned short trainID,
                                     unsigned short scheduleVersion,
                                     unsigned short nextScheduleVersion );

    private:
        

        /** Vector of version data. */
        std::vector<TA_Base_Core::TrainDataVersion> m_trainDataVersions;

        /** Thread lock to protect the vector from concurrent modifications. */
        mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };
} // namespace TR_IRS_Bus

#endif // TRAINDATAVERSIONS_H

