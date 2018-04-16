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
#if !defined(TrainStateHelper_H)
#define TrainStateHelper_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

// forward declaration
namespace TA_IRS_App
{
    class ITrainStatusManager;
}

namespace TA_IRS_App
{

    /**
     * This just holds some helper functions and helper classes for use by the TrainAgentStatusManager.
     * The things in here are small and don't really live on their own.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:34 PM
     */
    class TrainStateHelper
    {

    public:

        /**
         * Given an ongoing call (and the type of call), find out which trains are involved.
         *
         * @return the list of trains involved in the call
         *
         * @param callDetails    The details of the call (previously obtained from getDetailsForRadioCall)
         * @param trainStatusManager    The status manager holding train states
         */
        static TA_IRS_Bus::CommonTypes::TrainIdList getTrainsInCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails,
                                                                     const ITrainStatusManager& trainStatusManager );


        /**
         * Given a TSI, find the train ID it belongs to.
         *
         * @return the train id, or 0 if it doesnt match
         *
         * @param tsi
         * @param trainStatusManager
         */
        static TA_IRS_Bus::CommonTypes::TrainIdType getTrainIdFromTsi( const std::string& tsi,
                                                                       const ITrainStatusManager& trainStatusManager );


        /**
         * Given an ongoing call (and the type of call), find out which trains are involved.
         *
         * @return the list of trains involved in the call
         *
         * @param announcementId    The announcement ID to match
         * @param continueRequired    If true, return only trains requring continue
         * @param trainStatusManager    The status manager holding train states
         */
        static TA_IRS_Bus::CommonTypes::TrainIdList getTrainsInAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                             bool continueRequired,
                                                                             const ITrainStatusManager& trainStatusManager );


        /**
         * Find the call id for the given live announcement if it exists
         *
         * @return the details of this call
         *
         * @exception TrainRadioException if there is no call
         *
         * @param announcementID    The announcement ID to find the call for
         * @param trainStatusManager
         */
        static TA_IRS_Bus::StateUpdateTypes::RadioCallDetails getCallDetailsForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                                                 const ITrainStatusManager& trainStatusManager );


        /**
         * Find the static group used/to be used for the given live announcement
         *
         * @return the group TSI if set
         *
         * @param announcementID    The announcement ID to find the group for
         * @param trainStatusManager
         */
        static std::string getStaticGroupForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                              const ITrainStatusManager& trainStatusManager );

    };

} // TA_IRS_App

#endif // !defined(TrainStateHelper_H)
