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
#if !defined(TrainPaCorbaServant_H)
#define TrainPaCorbaServant_H


#include "bus/trains/TrainAgentCorba/idl/src/ITrainPaCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class ITimsPaManager;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainPaCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:30 PM
     */
    class TrainPaCorbaServant : public TA_Base_Core::ServantBase,
                                public POA_TA_Base_Bus::ITrainPaCorba
    {

    public:

        /**
         *
         * @param timsPaManager    to delegate calls to
         */
        TrainPaCorbaServant( ITimsPaManager& timsPaManager );


        /**
         * Destructor
         */
        virtual ~TrainPaCorbaServant();


        /**
         * Used to check the longest possible time a PA operation can take
         */
        CORBA::UShort getCommandTimeout();


        /**
         * Gets the PA library version on this train.
         *
         * @return the PA library version for the given train
         *
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train ID to query
         */
        CORBA::UShort getTrainPaLibraryVersion( TA_Base_Bus::ITrainCommonCorba::TrainId trainId );


        /**
         * Gets the PA library version for all known trains
         *
         * @return the PA library version for all trains
         */
        TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList* getTrainPaLibraryVersionList();


        /**
         * This will broadcast a PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned, this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number. If different from the version on the train the boradcast will not be done.
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        void preRecordedAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                      TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                      CORBA::Octet messageNumber,
                                      CORBA::UShort libraryVersion,
                                      CORBA::Boolean override,
                                      const char* sessionId );


        /**
         * This will broadcast a cyclic PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned, this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number. If different from the version on the train the boradcast will not be done.
         * @param announcementPeriod    This is the time parameters to broadcast the message
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        void cyclicAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                 TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                 CORBA::Octet messageNumber,
                                 CORBA::UShort libraryVersion,
                                 const TA_Base_Bus::ITrainPaCorba::MessageSchedule& announcementPeriod,
                                 CORBA::Boolean override,
                                 const char* sessionId );


        /**
         * This will start the live announcement set up process on each train.
         * This will spawn off the commands to each train, and return to the caller once this is done. The status of the command will be sent back using a PA Update Event.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * @exception ITrainExceptionsCorba::GroupInUseException if the given group TSI is in use
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param staticGroup    The optional group TSI to use to call the trains for the live announcement. Empty string for a single train announcement, or for a DGNA. If this is set for a single train, it will be ignored.
         * @param sessionId    The operator submitting the broadcast
         */
        void setupLiveAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                    TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                    CORBA::Boolean override,
                                    const char* staticGroup,
                                    const char* sessionId );


        /**
         * For a given live announcement in progress, when an audio call hasnt been made, this will make the audio call for the live announcement.
         *
         * Depending on the number of trains involved, and whether a static talkgroup was chosen, this will either simply call an individual TSI, a static talkgroup, or make a DGNA then call the DGNA.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * @exception ITrainExceptionsCorba::TrainRadioException if a radio operation failed
         *
         * @param announcementId    The ID of the announcement to begin
         * @param sessionId    The operator making the broadcast
         */
        void beginLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                    const char* sessionId );


        /**
         * For a given live announcement in progress, send a PA continue to all trains requiring it.
         *
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         */
        void continueLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                       const char* sessionId );


        /**
         * This ends a live announcment in progress. It will end any audio in progress for the announcement, and reset the announcement on each train.
         *
         * This will return once the command is spawned on each train, and status updates will be sent via PA Update Event.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         */
        void endLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                  const char* sessionId );


        /**
         * This ends any ongoing DVA or cyclic DVA boradcast on the train.
         * This is not used for live announcements.
         * The result will be sent back by a PA Update Event.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains to end this announcement on
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         */
        void endBroadcast( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                           TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                           const char* sessionId );


    private:


        /**
         * Private default constructor
         */
        TrainPaCorbaServant();


        /**
         * Used to delegate calls to
         */
        ITimsPaManager& m_timsPaManager;

    };

} // TA_IRS_App

#endif // !defined(TrainPaCorbaServant_H)
