/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainPublicAddress.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainCCTV.h
//  Implementation of the Class TrainPublicAdress
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#if !defined(TRAINPUBLICADDRESS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
#define TRAINPUBLICADDRESS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include <string>
#include <map>
#include <vector>

#include "app/trains/train_agent/src/TrainWorkQueue.h"

//#include "bus/trains/TrainAgentCorbaDef/idl/src/IPATrainCommandsCorbaDef.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "app/trains/train_agent/src/TrainWorkQueue.h"
#include "app/trains/train_agent/src/UnsolicitedTrainTimer.h"

namespace TA_IRS_App
{
	class ITrainManager;
    class paCommandReceivedEventReader;
    class paReadyForLiveAnnouncementEventReader;
    class paRequestResetEventReader;
    class paResetEventReader;
    class paTrainMessageCompletedEventReader;
    class OperationModeManager;

	/**
	* This class provides the functionality required to complete a Public
	* Announcement event. All Train commands are performed via the Radio
	* Agent.  There are 4 types of Public Announcements, Live, Broadcast, 
	* Cyclic and Prerecorded.
	*/
    class TrainPublicAddress : public ITrainEventReaderDelegator,
                               public ITrainCallEndDelegator,
                               public IUnsolicitedTimerCallback
    {

    public:

		/**
        * to decouple corba/c++
		*/
        struct PATimeInterval 
        {
            unsigned char startHour;
            unsigned char startMin;
            unsigned char stopHour;
            unsigned char stopMin;
        };

		/**
		* Used to encode/decode a PA notification
		*/
        enum PANotification 
        {
            PA_ReadyForLiveAnnouncement,
            PA_ContinueLiveAnnouncement,
            PA_LiveAnnouncementReset,
            PA_CompletedAnnouncement,
            PA_InteruptedAnnouncement,
            PA_ContinueTimeoutAnnouncement,
            PA_CommandResultAnnouncement
        };

		/**
		* Used to encode/decode a Live PA notification
		*/
        enum PALiveAnnouncementNotification 
        {
            PA_CallLostAnnouncement
        };

        TrainPublicAddress( ITrainManager& callback,
                            ITrainNotificationFacade& notifier,
                            TrainWorkQueueManager& trainWorkQueueManager);

        virtual ~TrainPublicAddress();
//        TrainPublicAddress(const TrainPublicAddress& theTrainPublicAddress);

	public:
        bool delegate(TrainEvent& event);
        bool delegate(unsigned long callId, std::vector<unsigned char>& trains);

        // parallel calls for multiple train operations

        void paPreRecordedAnnouncementParallel(
            const std::vector<unsigned char>& trainList,
            bool override, 
            unsigned char tableOffset, 
            unsigned short libraryVersion, 
            unsigned char announcementID,
            OperationModeManager& operationModeManager,
            std::string sessionID
            );

        void paCyclicAnnouncementParallel(
            const std::vector<unsigned char>& trainList,
            bool override,
            unsigned char tableOffset,
            unsigned short libraryVersion,
            const PATimeInterval& announcementPeriod,
            unsigned char announcementID,
            OperationModeManager& operationModeManager,
            std::string sessionID);

        void paSetupLiveAnnouncementParallel(
            const std::vector<unsigned char>& trainList,
            bool override, 
            unsigned char announcementID,
            OperationModeManager& operationModeManager,
            std::string sessionID);

        void paEndLiveAnnouncementParallel(
            unsigned char announcementId,
            std::string sessionID);

        void paEndBroadcastParallel(
            const std::vector<unsigned char>& trainList,
            unsigned char announcementId,
            std::string sessionID);


        //*** Servant implementation methods

        // origin OCC
        // - send (12) Prerecorded, wait for (19) Cmd Recv
        // - update trainStatus: PA library version
        void paPreRecordedAnnouncement(
            unsigned char trainID, 
            bool override, 
            unsigned char tableOffset, 
            unsigned short libraryVersion, 
            unsigned char announcementID,
            std::string sessionID
            );

        // origin OCC
        // - send (25) Cyclic, wait for (19) Cmd Recv
        void paCyclicAnnouncement(
            unsigned char trainID,
            bool override,
            unsigned char tableOffset,
            unsigned short libraryVersion,
            const PATimeInterval& announcementPeriod,
            unsigned char announcementID,
            std::string sessionID);

        void paSetupLiveAnnouncement(
            unsigned char trainID,
            bool override, 
            unsigned char announcementID,
            std::string sessionID);

        void paBeginLiveAnnouncement(unsigned char announcementId, std::string sessionID);

        // origin OCC
        // - send (18) Continue, no reply
        void paContinueLiveAnnouncement(unsigned char trainID, std::string sessionID);

        void paEndLiveAnnouncement(unsigned char trainID, std::string sessionID);
        void paEndBroadcast(unsigned char trainID, unsigned char announcementID, std::string sessionID);

          
        //*** Asynchronous-unsolicited messages

// not async       void onPACommandReceived(TrainEvent &event, paCommandReceivedEventReader * reader);

        // origin TIMS (20) Ready For Live Announcement
        // ? no notification to send
        // ? signal setup/begin idl methods so they can stop timers and return successfully.
        void onPAReadyForLiveAnnouncement(TrainEvent &event, paReadyForLiveAnnouncementEventReader * reader);

        // origin TIMS (22) Reset Req
        // - Update TrainStatus - manage retries
        // - Notify OCC (PAContinueLive) on 3rd retry
        void onPARequestReset(TrainEvent &event, paRequestResetEventReader * reader);

        // origin TIMS (14) Reset
        // - Notify OCC (PAResetLive)
        void onPAReset(TrainEvent &event, paResetEventReader * reader);

        // origin TIMS (24) PA Message Completed
        // ? Nothing to do - no train status kept, no notification type in idl
        void onPATrainMessageCompleted(TrainEvent &event, paTrainMessageCompletedEventReader * reader);


        // expiry of PA3 timer
        virtual void handleTimeout(unsigned char trainId);

    private:

        // starts the PA 3 timer, resets it if it is running
        void startPA3Timer(unsigned char trainId);
        // stops the PA 3 timer, no op if not running
        void stopPA3Timer(unsigned char trainId);
        // called when the PA 3 times out
        void onPA3Timeout(unsigned char trainId);

		ITrainManager& m_trainManager;
        ITrainNotificationFacade& m_notifier;

//        unsigned int m_retryLimit;
        unsigned int m_timerPA1;
        unsigned int m_timerPA2;
        unsigned int m_timerPA3;
        unsigned int m_timerPA4;

		/**
		* Control concurrent access to the stored information
		*/
		TA_Base_Core::NonReEntrantThreadLockable   m_ITSICollectionLock;
		std::map<std::string, std::vector<std::string> > m_ITSICollection;

        TrainWorkQueueManager& m_workQueueManager;

        UnsolicitedTrainTimer m_pa3TimerManager;
    };


