/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the state of the interface to the SDS Host.
  * Rate limiting calculations/strategies are also encapsulated within this class
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSInputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSMsgFactory.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRecvText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.h"



using namespace TA_Base_Core;
using namespace std;

namespace TA_IRS_App {

    const unsigned int SDSQueueManager::MAX_RECEIVED_DATA_DELAY_IN_SECS = 60;


    SDSQueueManager::SDSQueueManager(SDTSTransactionManager& transactionManager) : 
                        m_transactionManager(transactionManager),
                        m_Socket(NULL), m_Connected(false)//, m_isReconnect(false)
    {
        FUNCTION_ENTRY( "Constructor" );

        FUNCTION_EXIT;
    }

    SDSQueueManager::~SDSQueueManager()
    {
        FUNCTION_ENTRY( "Destructor" );

        if ( m_Socket != NULL)
        {
            m_Socket->terminateAndWait();
            delete m_Socket;
            m_Socket = NULL;
        }

        FUNCTION_EXIT;
    }

    void SDSQueueManager::setQueues(SDSInputQueue* input, SDSOutputQueue* output)
    {
        FUNCTION_ENTRY( "setQueues" );
        m_inputQueue = input;
        m_outputQueue = output;
        FUNCTION_EXIT;
    }


    void SDSQueueManager::rateLimit()
    {
        FUNCTION_ENTRY("rateLimit");
        
        // Delay for remainder of {SDTSperiod}, if any activity in the last {SDTSperiod} 

        ACE_Time_Value latestTime = (m_lastRequestTimestamp > m_lastResponseTimestamp) ?
                                     m_lastRequestTimestamp : m_lastResponseTimestamp;
        ACE_Time_Value timeElapsed = ACE_OS::gettimeofday() - latestTime;

        if ( timeElapsed < m_sdtsPeriod )
        {
            ACE_OS::sleep(m_sdtsPeriod - timeElapsed);
        }

        FUNCTION_EXIT;
    }

    void SDSQueueManager::updateRequestTimestamp()
    {
        FUNCTION_ENTRY("updateRequestTimestamp");

        m_lastRequestTimestamp = ACE_OS::gettimeofday();

        FUNCTION_EXIT;

    }

    void SDSQueueManager::updateResponseTimestamp()
    {
        FUNCTION_ENTRY("updateResponseTimestamp");

        m_lastResponseTimestamp = ACE_OS::gettimeofday();

        FUNCTION_EXIT;

    }

    void SDSQueueManager::connect(const std::string& host, const std::string& port)
    {
        FUNCTION_ENTRY("connect");

        ThreadGuard lvLock(m_socketLock);

        TA_ASSERT((m_inputQueue != NULL) && (m_outputQueue != NULL), "Queues are NULL");


		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"connect, IP address %s, port number is %s", host.c_str(), port.c_str());

        // disconnect first
        disconnect();

        // start I/O Q
        m_inputQueue->start();
        m_outputQueue->start();


        TA_ASSERT(m_Socket == NULL, "Socket already connected");

        // establish socket connection
        try
        {
            m_Connected = false;
//          m_Socket = new TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>(m_host,m_port);
//          m_Socket->addObserver(*this);
			m_Socket = new RadioSDSSocket(host, port, *this);
            m_Socket->start();
        }
        catch(...)
        {
            delete m_Socket;
            m_Socket = NULL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error when create socket.");

            throw;
        }
		
		int timeout = 30;
		while (!m_Connected && timeout > 0)
		{
			TA_Base_Core::Thread::sleep(100);
			--timeout;
		}

		if (!m_Connected)
		{
			throw TA_Base_Core::TransactiveException("not connected");
		}

