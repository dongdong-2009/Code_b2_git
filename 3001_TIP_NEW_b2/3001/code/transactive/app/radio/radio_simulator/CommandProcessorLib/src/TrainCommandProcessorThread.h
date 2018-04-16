/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\TrainCommandProcessorThread.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/08 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to process the incoming sds message and send a response based on the 
  * corresponding train command.
  */

#ifndef _TRAIN_COMMAND_PROCESSOR_THREAD_H
#define _TRAIN_COMMAND_PROCESSOR_THREAD_H
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "boost/shared_ptr.hpp"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"

#include "app/radio/radio_simulator/TrainSimulator/IModelLogger.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/IPACMData.h"
#include "app/radio/radio_simulator/TrainSimulator/FrameCheckSequence.h"
#include "app/radio/radio_simulator/TrainSimulator/GlobalDataTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ITimerResponse.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimerStrategyFactory.h"

#include "core/threads/src/IWorkItem.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

#include <map>
#include <string>

namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

	/**
     * This is an interface called by the queued Train Command tasks
     * to process the incoming data
     */
	class ITrainCommandTask
	{
	public:
		virtual void processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage) = 0;

	};


	template<class T>

	class TrainCommandProcessorThread : public TA_Base_Core::Thread, ITrainCommandTask
	{
	public:
		
		/**
		 * Construct
		 */
		TrainCommandProcessorThread(IPACMData& paCMData);

		/**
		 * Destructor
		 */
		~TrainCommandProcessorThread();

		/**
		 * This is the sds message processing thread, it waits on a timed semaphore
		 * (m_incomingMessageSemaphore) until either a new message is received or the
		 * semaphore times out.
		 */
		virtual void run();

		/**
		 * Ends the thread, only ever called on agent shutdown.
		 */
		virtual void terminate();

		/**
		 * setDataSender
		 */
		void setDataSender(IRadioCommsObserver* radioCommsObserver);

		void registerLogger(IModelLogger* logger);

		typedef std::map<int, TrainStrategyInfo> TrainStrategyMap;

		typedef std::map<int, ITimerResponse*> TimerResponseMap;

		void addTrainResponseStrategy(const TrainStrategyInfo& info);

		std::map<int, TrainStrategyInfo> getTrainStrategyMap();

		void removeTrainResponseStrategy(int trainID);

		

	protected:
		/**
		 * addProcessItem
		 *
		 * Adds an item to the incoming queue, and signal the semaphore to wake it up
		 *
		 * @param: boost::shared_ptr<T>& sdsMessage
		 */
		void addTrainCommand(boost::shared_ptr<T>& sdsMessage);

		/**
		 * processTrainCommand
		 *
		 * Process the item in the queue, implemented by child classes to provide
		 * implementation specific processing.
		 * 
		 * @param: boost::shared_ptr<T>& sdsMessage
		 */
		//virtual void processTrainCommand(boost::shared_ptr<T>& sdsMessage) = 0;

		/**
		 * Empties all items from the queue. Used when a connection is re-initialising etc.
		 * 
		 */
		void clearCache();

		/**
		 * processSendData
		 */
		void processSendData(int userDataIndex, std::vector<unsigned char>& resp, const std::vector<unsigned char>& pData);

		virtual std::string processorName() const = 0;

		IRadioCommsObserver* m_radioCommsObserver;

		IModelLogger* m_logger;

		IPACMData& m_cmData;

		TA_Base_Core::ReEntrantThreadLockable m_SDSDataLock;


		TrainStrategyMap m_trainStrategyMap;
		TimerResponseMap m_timerResponseMap;

		void applyTimerStategy(int trainID);

		TrainStrategyType getStrategyType(int trainID);

		

	private:
		/**
		 * Disable copy and assignment operator
		 */
		TrainCommandProcessorThread ( const TrainCommandProcessorThread& );
		TrainCommandProcessorThread& operator= ( const TrainCommandProcessorThread& );

		/**
		 * Indicates whether the thread is running or not.
	     */
		volatile bool m_running;

		TA_Base_Core::Semaphore m_incomingMessageSemaphore;
		TA_Base_Core::ReEntrantThreadLockable m_incomingMessageCacheLock;

		/**
		 * The list of SDSMessageData to process
		 */
		std::vector< boost::shared_ptr<T> > m_messageCache;

		TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;


	};

	template<class T>
	TrainCommandProcessorThread<T>::TrainCommandProcessorThread(IPACMData& paCMData)
		: m_incomingMessageSemaphore(0),
		  m_running(true),
		  m_radioCommsObserver(NULL),
		  m_logger(NULL),
		  m_cmData(paCMData),
		  m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
	{
		FUNCTION_ENTRY("TrainCommandProcessorThread");
		FUNCTION_EXIT;
	}

	template<class T>
	TrainCommandProcessorThread<T>::~TrainCommandProcessorThread()
	{
		FUNCTION_ENTRY("~TrainCommandProcessorThread");

		TimerResponseMap::iterator iter = m_timerResponseMap.begin();

		for ( iter; iter != m_timerResponseMap.end(); iter ++)
		{
			if ( NULL != (*iter).second )
			{
				delete (*iter).second;
				(*iter).second = NULL;
			}
		}

		m_timerResponseMap.clear();

		TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
		m_threadPool = NULL;

		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::registerLogger(IModelLogger* logger)
	{
		m_logger = logger;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::addTrainCommand(boost::shared_ptr<T>& sdsMessage)
	{
		FUNCTION_ENTRY("addProcessItem");
		//Adds the item in the messagequeue

		{
			TA_Base_Core::ThreadGuard guard(m_incomingMessageCacheLock);
			m_messageCache.push_back(sdsMessage);
		}
		
		m_incomingMessageSemaphore.post();
	
		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::clearCache()
	{
		FUNCTION_ENTRY("clearQueue");
		{
			TA_Base_Core::ThreadGuard guard(m_incomingMessageCacheLock);
			m_messageCache.clear();
		}
		
		FUNCTION_EXIT;
	}
	
	template<class T>
	void TrainCommandProcessorThread<T>::run()
	{
		FUNCTION_ENTRY("run");

		m_running = true;

		while(true == m_running)
		{
			{
 				TA_Base_Core::ThreadGuard guardForMessageCache(m_incomingMessageCacheLock);
				std::vector<boost::shared_ptr<T>>::iterator iter = m_messageCache.begin();
				for ( iter; iter != m_messageCache.end(); iter++ )
				{
					//processTrainCommand(*iter);
					TA_Base_Core::IWorkItemPtr workItem(new TrainCommandTask(*this,*iter));
					m_threadPool->queueWorkItem(workItem);
					
				}
				clearCache();
			}
			//Make sure that cache is clean after processing the Train command
			//clearCache();
			m_incomingMessageSemaphore.wait();
			
			
		}

		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_running = false;
		//terminateAndWait();

		m_incomingMessageSemaphore.post();

		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::setDataSender(TA_IRS_App::IRadioCommsObserver *radioCommsObserver)
	{
		FUNCTION_ENTRY("setDataSender");

		m_radioCommsObserver = radioCommsObserver;

		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::processSendData(int userDataIndex, std::vector<unsigned char>& resp, const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("processSendData");

		std::vector<unsigned char> rawData;
		std::vector<unsigned char> tempData;
		
		rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
		
		unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
		tempData = CMHelper::encode_u16(calcCRC);
		//resp.insert(resp.end(), tempData.begin(), tempData.end());
		resp.insert(resp.end(), tempData.at(1));
		resp.insert(resp.end(), tempData.at(0));

		// calculate TL Length
		int decLength = (resp.size()-userDataIndex +6 )*8;

		std::vector<unsigned char>::iterator iter;
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
		resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
		resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
		// Adaptation length, 2 bytes at beginning
		unsigned int nAdaptation = resp.size();
		tempData = CMHelper::encode_u16(nAdaptation);
		resp.insert( resp.begin(), tempData.begin(), tempData.end() );

		char _log[1024] = {0};
		std::sprintf(_log,"%s : Sending ISCS Response to Radio Agent\n", processorName().c_str());
		m_logger->setLogView(_log);
		m_cmData.logger(resp);
		// send the data
		if ( NULL != m_radioCommsObserver )
		{
			m_radioCommsObserver->sendData(resp);
		}
		


		FUNCTION_EXIT;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::addTrainResponseStrategy(const TrainStrategyInfo &info)
	{
		TrainStrategyMap::iterator iter = m_trainStrategyMap.find(info.TrainID);

		if ( iter == m_trainStrategyMap.end())
		{
			m_trainStrategyMap.insert(std::pair<int,TrainStrategyInfo>(info.TrainID, info));
		}


		TimerResponseMap::iterator iterTimer = m_timerResponseMap.find(info.TrainID);
		if ( iterTimer == m_timerResponseMap.end() )
		{
			ITimerResponse* timerResponseStrategy = TimerStrategyFactory::makeTimerStrategy( info.TrainID,
																							 info.StrategyType,
																							 info.TimerSet );
			m_timerResponseMap.insert(std::pair<int,ITimerResponse*>(info.TrainID,timerResponseStrategy));
		}
	}

	template<class T>
	std::map<int, TrainStrategyInfo> TrainCommandProcessorThread<T>::getTrainStrategyMap()
	{
		return m_trainStrategyMap;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::applyTimerStategy(int trainID)
	{
		TimerResponseMap::iterator iter = m_timerResponseMap.find(trainID);

		if ( iter != m_timerResponseMap.end() )
		{
			(*iter).second->waitingTime();
		}
	}

	template<class T>
	TrainStrategyType TrainCommandProcessorThread<T>::getStrategyType(int trainID)
	{
		TrainStrategyType _ret = TIMER_DEFAULT;
		TrainStrategyMap::iterator iter = m_trainStrategyMap.find(trainID);

		if ( iter != m_trainStrategyMap.end() )
		{
			_ret = (*iter).second.StrategyType;
		}

		return _ret;
	}

	template<class T>
	void TrainCommandProcessorThread<T>::removeTrainResponseStrategy(int trainID)
	{
		TrainStrategyMap::iterator iter = m_trainStrategyMap.find(trainID);
		//remove from the map
		if ( iter != m_trainStrategyMap.end() )
		{
			m_trainStrategyMap.erase(iter);
		}

		//remove the timer map
		TimerResponseMap::iterator iterTimer = m_timerResponseMap.find(trainID);
		if ( iterTimer != m_timerResponseMap.end() )
		{
			delete (*iterTimer).second;
			(*iterTimer).second = NULL;
			m_timerResponseMap.erase(iterTimer);
		}

	}

	/**
     * Task to process incoming events simultaneously
     */
	
	class TrainCommandTask : public TA_Base_Core::IWorkItem
	{
	public:
		TrainCommandTask( ITrainCommandTask& trainCommand, boost::shared_ptr<SDSMessageData>& sdsMessage )
			: m_trainCommandTask(trainCommand)
			, m_sdsData(sdsMessage){}

		virtual ~TrainCommandTask(){};

		virtual void executeWorkItem(){ m_trainCommandTask.processTrainCommand(m_sdsData); }

	private:
		TrainCommandTask();

		ITrainCommandTask& m_trainCommandTask;

		boost::shared_ptr<SDSMessageData> m_sdsData;

	};
}
#endif