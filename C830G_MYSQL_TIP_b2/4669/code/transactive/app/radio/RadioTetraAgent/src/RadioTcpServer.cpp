/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioTcpServer.cpp,v $
  * @author  R. van Hugten
  * @version $Revision: 1.1.2.40.2.6 $
  * Last modification : $Date: 2007/04/12 03:22:37 $
  * Last modified by : $Author: zhouyuan $
  *
  * Implementation of the RadioTcpServer class
  */

#include <iostream>
#include <algorithm>

#include "ace/OS.h"

#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
//#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"

#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"
//#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioAgentConstants.h"
#include "app/radio/RadioTetraAgent/src/RadioResourceManager.h"
#include "app/radio/RadioTetraAgent/src/RadioAuthorisationManager.h"
#include "app/radio/RadioTetraAgent/src/RadioBackgroundProcessor.h"
#include "app/radio/RadioTetraAgent/src/RadioConnectionManager.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionNotInitialisedException.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioTaskFactory.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioTransactionManager.h"
#include "app/radio/RadioTetraAgent/src/RadioMethodTimeoutException.h"



#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"


#include "core/utilities/src/DebugUtil.h"
//#include "core/utilities/src/ILogType.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#define MAXLOGMESGSIZE 2000

using namespace TA_IRS_App;


RadioTcpServer::RadioTcpServer( IRadioCallback & callback,
                                ServerIndex sIndex,
                                Radio & radio,
								//TA_Base_Bus::ItaScada& scada,
                                const std::string & primaryAddress,
                                const std::string & secondaryAddress,
								unsigned char serialiseTransactionLevel
                                ) 
      :	m_transactionManager(0),
	    m_connectionManager(0),
		m_audioResourceManager(0),
		m_authorisationManager(0),
        m_subscriberMonitor(0),
        m_callback(callback),
        m_serverIndex(sIndex),
        m_radio(radio),
		m_serialiseTransactionLevel(serialiseTransactionLevel),
		//m_scada(scada),
		m_isResetConnectionInProcess(false),
		m_ResetSocketInProcess(false)
{
	FUNCTION_ENTRY("Constructor");
	m_audioResourceManager = new RadioResourceManager(*this);  
	m_connectionManager = new RadioConnectionManager(*this);
	m_transactionManager = new RadioTransactionManager();
	m_authorisationManager = new RadioAuthorisationManager(*this);
    m_subscriberMonitor = new RadioSubscriberMonitor(*this);
	//regist for connection status
	m_connectionManager->registForConnectionStatus(this);
	m_transactionManager->setWaitTimeout( 200*1000 );

    std::ostringstream temp;
    temp << RadioAgentConstants::TCP_SERVER_DATAPOINT_PREFIX << static_cast<long>(sIndex+1);
    m_datapointAddress = temp.str();

    m_connectionManager->setRadioHost(true, primaryAddress);
	m_connectionManager->setRadioHost(false, secondaryAddress);

	FUNCTION_EXIT;
}


RadioTcpServer::~RadioTcpServer()
{
	FUNCTION_ENTRY("Destructor");
	delete m_audioResourceManager;
	delete m_connectionManager;
	delete m_transactionManager;
	m_sessionSerialisationThreadguardMap.clear();
	
	FUNCTION_EXIT;
}

void RadioTcpServer::setEntity(TA_Base_Core::IEntityData& entityData)
{
	m_connectionManager->setEntity(entityData);
}

IRadioCallback& RadioTcpServer::getRadioCallback()
{
    return m_callback;
}

void RadioTcpServer::updateControlConnectionState(EControlConnectionStatus controlConnectionStatus )
{
	m_connectionManager->setControlConnection(controlConnectionStatus);
}

void RadioTcpServer::start()
{
    m_connectionManager->start();
}

void RadioTcpServer::setToControlMode()
{
	FUNCTION_ENTRY("setToControlMode");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting RadioTcpServer %d to control mode", m_serverIndex);
	
    m_connectionManager->setToControlMode();
	m_subscriberMonitor->setToControlMode () ;

    // just in case the sessions arent all set up, tell them its ok to set up
    // has no effect if they already are
    for ( RadioSessionSet::iterator sessit = m_radioSessions.begin();
          sessit != m_radioSessions.end();
          ++sessit )
    {
        (*sessit)->initialiseSession();
    }

	FUNCTION_EXIT;
}

void RadioTcpServer::setToMonitorMode()
{
	FUNCTION_ENTRY("setToMonitorMode");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting RadioTcpServer %d to monitor mode", m_serverIndex);
	m_connectionManager->setToMonitorMode();
	m_subscriberMonitor->setToMonitorMode() ;
	FUNCTION_EXIT;
}


void RadioTcpServer::onTerminate(bool peerAvailable)
{
    FUNCTION_ENTRY("onTerminate");

    // stop any radio sessions initialising    
    RadioSessionSet::iterator sessionIter;
    for ( sessionIter = m_radioSessions.begin(); sessionIter != m_radioSessions.end(); ++sessionIter )
    {
        (*sessionIter)->stop();
    }
    
    // unblock any threads waiting on transactions
    m_transactionManager->releaseAllTransactions();
    
    if (false == peerAvailable)
    {
        // Close any radio sessions
        for ( sessionIter = m_radioSessions.begin(); sessionIter != m_radioSessions.end(); ++sessionIter )
        {
            (*sessionIter)->detachSession();
        }
    }
    
    FUNCTION_EXIT;
}


TA_IRS_App::RadioEvent* RadioTcpServer::sendMethod(RadioMethod& method)
{
	FUNCTION_ENTRY("sendMethod");
    // before sending anything - be sure this agent is in control mode
    if ( !m_radio.getIsActive() )
    {
        TA_THROW(RadioSessionException("Agent in monitor mode - cannot send anything to hardware", 0));
    }
    
	// 
	// According to OTE (this is undocumented behaviour), methods must be serialised per session,
	// i.e. a method must receive a response before the next method may be sent. To achieve this,
	// a threadguard is created per RadioSessionReference
	RadioMethod::RadioSessionReference sessionRef = method.getRadioSessionReference();

	// Add the session reference to the threadguard map
	addRadioSessionToThreadguardMap(sessionRef);

	// Now get a lock for the threadguard of the radio session reference
	TA_Base_Core::ThreadGuard* lock = NULL;
    TA_IRS_App::RadioEvent* reply = NULL;

	#define CLEAN_LOCK_AND_REPLY delete lock; \
								 delete reply; \
								 lock = NULL; \
								 reply = NULL;

    unsigned long transactionID = 0;

    try
    {
        // optionally lock, based on the serialisation level
        // if the methods serialisation level is less than the agent level then serialise
        // eg a method with level 2 will be serialised when the agent level is set to 2 or more
        // setting the agent level to 0 serialises nothing
        if ( method.getSerialiseTransactionLevel() <= m_serialiseTransactionLevel )
        {
            lock = new TA_Base_Core::ThreadGuard( *(m_sessionSerialisationThreadguardMap[sessionRef]) );
        }

	    // Get a new transaction ID
	    transactionID = m_transactionManager->getNewTransactionID();
	    method.setTransactionId(transactionID);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Radio Tcp Server: %d, session: %lu, transaction: %lu, sending radio method: %s",
                     m_serverIndex, sessionRef, transactionID, typeid(method).name());
    
	    // Send the message
	    m_connectionManager->SendMethod(method);

	    // Wait for the reply
	    reply = m_transactionManager->waitForEvent(transactionID);

	    if ( (reply->getBufferLength()<1) || (reply->getStatus()<0) )
	    {
            // Removed, because showLastError() calls the GetLastError API method :
            // 1. GetLastError only applies to Database errors on the HLR/GWLD. As
            //    the TCPSrv API 2151 shows, this method only applies to certain
            //    combinations of Method/Event and STATUS_KO
            // 2. As with all other methods, applies ONLY to the session that caused
            //    the db error. Note that it was previously called on the ISCSRadioSession,
            //    which may have even been hosted on an entirely different TcpServer
            //
            // Suggestions:
            // * If you *need* to use GetLastError(), do so from the correct
            //   RadioSession so that the correct error will be returned.
            // * Doing that will ensure you are more likely closer to the originating
            //   call of the method, and hence more likely to make a correct decision
            //   on whether the GetLastError() method is appropriate.
            // * Using logLastError() will require catching the RadioSessionException
            //   thrown and analysing the return value based on the API reference.
            //
		    // getRadio().getISCSRadioSession().showLastError();
            std::string textstatus = "Connection reset while waiting for response event";
            long status = STATUS_KO_SESSION_NOT_FOUND;

            if ( reply->getBufferLength()>0)
            {
                textstatus = reply->getTextStatus();
                status = reply->getStatus();

				if ( status == STATUS_KO_GWLD_LINK_LOST )
				{
					resetConnection();
				}
            
                // log the full verbose details
                // if not already logged - this detail is already logged at debug level
                // so if the debug level is not set that verbose then we'll log it
                if (TA_Base_Core::DebugUtil::getInstance().getLevel() < TA_Base_Core::DebugUtil::DebugDebug)
                {
                    std::string hexText;
                    std::string consoleText;
                
                    method.getLogText(hexText, consoleText);
                
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "Radio Tcp Server: %d, session: %lu, transaction: %lu, radio method: %s, returned status: %ld",
                                 m_serverIndex, sessionRef, transactionID, typeid(method).name(), status);
                    LOGMORE(SourceInfo, "Radio method data: %s", hexText.c_str());
                    LOGMORE(SourceInfo, "Radio method consoleText: %s", consoleText.c_str());
                }
            }
			CLEAN_LOCK_AND_REPLY
			TA_THROW(RadioSessionException(textstatus, status));
	    }
    }
    catch( RadioMethodTimeoutException& te )
    {
        // log the method timeout
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Radio Tcp Server: %d, session: %lu, transaction: %lu, radio method: %s, TIMED OUT WHILE WAITING FOR RESPONSE",
                     m_serverIndex, sessionRef, transactionID, typeid(method).name());
		CLEAN_LOCK_AND_REPLY
		m_transactionManager->waitListDeleteTransaction ( transactionID ) ;
        throw;
    }
    catch(...)
    {
		CLEAN_LOCK_AND_REPLY
		m_transactionManager->waitListDeleteTransaction ( transactionID ) ;
        throw;
    }

    // release lock
    delete lock;
	lock = NULL;

	FUNCTION_EXIT;
	return reply;
}


