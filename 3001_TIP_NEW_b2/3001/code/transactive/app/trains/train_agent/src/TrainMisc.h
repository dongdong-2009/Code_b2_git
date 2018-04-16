/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/trains/train_agent/src/TrainMisc.h $
  * @author:  Glen Kidd
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class manages the Misc commands (eg Wakeup).  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainMisc.h
//  Implementation of the Class TrainMisc
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#if !defined(TrainMisc_INCLUDED_)
#define TrainMisc_INCLUDED_

#include <string>

#include "app/trains/train_agent/src/TrainWorkQueue.h"

#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "core/alarm/src/NonUniqueAlarmHelper.h"

namespace TA_IRS_App
{
	class ITrainManager;
	class ITrainManagerTrainEventFacade;
    class miscChangeAreaCommand; // OCC(75)
    class miscChangeAreaReceivedEventReader; // (77)
    class miscChangeoverstatusEventReader; // TIMS(65)
    class miscTestCallEventReader; // TIMS(71)
    class miscRadioModeCommand;
    class paAudioSWCabToCabEventReader;
    class paAudioSWEventReader;

//	typedef ITrainNotificationFacade::TrainAgentLocationType TrainAgentLocationType;

	/**
	* This class is responsible for handling miscellaneous train SDS messages.
	* The class also handles alarms, handles radio degraded modes, wake up
	* calls
	*/
    class TrainMisc : public ITrainEventReaderDelegator,
                      public ITrainEventListener
	{
    public: 
        // to decouple corba/c++
        enum WakeupNotification 
        {
            Wakeup_TestCall,
            Wakeup_ChangeOver
        };

		enum RadioISCSMode
		{
			Radio_Normal,
			Radio_Degraded
		};

		public:
			TrainMisc( ITrainManager& callback,
                       ITrainManagerTrainEventFacade& trainEventHandler,
                       ITrainNotificationFacade& notifier,
                       TrainTypes::TrainAgentLocationType agentLocationType,
                       unsigned long subsystemKey /* used to raise alarms */ );
			
            virtual ~TrainMisc();

		public:
            bool delegate(TrainEvent& event);

            virtual void receive(TrainEvent& event);

            //*** Servant implementation methods

            // origin (agent)
            // - Send (73) Tetra/ISCS Radio Mode; no reply expected
			/**
			* The calling agent becomes the controller. 
			* Used for degraded mode or duty delegation.
			*/
            void degradedRadioMode(unsigned char trainID, unsigned char radioMode, std::string sessionID);

            // origin (agent)
            // - Send (75) Change Area; wait for (77) Command recv.
			/**
			* Tells TIMS whether it is in depot or mainline
			*/
            void wakeupChangeArea(unsigned char trainID, unsigned char status, std::string sessionID);

            //*** Asynchronous-unsolicited messages

            // ?? move to CallDriver ?? 
            // origin TIMS (26) Audio SW on Cab to Cab
            // ??? idl doesn't specifiy notification
            void onPAAudioSWCabToCab(TrainEvent &event, paAudioSWCabToCabEventReader * reader);
            
            // origin TIMS (23) Audio Switch on PA
            // ??? idl doesn't specifiy notification
            void onPAAudioSW(TrainEvent &event, paAudioSWEventReader * reader);
            
            // origin TIMS (71) Test Call
            // - Notify OCC (???)
			/**
			* This method will be called when a train test call event is received.  It will
			* send a test call result reply to confirm receipt of the message.
			*/
            void onTestCall(TrainEvent & event, miscTestCallEventReader * reader);

            // origin TIMS (65) ChangeOver
            // - Notify OCC (???)
			/**
			* This method will be called when a train location changes.  This will handle the
			* sending of the ChangeArea (if DCC<>Mainline) or TETRA/ISCS mode (if
			* degraded<>normal) command to the train.
			*/
            void onChangeOverStatus(TrainEvent &event, miscChangeoverstatusEventReader * reader);


            /** 
              * Called when a train leaves this agents area of control
              */
            void removeTrain(unsigned int trainId);


			/**********************
			** Non-corba methods **
			**********************/


        private:

            void submitAlarm(unsigned int trainId);
            void closeAlarm(unsigned int trainId);

            // Keep a registry of messages we can listen to asynchronously, then 
            // figure out how to tell TrainEventSubscriber about them.

			ITrainManager&                      m_trainManager;
			ITrainManagerTrainEventFacade&      m_trainEventHandler;
            ITrainNotificationFacade&           m_notifier;
			TrainTypes::TrainAgentLocationType  m_agentLocationType;
			//TD18093,jianghp
            //TA_Base_Core::NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;
			//TD18093,jianghp
            unsigned long                       m_subsystem;
            
            unsigned int m_timerRadioMode;
            std::string m_locationName;
			unsigned long m_locationKey; // for alarm, hongzhi, 20080508
    };


	/**
	* This class handles a Change Mode Command
	*/
	class TrainMiscChangeModeCommand : public AbstractTrainWorkQueueItem
	{
	public:
		TrainMiscChangeModeCommand(TrainMisc & trainMisc, TrainID trainID, unsigned char radioMode, const std::string & sessionID, bool changeArea = false, unsigned char status = 0) 
			: m_trainMisc(trainMisc), m_trainId(trainID), m_radioMode(radioMode), m_session(sessionID), m_changeArea(changeArea), m_status(status)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainMisc & m_trainMisc;
		TrainID m_trainId;
		unsigned char m_radioMode;
		std::string m_session;
		bool m_changeArea;
		unsigned char m_status;
	};
};

#endif // !defined(TrainMisc_INCLUDED_)
