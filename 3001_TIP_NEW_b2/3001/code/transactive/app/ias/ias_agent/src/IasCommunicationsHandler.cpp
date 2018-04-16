/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/IasCommunicationsHandler.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  IasCommunicationsHandler.cpp
//  Implementation of the Class IasCommunicationsHandler
//  Created on:      04-Mar-2005 08:24:08 AM
///////////////////////////////////////////////////////////

#include "IasCommunicationsHandler.h"


IasCommunicationsHandler::IasCommunicationsHandler(){

}



IasCommunicationsHandler::~IasCommunicationsHandler(){

}


/**
 * @param serverAddress    The IAS Server's IP address or hostname.
 * @param serverPort    IAS server's port
 * 
 */
IasCommunicationsHandler::IasCommunicationsHandler(const std::string& serverAddress, const std::string& serverPort){

}


/**
 * processReceivedData  This method is called whenever data is received from the
 * socket. Typically the implementation of this method will determine whether a
 * complete message is stored in the received data, and remove it if there is.
 * The remaining data is left in the buffer.
 * @param std::string & pData - received data
 * @param ISocket * pSocket   - the socket that received the data
 * @returns nothing
 * 
 * The datastream will be interpreted and the required Ias Message created.  The
 * start of a pack is determined by the character '#' and the end of a packet is
 * determined by the string '#END'.  The process can be detailed as follows:
 * 
 * 1. Skip data until the '#' character is found.
 * 2. Compare the first few characters in the stream with TestMessageReport and
 * IasStatusMessageReport getMessageId() (which include the starting '#').
 * 4. If no match, goto 1.
 * 5. If match, locate the string '#END'.
 * 6. If no match, then the buffer does not contain enough data.  Exit and wait
 * until more data is available.
 * 7. From the start of the stream, determine what type the message is.
 * 8. Construct the required message using the stream.  This must be done on the
 * heap.
 * 9. If the construction is successful, the IAS Message class will automatically
 * remove the processed bytes from the stream.
 * 
 * If everything goes well, the IasMessage will be added to the
 * SingletonQueue<AbstractIncomingIasMessage> instance.
 * 
 * If the IasInvalidMessageException is thrown in the construction process, or
 * unexpected data is found (and skipped) in the processing, log a debug warning
 * message.
 * @param pData
 * @param pSocket
 * 
 */
void IasCommunicationsHandler::processReceivedData(std::vector<unsigned char>& pData, ISocket* pSocket){

}


/**
 * Log a debug warning message.
 */
void IasCommunicationsHandler::writeFailed(){

}


/**
 * Log a debug info message.
 */
bool IasCommunicationsHandler::connectionLost(){

}


/**
 * Log a debug info message.
 */
void IasCommunicationsHandler::connectionEstablished(){

}


/**
 * Connect the socket (e.g. entering Control Mode).  Ensure the socket instance
 * exists and create it if not.
 */
void IasCommunicationsHandler::connect(){

}


/**
 * Disconnect the socket (e.g. destruction, entering Monitor Mode.).  Delete the
 * socket instance and set to NULL so that a subsequent connect() call will create
 * a new socket.  This is a work-around for problems in the socket close() method.
 * 
 */
void IasCommunicationsHandler::disconnect(){

}


/**
 * Handle Online Update parameter in a thread-safe manner.  Close and re-connect
 * the current connection if the new address is different to the current.
 * @param primaryServerAddress
 * 
 */
void IasCommunicationsHandler::updateServerAddress(const std::string& primaryServerAddress){

}


/**
 * Handle Online Update parameter in a thread-safe manner.  Close and re-connect
 * the current connection if the new port is different to the current.
 * @param primaryServerPort
 * 
 */
void IasCommunicationsHandler::updateServerPort(const std::string& primaryServerPort){

}


/**
 * Enable Control Mode operations.
 * Reconnect the socket.
 */
void IasCommunicationsHandler::onControlMode(){

}


/**
 * Disable Control Mode operations.
 * Disconnect the socket.
 */
void IasCommunicationsHandler::onMonitorMode(){

}


/**
 * Takes the IAS Message and adds the raw data to the write buffer to be queued
 * for transfer.
 * If m_isEnabled is false (i.e. in monitor mode), the method should return
 * without doing anything.
 * 
 * Must ensure that the socket is connected, and if not, a new connection should
 * be made.
 * @param data    The IAS message to be written to the stream
 * 
 */
void IasCommunicationsHandler::write(AbstractOutgoingIasMessage& data){

}