std::vector<RadioEvent*> RadioTcpServer::sendMethodMultipleResponse(RadioMethod& method)
{
	FUNCTION_ENTRY("sendMethodMultipleResponse");
	// 
	// According to OTE (this is undocumented behaviour), methods must be serialised per session,
	// i.e. a method must receive a response before the next method may be sent. To achieve this,
	// a threadguard is created per RadioSessionReference
	RadioMethod::RadioSessionReference sessionRef = method.getRadioSessionReference();

	// Add the session reference to the threadguard map
	addRadioSessionToThreadguardMap(sessionRef);

    TA_Base_Core::ThreadGuard* lock = NULL;
    std::vector<RadioEvent*> events;

    unsigned long transactionID = 0;

    try
    {
        // optionally lock, based on the serialisation level
        // if the methods serialisation level is less than the agent level then serialise
        // eg a method with level 2 will be serialised when the agent level is set to 2 or more
        // setting the agent level to 0 serialises nothing
        if ( method.getSerialiseTransactionLevel() <= m_serialiseTransactionLevel )
        {
            lock = new TA_Base_Core::ThreadGuard( *(m_sessionSerialisationThreadguardMap[sessionRef]) );
        }

	    // Get a new transaction ID
	    transactionID = m_transactionManager->getNewTransactionID();
	    method.setTransactionId(transactionID);

	    // Send the message
	    m_connectionManager->SendMethod(method);

        bool moreData = false;
        do
        {        
            RadioEvent* event = m_transactionManager->waitForEvent(transactionID);
            if (event != 0)
            {
                events.push_back(event);
                moreData = event->isStatusMoreData();
            }
        }
        while (moreData);
    }
    catch( RadioMethodTimeoutException& te )
    {
        // log the method timeout
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Radio Tcp Server: %d, session: %lu, transaction: %lu, radio method: %s, TIMED OUT WHILE WAITING FOR RESPONSE",
                     m_serverIndex, sessionRef, transactionID, typeid(method).name());

        // clean up
        delete lock;
		lock = NULL;

        for (std::vector<RadioEvent*>::iterator iter = events.begin(); iter != events.end(); ++iter)
        {
            delete *iter;
        }
        
        events.clear();
		m_transactionManager->waitListDeleteTransaction ( transactionID ) ;

        throw;
    }
    catch(...)
    {
        // clean up
        delete lock;
		lock = NULL;

        for (std::vector<RadioEvent*>::iterator iter = events.begin(); iter != events.end(); ++iter)
        {
            delete *iter;
        }
        
        events.clear();
		m_transactionManager->waitListDeleteTransaction ( transactionID ) ;

        throw;
    }

    // release lock
    delete lock;
	lock = NULL;

	// Wait for the reply
	FUNCTION_EXIT;
	return events;
}



void RadioTcpServer::getCallList(CallList& callList) 
{
	FUNCTION_ENTRY("getCallList");

	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	CallStack::iterator   it;

	// Don't reset the list - this is usually called from Radio::getCallList,
    // which accumulates calls from all the servers

	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		callList.push_back(it->callID);
	}

	FUNCTION_EXIT;
}


// TD12462 In fallback, cannot send SDS to train if audio already established
// Thus we allow for filtering on all AudioOnly calls
void RadioTcpServer::getTsiCallList(const std::string & tsi, bool audioOnly, CallList & callList)
{
	FUNCTION_ENTRY("getTsiCallList");

	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	CallStack::iterator   it;

	// Don't reset the list - this is usually called from Radio::getCallList,
    // which accumulates calls from all the servers

	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
        // if tsi is either caller or called then it matches
        if ( (tsi == it->callerTSI) || (tsi == it->calledTSI) )
        {
            // if audioOnly then only add calls with appropriate category
            if ( audioOnly )
            {
                if ( (HalfDuplex==it->callCategory) ||
                     (FullDuplex==it->callCategory) )
                {
                    callList.push_back(it->callID);
                }
            }
            else // want everything - put it on
            {
                callList.push_back(it->callID);
            }
        }
	}

	FUNCTION_EXIT;
}


bool RadioTcpServer::getCallDetails(CallID callId,CallDetailsType& details)
{
	FUNCTION_ENTRY("getCallDetails");
	
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Looking in RadioTcpServer %d for Call %lu", m_serverIndex, callId);

	CallStack::iterator  it;

	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		if (it->callID==callId)
		{
			// CORBA will delete this object
			details = *it;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getCallDetails() Found RadioTcpServer %d Session %lu Call %lu", m_serverIndex, it->sessionRef, callId);
			// Return the details
			FUNCTION_EXIT;
			return true;
		}
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getCallDetails() Did NOT find RadioTcpServer %d Call %lu", m_serverIndex, callId);
	FUNCTION_EXIT;
	return false;
}


bool RadioTcpServer::getCallDetailsPartial(CallID callId,CallDetailsType& details)
{
	FUNCTION_ENTRY("getCallDetailsPartial");
	
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Looking in RadioTcpServer %d for Call %lu", m_serverIndex, callId);

	CallStack::iterator  it;

	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		if (it->callID==callId)
		{
			details.callID 			= (*it).callID;
			details.sessionRef 		= (*it).sessionRef;
            details.consoleID       = (*it).consoleID;
			details.time 			= (*it).time;
			details.callTime 		= (*it).callTime;
			details.callerName 		= (*it).callerName;
			details.callerTSI 		= (*it).callerTSI;
			details.callerLocation 	= (*it).callerLocation;
			details.calledName 		= (*it).calledName;
			details.calledTSI 		= (*it).calledTSI;
			details.calledLocation 	= (*it).calledLocation;
			details.callStatus 		= (*it).callStatus;
			details.callCategory 	= (*it).callCategory;
			details.callType     	= (*it).callType;
			details.callSource 		= (*it).callSource;
			details.isEmergency 	= (*it).isEmergency;
			//TD 10234
            details.isInvisibleToGUIApps = (*it).isInvisibleToGUIApps;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getCallDetailsPartial() Found RadioTcpServer %d Session %lu Call %lu", m_serverIndex, it->sessionRef, callId);
			FUNCTION_EXIT;
			return true;
		}
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getCallDetailsPartial() Did NOT find RadioTcpServer %d Call %lu", m_serverIndex, callId);
	FUNCTION_EXIT;
	return false;
}


/**
* Indicates if the selected call is an SDS call  
*/
bool RadioTcpServer::isSDSCall(CallID callId)
{
	FUNCTION_ENTRY("isSDSCall");

	CallDetailsType   details;
	if (!getCallDetailsPartial(callId,details))
	{
		return false;
	}

	FUNCTION_EXIT;
	return (details.callCategory==TextMessage||details.callCategory==CallDriver);
}



void RadioTcpServer::onConnectionLost()
{
	FUNCTION_ENTRY("onConnectionLost");

	m_transactionManager->releaseAllTransactions();

    for (   RadioSessionSet::iterator sessit = m_radioSessions.begin() ;
    sessit != m_radioSessions.end() ;
    sessit++ )
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Link Lost] about to call RadioSession::onConnectionLost().");
		(*sessit)->onConnectionLost();
    }

	m_radio.onConnectionFailed(m_serverIndex);

	FUNCTION_EXIT;
}


/**
* Connection resumed.
*/
void RadioTcpServer::onConnectionEstablish()
{
	FUNCTION_ENTRY("onConnectionEstablish");

    //
    // if at least one connection was already up, no need to go around
    // switching stuff back "on"
    int numConnected = m_connectionManager->numConnected();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                 "RadioTcpServer %d Number of active connections: %d", m_serverIndex, numConnected);
    // TODO: What if both connections resume simultaneously ?
    
    for (   RadioSessionSet::iterator sessit = m_radioSessions.begin() ;
            sessit != m_radioSessions.end() ;
            sessit++ )
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Link Establish] about to call RadioSession::onConnectionEstablish().");
		(*sessit)->onConnectionEstablish();
    }

	m_radio.onConnectionResumed(m_serverIndex);

	FUNCTION_EXIT;
}

void RadioTcpServer::onSwitchConnection()
{
	FUNCTION_ENTRY("onConnectionEstablish");

	//delete the all the transaction
	m_transactionManager->releaseAllTransactions();

	for ( RadioSessionSet::iterator sessit = m_radioSessions.begin() ; 
		  sessit != m_radioSessions.end() ; ++sessit )
		  {
			  LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				  "[Link Switch] about to call RadioSession::onSwitchConnection().");
			  (*sessit)->onSwitchConnection();
		  }

	m_radio.onSwitchConnection(m_serverIndex);

	FUNCTION_EXIT;
}

/**
* createCompatibleTcpEvent
*/
ITcpEvent * RadioTcpServer::createCompatibleTcpEvent()
{
	RadioEvent * event = new TA_IRS_App::RadioEvent();

	return event;
}



