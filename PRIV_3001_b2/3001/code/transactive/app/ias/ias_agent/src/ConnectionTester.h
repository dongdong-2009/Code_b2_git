/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/ConnectionTester.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  ConnectionTester.h
//  Implementation of the Class ConnectionTester
//  Created on:      04-Mar-2005 08:23:59 AM
///////////////////////////////////////////////////////////

#if !defined(ConnectionTester_263B1D4C_EFFD_44a3_8481_C8C6FE1DE951__INCLUDED_)
#define ConnectionTester_263B1D4C_EFFD_44a3_8481_C8C6FE1DE951__INCLUDED_

#include "Thread.h"
#include "TestMessageRequest.h"
#include "IOutgoingMessageSender.h"

/**
 * The ConnectionTester is responsible for maintaining a worker thread to
 * periodically poll one of the IAS servers.  A TestMessageRequest is sent to the
 * IAS server and it is expected to respond with a TestMessageReport which arrives
 * from the MessageProcessor thread.  This message is then interpreted to update
 * the IAS server datapoint value and quality.
 * 
 * This class is also responsible for managing the process of retrying failed
 * operations.
 * @version 1.0
 * @created 04-Mar-2005 08:23:59 AM
 */
class ConnectionTester : private Thread 
{

public:
	ConnectionTester();
	virtual ~ConnectionTester();

public:
	/**
	 * @param taDataPointAccess    ItaIasAgentSummary cast into an ItaScada for access
	 * to DataPoint control functionality.
	 * @param outgoingMessageSender    The IOutgoingMessageSender instance used to
	 * send the TestMessageRequest packet to the required IAS Server.
	 * @param connectionStateObserver    Party interested in being notified of changes
	 * in connection states.
	 * @param connectionTestPollTimeInSecs    ConnectionTestPollTimeInSecs entity
	 * parameter representing the period of polling the Ias server.  (Note in seconds)
	 * @param connectionTestResponseInMsecs    ConnectionTestResponseInMsecs entity
	 * parameter representing the max time the Ias server is expected to respond to a
	 * TestMessageRequest. (Note in milliseconds)
	 * @param connectionTestRetries    ConnectionTestRetries entity parameter for the
	 * number of times to retry after a response timeout before flagging the
	 * connection with an error.
	 * @param myServerId    Which OCC server this instance of the IAS Agent is running
	 * on.
	 * 
	 */
	ConnectionTester(ItaScada& taDataPointAccess, IOutgoingMessageSender& outgoingMessageSender, IConnectionStateObserver& connectionStateObserver, time_t connectionTestPollTimeInSecs, unsigned long connectionTestResponseInMsecs, unsigned long connectionTestRetries, int myServerId);
	/**
	 * Handle Online Update parameter in a thread-safe manner
	 * @param connectionTestTimeoutInSecs
	 * 
	 */
	void updateConnectionTestPollTimeInSecs(unsigned long connectionTestTimeoutInSecs);
	/**
	 * Handle Online Update parameter in a thread-safe manner
	 */
	void updateConnectionTestResponseInMsecs();
	/**
	 * Handle Online Update parameter in a thread-safe manner
	 */
	void updateConnectionTestRetries();
	/**
	 * Handle Online Update parameter in a thread-safe manner.
	 * @param myHostId    Which OCC server this instance of the IAS Agent is running
	 * on.
	 * 
	 */
	void updateMyServerId(int myHostId);
	/**
	 * Executes after the thread is spawned by a call to start().  The poll loop is
	 * defined by:
	 * 1. Call sendTestMessageRequest
	 * 2. Sleep for ConnectionTestResponseInMsecs
	 * 3. Wake up and check that TestMessageRequest has been processed correctly in
	 * processMessage() by the Message Processing thread
	 * 4. Sleep for the remainder of the ConnectionTestPollTimeInSecs period (if any)
	 * 
	 * In the event that the process fails in step 3, steps 1-3 will be repeated
	 * ConnectionTestRetries times.  If all retries fail, then the OCC IAS Server
	 * datapoint is set to FAULT with a GOOD quality.  Proceed to call
	 * setConnectionIsDown().
	 * 
	 * The m_isRunning flag should be used to determine when to terminate this thread.
	 * 
	 * The m_isEnabled flag should be used to enable the entire poll loop in Control
	 * Mode.
	 * 
	 * Please ensure that all exceptions are caught and handled to prevent the thread
	 * dying.
	 */
	virtual void run();
	/**
	 * Second step in the server polling process.  Called by the IasAgent to match a
	 * TestMessageResponse with the current TestMessageRequest and to process the
	 * server state data.
	 * Call on setConnectionIsDown() upon completion.
	 * 
	 * The m_isEnabled flag should be used to enable this method in Control Mode.
	 * 
	 * If there is mismatch in the request and response messages, a debug warning
	 * message should be logged.  Please note that the hostId-receiveParty parameters
	 * will be swapped - i.e. TestMessageRequest::hostId = TestMessageReport::
	 * receiveParty and TestMessageRequest::receiveParty = TestMessageReport::hostId
	 * 
	 * @param message
	 * 
	 */
	void processMessage(TestMessageReport& message);
	/**
	 * Set m_isRunning to false to terminate this thread.
	 */
	virtual void terminate();
	/**
	 * Reset the TestMessageRequest member by calling
	 * setIsMatchingResponseFound(true)
	 * 
	 * Set m_isEnabled to enable Control Mode operations including
	 * - the entire thread polling loop
	 * - processMessage()
	 */
	void onControlMode();
	/**
	 * Set m_isEnabled to disable Control Mode operations including:
	 * - the entire thread polling loop
	 * - processMessage()
	 */
	void onMonitorMode();
	/**
	 * Wrapper for the m_isConnectionDown attribute.  This should compare the input
	 * value with the current m_isConnectionDown value.  If they differ, update
	 * m_isConnectionDown and then call notifyConnectionState() on the observer member.
	 * 
	 */
	void setConnectionIsDown();
	bool isConnectionDown();

protected:
	/**
	 * ItaIasAgentSummary cast into an ItaScada for access to DataPoint control
	 * functionality.
	 */
	ItaIasAgentSpecific& m_taDataPointAccess;
	/**
	 * The IOutgoingMessageSender instance used to send the TestMessageRequest packet
	 * to the required IAS Server.
	 */
	IOutgoingMessageSender& m_outgoingMessageSender;
	/**
	 * Party interested in being notified of changes in connection states.
	 */
	IConnectionStateObserver& m_connectionStateObserver;
	/**
	 * Flag used to terminate the Server Polling Thread.
	 */
	bool m_isRunning;
	/**
	 * Used to disable operations when in Monitor Mode.  Ensure that the thread is
	 * always running through the lifetime of the application.  Only toggle this flag
	 * to enable/disable Control mode functionality.  This is deliberately done to
	 * minimise failover times.
	 */
	bool m_isEnabled;
	/**
	 * Contains the last known success of a poll operation.   
	 */
	bool m_isConnectionDown;
	/**
	 * ConnectionTestPollTimeInSecs entity parameter representing the period of
	 * polling the Ias server.  (Note in seconds)
	 */
	time_t m_connectionTestPollTimeInSecs;
	/**
	 * ConnectionTestResponseInMsecs entity parameter representing the max time the
	 * Ias server is expected to respond to a TestMessageRequest. (Note in
	 * milliseconds)
	 */
	unsigned long m_connectionTestResponseInMsecs;
	/**
	 * ConnectionTestRetries entity parameter for the number of times to retry after a
	 * response timeout before flagging the connection with an error.
	 */
	unsigned long m_connectionTestRetries;
	/**
	 * Which OCC server this instance of the IAS Agent is running on.
	 */
	int m_myServerId;
	/**
	 * Last sent TestMessageRequest.  If the resulting TestMessageReponse is received,
	 * then call setIsMatchingResponseFound(true) to indicate a successful operation.
	 * 
	 */
	TestMessageRequest* m_currentTestMessageRequest;
	/**
	 * Represents the last 4 digits of the transaction no used in the IasMessages.  To
	 * be appended to the day of the month to complete this field.
	 */
	int m_nextTransactionIdForTheDay;
	/**
	 * The TestMessageRequest member is updated and sent to IOutgoingMessageSender
	 * instance.  Use the m_nextTransactionIdForTheDay and append to the day of the
	 * month to complete this field.  
	 */
	void sendTestMessageRequest();

};
#endif // !defined(ConnectionTester_263B1D4C_EFFD_44a3_8481_C8C6FE1DE951__INCLUDED_)