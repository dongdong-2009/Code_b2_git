/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date: 2011-01-21 10:00:00$
  * Last modified by:  $Author: Jian Huang $
  *
  */
#if !defined(PaTasks_H)
#define PaTasks_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainPaCorba.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/threads/src/IWorkItem.h"

// forward declarations
namespace TA_IRS_App
{
    class ITimsPaManagerTaskCallback;
    class IRadioGroupCoordinator;
}

namespace TA_IRS_App
{

    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:27 PM
     */
    class ContinueLiveAnnouncementTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train to continue
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        ContinueLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                      TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                      const std::string& sessionId,
                                      unsigned char origin );


        /**
         * Destructor
         */
        virtual ~ContinueLiveAnnouncementTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        ContinueLiveAnnouncementTask();


        /**
         * callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train to continue
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the live announcement to continue
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * The operator making the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;
    };


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:28 PM
     */
    class CyclicAnnouncementTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
                                    If different from the version on the train the boradcast will not be done.
         * @param announcementPeriod    This is the time parameters to broadcast the message
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operators console origin
         */
        CyclicAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                //unsigned char messageNumber,
                                unsigned short messageNumber,
                                //unsigned short libraryVersion,
                                unsigned long libraryVersion,
                                const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                const std::string& sessionId,
                                unsigned char origin );


        /**
         * Destructor
         */
        virtual ~CyclicAnnouncementTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        CyclicAnnouncementTask();


        /**
         * callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train to continue
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the live announcement to continue
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * This is the message number to boradcast. (Table offset)
         */
        //unsigned char m_messageNumber;
        unsigned short m_messageNumber;


        /**
         * This is the library version used to choose the message number.
         * If different from the version on the train the boradcast will not be done.
         */
        //unsigned short m_libraryVersion;
        unsigned long m_libraryVersion;


        /**
         * This is the time parameters to broadcast the message
         */
        TA_IRS_Bus::PaTypes::MessageSchedule m_announcementPeriod;


        /**
         * Whether to set override, or leave this as a normal request
         */
        const TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode m_requestMode;


        /**
         * The operator submitting the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;
    };


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:28 PM
     */
    class EndBroadcastTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         * @param origin    The operators console origin
         */
        EndBroadcastTask( ITimsPaManagerTaskCallback& callback,
                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                          TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                          const std::string& sessionId,
                          unsigned char origin );


        /**
         * Destructor
         */
        virtual ~EndBroadcastTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        EndBroadcastTask();


        /**
         * callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train to continue
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the live announcement to continue
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * The operator making the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;

    };


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:28 PM
     */
    class EndLiveAnnouncementTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         * @param origin    The operators console origin
         */
        EndLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                 TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                 TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                 const std::string& sessionId,
                                 unsigned char origin );


        /**
         * Destructor
         */
        virtual ~EndLiveAnnouncementTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        EndLiveAnnouncementTask();


        /**
         * callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train to continue
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the live announcement to continue
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * The operator making the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;
    };


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:28 PM
     */
    class PreRecordedAnnouncementTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train to execute the command on
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operator origin for the command
         */
        PreRecordedAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                     TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                     //unsigned char messageNumber,
                                     unsigned short messageNumber,
                                     //unsigned short libraryVersion,
                                     unsigned long libraryVersion,
                                     TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                     const std::string& sessionId,
                                     unsigned char origin,
									 TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority);


        /**
         * Destructor
         */
        virtual ~PreRecordedAnnouncementTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        PreRecordedAnnouncementTask();


        /**
         * callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train to continue
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the live announcement to continue
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * This is the message number to boradcast. (Table offset)
         */
        //unsigned char m_messageNumber;
        unsigned short m_messageNumber;


        /**
         * This is the library version used to choose the message number.
         * If different from the version on the train the boradcast will not be done.
         */
        //unsigned short m_libraryVersion;
        unsigned long m_libraryVersion;


        /**
         * Whether to set override, or leave this as a normal request
         */
        const TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode m_requestMode;


        /**
         * The operator submitting the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;

		/**
         * The message priority
         */
		const TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority m_messagePriority;
    };


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:29 PM
     */
    class SetupLiveAnnouncementMonitorTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param radioGroupCoordinator    USed to lock/unlock radio groups
         * @param numberOfTrains    How many trains to expect a response from
         * @param radioGroup    The radio group to be unlocked in the event of failure
         */
        SetupLiveAnnouncementMonitorTask( IRadioGroupCoordinator& radioGroupCoordinator,
                                          unsigned long numberOfTrains,
                                          const std::string& radioGroup );


        /**
         * Destructor
         */
        virtual ~SetupLiveAnnouncementMonitorTask();


        /**
         * This is used to signal that a train has successfully set up its live announcement
         */
        void signalSetupLiveAnnouncementSuccess();


        /**
         * This is used to signal that a train has failed to set up its live announcement
         */
        void signalSetupLiveAnnouncementFailure();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        SetupLiveAnnouncementMonitorTask();


        /**
         * Used to unlock the radio group if there are no successful trains
         */
        IRadioGroupCoordinator& m_radioGroupCoordinator;


        /**
         * The number of trains with a successful response
         */
        unsigned long m_numberOfTrainsSuccessful;


        /**
         * The radio group to unlock if all trains fail
         */
        std::string m_radioGroup;


        /**
         * This is used to wait on until all responses are received
         */
        TA_Base_Core::SingleThreadBarrier m_barrier;
    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:29 PM
     */
    typedef boost::shared_ptr< SetupLiveAnnouncementMonitorTask > SetupLiveAnnouncementMonitorTaskPtr;


    /**
     * This is a task used to setup a live announcement for a single train.
     * Many of these will be used in parallel for multi train operations.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:29 PM
     */
    class SetupLiveAnnouncementTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param callback    callback for task execution
         * @param trainId    The train the command is for
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param requestMode    Whether to set override, or leave this as a normal request
         * @param staticGroup    The optional group TSI to use to call the trains for the live announcement.
         *                       Empty string for a single train announcement, or for a DGNA.
         *                       If this is set for a single train, it will be ignored.
         * @param sessionId    The operator submitting the broadcast
         * @param origin    The operators console origin
         * @param monitorTask    Optionally the task to monitor the status of the setup,
         *                       and clear the radio group lock if all trains fail
         */
        SetupLiveAnnouncementTask( ITimsPaManagerTaskCallback& callback,
                                   TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                   const TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                   const std::string& staticGroup,
                                   const std::string& sessionId,
                                   unsigned char origin,
                                   SetupLiveAnnouncementMonitorTaskPtr monitorTask );


        /**
         * Destructor
         */
        virtual ~SetupLiveAnnouncementTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        SetupLiveAnnouncementTask();


        /**
         * Callback for task execution
         */
        ITimsPaManagerTaskCallback& m_callback;


        /**
         * The train the command is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The ID of the announcement for tracking purposes
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_announcementId;


        /**
         * Whether to set override, or leave this as a normal request
         */
        TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode m_requestMode;


        /**
         * The optional group TSI to use to call the trains for the live announcement.
         * Empty string for a single train announcement, or for a DGNA.
         * If this is set for a single train, it will be ignored.
         */
        std::string m_staticGroup;


        /**
         * The operator submitting the broadcast
         */
        std::string m_sessionId;


        /**
         * The operators console origin
         */
        unsigned char m_origin;


        /**
         * Optionally the task to monitor the status of the setup,
         * and clear the radio group lock if all trains fail
         */
        SetupLiveAnnouncementMonitorTaskPtr m_monitorTask;
    };

}

#endif // !defined(PaTasks_H)