/**
* This method will receive the incoming events from the radio system.  
* It will notify waiting RadioSession of a new event through the RadioTransactionManager.  
* It will process all unsolited events.
* @param    event
*/
void RadioTcpServer::onReceiveEvent(TA_IRS_App::ITcpEvent& itcpEvent)
{
	FUNCTION_ENTRY("onReceiveEvent");

	try
	{
		// For RadioTcpServer the ITcpEvent must be a RadioEvent.
		// Do a dynamic_cast and ensure this is the correct type
		RadioEvent &event = dynamic_cast<RadioEvent &>(itcpEvent);

		// The buffer size could exceed the LOG limit of 2000 chars. Check and break up into
		// seperate logs if necessary
		// [start of logging code]
		//
		// Since there is a lot of computation and stuffing around, more than we
		// want to waste cpu cycles on in the final deployment scenario, we check
		// the log level before diving in
		if ( TA_Base_Core::DebugUtil::getInstance().getLevel() >= TA_Base_Core::DebugUtil::DebugDebug)
		{
			unsigned long remainingBufferSize = event.getBufferDump().length();
			if(remainingBufferSize >= MAXLOGMESGSIZE)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					 "RadioTcpServer %d Received Radio Event exceeding Max Log size. Will be split into seperate log messages", m_serverIndex);
			}
			std::string remaingBuffer = event.getBufferDump();
			int countNumLogMessages = 1;
			while(remainingBufferSize >= MAXLOGMESGSIZE)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					 "RadioTcpServer %d Log part %d, Received radio event: ", m_serverIndex, countNumLogMessages);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					 "%s",remaingBuffer.substr(0, MAXLOGMESGSIZE).c_str());
				remainingBufferSize -= MAXLOGMESGSIZE;
				remaingBuffer = remaingBuffer.substr(MAXLOGMESGSIZE, remainingBufferSize);
				countNumLogMessages++;
			}
			if(remainingBufferSize > 0)
			{
				if(countNumLogMessages > 1)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						 "RadioTcpServer %d Log part %d, Received radio event: ", m_serverIndex, countNumLogMessages);
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						 "%s",remaingBuffer.substr(0, remainingBufferSize).c_str());
				}
				else
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						 "RadioTcpServer %d Received radio event: %s", m_serverIndex,remaingBuffer.substr(0, remainingBufferSize).c_str());
				}
			}
		} // if (getLevel >= Debug)
		long eventType = event.getEventType();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				 "RadioTcpServer %d, Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld",
				 m_serverIndex,
				 ((eventType<0xA000) ? '8':'A'),
				 (eventType-((eventType<0xA000)?0x8000:0xA000)),
				 event.getSessionRef(),
				 event.getTransactionId(),
				 event.getStatus());
		// [end of logging code]

		if ( event.getStatus() == STATUS_KO_TOO_MANY_ATTACH_SESSIONS )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[SELEX] Reset socket.");
			resetSocket();
			return;
		}

		//Call try to create the event reader. if it is a invalid message, 
		//it will throw RadioProtocolException
		event.getEventReader();

		// Pass to the handle for they type of message
		if (event.getTransactionId()==0)
		{
			onUnsolicitedEvent(event);
		}
		else
		{
			onSolicitedEvent(event);
		}

		// Do not block normal process
		if ( event.getStatus() < 0 && 
			( ( event.getEventType() == EVENT_ATTACH_SESSION ) || ( event.getEventType() == EVENT_INITIALISE ) || ( event.getEventType() == EVENT_LOGIN ) )	)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[SELEX] Reset session status.");

			resetSession( event.getSessionRef() );
			
			int retryConut = 0;
			
			// Wait for 5s 
			while ( !isResetSessionInProcess( event.getSessionRef() ) && retryConut < 5 )
			{
				TA_Base_Core::Thread::sleep( 1000 );
				retryConut ++;
			}
		}
	}
	catch(std::bad_cast)
	{
	}
	catch (...)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  "RadioProtocolException. the received event is invalid, will ignore it.");
	}

	FUNCTION_EXIT;
}




/**
* Handler for unsolicited events (e.g. incoming text message, incoming call)
* @param    event
*/
void RadioTcpServer::onUnsolicitedEvent(TA_IRS_App::RadioEvent& event)
{
	FUNCTION_ENTRY("onUnsolicitedEvent");
  
	// Get the reader for the event
    RadioEventReader*   eventReader = event.getEventReader();
	//TA_ASSERT(eventReader!=NULL,"Event reader not available for message type");
	if (eventReader==NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Unsolicited event not handled, transaction: %lu, event: %lu, session: %lu",
			 event.getTransactionId(), event.getEventType(), event.getSessionRef());
		return;
	}

	// Handle incoming SDS messages
	IncomingSDSEventReader* newSDSReader = dynamic_cast<IncomingSDSEventReader*>(eventReader);
	if (newSDSReader!=NULL)
	{
		onIncomingSDS(*newSDSReader);
		FUNCTION_EXIT;
		return;
	}

	// Handle Incoming Circuit Data Calls
	IncomingCircuitDataEventReader* icdReader = dynamic_cast<IncomingCircuitDataEventReader*>(eventReader);
	if (icdReader != 0) 
	{
		onIncomingCircuitData(*icdReader);
		FUNCTION_EXIT;
		return;
	}


	// Handle Circuit Data Capacity Event
	CircuitDataCapacityEventReader* cdcReader = dynamic_cast<CircuitDataCapacityEventReader*>(eventReader);
	if (cdcReader != 0) 
	{
		onCircuitDataCapacity(*cdcReader);
		FUNCTION_EXIT;
		return;
	}

	// Handle


	// Process new incoming voice calls
	IncomingCallEventReader* newCallReader = dynamic_cast<IncomingCallEventReader*>(eventReader);
	if (newCallReader!=NULL)
	{	
		//Modified by Raghu. To accept secondparameter SessionRef
		addCallToPendingAddtionList(newCallReader->getCallReference(),event.getSessionRef());//add by HuangQi     CL-20772


		onIncomingVoiceCall(*newCallReader);
		FUNCTION_EXIT;
		return;
	}


	// Process GroupAcknowledge Call events
	GroupCallAckEventReader* groupCallAckReader = dynamic_cast<GroupCallAckEventReader*>(eventReader);
	if (groupCallAckReader!=NULL)
	{
		onGroupCallAck(*groupCallAckReader,event.getSessionRef());
		FUNCTION_EXIT;
		return;
	}

	// Check for a Demand or Cease TX message
	if (onTXEvent(event))
	{
		FUNCTION_EXIT;
		return;
	}

	// Process authorisation requests
	RequestAuthoriseEventReader* newAuthoriseRequestReader = dynamic_cast<RequestAuthoriseEventReader*>(eventReader);
	if (newAuthoriseRequestReader!=NULL)
	{
        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CAD]receive CAD Request");
		m_authorisationManager->addAuthorisationRequest(event.getSessionRef(),
			                                                 newAuthoriseRequestReader->getCADReference());
		FUNCTION_EXIT;
		return;
	}


	// Check if unsolicited disconnect
	DisconnectEventReader* disconnectReader = dynamic_cast<DisconnectEventReader*>(eventReader);
	if (disconnectReader!=NULL)
	{
        CallID callId = disconnectReader->getCallReference();
        std::string tsi;

		// Check if this is a CAD request

        // ++AZ TD9389 : hardware sending "CADCanceled"
		if (disconnectReader->getReason()=="CADCancelled" ||
            disconnectReader->getReason() == "CADCanceled")
        // AZ++
		{
			m_authorisationManager->cancelRequest(callId);
		}
        else if (m_radio.getRadioCircuitModeManager()->isRadioCircuitTransfer(callId, tsi))
        {
            m_radio.getRadioCircuitModeManager()->processCircuitModeDisconnection(callId);
        }
		else
		{
            unsigned long disconnectCallReference = disconnectReader->getCallReference();
            // Release any audio in use
            m_audioResourceManager->releaseAudioReference(
                                            event.getSessionRef(),
                                            disconnectCallReference,
                                            false,  // Indirect call!
                                            m_subscriberMonitor->isListenCall(callId) // release *monitored* audio if required
                                            );
			// TODO Update call stack
			removeCallFromCallStack(disconnectCallReference,event.getSessionRef());
            addBackgroundTask(RadioTaskFactory::createDeleteReferenceTask(this, event.getSessionRef(), disconnectCallReference, RadioMethodDeleteReference::CALL_REFERENCE));
            
		}
	}


	// Check if "unsolicited" subscriber activity event
	SubscriberActivityEventReader* subactivityReader = dynamic_cast<SubscriberActivityEventReader*>(eventReader);
	if (subactivityReader != 0)
	{
        onSubscriberActivity(event, subactivityReader);
    }

	// Check if unsolicited MonitorSubscriber event
	MonitorSubscriberEventReader* monsubReader = dynamic_cast<MonitorSubscriberEventReader*>(eventReader);
	if (monsubReader != 0)
	{
        onMonitorSubscriber(event, monsubReader);
    }

	DetachSessionEventReader* detachSessionReader = dynamic_cast<DetachSessionEventReader*>(eventReader);
	if ( detachSessionReader != 0 )
	{
		onDetachSession( event, detachSessionReader );
	}

	// Radio System error
	SystemErrorEventReader* systemErrorReader = dynamic_cast<SystemErrorEventReader*>(eventReader);
	if (systemErrorReader != 0)
	{
		onSystemError(event, systemErrorReader);
		FUNCTION_EXIT;
		return;
	}

	FUNCTION_EXIT;
}


/**
* Handler for solicited events - This will send them to the RadioTransactionManager for processing
* @param    event
*/
void RadioTcpServer::onSolicitedEvent(TA_IRS_App::RadioEvent& event)
{
	FUNCTION_ENTRY("onSolicitedEvent");


	if (!m_transactionManager->receivedEvent(event))
	{
		// Check for a Demand TX message (Delayed by radio queuing request)
		if (onTXEvent(event))
		{
			FUNCTION_EXIT;
			return;
		}

		// Solicited event not handled !!
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Solicited event not handled, transaction: %lu, event: %lu, session: %lu",
			 event.getTransactionId(), event.getEventType(), event.getSessionRef());

		onUnhandledSolicitedEvent(event);
	}
	
	FUNCTION_EXIT;
}


void RadioTcpServer::onUnhandledSolicitedEvent(RadioEvent& event)
{
    FUNCTION_ENTRY("onUnsolicitedEvent");
    
	// Get the reader for the event
    RadioEventReader* eventReader = event.getEventReader();
    
    // Check if MonitorSubscriber event
	MonitorSubscriberEventReader* monsubReader = dynamic_cast<MonitorSubscriberEventReader*>(eventReader);
	if (monsubReader != 0)
	{
        onMonitorSubscriber(event, monsubReader);
		FUNCTION_EXIT;
		return;
    }
    
    FUNCTION_EXIT;
}


