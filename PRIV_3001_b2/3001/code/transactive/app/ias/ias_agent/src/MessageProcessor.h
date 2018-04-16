/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/MessageProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  MessageProcessor.h
//  Implementation of the Class MessageProcessor
//  Created on:      04-Mar-2005 08:24:14 AM
///////////////////////////////////////////////////////////

#if !defined(MessageProcessor_C9B8D95D_F8ED_4735_94DF_F6A182A1CEA6__INCLUDED_)
#define MessageProcessor_C9B8D95D_F8ED_4735_94DF_F6A182A1CEA6__INCLUDED_

#include "IIncomingMessageProcessor.h"
#include "Thread.h"

/**
 * The MessageProcessor class is responsible for maintaining a worker thread to
 * frequently pull off TestMessageReports and IasStatusReports off the queue for
 * processing by the IIncomingMessageProcessor instance.
 * 
 * This class is also responsible for deleting all AbstractIncomingIasMessages
 * from the SingletonQueue instance after they are processed.  Please ensure that
 * during processing, all exceptions are caught to prevent memory leaks.
 * @version 1.0
 * @created 04-Mar-2005 08:24:14 AM
 */
class MessageProcessor : public Thread 
{

public:
	MessageProcessor();
	virtual ~MessageProcessor();

public:
	/**
	 * @param incomingMessageProcessor    Instance of the IIncomingMessageProcessor
	 * for processing received IAS Messages
	 * @param myServerId    Which OCC server this instance of the IAS Agent is running
	 * on.
	 * 
	 */
	MessageProcessor(IIncomingMessageProcessor& incomingMessageProcessor, int myServerId);
	/**
	 * Set m_inEnabled to disable message processing.  Also purge any queued messages.
	 */
	void onMonitorMode();
	/**
	 * Set m_inEnabled to enable message processing.
	 */
	void onControlMode();
	/**
	 * Executed when the thread is spawned by a call to start().  All this method
	 * needs to do is to call processNextMessage and then sleep for a small amount of
	 * time (e.g. 100ms).  The m_isRunning flag should be used to determine when to
	 * terminate this thread.
	 * 
	 * Please ensure that all exceptions are caught and handled to prevent the thread
	 * dying.
	 */
	void run();
	/**
	 * Handle Online Update parameter in a thread-safe manner.
	 */
	void updateMyServerId();
	/**
	 * Set m_isRunning to false to terminate this thread.
	 */
	void terminate();

protected:
	/**
	 * Flag used to terminate the Message Processing Thread.
	 */
	bool m_isRunning;
	/**
	 * Flag used to enable/disable the processing of messages for Control/Monitor
	 * modes.
	 */
	bool m_isEnabled;
	/**
	 * Which OCC server this instance of the IAS Agent is running on.
	 */
	int m_myServerId;
	SingletonQueue<AbstractIncomingIasMessage>* m_incomingMessageQ;
	/**
	 * Instance of the IIncomingMessageProcessor for processing received IAS Messages
	 */
	IIncomingMessageProcessor& m_incomingMessageProcessor;
	/**
	 * This method should only be executed if m_isEnabled is true(i.e. in Control
	 * Mode).
	 * 
	 * The next message should be taken off the SingletonQueues and processed.  If the
	 * getItem() call returns NULL, no messages are currently available.  The thread
	 * should return to the run loop to sleep.
	 * 
	 * If a new message is returned, check the ReceiveParty parameter of the
	 * IasMessage and compare to m_myHostId to see if the message is destined for this
	 * instance of the IasAgent.  If not, it is discarded and a debug warning message
	 * should be logged.
	 * 
	 * Check the message type obtained from the queue to determine which of the
	 * IIncomingMessageProcessor methods to call.  When processing is completed, this
	 * class will ensure that the message is destroyed to prevent a memory leak.
	 */
	void processNextMessage();

};
#endif // !defined(MessageProcessor_C9B8D95D_F8ED_4735_94DF_F6A182A1CEA6__INCLUDED_)