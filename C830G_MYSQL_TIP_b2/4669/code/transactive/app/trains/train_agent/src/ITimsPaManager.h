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
#if !defined(ITimsPaManager_H)
#define ITimsPaManager_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:48 PM
     */
    class ITimsPaManager
    {

    public:

        /**
         * Used to check the longest possible time a PA operation can take
         */
        virtual unsigned short getCommandTimeout() = 0;


        /**
         * Gets the PA library version on this train.
         *
         * @return the PA library version for the given train
         *
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train ID to query
         */
        virtual unsigned short getTrainPaLibraryVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * Gets the PA library version for all known trains
         *
         * @return the PA library version for all trains
         */
        virtual TA_IRS_Bus::PaTypes::TrainLibraryInfoList getTrainPaLibraryVersionList() = 0;


        /**
         * This will broadcast a PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned,
         * this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number. If different from the version on the train the boradcast will not be done.
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        virtual void preRecordedAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                              TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                              unsigned char messageNumber,
                                              unsigned short libraryVersion,
                                              bool override,
                                              const std::string& sessionId ) = 0;


        /**
         * This will broadcast a cyclic PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned,
         * this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number. If different from the version on the train the boradcast will not be done.
         * @param announcementPeriod    This is the time parameters to broadcast the message
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        virtual void cyclicAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                         TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                         unsigned char messageNumber,
                                         unsigned short libraryVersion,
                                         const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                         bool override,
                                         const std::string& sessionId ) = 0;


        /**
         * This ends any ongoing DVA or cyclic DVA boradcast on the train.
         * This is not used for live announcements.
         * The result will be sent back by a PA Update Event.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains to end this announcement on
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         */
        virtual void endBroadcast( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                   TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                   const std::string& sessionId ) = 0;


        /**
         * This will start the live announcement set up process on each train.
         * This will spawn off the commands to each train, and return to the caller once this is done.
         * The status of the command will be sent back using a PA Update Event.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception GroupInUseException if the given group TSI is in use
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param staticGroup    The optional group TSI to use to call the trains for the live announcement.
         *                       Empty string for a single train announcement, or for a DGNA. If this is set for a single train, it will be ignored.
         * @param sessionId    The operator submitting the broadcast
         */
        virtual void setupLiveAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                            TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                            bool override,
                                            const std::string& staticGroup,
                                            const std::string& sessionId ) = 0;


        /**
         * For a given live announcement in progress, when an audio call hasnt been made, this will make the audio call for the live announcement.
         *
         * Depending on the number of trains involved, and whether a static talkgroup was chosen,
         * this will either simply call an individual TSI, a static talkgroup, or make a DGNA then call the DGNA.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if a radio operation failed
         *
         * @param announcementId    The ID of the announcement to begin
         * @param sessionId    The operator making the broadcast
         */
        virtual void beginLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                            const std::string& sessionId ) = 0;


        /**
         * For a given live announcement in progress, send a PA continue to all trains requiring it.
         *
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         */
        virtual void continueLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                               const std::string& sessionId ) = 0;


        /**
         * This ends a live announcment in progress. It will end any audio in progress for the announcement, and reset the announcement on each train.
         *
         * This will return once the command is spawned on each train, and status updates will be sent via PA Update Event.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         */
        virtual void endLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                          const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsPaManager_H)
