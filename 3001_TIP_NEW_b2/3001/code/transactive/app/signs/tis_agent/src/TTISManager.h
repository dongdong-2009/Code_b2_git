#ifndef TTISMANAGER_H_INCLUDED
#define TTISMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TTISManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2015/07/09 14:25:28 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/signs/tis_agent/src/TrainAgentSubscriber.h"

#include "bus/signs/tis_agent_access/src/TrainDataVersions.h"
#include "bus/signs_4669/TisManagerCorbaDef/src/ITTISManagerCorbaDef.h"

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include <string>

namespace TA_IRS_App
{
    //enum ETrainComms { LIBRARY_DOWNLOAD, SCHEDULE_DOWNLOAD, LIBRARY_UPGRADE, SCHEDULE_UPGRADE };

    class TTISManager : public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >,
                        public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::StateUpdateMessageCorbaDef >,
                        public virtual TA_Base_Core::ServantBase,
                        public virtual POA_TA_Base_Bus::ITTISManagerCorbaDef,
                        public virtual TA_Base_Bus::IEntity,
                        public ITrainEventCallback
	{

	public:

	

		/**
		  * destructor
		  *
		  */
		virtual ~TTISManager();

		/**
		  * getInstance
		  *
		  * Gets the one and only TTISManager.
		  *
		  * @return     TTISManager&
		  *             The only TTISManager.
		  */
		static TTISManager* getInstance();


        /**
         *
         * @param EntityData
         */
        void initialise( TA_Base_Core::IEntityDataPtr EntityData );


        virtual bool isValid();


        /**
         *
         * @param configUpdateDetails
         */
        virtual void update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails );


        virtual void remove();


        virtual void start();


        virtual void stop();


		virtual void setToControlMode();
		  
		virtual void setToMonitorMode();
        
        void registerForStateUpdates();


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         *
         * @exception TA_Base_Core::OperationModeException
         */
        unsigned short getFlexibleTimeOutValue();


        virtual TA_Base_Core::TrainDownloadList* getCurrentTrainDownloads();


        virtual TA_Base_Core::TrainDataVersionList* getTrainDataVersions();


        /**
         * Gets the alarm status for each train
         *
         * @return the pre-defined and time schedule version alarms for each train.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         */
        TA_Base_Core::TrainDataVersionAlarmList* getTrainDataAlarms();


