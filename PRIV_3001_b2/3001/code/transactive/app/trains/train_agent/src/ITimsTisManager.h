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
#if !defined(ITimsTisManager_H)
#define ITimsTisManager_H

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
     * @created 01-Apr-2008 2:47:50 PM
     */
    class ITimsTisManager
    {

    public:

        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue() = 0;


        /**
         * Gets the library versions for all known trains
         *
         * @return the library versions for all kown trains
         */
        virtual TA_IRS_Bus::TtisTypes::TisLibraryVersionList getLibraryVersions() = 0;


        /**
         * Gets the library versions for the given train.
         *
         * @return the library version for the train specified
         *
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train to get the library versions for
         */
        virtual TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate getLibraryVersionsForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * Displays a free text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The parameters required to display the message
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void displayFreeTextMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                             const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                             time_t timestamp,
                                             const std::string& sessionId ) = 0;


        /**
         * Displays a pre defined text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The predefined display parameters
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void displayStoredMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                           const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                           time_t timestamp,
                                           const std::string& sessionId ) = 0;


        /**
         * Sends a reset/clear request for messages of a certain priority on the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to clear the message on
         * @param displayParameters    The parameters for the reset message
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        virtual void resetMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                   const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                   time_t timestamp,
                                   const std::string& sessionId ) = 0;


        /**
         * This will start a predefined message library download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate the download start action,
         * and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these messages will be sent indicating the status.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given library to
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         */
        virtual void libraryDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                      const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                                      unsigned long libraryVersion,
                                      const std::string& sessionId ) = 0;


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a library upgrade command to
         * @param sessionId    The operator initiating the upgrade
         */
        virtual void libraryUpgrade( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                     const std::string& sessionId ) = 0;

		/**
		 * This will cancel the library download with the given train ID.
		 *
		 * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * 
         * @param trains      The trains ID to cancel the library download
         * @param sessionId    The operator initiating the download
		 */
		virtual void cancelLibraryDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
											const std::string& sessionId ) = 0;

        /**
         * This will start a time schedule download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate the download start action,
         * and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these messages will be sent indicating the status.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given schedule to
         * @param scheduleData    The time schedule data to download to the train
         * @param scheduleVersion    The version of the time schedule data
         * @param sessionId    The operator initiating the download
         */
        virtual void scheduleDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                       const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                                       unsigned long scheduleVersion,
                                       const std::string& sessionId ) = 0;


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a schedule upgrade command to
         * @param sessionId    the operator initiating the upgrade
         */
        virtual void scheduleUpgrade( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                      const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsTisManager_H)