void RadioTcpServer::onIncomingVoiceCall(const TA_IRS_App::IncomingCallEventReader& eventReader)
{
	FUNCTION_ENTRY("onIncomingVoiceCall");


	CallDetailsType  details; 

	details.callStatus   = Ringing;
	details.callSource   = IncomingCall;
    details.time         = 0;   // Added by addCallToCallStack
	details.callTime     = "";

	// Determine call category
	// if (eventReader.getCallType()==IncomingCallEventReader::SINGLE_CALL)
    if (eventReader.isFullDuplex())
	{
		details.callCategory = FullDuplex; 
	}
	else
	{
		details.callCategory = HalfDuplex; 
	}

    TA_IRS_App::IncomingCallEventReader::ECallType inCallType = eventReader.getCallType();
    switch (inCallType)
    {
        case IncomingCallEventReader::GROUP_UNACKNOWLEDGE:
            details.callType = GroupUnacknowledgeCall;
            break;
        case IncomingCallEventReader::GROUP_ACKNOWLEDGE:
            details.callType = GroupAcknowledgeCall;
            break;
        case IncomingCallEventReader::BROADCAST:
            details.callType = GroupBroadcastCall;
            break;
        default:
            details.callType = SingleCall;
            break;
    }

	// Get the call details from the event
	details.callID = eventReader.getCallReference();
	details.sessionRef = eventReader.getEvent().getSessionRef();
	details.calledTSI = eventReader.getCalledNumber();
	details.callerTSI = eventReader.getCallingNumber();

    details.isEmergency = eventReader.isEmergency();
    details.isInvisibleToGUIApps = false;

	// Get the caller name and location in the subscriber directory
	// Can't do it here as it will block, use a separate process!
	// This will be done with conference calls which also need this.
	details.calledLocation = "";
	details.calledName     = "";
	details.callerLocation = "";
	details.callerName     = "";

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		 "New voice call %ld",details.callID);

    // Doing so in the background gives an opportunity to update the caller
    // and called details.


    addBackgroundTask(RadioTaskFactory::createCallStackUpdateTask(this, details));


	FUNCTION_EXIT;
}
std::string RadioTcpServer::getKeyForSessionCallID(CallID callid,SessionReference sessionRef)
{
	char buffer[255] = {0};
	sprintf(buffer,"%lu_%lu",callid,sessionRef);
	std::string ret(buffer);
	return ret;
}
void RadioTcpServer::onGroupCallAck(const TA_IRS_App::GroupCallAckEventReader & eventReader,SessionReference sessionRef)
{
    // try to join the incoming unsolicited event details to pre-stored call.
    // If the other details aren't there already, they are probably (hopefully) on their way.
    FUNCTION_ENTRY("onGroupCallAck()");

    TA_Base_Core::ThreadGuard pgcag(m_groupCallAckLock);
    
    unsigned long callRef = eventReader.getCallReference();
	std::string key=getKeyForSessionCallID(callRef,sessionRef);
    CallDetailsMap::iterator found = m_pendingGCADetails.find(key);
    if ( found != m_pendingGCADetails.end() )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
             "Found pending CallDetails for GroupCallAck call=%lu key=%s", callRef,key.c_str());
        // arrange to get all those subscriber details
        addBackgroundTask( RadioTaskFactory::createGroupCallAckTask(this, (*found).second, eventReader) );
        // remove from the map
        m_pendingGCADetails.erase(key);
    }
    else
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
             "CallDetails for GroupCallAck call=%lu not found, placing event details in pending list", callRef);
        GroupCallAckTsiVectorsMap::iterator fndTsiVector = m_pendingGCATsiVectors.find(key);
        if ( fndTsiVector != m_pendingGCATsiVectors.end() )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                 "TSI Vector for GroupCallAck call=%lu key=%s already found, replacing with more currently supplied event details",callRef,key.c_str());
        }

        m_pendingGCATsiVectors.erase(key);
        TSIVector tsiVector;
        eventReader.getTsiVector(tsiVector);
        m_pendingGCATsiVectors[key] = tsiVector;
    }
    FUNCTION_EXIT;

}


/**
* Determine SDS type, if textual add to incoming call stack and notify, 
* if train message pass to onTrainEvent
*/
void RadioTcpServer::onIncomingSDS(const TA_IRS_App::IncomingSDSEventReader& eventReader)
{
	FUNCTION_ENTRY("onIncomingSDS");


	// Check SDS type (pre-coded or text)
	if (eventReader.isPreCoded())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			 "New pre-coded SDS (%hd)",eventReader.getPreCodedStatus());
		// TA_ASSERT(false,"Coded status message not handled.");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			 "Pre-coded SDS not handled");
	}
	else if (eventReader.isText())
	{
		// Check if it is a train message or normal text
		if (eventReader.isTrainMessage())
		{
			// Let radio servant handle the notification
			m_callback.onTrainEvent(eventReader.getSenderITSI(),eventReader.getMessageData());

			// Release the resources used in the radio
			m_audioResourceManager->releaseSDSReference(eventReader.getEvent().getSessionRef(),eventReader.getSDSReference());

    		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	    		 "event.session = %d iscssession = %d",eventReader.getEvent().getSessionRef(),m_radio.getISCSRadioSessionRef());
		}
		else
		{
			// Add the text message to the call stack
			// Resources will be released when the call is removed from the call stack (like voice)
			onTextMessage(eventReader);
		}
	}
	else
	{
		// Unknown message type
		// TA_ASSERT(false,"Received SDS event has unknown data type");
        // TODO: bulk out he error log with the sds data
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			 "Received SDS event has unknown data type: ");
	}


	FUNCTION_EXIT;
}

/**
* Sent when the hardware is acknowledging a circuit mode data transfer packet.
* This will be recorded in the RadioCircuitModeManager and another packet sent
* for that call.
*/
void RadioTcpServer::onIncomingCircuitData(const TA_IRS_App::IncomingCircuitDataEventReader& eventReader)
{
	FUNCTION_ENTRY("onIncomingCircuitData");

    m_radio.onIncomingCircuitData(eventReader);

	FUNCTION_EXIT;
}


void RadioTcpServer::onCircuitDataCapacity(const TA_IRS_App::CircuitDataCapacityEventReader& eventReader)
{
	FUNCTION_ENTRY("onCircuitDataCapacity");

    m_radio.onCircuitDataCapacity(eventReader);

	FUNCTION_EXIT;
}



void RadioTcpServer::addSession(RadioSession * session)
{
	FUNCTION_ENTRY("addSession");
    // Must NOT use the session object here, as it has not completed construction
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioTcpServer %d adding Session for Console %lu", m_serverIndex, session->getConsoleID());
    m_radioSessions.insert(session);


	//if the session is not yet initialised, set it to 0. 
	//will be updated after initialisation.
	unsigned long sessionId = 0;
	try
	{
		sessionId = session->getValidSessionRef();
	}
	catch (RadioSessionNotInitialisedException&)
	{
	}
	
	m_radio.addSessionServerConsole(sessionId,m_serverIndex,session->getConsoleID());

    // if the connection has been established already, then make the session initialise itself
    if (m_connectionManager->numConnected() > 0)
    {
        session->onConnectionEstablish();
    }


	FUNCTION_EXIT;
}


/**
* An incoming text message.
*/
void RadioTcpServer::onTextMessage(const TA_IRS_App::IncomingSDSEventReader& eventReader)
{
	FUNCTION_ENTRY("onTextMessage");


	CallDetailsType * details = new TA_IRS_App::CallDetailsType;

	details->callStatus   = Ringing;
	details->callSource   = IncomingCall;
	details->callCategory = TextMessage;
	// details->callType     = ; No such call type.  Perhaps a "DataCall" value should be added to the enum
	details->callType     = SingleCall; // for the time being, to prevent assertions
    details->time         = 0;   // Added by addCallToCallStack
	details->callTime     = "";

	// Get the call details from the event
	details->callID     = eventReader.getSDSReference();
	details->sessionRef = eventReader.getEvent().getSessionRef();
	details->calledTSI  = eventReader.getDestinationITSI();
	details->callerTSI  = eventReader.getSenderITSI();

	// SDS Text Messages don't have locations and names, so null the strings
	// RvH 20050201 Pre-Radio-iFAT
	// need to provide the caller Location, which will be filled by the new bg task
	details->calledLocation = "";
	details->calledName     = "";
	details->callerLocation = "";
	details->callerName     = "";

	details->callText       = eventReader.getMessageText();
	details->isEmergency    = false;
    details->isInvisibleToGUIApps = false;


	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		 "RadioTcpServer %d New SDS call %ld",m_serverIndex, details->callID);


	// Add to the call stack
	// addCallToCallStack(details);
    // 
    // Doing so in the background gives an opportunity to update the caller
    // and called details.
    addBackgroundTask(RadioTaskFactory::createSDSDetailsUpdateTask(this, details));

	FUNCTION_EXIT;
}


bool RadioTcpServer::onTXEvent(TA_IRS_App::RadioEvent& event)
{

	FUNCTION_ENTRY("onTXEvent");

    RadioEventReader*   eventReader = event.getEventReader();

	// Check if the message is a demand TX
	DemandTXEventReader* demandTXReader = dynamic_cast<DemandTXEventReader*>(eventReader);
	if (demandTXReader!=NULL)
	{
		// Yes it is, notify clients (e.g. PTT library)
		TXStateInfo  txInfo;
        RadioEvent::StatusType status = event.getStatus();
		switch (status)
		{
			case STATUS_OK : 
			case STATUS_TXTAKEN :  // AJP TODO - should make new enum
				txInfo.state  = TX_GRANTED;
				break;
			case STATUS_DEMANDTX_QUEUED: 
				txInfo.state  = TX_QUEUE;
				break;
			case STATUS_DEMANDTX_TIMEOUT: 
				txInfo.state  = TX_TIMEOUT;
				break;
            // this can also come unsolicited if the demandtx was implicit,
            // eg. on include into a call.  We could ignore, but should ensure
            // that the correct state is displayed in the gui
            case STATUS_KO_DEMAND_TX_FAILED:
				txInfo.state  = TX_CEASE;
                break;
			default:
                if (event.getStatus() < 0)
                {
                    // negative/error status, like. This happens when the
                    // error comes in via the unsolicited channel (the solicited
                    // event handler normally deals with error codes).
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                         "DemandTX error status %ld", status);
                    TA_THROW(RadioSessionException(event.getTextStatus(), event.getStatus()));
                }
                else
                {
                    // Unknown OK/positive status
                    // TA_ASSERT(false,"Unknown demand TX status");
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                         "Unknown DemandTX status %ld", status);
                }
                FUNCTION_EXIT;
                return true;
		}

		txInfo.callID        = demandTXReader->getCallReference();
		txInfo.sessionRef    = event.getSessionRef();
		txInfo.transactionID = event.getTransactionId();
		txInfo.userITSI      = demandTXReader->getUserTSI();
		txInfo.tpiITSI       = demandTXReader->getTPI();

		m_callback.onTXChange(txInfo);

		return true;
	}

	// Check if the message is a cease TX
	CeaseTXEventReader* ceaseTXReader = dynamic_cast<CeaseTXEventReader*>(eventReader);
	if (ceaseTXReader!=NULL)
	{
		// Yes it is, notify clients (e.g. PTT library)
		TXStateInfo  txInfo;
		txInfo.state         = TX_CEASE;
		txInfo.callID        = ceaseTXReader->getCallReference();
		txInfo.sessionRef    = event.getSessionRef();
		txInfo.transactionID = event.getTransactionId();

		m_callback.onTXChange(txInfo);
		return true;
	}

	FUNCTION_EXIT;
	return false;  // Not a ceaseTX or demandTX message
}


