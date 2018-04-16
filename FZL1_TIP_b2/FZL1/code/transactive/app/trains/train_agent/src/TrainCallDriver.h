/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainCallDriver.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the CallDriver commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainCallDriver.h
//  Implementation of the Class TrainCallDriver
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#if !defined(TrainCallDriver_INCLUDED_)
#define TrainCallDriver_INCLUDED_

#include <string>

#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"

#include "app/trains/train_agent/src/TrainWorkQueue.h"
#include "ace/Synch.h"

namespace TA_IRS_App
{
	class ITrainManager;
    class callEndOfOCCCallEventReader;
    class callRequestForOCCCallEventReader;
    class callTrainCommandReceivedEventReader;
    class OperationModeManager;
    class TrainWorkQueueManager;

    /**
    * This class provides the functionality required to prepare a train to
    * receive a voice call.  This call will be directed to the driver.  The
    * class is used exclusively by the radio manager.
    *
    * This is not managing the voice call as in some cases the Radio Manager
    * might need to include ITSI that are not trains, or use include functionality.
    */
    class TrainCallDriver : public ITrainEventReaderDelegator,
                            public ITrainCallEndDelegator
	{

		public:
            // to decouple corba/c++
            enum CallDriverNotification 
            {
                CD_DriverResetRequest
            };
            
            TrainCallDriver( ITrainManager& callback,
                             ITrainNotificationFacade& notifier,
                             TrainWorkQueueManager& trainWorkQueueManager );

			virtual ~TrainCallDriver();

		public:
            bool delegate(TrainEvent& event);

            bool delegate(unsigned long callId, std::vector<unsigned char>& trains);

            //*** Servant implementation methods

            // origin OCC
            // - Send (68) Cmd Recv
            // - Setup Audio
            void setupDriverCall(unsigned char trainID, bool override, std::string sessionID);

            // origin OCC
            // - Send () End Voice Call; no reply
            void endDriverCall(unsigned char trainID, std::string sessionID);

            // origin OCC
            // - Send () ACK; no reply
            void answerDriverCall(unsigned char trainID, std::string sessionID);

            //*** Asynchronous-unsolicited messages

            // origin TIMS (63) Request for OCC Call
            // - Don't Notify OCC, no need
            // - Call Radio -> driver call ending
            void onCallEndOfOCCCall(TrainEvent & event, callEndOfOCCCallEventReader * reader);

            // origin TIMS (63) Request for OCC Call
            // - Don't Notify OCC, no need
            // - Call Radio -> driver call waiting
            void onCallRequestForOCCCall(TrainEvent & event, callRequestForOCCCallEventReader * reader);



            // parallel commands for multiple trains
            std::vector<unsigned char> setupDriverCallParallel(
                std::vector<unsigned char> trains,
                bool override,
                const std::string& sessionID,
                const std::string& groupTSI );

            void endDriverCallParallel(
                std::vector<unsigned char> trains,
                const std::string& sessionID,
                unsigned long callId );

            // This is the common code for ending a driver call for single or multiple trains
            void endDriverCallInternal(unsigned char trainID, std::string sessionID);

            // This is the common code for setting up a driver call for single or multiple trains
            // it will optionally initiate the call
            void setupDriverCallInternal(unsigned char trainID, bool override, const std::string& sessionID, bool makeCall);
            
// sync            void onCallTrainCommandReceived(TrainEvent & event, callTrainCommandReceivedEventReader * reader);

        private:

			ITrainManager& m_trainManager;
            ITrainNotificationFacade& m_notifier;
            TrainWorkQueueManager& m_workQueueManager;

//            unsigned int m_retryLimit;
            unsigned int m_timerVC1;
            unsigned int m_timerVC2;

	};



    /**
    * Starts a Driver Call.  Executes the Driver Call train command transaction with the associated train.
    */
    class SetupDriverCallCommand : public AbstractTrainWorkQueueItem
	{
	public:
		SetupDriverCallCommand( TrainCallDriver& trainCallDriver,
                                ITrainNotificationFacade& notify,
                                unsigned char trainID,
                                bool override, 
                                const std::string& sessionID,
                                ACE_Barrier& barrier,
                                ITrainManager& trainManager )
		: m_trainCallDriver(trainCallDriver),
          m_notify(notify),
          m_trainId(trainID),
          m_override(override),
          m_sessionId(sessionID),
          m_barrier(barrier),
          m_trainManager(trainManager)
		{
		}

        virtual int call();
		virtual std::string info();


	private:
        TrainCallDriver&        m_trainCallDriver;
        ITrainNotificationFacade& m_notify;
        unsigned char           m_trainId;
        bool                    m_override;
        std::string             m_sessionId;
        ACE_Barrier&            m_barrier;
        ITrainManager&          m_trainManager;
        bool                    m_success;
	};


    /**
    * Ends the Driver Call.  Executes the End Drive Call train command transaction with the associated train.
    */
    class EndDriverCallCommand : public AbstractTrainWorkQueueItem
	{
	public:
		EndDriverCallCommand( TrainCallDriver& trainCallDriver,
                              ITrainNotificationFacade& notify,
                              TrainID trainID,
                              const std::string& sessionID,
                              ACE_Barrier& barrier,
                              ITrainManager& trainManager )
		: m_trainCallDriver(trainCallDriver),
          m_notify(notify),
          m_trainId(trainID),
          m_sessionId(sessionID),
          m_barrier(&barrier),
          m_trainManager(trainManager)
		{
		}
		
		EndDriverCallCommand( TrainCallDriver& trainCallDriver,
                              ITrainNotificationFacade& notify,
                              TrainID trainID,
                              const std::string& sessionID,
                              ITrainManager& trainManager )
		: m_trainCallDriver(trainCallDriver),
          m_notify(notify),
          m_trainId(trainID),
          m_sessionId(sessionID),
          m_barrier(NULL),
          m_trainManager(trainManager)
		{
		}

        virtual int call();
		virtual std::string info();

	private:
        TrainCallDriver&        m_trainCallDriver;
        ITrainNotificationFacade& m_notify;
        TrainID                 m_trainId;
        std::string             m_sessionId;
        ACE_Barrier*            m_barrier;
        ITrainManager&          m_trainManager;
	};


};

#endif // !defined(TrainCallDriver_INCLUDED_)
