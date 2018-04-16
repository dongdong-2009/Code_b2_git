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
#if !defined(TimsTisManager_H)
#define TimsTisManager_H

#include "app/trains/train_agent/src/ITimsTisManager.h"
#include "app/trains/train_agent/src/IDataCallEndObserver.h"
#include "app/trains/train_agent/src/ITimsTisManagerTaskCallback.h"

#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/IUnsolicitedTrainTimerCallback.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfTtisDownloadEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisLibraryUpgradeCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisScheduleUpgradeCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtdFrame.h"

#include "app/trains/train_agent/src/ISpecializedMsgProc.h"
#include "app/trains/train_agent/src/ITrainLibDownloadSequence.h"
#include "app/trains/train_agent/src/IEndDownloadCallBack.h"

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
     * @created 01-Apr-2008 2:48:15 PM
     */

    class TimsTisManager : public ITimsTisManager,
                           public IDataCallEndObserver,
                           public TA_IRS_Bus::ITrainEventProcessor,
                           public TA_IRS_Bus::IUnsolicitedTrainTimerCallback,
                           public ITimsTisManagerTaskCallback,
						   public TA_IRS_App::ISpecializedMsgProc
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    used to access agent configuration
         * @param statusManager    used to set and get train state
         * @param transactionManager    used to send and register for messages from trains
         * @param operationModeManager    used to query and register for operation mode changes
         * @param messageSender    used to send client updates
         * @param rightsChecker    used to check rights for operator actions
         * @param radioAgent    used to perform radio operations
         * @param radioMessageProcessor    used to subscribe to Radio Updates
         */
        TimsTisManager( ITrainAgentConfiguration& configuration,
                        ITrainStatusManager& statusManager,
                        TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                        IOperationModeManager& operationModeManager,
                        IMessageSender& messageSender,
                        IRightsChecker& rightsChecker,
                        IRadioAgentInterface& radioAgent,
                        RadioMessageProcessor& radioMessageProcessor );


        /**
         * Destructor
         */
        virtual ~TimsTisManager();


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue();


        /**
         * Gets the library versions for all known trains
         *
         * @return the library versions for all kown trains
         */
        virtual TA_IRS_Bus::TtisTypes::TisLibraryVersionList getLibraryVersions();


        /**
         * Gets the library versions for the given train.
         *
         * @return the library version for the train specified
         *
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train to get the library versions for
         */
        virtual TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate getLibraryVersionsForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Displays a free text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
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
                                             const std::string& sessionId );


        /**
         * Displays a pre defined text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
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
                                           const std::string& sessionId );


        /**
         * Sends a reset/clear request for messages of a certain priority on the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
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
                                   const std::string& sessionId );


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
                                      const std::string& sessionId );


		/**
		 * This will cancel the library download with the given train ID.
		 *
		 * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * 
         * @param trains      The trains  to cancel the library download
         * @param sessionId    The operator initiating the download
		 */
		virtual void cancelLibraryDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
											const std::string& sessionId ); 

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
                                     const std::string& sessionId );


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
                                       const std::string& sessionId );


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a schedule upgrade command to
         * @param sessionId    the operator initiating the upgrade
         */
        virtual void scheduleUpgrade( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                      const std::string& sessionId );

        /**
         * This is called once a download has completed, but no end of download has been received from TIMS in the specified timeout.
         * The download will be considered failed.
         *
         * @param trainId    The train the timer is for
         */
        virtual void handleTrainTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is a handler for data call end events.
         * It means a transfer to a train has completed, we now await the message from TIMS to indicate success or failure.
         *
         * @param trainId    The train the transfer was for
         * @param success    Whether the transfer was successful
         * @param details    The failure reason if the transfer wasnt successful
         */
        virtual void processDataCallEndEvent( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              bool success,
                                              const std::string& details );


        /**
         * This is implemented by concrete classes to process incoming events.
         *
         * @param event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );
		
		void processReleaseOfCtrl();

    private:


        /**
         * Private default constructor
         */
        TimsTisManager();


        /**
         * Displays a free text message to the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message.
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
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
                                             const std::string& sessionId );


        /**
         * Displays a pre defined text message to the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
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
                                           const std::string& sessionId );


        /**
         * Sends a reset/clear request for messages of a certain priority on the given PIDs on the given train.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
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
                                   const std::string& sessionId );


        /**
         * This is a generic version used by displayFreeTextMessage, displayStoredMessage
         * and resetMessage
         */
        template <typename DisplayParametersType>
        void genericDisplayRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    unsigned char origin,
                                    const DisplayParametersType& displayParameters,
                                    time_t timestamp,
                                    const std::string& sessionId,
                                    const std::string& operationDescription );

        
        /**
         * The template function above needs to be able to create specific commands
         * for each type of display request. These operations will create the correct
         * commands for each type of display parameters
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The parameters for the message
         */
        TA_IRS_Bus::AbstractTtisCommandPtr createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          unsigned char origin,
                                                          const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                                          TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const;

        
        TA_IRS_Bus::AbstractTtisCommandPtr createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          unsigned char origin,
                                                          const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                                          TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const;

        
        TA_IRS_Bus::AbstractTtisCommandPtr createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          unsigned char origin,
                                                          const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                                          TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const;


        /**
         * The template function above needs to be able to handle a command result
         * for each type of display request. Some display requests handle the command result differently.
         *
         * @param displayParameters    The parameters for the message
         * @param result    The result of the command
         * @param tisReturnStatus    a reference to the return result structure. This will hold the result
         *
         * @return whether this operation was considered a success or not
         */
        bool handleResult( const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                           TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                           TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const;


        bool handleResult( const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                           TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                           TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const;


        bool handleResult( const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                           TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                           TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const;


        /**
         * This will start a predefined message library download to the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate the download start action, and whether it was successful
         * or failed, and give a failure reason.
         * If the download started, once the download completes another one
         * of these messages will be sent indicating the status.
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
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
                                      unsigned long libraryVersion,
                                      const std::string& sessionId );


        /**
         * This will upgrade the predefined message library on the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param sessionId    The operator initiating the upgrade
         */
        virtual void libraryUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     unsigned char origin,
                                     const std::string& sessionId );

		/**
	    *This will cancel the library download with the given train ID.
		*
		* The result will be returned via a TisLibraryUpdate message.
		* It will indicate whether the upgrade was successful or failed, and give a failure reason.
		*
		* @param trainId    The train the operation is for
		* @param sessionId    The operator initiating the upgrade
		*/
		virtual void cancelLibraryDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
											const std::string& sessionId );


        /**
         * This will start a time schedule download to the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate the download start action, and whether it was successful
         * or failed, and give a failure reason.
         * If the download started, once the download completes another one
         * of these messages will be sent indicating the status.
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
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
                                       unsigned long scheduleVersion,
                                       const std::string& sessionId );


        /**
         * This will upgrade the predefined message library on the given train.
         *
         * The result will be returned via a TisLibraryUpdate message.
         * It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * This can be declared private here because the interface declares it public.
         * This means only the tasks have the ability to call this method.
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param sessionId    the operator initiating the upgrade
         */
        virtual void scheduleUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      unsigned char origin,
                                      const std::string& sessionId );


        /**
         * This is the common logic used by both libraryDownload and scheduleDownload
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         * @param downloadType    whether library or schedule
         * @param command   the command to send to the train
         */
        template <typename CommandTypePtr>
        void genericDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                              unsigned char origin,
                              const TA_IRS_Bus::TtisTypes::LibraryDataType& data,
                              unsigned long version,
                              const std::string& sessionId,
                              TA_IRS_Bus::TtisTypes::ETtisDownloadType downloadType,
                              TA_IRS_Bus::TtisTypes::ETisUpdateType updateType,
                              TA_IRS_Bus::TtdFrame::EDownloadDataType dataType,
                              CommandTypePtr command,
                              const std::string& operationDescription );


        /**
         * This is the common logic used by both libraryUpgrade and scheduleUpgrade
         *
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param upgradeType    whether library or schedule
         * @param sessionId    the operator initiating the upgrade
         */
        template <typename CommandTypePtr>
        void genericUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             const std::string& sessionId,
                             TA_IRS_Bus::TtisTypes::ETtisDownloadType upgradeType,
                             TA_IRS_Bus::TtisTypes::ETisUpdateType updateType,
                             unsigned long currentVersion,
                             CommandTypePtr command,
                             const std::string& operationDescription );


        /**
         * The template function above needs to be able to handle a successful upgrade
         * for the different libraries
         *
         * @param command   the original upgrade command sent to the train
         */
        void handleSuccessfulUpgrade( TA_IRS_Bus::TtisLibraryUpgradeCommandPtr command,
                                      TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived,
                                      time_t timestamp );
        
        
        void handleSuccessfulUpgrade( TA_IRS_Bus::TtisScheduleUpgradeCommandPtr command,
                                      TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived,
                                      time_t timestamp );


        /**
         * Processes incoming end of download events
         *
         * @param event    The incoming event
         */
        void processEndOfTtisDownloadEvent( TA_IRS_Bus::EndOfTtisDownloadEventPtr event );


        /**
         * Processes incoming MPU changeover events
         *
         * @param event    The incoming event
         */
        void processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event );


        /**
         * This will perform checks to ensure that the operator action is allowed.
         *
         * @exception InvalidTrainException if the agent isnt in control of this train
         *
         * @param trainId    The train to check
         */
        void performPreDisplayTrainChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will start the TIS2 timer for the given train (or reschedule it if it is already active)
         *
         * @param trainId the train to start the timer for
         */
        void startTis2Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will stop the TIS2 timer for the given train if it is active
         *
         * @param trainId the train to stop the timer for
         */
        void stopTis2Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is the name of the TIS2 timer given to the Unsolicited Train Timer Manager
         */
        static const std::string TIS2_TIMER_TEXT;


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
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * This is used to manage the TIS2 timer scheduling, cancelling, and expiration.
         */
        TA_IRS_Bus::UnsolicitedTrainTimerManager* m_tis2TimerManager;
		
		bool m_bProcessDutyHandover;
		
		IEndDownloadCallBack * m_endDownloadCallBack;
		ITrainLibDownloadSequence * m_libDownloadSeq;

    };

}

#endif // !defined(TimsTisManager_H)