void RadioTcpServer::onDetachSession(TA_IRS_App::RadioEvent & event, DetachSessionEventReader * reader)
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "RadioTcpServer %d received unsolicited DetachSession event. Status=%d", m_serverIndex, event.getStatus() );

	unsigned long sessRef = 0xFE;

	try
	{
		sessRef = event.getSessionRef();

		RadioSession* session = NULL;
		
		for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
		iter != m_radioSessions.end() ;
		iter++ 
			)
		{
			if ( (*iter)->getRefID() == sessRef )
			{
				session = *iter;
				break;
			}
		}

		if ( session != NULL )
		{
			session->overwriteSessionStatus( TA_IRS_App::RadioSession::SessionUnassigned );
			
			resetSession( sessRef );
		}

	}
	catch ( ... )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The Session reference of DetachSession event is not valid." );
	}

}

/**
* This method will handle unsolicited SystemError events
*/
void RadioTcpServer::onSystemError(TA_IRS_App::RadioEvent & event, SystemErrorEventReader * seReader)
{
	FUNCTION_ENTRY("onSystemError");

    std::string tmstmp = seReader->getTimestamp();
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "RadioTcpServer %d received unsolicited SystemError event. Status=%d, Timestamp=%s",
        m_serverIndex, event.getStatus(), tmstmp.c_str() );

	switch(event.getStatus())
	{
		// If we get -3 or -20, we will rest all connections of this TCP server.
		case STATUS_KO_SWITCH_NOT_REACHABLE:
		case STATUS_KO_GWLD_LINK_LOST:
			{
				if (!isResetConnectionInProcess())
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"[SystemError] RadioTcpServer %d received %l for session %lu. We will re-attach all sessions now.", 
						m_serverIndex, event.getStatus(), event.getSessionRef() );
					resetConnection();
				}
				else
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"[SystemError] RadioTcpServer %d received %l for session %lu while resetConnection is in processing, event will be ignored", 
						m_serverIndex, event.getStatus(), event.getSessionRef() );
				}
			}
			break;
		// Do nothing here.
		default:
			break;
	}

	FUNCTION_EXIT;
}

void RadioTcpServer::onSubscriberActivity(RadioEvent & event, SubscriberActivityEventReader * saReader)
{

    std::string subnum = saReader->getSubscriberNumber();

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
         "Received SubscriberActivity event TSI=\"%s\" MonitorRef=%lu CallRef=%lu CallStatus=%c",
         subnum.c_str(), saReader->getMonitorReference(),
         saReader->getCallReference(), saReader->getCallStatus());

    std::string subscriber_tsi = saReader->getSubscriberNumber();
    MonitorReference monitor_ref = saReader->getMonitorReference();
    CallID call_id = saReader->getCallReference();
    bool call_start = saReader->isCallStart();
	
    // only send up to monitor gui if we're actually monitoring this one
    if (getSubscriberMonitor().isMonitored(saReader->getMonitorReference()))
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
             "Adding SubscriberActivityTask for TSI=\"%s\" MonitorRef=0x%X CallRef=0x%X CallStatus=%c",
             subnum.c_str(), saReader->getMonitorReference(), saReader->getCallReference(), ((call_start)?"Started":"Ended"));

        addBackgroundTask( RadioTaskFactory::createSubscriberActivityTask(
                                this, 
								*m_subscriberMonitor, 
								event.getSessionRef(),
                                saReader->getCallReference(),
                                monitor_ref,
                                call_start)
                                       );
    }
    else
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
             "Not monitoring MonitorRef=%ld TSI='%s'", saReader->getMonitorReference(),subnum.c_str());
    }

}

void RadioTcpServer::onMonitorSubscriber(RadioEvent & event, MonitorSubscriberEventReader * msReader)
{
    std::string subscriber_tsi = msReader->getSubscriberNumber();
    MonitorReference monitor_ref = msReader->getMonitorReference();

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
         "RadioTcpServer %d Monitor %lu MonitorSubscriber event TSI '%s' RejectReason '%c'",
         m_serverIndex, monitor_ref, subscriber_tsi.c_str(), msReader->getRejectReason());

    // for one shot monitoring, after we receive all subscriber activity events
    // we get a MonitorSubscriber Event, which indicates the end of monitoring
    
    // remove the monitor reference from the subscriber monitor
    getSubscriberMonitor().removeMonitor(monitor_ref);
}


void RadioTcpServer::sessionLoggedOut(SessionReference sessionRef)
{
	FUNCTION_ENTRY("sessionLoggedOut");

    { // Threadguard scope restriction
    // essentially need to drop all calls, as the RCS won't tell us if the calls for a
	// session have ended when it logs out, it will just drop them silently - and
	// hence so should we.
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	

    RadioRemoveCallFromRadioCallStack message;

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "RadioTcpServer %d Session %lu logged out, removing calls from stack", m_serverIndex, sessionRef);

	// Find the call in the stack
	CallStack::iterator it = m_radioCallStack.begin();
    while (it != m_radioCallStack.end())
	{
		// Is this a call from the session
		if ((*it).sessionRef == sessionRef)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"RadioTcpServer %d found Call %lu for session %lu",
                        m_serverIndex, (*it).callID, sessionRef);

			// Notify interested subscribers, eg. RadioManager, CallBanner
			it->callStatus = Deleted;

			onCallbackCallStatusChange(*it);

			// Remove the call
            message.callID = it->callID;
			message.consoleID = it->consoleID;
			it = m_radioCallStack.erase(it--);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "RadioTcpServer %d Call %lu removed from call stack for Session %lu",
                        m_serverIndex, message.callID, sessionRef);

            // Broadcast a state update message to the peer agent in monitor mode
            RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveCallFromRadioCallStack( message);
			std::string key=getKeyForSessionCallID(message.callID,sessionRef);
			// remove it from the pending deletion list
			std::list<std::string>::iterator findIter = 
				std::find(m_radioCallsPendingDeletion.begin(), m_radioCallsPendingDeletion.end(), key);
			if (findIter != m_radioCallsPendingDeletion.end())
			{
				m_radioCallsPendingDeletion.erase(findIter);
			}

			// TD16752 erasing from a vector invalidates the current iterator
			// Just have to start from the beginning again
            // No. Now use return value of erase()
			// it = m_radioCallStack.begin();

            if (it != m_radioCallStack.end())
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                            "RadioTcpServer %d Session %lu next item on stack is Call %lu Session %lu",
                            m_serverIndex, sessionRef, (*it).callID, (*it).sessionRef);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                            "RadioTcpServer %d Session %lu at end of call stack.",
                            m_serverIndex, sessionRef);
            }
		}
        else
        {
            it++;
        }
	}

	} // Threadguard scope restriction

    getAuthorisationManager().sessionLoggedOut(sessionRef);

	FUNCTION_EXIT;
}


void RadioTcpServer::sessionLoggedIn(SessionReference sessionRef)
{

}

/**
* This method will handle changes to radio fall back mode.  Some functions will
* become unavailable therefore applications must be notified.  An alarm should
* be raised/closed to alert the operator.
*/
void RadioTcpServer::onRadioFallback(bool isFallback, EFallbackReason fallbackReason)
{
	FUNCTION_ENTRY("onRadioFallback");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
         "RadioTcpServer %d Received Notification of RadioFallback [%s] (Currently %s)", m_serverIndex, (isFallback)?"Fallback":"Normal", (inFallback())?"Fallback":"Normal");

    // only trigger activities if we haven't already
    if ( inFallback() != isFallback )
    {
        m_radio.onRadioFallback(isFallback, fallbackReason);

    }
    
	FUNCTION_EXIT;
}


/**
* Add a new call to the call stack
* @param    details
*/

void RadioTcpServer::addCallToCallStack(CallDetailsType& details)
{
	FUNCTION_ENTRY("addCallToCallStack");

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
         "RadioTcpServer %d Call %lu Session %lu request to add to call stack.", m_serverIndex, details.callID,details.sessionRef);

	TA_Base_Core::ThreadGuard  guard(m_callStackLock);

	// Discard if a duplicate
	CallStack::iterator  it;

	//set the console id
	unsigned long consoleID = 0;
	for(RadioSessionSet::iterator snIter = m_radioSessions.begin(); 
		snIter != m_radioSessions.end(); snIter ++)
	{
		try
		{
			if((*snIter)->getValidSessionRef() == details.sessionRef)
			{
				consoleID = (*snIter)->getConsoleID();
				break;
			}		
		}
		catch(RadioSessionNotInitialisedException)
		{
			// ignore - only interested in sessions that do match
		}
	}

	//update detail console Id
	details.consoleID = consoleID;

	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		if (it->callID==details.callID && it->sessionRef==details.sessionRef) 
		{
			updateExistingCallInCallStack(details);
			FUNCTION_EXIT;
			return;
		}
	}
    
    RadioUpdateCallInRadioCallStack message;
    
	// Set the time if not already set
	if (details.callTime.size()<1)
	{
		details.time = RadioHelper::getCurrentTime();
		details.callTime = RadioHelper::getCurrentTimeString(&(details.time));
	}


    if (consoleID == 0)
    {
        // no entities should have 0 console id - so we can assume it was not found
        // So what to do ? The call has already been received. But if it cannot be seen
        // then it is never answered, so we can safely refuse to add to the callstack
        // and it the radio system will cancel it from their side eventually
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
             "RadioTcpServer %d Call %lu did NOT find Session %lu", m_serverIndex, details.callID, details.sessionRef);
        return;
    }
	details.consoleID = consoleID;
std::string key=getKeyForSessionCallID(details.callID,details.sessionRef);
	// if its in the pending deletion list then dont add it
std::list<std::string>::iterator findIter = 
		std::find(m_radioCallsPendingDeletion.begin(), m_radioCallsPendingDeletion.end(), key);

	if (findIter == m_radioCallsPendingDeletion.end())
	{
    m_radioCallStack.push_back(details);

    LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
         "RadioTcpServer %d Session %lu Call %lu added to call stack.", m_serverIndex, details.sessionRef, details.callID);

    // Broadcast a state update message to the peer agent in monitor mode
    // m_radio.addCallToCallStack(details);
    message.callDetails = details;
    RadioStateUpdateBroadcaster::getInstance()->sendRadioUpdateCallInRadioCallStack(message);
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
         "[Add Call] State update sent to monitor agent");

		// find it in the pending additions list and remove it if necessary
		findIter = 
			std::find(m_radioCallsPendingAddition.begin(), m_radioCallsPendingAddition.end(), getKeyForSessionCallID(details.callID,details.sessionRef));
		if (findIter != m_radioCallsPendingAddition.end())
		{
			m_radioCallsPendingAddition.erase(findIter);
		}

    onCallbackCallStatusChange(details);

	}
	else
	{
		// prevented from adding the call
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		 "Call %ld deleted before it was added to the call stack. Not added",details.callID);

		// remove it from the pending deletion list
		m_radioCallsPendingDeletion.erase(findIter);
	}

	FUNCTION_EXIT;
}


