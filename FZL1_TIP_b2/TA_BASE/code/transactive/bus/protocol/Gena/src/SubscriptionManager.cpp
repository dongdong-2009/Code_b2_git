
#include "SubscriptionManager.h"
#include "GenaCallbackProccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "SIDNotFoundException.h"
#include "NoActiveConException.h"
using namespace TA_Base_Core;
namespace TA_Base_Bus
{
	
	/*
	* Constructor
	*/
	SubscriptionManager::SubscriptionManager(GenaClientParams clientParams, GenaCallbackProccessor* pCallbackProc, ConnectionFactoryPtr pConnectionMgr)
		: m_pCallbackProc(pCallbackProc)
		, m_GenaClientParams(clientParams)
		, m_bTerminate(false)
		, m_pConnectionMgr(pConnectionMgr)
		, m_stoppedSubscriber(true)
	{
		FUNCTION_ENTRY("Constructor");
		// Process the initial status immediately
		//m_condition.signal();
		FUNCTION_EXIT;
	}

	/*
	* Destructor
	*/
	SubscriptionManager::~SubscriptionManager(void)
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		FUNCTION_EXIT;
	}

 
	void SubscriptionManager::addSubscribeTopic(T_ClientTopicList& topicList)
	{
		if (topicList.size() != 0)
		{	// Start Subscribing
			//TA_ASSERT(pTopicListener != 0, "topic listener should not be null");
			for (int idx=0; idx<topicList.size(); idx++)
			{
				GenaClientTopic objTopic = topicList[idx];
				_addSubscribeTopic(objTopic.topicUrl, objTopic.notification);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add Subscribed to topic: %s", objTopic.topic.c_str());
			}
			m_condition.signal();
		}

	}

	
	void SubscriptionManager::_addSubscribeTopic(const std::string& strTopicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("addSubscribeTopic");			
		// Check for duplicate
		bool isExisting = _isTopicSubscribed(strTopicURL);
		if (isExisting)
		{
			_setSubscriberStatus(strTopicURL,InitialSubscribe);
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Topic %s is already subscribed", strTopicURL.c_str());
			return;
		}

		GenaSubscriberPtr pNewSubscriber( new GenaSubscriber(strTopicURL, notification, m_GenaClientParams, m_pCallbackProc, m_pConnectionMgr));
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_subscribers.push_back(pNewSubscriber);
		 
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add subscribe topic for URI %s", strTopicURL.c_str());
		
		FUNCTION_EXIT;
	}

	/*
	* unsubscribe
	*/

	void SubscriptionManager::unsubscribe(const std::string& strTopicURL, const std::string& notification)
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		T_subscriberVec::iterator itr = m_subscribers.begin();
		for (itr; itr!= m_subscribers.end(); itr++)
		{
			GenaSubscriberPtr pTemp = *itr;
			if (strTopicURL == pTemp->getUri())
			{
				pTemp->unsubscribe();
				m_subscribers.erase(itr);
				break;
			}
		}
	}

	 
	/*
	* subscribe
	*/
	void SubscriptionManager::resetSubscribeTopic(const std::string& strTopicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("resetSubscribeTopic");
		// Check for duplicate
		bool isExisting = false;			
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			if (m_subscribers[idx]->getUri() == strTopicURL)
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "rest Topic %s is already subscribed", strTopicURL.c_str()); 
				m_subscribers[idx]->resetSubscribe();
				isExisting=true;
			}
		}
		if (!isExisting)
		{		
			GenaSubscriberPtr pNewSubscriber( new GenaSubscriber(strTopicURL, notification, m_GenaClientParams, m_pCallbackProc, m_pConnectionMgr));		
			m_subscribers.push_back(pNewSubscriber);
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add subscribe topic for URI %s", strTopicURL.c_str());
		}
		FUNCTION_EXIT;
	}

 

	void SubscriptionManager::startSubscribeMgr()
	{
		FUNCTION_ENTRY("startSubscribeMgr");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_stoppedSubscriber=false;
		m_bTerminate = false;
		start();
		FUNCTION_EXIT;
	}


	void SubscriptionManager::stopSubscribeAllTopic()
	{
		FUNCTION_ENTRY("stopSubscribeAllTopic");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_stoppedSubscriber=true;
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->setState(StopSubscribe);
		}
		m_condition.signal();
		FUNCTION_EXIT;
	}


	/*
	* run
	*/
	void SubscriptionManager::run()
	{
		while (!m_bTerminate)
		{
			if (!m_bTerminate)	m_condition.timedWait(2000);

			if (!m_bTerminate)  _checkSubscriberState();
		}
	}

	/*
	* terminate
	*/
	void SubscriptionManager::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;
		m_condition.signal();

		_cleanUpSubscribers();
		FUNCTION_EXIT;
	}

	void SubscriptionManager::resetSubscribeAllToic()
	{
		FUNCTION_ENTRY("resubscribeAllTopic");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->setState(ResetSubscribe);
		}

		m_condition.signal();
		FUNCTION_EXIT;
	}


	void SubscriptionManager::_checkSubscriberState()
	{
		FUNCTION_ENTRY("_checkSubscriberState");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		unsigned long l=m_subscribers.size();
		for (int idx=0; idx<m_subscribers.size() && !m_bTerminate; idx++)
		{
			try
			{	
				//trigger a InitialSubscribe when it is in active mode and remote connection is lost
				if (idx==0 && m_stoppedSubscriber==false &&
					m_pConnectionMgr->getActiveServer()->isActiveServerConnected()==false)
				{
					if (m_subscribers[idx]->getState()==RenewSubscribe)
					{
						m_subscribers[idx]->setState(InitialSubscribe);
					}					
				}
				m_subscribers[idx]->processStateCheck();
			}			
			catch (const TA_Base_Core::NoActiveConException& e)
			{
				//Lost connection, so no need to try reminds
				break;
			}			
		}

		FUNCTION_EXIT;
	}

	void SubscriptionManager::_setSubscriberStatus(const std::string& strTopicURL,SubscriberStatus status)
	{
		FUNCTION_ENTRY("_setSubscriberStatus");
		std::string sid;
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			if (m_subscribers[idx]->getUri() == strTopicURL)
			{
				  m_subscribers[idx]->setState(status);
			}
		}

		FUNCTION_EXIT; 
	}

	bool SubscriptionManager::_isTopicSubscribed(const std::string& strTopicURL)
	{
		FUNCTION_ENTRY("_isTopicSubscribed");
		std::string sid;
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			if (m_subscribers[idx]->getUri() == strTopicURL)
			{
				return true;
			}
		}

		FUNCTION_EXIT;
		return false;
	}

	void SubscriptionManager::_cleanUpSubscribers()
	{
		FUNCTION_ENTRY("_cleanUpSubscribers");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->unsubscribe();
		}
		m_subscribers.clear();
		FUNCTION_EXIT;
	}

} //TA_Base_Bus