    // These PA commands are executed in parallel for multi train requests


	/**
	* Starts a PA PreRecorded Announcement.  Executes the PA PreRecorded
	* Announcement train command transaction with the associated train.
	*/
    class PaPreRecordedAnnouncementCommand : public AbstractTrainWorkQueueItem
	{
	public:
		PaPreRecordedAnnouncementCommand( TrainPublicAddress& trainPublicAddress,
                                          ITrainNotificationFacade& notify,
                                          TrainID trainID,
                                          bool override, 
                                          unsigned char tableOffset, 
                                          unsigned short libraryVersion, 
                                          unsigned char announcementID,
                                          OperationModeManager& operationModeManager,
                                          const std::string& sessionID,
                                          ACE_Barrier& barrier )
			: m_trainPublicAddress(trainPublicAddress),
              m_notify(notify),
              m_trainId(trainID),
              m_override(override),
              m_tableOffset(tableOffset),
              m_libraryVersion(libraryVersion),
              m_announcementID(announcementID),
              m_operationModeManager(operationModeManager),
              m_sessionId(sessionID),
              m_barrier(barrier)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainPublicAddress& m_trainPublicAddress;
        ITrainNotificationFacade& m_notify;
        TrainID                 m_trainId;
        bool                    m_override;
        unsigned char           m_tableOffset;
        unsigned short          m_libraryVersion;
        unsigned char           m_announcementID;
        OperationModeManager&   m_operationModeManager;
        std::string             m_sessionId;
        ACE_Barrier&            m_barrier;
	};