        /**
         * This method allows clients to initiate the display of a predefined message.
         * The result will be sent asynchronously back in a TTISDisplayResult message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure for a general error
         *
         * @param trains
         * @param displayParameters
         * @param timestamp
         * @param sessionId
         */
        virtual void submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trains,
                                                     const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
                                                     long timestamp,
                                                     const char* sessionId );


        /**
         * This method allows clients to initiate the display of an adhoc message.
         * The result will be sent asynchronously back in a TTISDisplayResult message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure for a general error
         *
         * @param trains
         * @param displayParameters
         * @param timestamp
         * @param sessionId
         */
        virtual void submitAdHocDisplayRequest( const TA_Base_Core::TrainList& trains,
                                                const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                long timestamp,
                                                const char* sessionId );


        /**
         * This method allows clients to initiate the clearing of a PID and its queue.
         * The result will be sent asynchronously back in a TTISDisplayResult message.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure for a general error
         *
         * @param trains
         * @param displayParameters
         * @param sessionId
         */
        virtual void submitClearRequest( const TA_Base_Core::TrainList& trains,
                                         const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                         CORBA::Long timestamp,
                                         const char* sessionId );


        /**
         * This method allows clients to initiate the download of the next predefined message library.
         * The result will be sent asynchronously back in a TrainDownloadStatus message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure for a general error
         *
         * @param trains
         * @param sessionId
         */
        virtual void downloadNextMessageLibrary( const TA_Base_Core::TrainList& trains, const char* sessionId );

		virtual void cancelDownloadMessageLibrary( const TA_Base_Core::TrainList& trains, const char* sessionId );




        /**
         * This method allows clients to send an upgrade command to inform a train to switch to the recently downloaded message library.
         * The result will be sent asynchronously back in a TrainDownloadStatus message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure for a general error
         *
         * @param trains
         * @param sessionId
         */
        virtual void upgradeMessageLibrary( const TA_Base_Core::TrainList& trains, const char* sessionId );


        /**
         *
         * @param message
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


        /**
         *
         * @param message
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message );


        /**
         * This will update the library versions stored on the train in train wakeup.
         * It will override any next version downloaded to the train.
         *
         * @param trainId    The train number
         * @param libraryVersion    The library version on the train
         * @param scheduleVersion    The time schedule version on the train
         */
        virtual void updateTrainLibraryVersions( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 unsigned long libraryVersion,
                                                 unsigned short scheduleVersion );


        /**
         * This indicates that a train dowload has started successfully.
         *
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param nextVersion
         */
        virtual void trainDownloadStarted( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType,
                                           unsigned long nextVersion );


        /**
         * This indicates that a train dowload has completed.
         *
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param success    Whether the dowload completed successfully or not
         * @param statusString    The download status string to display to the user
         * @param nextVersion    The library or schedule version downloaded to the train if successful
         */
        virtual void trainDownloadComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType,
                                            bool success,
                                            std::string statusString,
                                            unsigned long nextVersion );


        /**
         * This indicates that a train upgrade has completed.
         *
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param success    Whether the dowload completed successfully or not
         * @param statusString    The download status string to display to the user
         * @param currentVersion    The library or schedule current version upgraded on the train if successful
         */
        virtual void trainUpgradeComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType,
                                           bool success,
                                           std::string statusString,
                                           unsigned long currentVersion );


        /**
         * This notifies that a predefined message command sent to a train has completed, and its status.
         *
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void predefinedMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                       time_t timestamp,
                                                       std::string sessionId,
                                                       const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
                                                       bool success,
                                                       std::string statusString );


        /**
         * This notifies that a free text message command sent to a train has completed, and its status.
         *
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void freeTextMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     time_t timestamp,
                                                     std::string sessionId,
                                                     const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                     bool success,
                                                     std::string statusString );


        /**
         * This notifies that a clear command sent to a train has completed, and its status.
         *
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void clearRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timestamp,
                                           std::string sessionId,
                                           const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                           bool success,
                                           std::string statusString );


        /**
         * This will verify that the train has matching library and schedule versions to the ISCS versions.
         * It will also ensure that if the alarm should be raised by this agent it is, or also if the alarm should be closed it is.
         *
         * This is called when a train's version updates, or when a train moves.
         *
         * @param trainId    The train to verify
         */
        void verifyTrainDataVersions( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will verify that the all known trains have matching library and schedule versions to the ISCS versions.
         * It will also ensure that if the alarm should be raised by this agent it is, or also if the alarm should be closed it is.
         */
        void verifyAllTrainDataVersions();

    private:

        TTISManager();


        /**
         * This will subscribe to the download and version update messages from other agents.
         */
        void subscribeToNotifications( bool controlMode );


        /**
         * Gets the initial train library and schedule versions from the peer agent, then if failure OCC/Depot agent.
         */
        void getInitialTrainDataVersions();


        /**
         * Gets the initial downloads from the peer agent
         */
        void getInitialTrainDownloads();


        /**
         * Gets the initial data version alarms from the peer agent
         */
        void getInitialTrainDataVersionAlarms();


        /**
         * Sends the comms message out if this agent is in control
         *
         * @param trainData
         */
        void sendVersionUpdateMessage( const TA_Base_Core::TrainDataVersion& trainData );


        /**
         * Sends the comms message out if this agent is in control
         * Also the audit message.
         *
         * @param downloadStatus
         * @param version
         */
        void sendDownloadUpdateMessage( const TA_Base_Core::TrainDownloadStatus& downloadStatus, unsigned long version );


        /**
         * Sends the state update message out if this agent is in control
         *
         * @param update    The update message
         */
        void sendLibraryVersionAlarmUpdate( const TA_Base_Core::TrainDataVersionAlarms& update );


        /**
         * Raises a library version mismatch alarm
         *
         * @return the alarm ID
         *
         * @param trainId    The train
         * @param trainVersion    the train's library version
         * @param iscsVersion    The ISCS library version
         */
        std::string raiseLibraryVersionAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              unsigned long trainVersion,
                                              unsigned long iscsVersion );



        /**
         * Raises a library version mismatch alarm
         *
         * @return the alarm ID
         *
         * @param alarmType    the alarm type to raise
         * @param trainId    The train
         * @param trainVersion    the train's library version
         * @param iscsVersion    The ISCS library version
         */
        std::string raiseVersionAlarm( const TA_Base_Core::MessageType& alarmType,
                                       TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       unsigned long trainVersion,
                                       unsigned long iscsVersion );


        /**
         * Closes all library and schedule alarms raised previously by this agent
         */
        void closeAllLibraryVersionAlarms();


        /**
         * Closes the given alarm
         *
         * @param alarmId    The alarm to close
         */
        void closeAlarm( const std::string& alarmId );


        /**
         * Converts the TIS CORBA train list to the train proxy style train list
         *
         * @param trainList
         */
        TA_IRS_Bus::CommonTypes::TrainIdList convertTrainList( const TA_Base_Core::TrainList& trainList );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted parameters
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         *
         * @param displayParameters    TTIS CORBA display parameters
         */
        TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters convertDisplayParameters( const TA_Base_Core::TTISPredefinedMessageParameters displayParameters );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted parameters
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         *
         * @param displayParameters    TTIS CORBA display parameters
         */
        TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters convertDisplayParameters( const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted parameters
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the options given for the operation is erroneous
         *
         * @param displayParameters    TTIS CORBA display parameters
         */
        TA_IRS_Bus::TtisTypes::TtisResetParameters convertDisplayParameters( const TA_Base_Core::TTISMessageResetParameters& displayParameters );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param pidList
         */
        TA_IRS_Bus::TtisTypes::TtisPidList convertTisPidListToTrain( const TA_Base_Core::TrainPIDList& pidList );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param pid
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisPid convertTisPidToTrain( TA_Base_Core::EPIDSelection pid );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param messageType
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisResetMessageType convertTisClearTypeToTrain( TA_Base_Core::ETTISClearType messageType );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param librarySection
         */
        unsigned char convertTisLibrarySectionToTrain( TA_Base_Core::ELibrarySection librarySection );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param priority
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority convertTisMessagePriorityToTrain( TA_Base_Core::ETTISMessagePriority priority );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param priority    The message priority in int form
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority convertTisPriorityToTrain( int priority );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param startTime
         * @param endTime
         */
        TA_IRS_Bus::TtisTypes::TimeInterval convertTisTimeIntervalToTrain( const std::string& startTime, const std::string& endTime );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param fontSize
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize convertTisLedFontSizeToTrain( TA_Base_Core::ETTISLEDFontSize fontSize );
        TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize convertTisFontSizeToTrain( TA_Base_Core::ETTISLEDFontSize fontSize );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param justifcation
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisJustification convertTisJustificationToTrain( TA_Base_Core::EJustification justifcation );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param intensity
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisIntensity convertTisIntensityToTrain( TA_Base_Core::ETTISLEDIntensity intensity );


        /**
         * Converts the TIS CORBA display parameters to Train C++ display parameters
         *
         * @return the converted type
         *
         * @exception TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter if one of the parameters given is incorrect
         *
         * @param displayMode
         */
        TA_IRS_Bus::ProtocolTtisTypes::ETtisDisplayMode convertTisDisplayModeToTrain( TA_Base_Core::EDisplayMode displayMode );


        /**
         * For failover issue..
         */
		void setToControlModeInThread();//limin, for failover

        /**
         * The one and only instance of this class.
         */
        static TTISManager* m_theClassInstance;


        /**
         * Protect singleton creation
         */
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;


        mutable TA_Base_Core::ReEntrantThreadLockable m_trainDataLock;


        TA_Base_Core::ReEntrantThreadLockable m_trainDownloadsLock;


        unsigned long m_locationKey;


        TA_Base_Core::ILocation::ELocationType m_locationType;


        /**
         * Location of this agent
         */
        std::string m_locationName;


        /**
         * Name this object will use when registering with the naming service Stored as an entity parameter
         */
        std::string m_entityName;


        unsigned long m_entityKey;


        unsigned long m_entityTypeKey;


        unsigned long m_subsystemKey;


        /**
         * If this is the OCC agent, the depot location, if the depot agent, the OCC location.
         * Otherwise empty. There may be more than 1 depot, so here using vector to store the location name
         */
		std::vector<std::string> m_otherAgentLocationNames;


        /**
         * List of current downloads
         */
        std::map< unsigned char, TA_Base_Core::TrainDownload > m_trainDownloadList;


        /**
         * Container containing train data versions.
         */
        TA_IRS_Bus::TrainDataVersions m_trainDataVersions;


        /**
         * List of current alarms for data version mismatch by train
         */
        std::map< unsigned char, TA_Base_Core::TrainDataVersionAlarms > m_trainDataVersionAlarmList;


        TrainAgentSubscriber* m_trainAgentSubscriber;


        /**
         * used to send comms messages
         */
        TA_Base_Core::CommsMessageSender* m_tisMessageSender;


        /**
         * used to send comms messages
         */
        TA_Base_Core::CommsMessageSender* m_tisBroadcastMessageSender;


        /**
         * used to send state update messages
         */
        TA_Base_Core::StateUpdateMessageSender* m_tisStateUpdateMessageSender;


        TA_Base_Core::AuditMessageSender* m_auditMessageSender;


        /**
         * An interface to the local train agent TIS interface
         */
        TA_IRS_Bus::TrainTisCorbaProxyPtr m_trainTisCorbaProxy;

		//store sessionID for TrainLibrary download from trainborne manager
		std::string m_sessionID;
    };
}

#endif // TTISMANAGER_H_INCLUDED
