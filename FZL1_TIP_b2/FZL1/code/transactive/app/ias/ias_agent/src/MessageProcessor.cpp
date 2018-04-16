/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ias/ias_agent/src/MessageProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  MessageProcessor.cpp
//  Implementation of the Class MessageProcessor
//  Created on:      04-Mar-2005 08:24:14 AM
///////////////////////////////////////////////////////////

#include "MessageProcessor.h"


MessageProcessor::MessageProcessor(){

}



MessageProcessor::~MessageProcessor(){

}


/**
 * @param incomingMessageProcessor    Instance of the IIncomingMessageProcessor
 * for processing received IAS Messages
 * @param myServerId    Which OCC server this instance of the IAS Agent is running
 * on.
 * 
 */
MessageProcessor::MessageProcessor(IIncomingMessageProcessor& incomingMessageProcessor, int myServerId){

}


/**
 * Set m_inEnabled to disable message processing.  Also purge any queued messages.
 */
void MessageProcessor::onMonitorMode(){

}


/**
 * Set m_inEnabled to enable message processing.
 */
void MessageProcessor::onControlMode(){

}


/**
 * Executed when the thread is spawned by a call to start().  All this method
 * needs to do is to call processNextMessage and then sleep for a small amount of
 * time (e.g. 100ms).  The m_isRunning flag should be used to determine when to
 * terminate this thread.
 * 
 * Please ensure that all exceptions are caught and handled to prevent the thread
 * dying.
 */
void MessageProcessor::run(){

}


/**
 * Handle Online Update parameter in a thread-safe manner.
 */
void MessageProcessor::updateMyServerId(){

}


/**
 * Set m_isRunning to false to terminate this thread.
 */
void MessageProcessor::terminate(){

}


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
void MessageProcessor::processNextMessage(){

}