        FUNCTION_EXIT;
    }


    void SDSQueueManager::disconnect()
    {
        FUNCTION_ENTRY("disconnect");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to disconnect()");
        ThreadGuard lvLock(m_socketLock);

		TA_ASSERT((m_inputQueue != NULL) && (m_outputQueue != NULL), "Queues are NULL");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"disconnect()");

        if (m_Socket != NULL)
        {
            m_Socket->terminateAndWait();
			delete m_Socket;
			m_Socket = NULL;
			m_Connected = false;
		}
		
        // stop I/O Qs
        m_inputQueue->stop();
        m_outputQueue->stop();

        FUNCTION_EXIT;

    }

    bool SDSQueueManager::isConnected() const
    {
        FUNCTION_ENTRY("isConnected");
        FUNCTION_EXIT;
        return m_Connected;
    }


    void SDSQueueManager::processReceivedData(std::vector<unsigned char> & pData)
    {
        FUNCTION_ENTRY("processReceivedData");

        ThreadGuard lvLock(m_socketLock_Rev);

        ACE_Time_Value currentTime = ACE_OS::gettimeofday();

        // if there were incomplete packet before, make sure it is not too "old"
        // or consider it all invalid
        if (!m_receivedData.empty())
        {
            if ((currentTime - m_lastReceivedDataTimestamp).sec() > MAX_RECEIVED_DATA_DELAY_IN_SECS)
            {
                m_receivedData.clear();
            }
        }

        m_lastReceivedDataTimestamp = currentTime;

        // move the incoming data to the internal buffer
        m_receivedData.insert(m_receivedData.end(), pData.begin(), pData.end());
        pData.clear();

        if (m_receivedData.size() <= 3)
        {
            // buffer does not contain enough data (at least 2 bytes adaptation length + some data)
            return;
        }

        // scan for possible valid adaptation length (2 bytes long)
        int startIndex = -1;
        for (unsigned int i = 0; i < m_receivedData.size(); ++i)
        {
            if (m_receivedData[i] != 0)
            {
                startIndex = i;
                break;
            }
        }

        if (startIndex == -1) // no valid data
        {
            m_receivedData.clear();
            return;
        }

        if (startIndex > 0) --startIndex; // adjust position

        if (startIndex > 0) // remove the invalid 0's
        {
            m_receivedData.erase(m_receivedData.begin(), m_receivedData.begin() + startIndex);
        }


		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Full Data Received from SDR,DATA SIZE = %d",m_receivedData.size());	


		while (m_receivedData.size() > 0)
		{

			vector<unsigned char> adaptation(m_receivedData.begin(), m_receivedData.begin() + 2);
			unsigned int adaptationLength = SDTSHelper::decode_u16(adaptation);

			if (m_receivedData.size() < adaptationLength + 2) // still not enough data, must wait
			{
				return;
			}

			
		
			SDTSProtocolEvent rawEvent;
			vector<unsigned char> rawEventData(m_receivedData.begin(), m_receivedData.begin() + adaptationLength + 2);
			SDTSHelper::printData(rawEventData,true);
			// remove the message from the internal buffer
			m_receivedData.erase(m_receivedData.begin(), m_receivedData.begin() + adaptationLength + 2);
			
			rawEvent.setRawMessage(rawEventData);

			

			try
			{
				SDTSProtocolEvent *protocolEvent = SDTSMsgFactory::createSDTSProtocolMsg(&rawEvent);

				if (protocolEvent != NULL)
				{
					SDSEvent* event = NULL;
					bool solicited = false;

					// check if this event was solicited or not, by checking the transaction manager
					CM_RegisterResponse* pRegister = dynamic_cast<CM_RegisterResponse*>(protocolEvent);
					if (pRegister != NULL)
					{
					}
					else
					{
						CMCommon2* common2 = dynamic_cast<CMCommon2*>(protocolEvent);
						if (common2 != NULL)
						{
							unsigned long src = common2->getSourceSSI();
							unsigned long dst = common2->getDestSSI();
							unsigned char msgRef = common2->getMessageRef();
							CMProtocolIdentifier pi = common2->getProtocolId();
							solicited = m_transactionManager.findTransaction(src, dst, pi, msgRef);
						}
						else
						{
							TLCommon* tlCommon = dynamic_cast<TLCommon*>(protocolEvent);
							if (tlCommon != NULL)
							{
								CMCommon3* common3 = tlCommon->getCMHeader();

								if (common3 != NULL)
								{
									unsigned long src = common3->getSourceSSI();
									unsigned long dst = common3->getDestSSI();
									unsigned char msgRef = common3->getMessageRef();
									CMProtocolIdentifier pi = common3->getProtocolId();
									solicited = m_transactionManager.findTransaction(src, dst, pi, msgRef);
								}
							}
						}
					}

					if (!solicited)
					{
						event = new SDSRecvText();
					}
					else
					{
						event = new SDSAckSendText(m_transactionManager);
					}

					TA_ASSERT(event != NULL, "Not enough memory to create event");

					event->setProtocolEvent(protocolEvent);
            
					TA_ASSERT((m_inputQueue != NULL) && (m_outputQueue != NULL), "Queues are NULL");
					m_inputQueue->enqueue(event);
				}
			}
			catch (TransactiveException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid event message received");
			}
		}


        FUNCTION_EXIT;
    }

    void SDSQueueManager::writeFailed()
    {
        FUNCTION_ENTRY("writeFailed");
        FUNCTION_EXIT;

    }

    void SDSQueueManager::connectionLost()
    {
        FUNCTION_ENTRY("connectionLost");

        m_Connected = false; 
		//m_isReconnect = true;

		//If SDS Server is disconnected,
		//Registration state must change into Unregistered State
		notifyConnectionStatus();

        FUNCTION_EXIT;
    }


    void SDSQueueManager::connectionEstablished()
    {
        FUNCTION_ENTRY("connectionEstablished");

        m_Connected = true;
		
		//Commented : Unregistered State must only be used during Connection lost
		//if (m_isReconnect)
// 		{
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "zhaohua: 6");
// 			notifyConnectionStatus();
// 			//m_isReconnect = false;
// 		}

        FUNCTION_EXIT;
    }

    void SDSQueueManager::sendMessage(const std::vector<unsigned char>& reqMsg)
    {
        FUNCTION_ENTRY("sendMessage");

        ThreadGuard lvLock(m_socketLock);

		SDTSHelper::printData(reqMsg,false);
		
        // write data to socket
        if ( (m_Connected) && (m_Socket != NULL ))
        {
            if ( !reqMsg.empty())
            {
                //m_Socket->write(reinterpret_cast<const char*>(&reqMsg[0]),reqMsg.size());
				m_Socket->sendPacket(reqMsg);
            }
        }

        FUNCTION_EXIT;
    }

    void SDSQueueManager::setPeriod(const ACE_Time_Value& period)
    {
        m_sdtsPeriod = period;
    }

	//SDSEventSubject
	void SDSQueueManager::registerObserver(SDSEventObserver* evtObj)
	{
		FUNCTION_ENTRY("registerObserver");

        ThreadGuard lvLock(m_lock);

        // do not add if already registered
		std::vector<SDSEventObserver*>::iterator pos;
		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			if (evtObj == *pos)
			{
                return;
			}
		}    
		m_observers.push_back(evtObj);

		FUNCTION_EXIT;
	}


	void SDSQueueManager::deregisterObserver(SDSEventObserver* evtObj)
	{
		FUNCTION_ENTRY("deregisterObserver");

        ThreadGuard lvLock(m_lock);

		std::vector<SDSEventObserver*>::iterator pos;
		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			if (evtObj == *pos)
			{
				pos=m_observers.erase(pos);
			}
		}

		FUNCTION_EXIT;
	}

	void SDSQueueManager::notifyConnectionStatus()
	{
		FUNCTION_ENTRY("notifyConnectionStatus");
        ThreadGuard lvLock(m_lock);

		std::vector<SDSEventObserver*>::iterator pos;

		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			(*pos)->setUnregistrationState();
		}
		FUNCTION_EXIT;
	}

}
