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
#if !defined(ITimsPaManagerTaskCallback_H)
#define ITimsPaManagerTaskCallback_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * This is the callback interface for PA tasks.
     * It is each single train operation called by PA tasks, so that multiple trains can be processed in parallel.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:49 PM
     */

    class ITimsPaManagerTaskCallback
    {

    public:

        /**
         * This will broadcast a PA pre-recorded announcement for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * @param trainId    The train to execute the command on
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operator origin for the command
         */
        virtual void preRecordedAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                         TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                         unsigned char messageNumber,
                                                         unsigned short libraryVersion,
                                                         TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                         const std::string& sessionId,
                                                         unsigned char origin ) = 0;


        /**
         * This will broadcast a cyclic PA pre-recorded announcement for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
         * @param announcementPeriod    This is the time parameters to broadcast the message
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operators console origin
         */
        virtual void cyclicAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                    unsigned char messageNumber,
                                                    unsigned short libraryVersion,
                                                    const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                                    TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                    const std::string& sessionId,
                                                    unsigned char origin ) = 0;


        /**
         * This will start the live announcement set up process on a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param staticGroup    The optional group TSI to use to call the trains for the live announcement.
         *                       Empty string for a single train announcement, or for a DGNA. If this is set for a single train, it will be ignored.
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operators console origin
         * @param monitorTask    Optionally the task to monitor the status of the setup, and clear the radio group lock if all trains fail
         */
        virtual void setupLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                       TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                       TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                       const std::string& staticGroup,
                                                       const std::string& sessionId,
                                                       unsigned char origin,
                                                       SetupLiveAnnouncementMonitorTaskPtr monitorTask ) = 0;


        /**
         * For a given live announcement in progress on a train, send a PA continue.
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * @param trainId    The train to continue
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        virtual void continueLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                          const std::string& sessionId,
                                                          unsigned char origin ) = 0;


        /**
         * This ends a live announcement in progress for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        virtual void endLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                     const std::string& sessionId,
                                                     unsigned char origin ) = 0;


        /**
         * This ends any ongoing DVA or cyclic DVA boradcast on the train.
         * This is not used for live announcements.
         * The result will be sent back by a PA Update Event.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         * @param origin    The operators console origin
         */
        virtual void endBroadcastSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                              const std::string& sessionId,
                                              unsigned char origin ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsPaManagerTaskCallback_H)