void RadioTcpServer::updateExistingCallInCallStack(CallDetailsType& details)
{
	FUNCTION_ENTRY("updateExistingCallInCallStack");
    
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
         "RadioTcpServer %d Call %lu Session %lu ConsoleID %d request to update in call stack.", m_serverIndex, details.callID,details.sessionRef,details.consoleID);
    
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	
    RadioUpdateCallInRadioCallStack message;
        
	// check existing
	CallStack::iterator callIter;
    bool found = false;
    
	// Find the call in the stack
	for (callIter = m_radioCallStack.begin(); callIter != m_radioCallStack.end(); ++callIter)
	{
		// Is this the call
		if (callIter->callID == details.callID && callIter->sessionRef==details.sessionRef) 
		{
            found = true;
            break;
		}
	}
    
    if (false == found)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Call %d not found in call stack to update - possibly ended earlier", details.callID);

        return;
    }
    
    // selectively update fields
    // only those possibly updated by the call stack update task.
    callIter->callerName = details.callerName;
    callIter->callerTSI = details.callerTSI;
    callIter->callerLocation = details.callerLocation;
    callIter->calledName = details.calledName;
    callIter->calledTSI = details.calledTSI;
    callIter->calledLocation = details.calledLocation;
    callIter->isEmergency = details.isEmergency;
    callIter->participants = details.participants;
    
    LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
          "RadioTcpServer %d Session %lu Call %lu updated in call stack.",
          m_serverIndex, details.sessionRef, details.callID);
    
    // Broadcast a state update message to the peer agent in monitor mode
    // m_radio.addCallToCallStack(details);
    message.callDetails = *callIter;
    RadioStateUpdateBroadcaster::getInstance()->sendRadioUpdateCallInRadioCallStack(message);
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                 "[Update Call] State update sent to monitor agent");

    onCallbackCallStatusChange(details);
    
	FUNCTION_EXIT;
}

/**
* Update the status of a call in the call stack
* @param    callId
* @param    status
*/
bool RadioTcpServer::updateCallStatusInCallStack(CallID callId,ECallStatusType status,SessionReference sessionRef,bool isTrain/*=false*/)
{
	FUNCTION_ENTRY("updateCallStatusInCallStack");

	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	

	CallStack::iterator  it;
    RadioUpdateCallInRadioCallStack message;

	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		bool fromTrain = (it->callCategory==TA_IRS_App::CallDriver);
		if ((it->callID==callId)&&(fromTrain==isTrain)&&(it->sessionRef==sessionRef))
		{
			// Update the status
			it->callStatus = status;

			// Notify interested subscribers
			onCallbackCallStatusChange(*it);

            // Broadcast a state update message to the peer agent in monitor mode
            message.callDetails = *it;
            RadioStateUpdateBroadcaster::getInstance()->sendRadioUpdateCallInRadioCallStack(message);

            LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                 "RadioTcpServer %d Session %lu Call %lu updated.", m_serverIndex, it->sessionRef, callId);

			FUNCTION_EXIT;
			return true;
		}
	}

	// Did not find call to update
	LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
		 "RadioTcpServer %d Call %lu NOT found in call stack to update status.", m_serverIndex, callId);
	FUNCTION_EXIT;
    return false;
}

bool RadioTcpServer::removeCallFromCallStack(CallID callId, SessionReference sessionId,bool isTrain /*=false*/)
{
	FUNCTION_ENTRY("removeCallFromCallStack()");
    
    {
        // handle any group call ack details
        TA_Base_Core::ThreadGuard pgcag(m_groupCallAckLock);
		
	std::string key=getKeyForSessionCallID(callId,sessionId);       
        // remove from the map
		m_pendingGCADetails.erase(key);
		m_pendingGCATsiVectors.erase(key);
    }
	
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	
	
    RadioRemoveCallFromRadioCallStack message;
	
	CallStack::iterator  it;
	
	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		bool fromTrain = (it->callCategory==TA_IRS_App::CallDriver);
		//if ((it->callID==callId)&&(fromTrain==isTrain))//Changed By Raghu on 3/6/10
		if ((it->callID==callId)&&(fromTrain==isTrain)&&(sessionId==it->sessionRef))
		{
            SessionReference sessionRef = it->sessionRef;
			// Notify interested subscribers
			it->callStatus = Deleted;
			onCallbackCallStatusChange(*it);
			
			// Remove the call
            message.callID = it->callID;
			message.consoleID = it->consoleID;
			m_radioCallStack.erase(it);
			
            // Broadcast a state update message to the peer agent in monitor mode
            RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveCallFromRadioCallStack(
                message);
			std::string key=getKeyForSessionCallID(callId,sessionId);
			// remove it from the pending deletion list
			std::list<std::string>::iterator findIter = 
				std::find(m_radioCallsPendingDeletion.begin(), m_radioCallsPendingDeletion.end(), key);
			if (findIter != m_radioCallsPendingDeletion.end())
			{
				m_radioCallsPendingDeletion.erase(findIter);
			}
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"RadioTcpServer %d Session %lu Call %lu removed from call stack.",
				m_serverIndex, sessionRef, callId);
			
			FUNCTION_EXIT;
			return true;
		}
	}
	
	// Did not find call to delete
	LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
		"RadioTcpServer %d Call %lu not found in call stack to delete.", m_serverIndex, callId);
	
	// add the call to the pending deletion list if it is pending addition
	std::string key=getKeyForSessionCallID(callId,sessionId);
	std::list<std::string>::iterator findIter = 
		std::find(m_radioCallsPendingAddition.begin(), m_radioCallsPendingAddition.end(), key);
	if (findIter != m_radioCallsPendingAddition.end())
	{
		m_radioCallsPendingAddition.erase(findIter);
		m_radioCallsPendingDeletion.push_back(key);
	}
	
	FUNCTION_EXIT;
    return false;
}


/**
* Removes a call from the radio call stack. This method is only used by the Radio Class to forward the trainId which is assumed to be unique.
* @param    callId
*/
bool RadioTcpServer::removeCallFromCallStack(CallID callId, bool isTrain /*=false*/)
{
	FUNCTION_ENTRY("removeCallFromCallStack()");
    char C_callID[255] = {0};
	sprintf(C_callID,"%lu_",callId);
	std::string s_callid(C_callID);
    {
        // handle any group call ack details
        TA_Base_Core::ThreadGuard pgcag(m_groupCallAckLock);
		for(CallDetailsMap::iterator it=m_pendingGCADetails.begin();it!=m_pendingGCADetails.end();it++)
		{
			std::string key=it->first;
			if(key.find(s_callid)==std::string::npos) // Not Found!
			continue;
			//Remove from map
			m_pendingGCADetails.erase(key);
			m_pendingGCATsiVectors.erase(key);
			break;//Break the loop as there SHOULD BE only one match with TrainID
		}
    }

	TA_Base_Core::ThreadGuard  guard(m_callStackLock);	

    RadioRemoveCallFromRadioCallStack message;

	CallStack::iterator  it;

	// Find the call in the stack
	for (it = m_radioCallStack.begin();it!=m_radioCallStack.end();it++)
	{
		// Is this the call
		bool fromTrain = (it->callCategory==TA_IRS_App::CallDriver);
		if ((it->callID==callId)&&(fromTrain==isTrain))
		{
            SessionReference sessionRef = it->sessionRef;
			// Notify interested subscribers
			it->callStatus = Deleted;
			onCallbackCallStatusChange(*it);

			// Remove the call
            message.callID = it->callID;
			message.consoleID = it->consoleID;
			m_radioCallStack.erase(it--);

            // Broadcast a state update message to the peer agent in monitor mode
            RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveCallFromRadioCallStack(
                message);
			
			// remove it from the pending deletion list
			for(std::list<std::string>::iterator it=m_radioCallsPendingDeletion.begin();it!=m_radioCallsPendingDeletion.end();it++)
			{
				if((*it).find(s_callid)==std::string::npos) // Not Found!
				continue;
				m_radioCallsPendingDeletion.erase(it);
				break;//Break the loop as there SHOULD BE only one match with TrainID
			}

			/*
			// remove it from the pending deletion list
			std::list<CallID>::iterator findIter = 
				std::find(m_radioCallsPendingDeletion.begin(), m_radioCallsPendingDeletion.end(), callId);
			if (findIter != m_radioCallsPendingDeletion.end())
			{
				m_radioCallsPendingDeletion.erase(findIter);
			}
			*/
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "RadioTcpServer %d Session %lu Call %lu removed from call stack.",
                        m_serverIndex, sessionRef, callId);

			FUNCTION_EXIT;
			return true;
		}
	}

	// Did not find call to delete
	LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
		  "RadioTcpServer %d Call %lu not found in call stack to delete.", m_serverIndex, callId);
	
	// add the call to the pending deletion list if it is pending addition
	for(std::list<std::string>::iterator it1=m_radioCallsPendingAddition.begin();it1!=m_radioCallsPendingAddition.end();it1++)
	{
		if((*it1).find(s_callid)==std::string::npos) // Not Found!
			continue;
		m_radioCallsPendingAddition.erase(it1);
		m_radioCallsPendingDeletion.push_back(*it1);
		break;//Break the loop as there SHOULD BE only one match with TrainID
	}

/*TODO: LOOP AND DELETE
	// add the call to the pending deletion list if it is pending addition
	std::list<CallID>::iterator findIter = 
		std::find(m_radioCallsPendingAddition.begin(), m_radioCallsPendingAddition.end(), callId);
	if (findIter != m_radioCallsPendingAddition.end())
	{
		m_radioCallsPendingAddition.erase(findIter);
		m_radioCallsPendingDeletion.push_back(callId);
	}
*/
	FUNCTION_EXIT;
    return false;
}


void RadioTcpServer::addBackgroundTask(RadioTaskPtr task)
{
    m_radio.addBackgroundTask(task);
}


