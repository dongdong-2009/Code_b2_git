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
#if !defined(ITimsTisManagerTaskCallback_H)
#define ITimsTisManagerTaskCallback_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"

#include <string>
#include <ctime>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:51 PM
     */
    class ITimsTisManagerTaskCallback
    {

    public:

        /**
         * Displays a free text message to the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The parameters required to display the message
         * @param timestamp    The time of the request submission.
         *                     This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void displayFreeTextMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             unsigned char origin,
                                             const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                             time_t timestamp,
                                             const std::string& sessionId ) = 0;


        /**
         * Displays a pre defined text message to the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The predefined display parameters
         * @param timestamp    The time of the request submission.
         *                     This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void displayStoredMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           unsigned char origin,
                                           const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                           time_t timestamp,
                                           const std::string& sessionId ) = 0;


        /**
         * Sends a reset/clear request for messages of a certain priority on the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The parameters for the reset message
         * @param timestamp    The time of the request submission.
         *                     This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void resetMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   unsigned char origin,
                                   const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                   time_t timestamp,
                                   const std::string& sessionId ) = 0;


        /**
         * This will start a predefined message library download to the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate the download start action, and whether it was
         * successful or failed, and give a failure reason.
         * If the download started, once the download completes another
         * one of these messages will be sent indicating the status.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         */
        virtual void libraryDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      unsigned char origin,
                                      const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                                      unsigned short libraryVersion,
                                      const std::string& sessionId ) = 0;


        /**
         * This will upgrade the predefined message library on the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param sessionId    The operator initiating the upgrade
         */
        virtual void libraryUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     unsigned char origin,
                                     const std::string& sessionId ) = 0;


        /**
         * This will start a time schedule download to the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate the download start action, and whether it was
         * successful or failed, and give a failure reason.
         * If the download started, once the download completes another
         * one of these messages will be sent indicating the status.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param scheduleData    The time schedule data to download to the train
         * @param scheduleVersion    The version of the time schedule data
         * @param sessionId    The operator initiating the download
         */
        virtual void scheduleDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       unsigned char origin,
                                       const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                                       unsigned short scheduleVersion,
                                       const std::string& sessionId ) = 0;


        /**
         * This will upgrade the predefined message library on the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @param trainId    the operator's origin value
         * @param origin    the operator's origin value
         * @param sessionId    the operator initiating the upgrade
         */
        virtual void scheduleUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      unsigned char origin,
                                      const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsTisManagerTaskCallback_H)