	/**
	* Starts a PA Cyclic Announcement.  Executes the PA Cyclic Announcement
	* train command transaction with the associated train.
	*/
    class PaCyclicAnnouncementCommand : public AbstractTrainWorkQueueItem
	{
	public:
		PaCyclicAnnouncementCommand( TrainPublicAddress& trainPublicAddress,
                                     ITrainNotificationFacade& notify,
                                     TrainID trainID,
                                     bool override, 
                                     unsigned char tableOffset, 
                                     unsigned short libraryVersion,
                                     const TrainPublicAddress::PATimeInterval& announcementPeriod,
                                     unsigned char announcementID,
                                     OperationModeManager& operationModeManager,
                                     const std::string& sessionID,
                                     ACE_Barrier& barrier )
		: m_trainPublicAddress(trainPublicAddress),
          m_notify(notify),
          m_trainId(trainID),
          m_override(override),
          m_tableOffset(tableOffset),
          m_libraryVersion(libraryVersion),
          m_announcementPeriod(announcementPeriod),
          m_announcementID(announcementID),
          m_operationModeManager(operationModeManager),
          m_sessionId(sessionID),
          m_barrier(barrier)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainPublicAddress& m_trainPublicAddress;
        ITrainNotificationFacade& m_notify;
        TrainID                 m_trainId;
        bool                    m_override;
        unsigned char           m_tableOffset;
        unsigned short          m_libraryVersion;
        TrainPublicAddress::PATimeInterval      m_announcementPeriod;
        unsigned char           m_announcementID;
        OperationModeManager&   m_operationModeManager;
        std::string             m_sessionId;
        ACE_Barrier&            m_barrier;
	};

	/**
	* Starts a PA Live Announcement.  Executes the PA Live Announcement train
	* command transaction with the associated train.
	*/
    class PaSetupLiveAnnouncementCommand : public AbstractTrainWorkQueueItem
	{
	public:
		PaSetupLiveAnnouncementCommand( TrainPublicAddress& trainPublicAddress,
                                        ITrainNotificationFacade& notify,
                                        TrainID trainID,
                                        bool override, 
                                        unsigned char announcementID,
                                        OperationModeManager& operationModeManager,
                                        const std::string& sessionID )
		: m_trainPublicAddress(trainPublicAddress),
          m_notify(notify),
          m_trainId(trainID),
          m_override(override),
          m_announcementID(announcementID),
          m_operationModeManager(operationModeManager),
          m_sessionId(sessionID)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainPublicAddress&     m_trainPublicAddress;
        ITrainNotificationFacade& m_notify;
        TrainID                 m_trainId;
        bool                    m_override;
        unsigned char           m_announcementID;
        OperationModeManager&   m_operationModeManager;
        std::string             m_sessionId;
	};

	/**
	* Ends a PA Live Announcement.  Executes the End PA Live Announcement train
	* command transaction with the associated train.
	*/
    class PaEndLiveAnnouncementCommand : public AbstractTrainWorkQueueItem
	{
	public:
        // used when operator triggered. waits and provides feedback
		PaEndLiveAnnouncementCommand( TrainPublicAddress& trainPublicAddress,
                                      ITrainNotificationFacade& notify,
                                      TrainID trainID,
                                      unsigned char announcementID,
                                      const std::string& sessionID,
                                      ACE_Barrier* barrier = NULL )
		: m_trainPublicAddress(trainPublicAddress),
          m_notify(notify),
          m_trainId(trainID),
          m_announcementID(announcementID),
          m_sessionId(sessionID),
          m_barrier(barrier)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainPublicAddress& m_trainPublicAddress;
        ITrainNotificationFacade& m_notify;
        TrainID             m_trainId;
        unsigned char       m_announcementID;
        std::string         m_sessionId;
        ACE_Barrier*        m_barrier;
	};

	/**
	* Ends a PA broadcast.  Executes the End PA Broadcast train command
	* transaction with the associated train.
	*/
    class PaEndBroadcastCommand : public AbstractTrainWorkQueueItem
	{
	public:
        // used when operator triggered. waits and provides feedback
		PaEndBroadcastCommand( TrainPublicAddress& trainPublicAddress,
                              ITrainNotificationFacade& notify,
                              TrainID trainID,
                              unsigned char announcementID,
                              const std::string& sessionID,
                              ACE_Barrier* barrier = NULL )
		: m_trainPublicAddress(trainPublicAddress),
          m_notify(notify),
          m_trainId(trainID),
          m_announcementID(announcementID),
          m_sessionId(sessionID),
          m_barrier(barrier)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainPublicAddress& m_trainPublicAddress;
        ITrainNotificationFacade& m_notify;
        TrainID             m_trainId;
        unsigned char       m_announcementID;
        std::string         m_sessionId;
        ACE_Barrier*        m_barrier;
	};

};
#endif // !defined(TRAINPUBLICADDRESS_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