void RadioTcpServer::addPendingGroupCallAck(TA_IRS_App::CallDetailsType& pendingDetails)
{
    FUNCTION_ENTRY("addPendingGroupCallAck(TA_IRS_App::CallDetailsType *)");
    TA_Base_Core::ThreadGuard pgcag(m_groupCallAckLock);
	
	std::string key=getKeyForSessionCallID(pendingDetails.callID,pendingDetails.sessionRef);
    //unsigned long key = pendingDetails.callID;

	GroupCallAckTsiVectorsMap::iterator fndTV = m_pendingGCATsiVectors.find(key);

	// if the GroupCallAck event has already occurred, send this for using, already
	if (fndTV != m_pendingGCATsiVectors.end())
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
			 "RadioTcpServer %d found TSI Vector for GroupCallAck Call %lu key %s", m_serverIndex, pendingDetails.callID,key.c_str());
        
        // add to the call stack first
       addCallToCallStack(pendingDetails);

        // arrange to get all those subscriber details
        addBackgroundTask( RadioTaskFactory::createGroupCallAckTask(this, pendingDetails, (*fndTV).second) );
         
        // remove the data from the map
        m_pendingGCATsiVectors.erase(fndTV);
        
        // also remove any pending stuff just in case
        m_pendingGCADetails.erase(key);
    }
	else
	{
        // add to the call stack first
        addCallToCallStack(pendingDetails);
        
		// The GroupCallAck event is yet to happen (if it ever does)
        // so stick these details on the backburner

        // in case there was a previous group call waiting for details (that never happened)
		m_pendingGCADetails.erase(key);

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			 "RadioTcpServer %d adding pending CallDetails for GroupCallAck Call %s", m_serverIndex, key.c_str());

		m_pendingGCADetails[key] = pendingDetails;
	}
	

  
    FUNCTION_EXIT;
}


void RadioTcpServer::onSessionUpdate(SessionReference newSessionRef, SessionReference oldSessionRef)
{
    FUNCTION_ENTRY("onSessionUpdate");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
         "RadioTcpServer %d Session %lu updated to Session %lu", m_serverIndex, oldSessionRef, newSessionRef);

    TA_IRS_App::CallDetailsType details;

    details.callID = newSessionRef;
    details.sessionRef = oldSessionRef;
    details.callStatus = TA_IRS_App::SessionUpdate;
    details.callCategory = TA_IRS_App::FullDuplex;

    onCallbackCallStatusChange(details);

    FUNCTION_EXIT;
}

void RadioTcpServer::applyFullState(const TcpServerType& state)
{
	FUNCTION_ENTRY("applyFullState");
	
	clearFullState();

	std::vector<ConsoleSessionRef> conSessions;
	std::set<ConsoleID> consoles;
    for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
            iter != m_radioSessions.end() ;
            iter++ 
        )
    {
        consoles.insert( (*iter)->getConsoleID() );
		try
		{
			ConsoleSessionRef csRef;
			csRef.consoleKey = (*iter)->getConsoleID();
			csRef.sessionRef = (*iter)->getValidSessionRef();
			conSessions.push_back(csRef);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
				"RadioTcpServer::applyFullState Cannot get SessionRefs for ConsoleID %d",(*iter)->getConsoleID());
		}
    }

    {
        TA_Base_Core::ThreadGuard guard(m_callStackLock);

        // clear the stack before applying the new calls
        m_radioCallStack.clear();
        
        CallDetailsTypeList::const_iterator it;
        for (it = state.radioCallStack.begin(); it != state.radioCallStack.end(); it++)
        {
			 if (consoles.find(it->consoleID) != consoles.end())
				m_radioCallStack.push_back(*it);
        }
    }
	
	SessionAudioMapType tcpSessionAudioMap;
	// check whether the AudioMap should be applied for this TcpServer
	// based on the ConsoleSessionRef
    for (std::vector<ConsoleSessionRef>::iterator conIt = conSessions.begin() ;
            conIt != conSessions.end() ;
            conIt++ 
        )
    {
		SessionAudioMapType::const_iterator csMap = state.radioResourceManager.sessionAudioMap.find(*conIt);
		if ( csMap != state.radioResourceManager.sessionAudioMap.end() )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"RadioTcpServer::applyFullState Insert(ConsoleID %d, SessionRef %d) to TcpServer %d",
				 csMap->first.consoleKey,csMap->first.sessionRef,m_serverIndex);

			tcpSessionAudioMap.insert(std::pair<ConsoleSessionRef,AudioEntryMapType>(csMap->first,csMap->second));
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"RadioTcpServer::applyFullState NotFound(ConsoleID %d, SessionRef %d) to TcpServer %d",
					conIt->consoleKey, conIt->sessionRef,m_serverIndex);
		}
	}
	m_audioResourceManager->applyFullState(state.radioResourceManager);

	m_subscriberMonitor->applyFullState(state.radioMonitorManager);

	m_connectionManager->setControlConnection(state.controlConnectionStatus);

	FUNCTION_EXIT;
}

void RadioTcpServer::clearFullState()
{
    {
        TA_Base_Core::ThreadGuard guard(m_callStackLock);
        
        m_radioCallStack.clear();

		m_subscriberMonitor->clearFullState();
    }
}

void RadioTcpServer::getFullState(TcpServerType& state)
{
	FUNCTION_ENTRY("getFullState");

    {
        TA_Base_Core::ThreadGuard guard(m_callStackLock);

		state.radioCallStack.clear();
        
        CallDetailsTypeList::const_iterator it;
        
        for (it = m_radioCallStack.begin(); it != m_radioCallStack.end(); it++)
        {
            state.radioCallStack.push_back(*it);
        }

		m_subscriberMonitor->getFullState(state.radioMonitorManager);

    }

	std::map<SessionReference,ConsoleID> sessions;
    for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
            iter != m_radioSessions.end() ;
            iter++ 
        )
    {
		try
		{
			SessionReference sessionRef = (*iter)->getValidSessionRef();
			sessions[sessionRef] = (*iter)->getConsoleID();
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
				"RadioTcpServer::getFullState Cannot get SessionRef for ConsoleID %d",(*iter)->getConsoleID());
		}
    }
	// Resource Manager
	m_audioResourceManager->getFullState(state.radioResourceManager,sessions);

	//get Full state of the Authorisation Manager
	m_authorisationManager->getFullState(state.radioAuthorisationManager);

	m_connectionManager->getControlConnectionStatus(state.controlConnectionStatus);

	FUNCTION_EXIT;
}

void RadioTcpServer::updateCallInRadioCallStack(const CallDetailsType& callDetails)
{
    CallDetailsTypeList::iterator it;
    CallDetailsTypeList::iterator callIterator;
    bool callFound = false;
    
    {
        TA_Base_Core::ThreadGuard guard(m_callStackLock);
        
        // Check if call exists first
        for (it = m_radioCallStack.begin(); it != m_radioCallStack.end() && !callFound; it++)
        {
            if (it->callID == callDetails.callID)
            {
                callIterator = it;
                callFound = true;
            }
        }
        
        if (callFound)
        {
            // update the call details
            *callIterator = callDetails;
        }
        else
        {
            // add the call details
            m_radioCallStack.push_back(callDetails);
        }
    }

}

void RadioTcpServer::removeCallFromRadioCallStack(CallID callID)
{
    CallDetailsTypeList::iterator it;
    bool callFound = false;
    
    {
        TA_Base_Core::ThreadGuard guard(m_callStackLock);
        
        // Check if call exists first
        for (it = m_radioCallStack.begin(); it != m_radioCallStack.end() && !callFound; /* it++ */ ) // limin-- Cl-21297
        {
            if (it->callID == callID)
            {	
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Removing callID %d from radio call stack",callID);

                // Call was found so remove it
                it = m_radioCallStack.erase(it); // limin++ Cl-21297
                callFound = true;
            }
            else
            {
                ++it; // limin++ Cl-21297
            }
        }
    }
}


bool RadioTcpServer::inFallback() const
{
    return m_radio.inFallback();
}

std::string RadioTcpServer::getFallbackReplacementString()
{
    return m_radio.getFallbackReplacementString();
}

void RadioTcpServer::resetConnection()
{
    // because this could be called from an unsolicited event, and does itself
    // use the tcp connection, this needs backgrounding
    addBackgroundTask( RadioTaskFactory::createResetConnectionTask(this) );

}

void RadioTcpServer::doResetConnection()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"RadioTcpServer %d will reset connection", m_serverIndex);

	if (isResetConnectionInProcess())
	{
		return;
	}

	setResetConnectionInProcess();

	try
	{
		// because we now block waiting receipt of events that may never arrive
		// (eg. due to GWLD_LINK_LOST), we need to clear the transactions
		m_transactionManager->releaseAllTransactions();

		// perform detach on all sessions
		for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
				iter != m_radioSessions.end() ;
				iter++ 
			)
		{
			RadioSession * radioSession = *iter;

			// NO logout.  Recommended against by OTE during preVTT4-1
			// radioSession->logoutSession();
			if ( radioSession->isAttached() )
			{
				radioSession->detachSession();
			}
		}

		//
		// Wait until ALL sessions are detached
		unsigned long num_attached = 0;
		do
		{
			TA_Base_Core::Thread::sleep(40);
			num_attached = 0;
			for ( RadioSessionSet::iterator iter2 = m_radioSessions.begin() ;
				  iter2 != m_radioSessions.end() ;
				  iter2++ 
				)
			{
				//
				if ((*iter2)->isAttached())
				{
					num_attached++;
				}
			}
		}
		while (num_attached > 0);

		// Wait for 5 sec before attach.
		TA_Base_Core::Thread::sleep(8000);

		//
		// perform attach/initialise/login on all sessions
		for ( RadioSessionSet::iterator iter3 = m_radioSessions.begin() ;
			  iter3 != m_radioSessions.end() ;
			  iter3++ 
			)
		{
			(*iter3)->initialiseSession();
		}
	}
	catch ( ... ) 
	{
		clearResetConnectionInProcess();
		throw;
	}

	clearResetConnectionInProcess();

}

void RadioTcpServer::resetSession( unsigned long Ref )
{
	addBackgroundTask( RadioTaskFactory::createResetSessionTask(this, Ref) );
}

