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
#if !defined(TimsPaManager_H)
#define TimsPaManager_H

#include "app/trains/train_agent/src/ITimsPaManager.h"
#include "app/trains/train_agent/src/PaTasks.h"
#include "app/trains/train_agent/src/ITimsPaManagerTaskCallback.h"
#include "app/trains/train_agent/src/ICallEndObserver.h"

#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/IUnsolicitedTrainTimerCallback.h"

#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaReadyForLiveAnnouncementEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/AudioCabToCabEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaAudioSwitchEvent.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class IOperationModeManager;
    class IMessageSender;
    class IRightsChecker;
    class IRadioAgentInterface;
    class RadioMessageProcessor;
    class IRadioGroupCoordinator;
}

namespace TA_IRS_Bus
{
    class ITrainTransactionManager;
    class UnsolicitedTrainTimerManager;
}

namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:11 PM
     */
    class TimsPaManager : public ITimsPaManager,
                          public ITimsPaManagerTaskCallback,
                          public ICallEndObserver,
                          public TA_IRS_Bus::ITrainEventProcessor,
                          public TA_IRS_Bus::IUnsolicitedTrainTimerCallback
    {

    public:

        /**
         * Constructor
         *
         * @param configuration
         * @param statusManager
         * @param transactionManager
         * @param operationModeManager
         * @param messageSender    Used to send client updates
         * @param rightsChecker
         * @param radioAgent    Used to perform radio functions (calls, DGNA etc)
         * @param radioMessageProcessor    Used to subscribe to radio updates
         * @param radioGroupCoordinator    Used to lock and unlock GTSI groups for group train calls
         */
        TimsPaManager( ITrainAgentConfiguration& configuration,
                       ITrainStatusManager& statusManager,
                       TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                       IOperationModeManager& operationModeManager,
                       IMessageSender& messageSender,
                       IRightsChecker& rightsChecker,
                       IRadioAgentInterface& radioAgent,
                       RadioMessageProcessor& radioMessageProcessor,
                       IRadioGroupCoordinator& radioGroupCoordinator );


        /**
         * Destructor
         */
        virtual ~TimsPaManager();


        /**
         * Used to check the longest possible time a PA operation can take
         */
        virtual unsigned short getCommandTimeout();


        /**
         * Gets the PA library version on this train.
         *
         * @return the PA library version for the given train
         *
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train ID to query
         */
        virtual unsigned short getTrainPaLibraryVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Gets the PA library version for all known trains
         *
         * @return the PA library version for all trains
         */
        virtual TA_IRS_Bus::PaTypes::TrainLibraryInfoList getTrainPaLibraryVersionList();


        /**
         * This will broadcast a PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned,
         * this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        virtual void preRecordedAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                              TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                              unsigned char messageNumber,
                                              unsigned short libraryVersion,
                                              bool override,
                                              const std::string& sessionId );


        /**
         * This will broadcast a cyclic PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been spawned,
         * this method will return, the result of each command on each train will be returned asynchronously using a PA Update Event.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
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
                                         const std::string& sessionId );


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
                                   const std::string& sessionId );


        /**
         * This will start the live announcement set up process on each train.
         * This will spawn off the commands to each train, and return to the caller once this is done.
         * The status of the command will be sent back using a PA Update Event.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
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
                                            const std::string& sessionId );


        /**
         * For a given live announcement in progress, when an audio call hasnt been made, this will make the audio call for the live announcement.
         *
         * Depending on the number of trains involved, and whether a static talkgroup was chosen,
         * this will either simply call an individual TSI, a static talkgroup, or make a DGNA then call the DGNA.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if a radio operation failed
         *
         * @param announcementId    The ID of the announcement to begin
         * @param sessionId    The operator making the broadcast
         */
        virtual void beginLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                            const std::string& sessionId );


        /**
         * For a given live announcement in progress, send a PA continue to all trains requiring it.
         *
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         */
        virtual void continueLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                               const std::string& sessionId );


        /**
         * This ends a live announcment in progress. It will end any audio in progress for the announcement, and reset the announcement on each train.
         *
         * This will return once the command is spawned on each train, and status updates will be sent via PA Update Event.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         */
        virtual void endLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                          const std::string& sessionId );

        /**
         * This handles a call end event.
         * It can do any required processing to clean up on unexpected call termination.
         * This is called in its own thread, so processing can be done.
         *
         * @param callEnd    The details of the call that ended
         * @param trainList    The trains involved in the radio call
         */
        virtual void processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                          const TA_IRS_Bus::CommonTypes::TrainIdList& trainList );


        /**
         * This is a callback from the Unsolicited train timer, it means that a timer for a train has expired.
         *
         * @param trainId    The train the timer is for
         */
        virtual void handleTrainTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will process any incoming events from trains.
         *
         * @param event    The incoming event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


    private:


        /**
         * Private default constructor
         */
        TimsPaManager();


        /**
         * This will broadcast a PA pre-recorded announcement for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
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
                                                         unsigned char origin );


        /**
         * This will broadcast a cyclic PA pre-recorded announcement for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
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
                                                    unsigned char origin );

        
        /**
         * This is a generic version used by preRecordedAnnouncementSingleTrain and cyclicAnnouncementSingleTrain
         */
        template <typename CommandTypePtr>
        void genericPreRecordedAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                        TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                        const std::string& sessionId,
                                                        const std::string& operationDescription,
                                                        CommandTypePtr command );


        /**
         * This ends any ongoing DVA or cyclic DVA boradcast on the train.
         * This is not used for live announcements.
         * The result will be sent back by a PA Update Event.
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         * @param origin    The operators console origin
         */
        virtual void endBroadcastSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                              const std::string& sessionId,
                                              unsigned char origin );


        /**
         * This will start the live announcement set up process on a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
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
                                                       SetupLiveAnnouncementMonitorTaskPtr monitorTask );


        /**
         * For a given live announcement in progress on a train, send a PA continue.
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
         *
         * @param trainId    The train to continue
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        virtual void continueLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                          const std::string& sessionId,
                                                          unsigned char origin );


        /**
         * This ends a live announcement in progress for a single train.
         * The result of this command will be returned asynchronously using a PA Update Event.
         *
         * This can be declared private here because in the interface used by the task objects it is declared public.
         * Only the tasks should be calling this method.
         *
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        virtual void endLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                     const std::string& sessionId,
                                                     unsigned char origin );


        /**
         * This will perform checks to ensure that the operator action is allowed (either calling or answering a train).
         *
         * @exception InvalidTrainException if the agent isnt in control of this train
         *
         * @param trainId    The train to check
         */
        void performPreBroadcastTrainChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processPaTrainMessageCompletedEvent( TA_IRS_Bus::PaTrainMessageCompletedEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processPaReadyForLiveAnnouncementEvent( TA_IRS_Bus::PaReadyForLiveAnnouncementEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processPaResetEvent( TA_IRS_Bus::PaResetEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processPaRequestForResetEvent( TA_IRS_Bus::PaRequestForResetEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processAudioCabToCabEvent( TA_IRS_Bus::AudioCabToCabEventPtr event );


        /**
         * This will process an incoming train event
         *
         * @param event    The incoming event
         */
        virtual void processPaAudioSwitchEvent( TA_IRS_Bus::PaAudioSwitchEventPtr event );


        /**
         * This will start the PA3 timer for the given train (or reschedule it if it is already active)
         *
         * @param trainId
         */
        void startPA3Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will stop the PA3 timer for the given train if it is active
         *
         * @param trainId
         */
        void stopPA3Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This handles the case a PA3 timer expires
         *
         * @param trainId
         */
        void onPA3Timeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is the name of the PA3 timer given to the Unsolicited Train Timer Manager
         */
        static const std::string PA3_TIMER_TEXT;


        /**
         * Used to access agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to set and get train state
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Used to send and register for messages from trains
         */
        TA_IRS_Bus::ITrainTransactionManager& m_transactionManager;


        /**
         * Used to query and register for operation mode changes
         */
        IOperationModeManager& m_operationModeManager;


        /**
         * Used to send client updates
         */
        IMessageSender& m_messageSender;


        /**
         * Used to check rights for operator actions
         */
        IRightsChecker& m_rightsChecker;


        /**
         * Used to perform radio operations
         */
        IRadioAgentInterface& m_radioAgent;


        /**
         * Used to subscribe to Radio Updates
         */
        RadioMessageProcessor& m_radioMessageProcessor;


        /**
         * Used to lock and unlock GTSI groups for group train calls
         */
        IRadioGroupCoordinator& m_radioGroupCoordinator;


        /**
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * This is used to manage the PA3 timer scheduling, cancelling, and expiration.
         */
        TA_IRS_Bus::UnsolicitedTrainTimerManager* m_pa3TimerManager;

    };

}

#endif // !defined(TimsPaManager_H)
