/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

///////////////////////////////////////////////////////////
//  TrainManager.h
//  Implementation of the Class TrainManager
//  Created on:      26-Nov-2003 02:56:57 PM
///////////////////////////////////////////////////////////

#if !defined(TrainManager_CE173ADC_F421_4857_A775_3DFC6273F1B5__INCLUDED_)
#define TrainManager_CE173ADC_F421_4857_A775_3DFC6273F1B5__INCLUDED_

#include <string>
#include <set>
#include <map>

#include "app/trains/train_agent/src/TrainTransactionManager.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainManager.h"
#include "app/trains/train_agent/src/ITrainManagerTrainEventFacade.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"


/**
 * This class is responsible for sending all trainborne commands and managing and
 * trainborne responses.  It also provides the interface to client applications
 * through the published IDL.  It hides the underlying protocol from clients.
 */
namespace TA_IRS_App
{
	class TrainCommand;
	class ITrainAgent;
	class ITrainStatusUpdateMonitor;
    class TrainTimer;

	class TrainManager : public ITrainManager, public ITrainManagerTrainEventFacade
	{
		public:
			TrainManager(ITrainAgent& callback,ITrainStatusUpdateMonitor& statusUpdateMonitor, ITrainNotificationFacade& notifier);
			virtual ~TrainManager();

		public:

            virtual void addListener(ITrainEventListener *listener);

            /**
            * @param forceFront only set to true if want to ensure messages are handled earlier (eg. Alarm events). 
            *        No guarantee they will really be first.
            */
            virtual void addDelegate(ITrainEventReaderDelegator *delegate, bool priorityEvent = false); 
			virtual void addDelegate(ITrainATSDelegator *delegate, bool priorityEvent = false);
			virtual void addDelegate(ITrainStateUpdateDelegator *delegate, bool priorityEvent = false);
			virtual void addDelegate(ITrainDutyDelegator *delegate, bool priorityEvent = false);
            virtual void addDelegate(ITrainCallEndDelegator *delegate, TrainStatus::ResourceCategory resource);
            virtual void addDelegate(ITrainDownloadDelegator *delegate);

			void onInvalidTrainEvent(TrainID t); /// TIP5-PROTO called if we failed to correctly initialise a train event.
            void onTrainEvent(TrainEvent& event);
            void onATSEvent(ATSTrainMessage & event);
			void onTrainStateUpdate(std::vector<TrainStatus::TrainInformation> & stateUpdate);
			void onDutyEvent(DutyTrainMessage & event);
            void onTrainLibraryDownloadEvent(unsigned char trainId, bool success, const std::string& failReason);

            void onCallEndEvent(unsigned long callId);

//            void setTrainDetails(unsigned int trainID, unsigned long currentLocation, std::string serviceNumber, bool inBound, bool isMute);

		private:
// delegated to TrainMisc			void onTrainLocationChange();
			void onTrainTSI(unsigned int trainId, std::string subscriberId, unsigned char origin);
//			void onRequestCallDriverEvent();
// delegated to TrainMisc			void onTIMSChangeoverEvent();
// delegated to TrainMisc			void onTrainTestCallEvent();


		public:
			//**********************************
			// ITrainManager interface methods *
			//**********************************

			virtual void sendTrainCommandNoReply(unsigned char trainID, TrainCommand& trainCommand);
            // note that if the retry limit is 0, the default will be used.
            // the same applies for the timeout
            virtual TrainEventPtr sendTrainCommandWithWait(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis = 0, unsigned int retryLimit = 0);
            virtual TrainEventPtr sendTrainCommandWithWaitNoRetries(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis = 0);
			virtual TA_Base_Core::TrainSettingsEntityDataPtr getGlobalTrainAgentEntity();
			virtual TA_Base_Core::TrainAgentEntityDataPtr getLocalTrainAgentEntity();
			virtual TrainStatusManager& getTrainStatusManager();
            virtual TrainTransactionManager& getTrainTransactionManager();
            virtual TrainTimer & getTrainTimer();

        private:

			TrainManager(const TrainManager& theTrainManager);

            typedef std::vector<ITrainEventListener*> ListenerCollection;
            ListenerCollection m_eventListeners;

            typedef std::vector<ITrainEventReaderDelegator*> delegatorCollection;
            delegatorCollection m_priorityEventReaderDelegators;
            delegatorCollection m_eventReaderDelegators;
			ITrainATSDelegator* m_atsDelegator;
			ITrainStateUpdateDelegator* m_stateUpdateDelegator;
			ITrainDutyDelegator* m_dutyDelegator;
            ITrainCallEndDelegator* m_calldriverDelegator;
            ITrainCallEndDelegator* m_pecDelegator;
            ITrainCallEndDelegator* m_paDelegator;
            ITrainDownloadDelegator* m_downloadDelegator;

			TrainTransactionManager m_trainTransactionManager;
 			ITrainAgent&            m_trainAgent;
			TrainStatusManager      m_trainStatusManager;

			std::map<std::string, unsigned char> m_origin;

		    TrainTimer * m_trainTimer;
			unsigned int m_defaultRetryLimit;

			TrainEventPtr TrainManager::sendTrainCommand(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis, unsigned int retryLimit, bool wait);
	};
};
#endif // !defined(TrainManager_CE173ADC_F421_4857_A775_3DFC6273F1B5__INCLUDED_)