void RadioTcpServer::doResetSession( unsigned long Ref )
{
	FUNCTION_ENTRY("doResetSession");

	std::vector<unsigned long>::iterator it;

	{
		TA_Base_Core::ThreadGuard guard( m_ResetSessionLock );

		it = std::find( m_ResetSessionQueue.begin(), m_ResetSessionQueue.end(), Ref);

		if (  it != m_ResetSessionQueue.end() )
		{
			return;
		}
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                     "[SELEX] doResetSession: detach & re-attach.");

	{
		TA_Base_Core::ThreadGuard guard( m_ResetSessionLock );
		m_ResetSessionQueue.push_back( Ref );
	}

	RadioSession* session = NULL;

	for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
	iter != m_radioSessions.end() ;
	iter++ 
        )
    {
        if ( (*iter)->getRefID() == Ref )
        {
            session = *iter;
            break;
        }
    }

	if ( session != NULL )
	{
		session->detachSession();
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[SELEX] doResetSession: set to detach.");
		
		session->initialiseSession();
		
		{
			TA_Base_Core::ThreadGuard guard( m_ResetSessionLock );
			
			it = std::find( m_ResetSessionQueue.begin(), m_ResetSessionQueue.end(), Ref);
			
			while ( it != m_ResetSessionQueue.end() )
			{
				m_ResetSessionQueue.erase( it );
				it = std::find( m_ResetSessionQueue.begin(), m_ResetSessionQueue.end(), Ref);
			}		
		}
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[SELEX] doResetSession: set to re-attach.");
	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "[SELEX] doResetSession: no such session found.");
	}

	FUNCTION_EXIT;
}

void RadioTcpServer::resetSocket()
{
	addBackgroundTask( RadioTaskFactory::createResetSocketTask(this) );
}

void RadioTcpServer::doResetSocket()
{
	if ( isResetSocketInProcess() )
	{
		return;
	}

	m_ResetSocketInProcess = true;

	try
	{
		m_connectionManager->onCloseSocket();
	}
	catch ( ... )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error while trying to close socket.");
	}
	
	m_ResetSocketInProcess = false;
}

bool RadioTcpServer::isResetSessionInProcess( unsigned long Ref )
{
	TA_Base_Core::ThreadGuard guard( m_ResetSessionLock );

	std::vector<unsigned long>::iterator it = std::find( m_ResetSessionQueue.begin(), m_ResetSessionQueue.end(), Ref );

	return it != m_ResetSessionQueue.end();
}

RadioSession * RadioTcpServer::findSession(SessionReference sessionRef)
{
	FUNCTION_ENTRY("findSession");
    RadioSession * session = 0;

    for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
          iter != m_radioSessions.end() ;
          iter++ 
        )
    {
        if ( (*iter)->getValidSessionRef() == sessionRef )
        {
            session = *iter;
            break;
        }
    }

    FUNCTION_EXIT;
    return session;
}


void RadioTcpServer::addRadioSessionToThreadguardMap(RadioMethod::RadioSessionReference radioSessionRef)
{
	FUNCTION_ENTRY("addRadioSessionToThreadguardMap");
    
	// Look for an existing threadguard for the RadioSessionReference of the method
	SessionThreadGuardMap::iterator sesssionGuardIterator = 
		m_sessionSerialisationThreadguardMap.find(radioSessionRef);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "addRadioSessionToThreadguardMap: looking for radioSessionRef %ld",radioSessionRef);
	
	// create a new threadguard to be used for this session if it doesnt already exist
	if(sesssionGuardIterator == m_sessionSerialisationThreadguardMap.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "addRadioSessionToThreadguardMap: radioSessionRef %ld not found, adding to map",radioSessionRef);

		TA_Base_Core::ReEntrantThreadLockable * threadLockable = new TA_Base_Core::ReEntrantThreadLockable();
		m_sessionSerialisationThreadguardMap.insert(SessionThreadGuardMap::value_type(radioSessionRef,threadLockable));

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "addRadioSessionToThreadguardMap: radioSessionRef %ld succesfully added to map",radioSessionRef);
	}

    FUNCTION_EXIT;
}

void RadioTcpServer::removeSessionFromAudioMap(SessionID sessionReference)
{	
	FUNCTION_ENTRY("removeSessionFromAudioMap");

	this->m_audioResourceManager->removeSessionFromAudioMap(sessionReference);

	FUNCTION_EXIT;

}
			
void RadioTcpServer::updateAudioEntryInSessionMap(SessionID sessionReference, 
								  AudioReference audioReference,
								  const AudioEntryType& audioEntry)
{	
	FUNCTION_ENTRY("updateAudioEntryInSessionMap");

    for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ;
            iter != m_radioSessions.end() ;
            iter++ 
        )
    {
        try
        {
		   if ((*iter)->getValidSessionRef()==sessionReference)
			{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Update Audio Entry session=%d audioref = %d",sessionReference,audioReference);
			this->m_audioResourceManager->updateAudioEntryInSessionMap(sessionReference, 
																	   audioReference,
																	   audioEntry);
			break;
			}
        }
        catch(...)
        {}
    }

	FUNCTION_EXIT;
}

void RadioTcpServer::updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
										   int currentVolume)
{	
	FUNCTION_ENTRY("updateRadioResourceManagerVars");

	this->m_audioResourceManager->updateCallVolume(sessionReference, callReference, currentVolume);

	FUNCTION_EXIT;
}

void RadioTcpServer::onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details)
{
	FUNCTION_ENTRY("onAuthorisationChange");

    RadioSession * session = findSession(details.sessionRef);
    if (session != 0)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "RadioTcpServer %d CAD %lu found Session %lu for onAuthoriseChange",
                    m_serverIndex, details.callID, details.sessionRef);
        session->onAuthorisationChange(action, details);
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "RadioTcpServer %d CAD %lu could NOT find Session %lu for onAuthoriseChange",
                    m_serverIndex, details.callID, details.sessionRef);
        std::ostringstream logstr;
        logstr << "Sessions available:";
        for ( RadioSessionSet::iterator iter = m_radioSessions.begin() ; iter != m_radioSessions.end() ; iter++ )
        {
            try
            {
                logstr << " " << (*iter)->getValidSessionRef();
            }
            catch (...)
            {
                logstr << " (N/A)";
            }
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, logstr.str().c_str() );
    }
    FUNCTION_EXIT;
}

void RadioTcpServer::removeMonitoredSubscriberDetails(unsigned long monitoredReference)
{
	m_subscriberMonitor->removeMonitor(monitoredReference);

}

void RadioTcpServer::updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest)
{
	FUNCTION_ENTRY("removeAuthorisationRequest");

	m_authorisationManager->updateAuthorisationRequest(authorisationRequest);

	FUNCTION_EXIT;
}

void RadioTcpServer::removeAuthorisationRequest(CallID callID)
{
	FUNCTION_ENTRY("removeAuthorisationRequest");

	m_authorisationManager->removeAuthorisationRequest(callID);

	FUNCTION_EXIT;

}

void RadioTcpServer::onCallbackCallStatusChange(TA_IRS_App::CallDetailsType& details)
{	
	FUNCTION_ENTRY("onCallbackCallStatusChange");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[onCallStatusChange] Check if Call %d is Invisible to RadioManager GUI", details.callID);

	if ((details.callStatus == TA_IRS_App::Connected) 
		&& getSubscriberMonitor().isListenCall(details.callID))
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[onCallStatusChange] Invisible to RadioManager GUI");
		details.isInvisibleToGUIApps = true;
	}

	m_callback.onCallStatusChange(details);

	FUNCTION_EXIT;
}

//add by HuangQi     CL-20772
//Modified by Raghu. To accept secondparameter SessionRef
bool RadioTcpServer::addCallToPendingAddtionList(CallID callId,SessionReference sessionRef)
{
	FUNCTION_ENTRY("addCallToPendingAddtionList");
	bool bAdded = false;  //whether had add the callid into callstack
	TA_Base_Core::ThreadGuard  guard(m_callStackLock);

	try
	{
		std::vector<TA_IRS_App::CallDetailsType>::iterator it;
		
		for(it = m_radioCallStack.begin(); it != m_radioCallStack.end(); it++)
		{
			if( callId == it->callID && it->sessionRef==sessionRef)
			{
				bAdded = true;
				break;
			}
		}
		
		if( true == bAdded )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This call had add to call stack, callID = %d", callId);	
		}
		else
		{
			m_radioCallsPendingAddition.push_back(getKeyForSessionCallID(callId,sessionRef));
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This call had not add to call stack, then we add a pending call, callID = %d", callId);
		}
	}	
	catch(...)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown Exception when rememberPendingCall");
	}

	FUNCTION_EXIT;
	return !bAdded;
}

void RadioTcpServer::sendMethodNoReply (RadioMethod& method)
{
	
	FUNCTION_ENTRY("sendMethodNoReply");

    // before sending anything - be sure this agent is in control mode
    if ( !m_radio.getIsActive() )
    {
        TA_THROW(RadioSessionException("Agent in monitor mode - cannot send anything to hardware", 0));
    }
    
	// 
	// According to OTE (this is undocumented behaviour), methods must be serialised per session,
	// i.e. a method must receive a response before the next method may be sent. To achieve this,
	// a threadguard is created per RadioSessionReference
	RadioMethod::RadioSessionReference sessionRef = method.getRadioSessionReference();
	
	// Add the session reference to the threadguard map
	addRadioSessionToThreadguardMap(sessionRef);
	
	// Now get a lock for the threadguard of the radio session reference
	TA_Base_Core::ThreadGuard* lock = NULL;
	
#define CLEAN_LOCK_AND_REPLY delete lock; \
	lock = NULL; \

    unsigned long transactionID = 0;
	
    try
    {
        // optionally lock, based on the serialisation level
        // if the methods serialisation level is less than the agent level then serialise
        // eg a method with level 2 will be serialised when the agent level is set to 2 or more
        // setting the agent level to 0 serialises nothing
        if ( method.getSerialiseTransactionLevel() <= m_serialiseTransactionLevel )
        {
            lock = new TA_Base_Core::ThreadGuard( *(m_sessionSerialisationThreadguardMap[sessionRef]) );
        }
		
		// Get a new transaction ID
		//transactionID = m_transactionManager->getNewTransactionID();
		
		// get transaction ID but do not add the transaction ID to the wait list
		transactionID = m_transactionManager->getNewTransactionIdNoWait();

		method.setTransactionId(transactionID);
		
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Radio Tcp Server: %d, session: %lu, transaction: %lu, sending radio method: %s",
					 m_serverIndex, sessionRef, transactionID, typeid(method).name());
		
		// Send the message
		m_connectionManager->SendMethod(method);
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test DeRef Issue : Send Method Done, returning already, no wait for reply");

	}
	
	catch (...)
	{
		CLEAN_LOCK_AND_REPLY;
		throw;
	}
	
	if (lock)
	{
		delete lock;
		lock = NULL;
	}

	FUNCTION_EXIT
}