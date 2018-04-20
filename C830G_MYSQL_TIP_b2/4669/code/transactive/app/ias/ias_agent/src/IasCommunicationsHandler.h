/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ias/ias_agent/src/IasCommunicationsHandler.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  IasCommunicationsHandler.h
//  Implementation of the Class IasCommunicationsHandler
//  Created on:      04-Mar-2005 08:24:08 AM
///////////////////////////////////////////////////////////

#if !defined(IasCommunicationsHandler_E69ADF8C_A309_430b_85BF_43155F75E4BC__INCLUDED_)
#define IasCommunicationsHandler_E69ADF8C_A309_430b_85BF_43155F75E4BC__INCLUDED_

#include "SingletonQueue.h"
#include "TcpObservedSocket.h"

/**
 * The IasCommunicationsHandler class is used to manage a socket connection to a
 * single IAS server.  Write operations are asynchronous and therefore placed in a
 * queue for processing by an Observed Socket thread.  The same thread also
 * informs this class of connection state changes, write errors and received data.
 * 
 * @version 1.0
 * @created 04-Mar-2005 08:24:08 AM
 */
class IasCommunicationsHandler
{

public:
	IasCommunicationsHandler();
	virtual ~IasCommunicationsHandler();

public:
	/**
	 * @param serverAddress    The IAS Server's IP address or hostname.
	 * @param serverPort    IAS server's port
	 * 
	 */
	IasCommunicationsHandler(const std::string& serverAddress, const std::string& serverPort);
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
	virtual void processReceivedData(std::vector<unsigned char>& pData, ISocket* pSocket);
	/**
	 * Log a debug warning message.
	 */
	virtual void writeFailed();
	/**
	 * Log a debug info message.
	 */
	virtual bool connectionLost();
	/**
	 * Log a debug info message.
	 */
	virtual void connectionEstablished();
	/**
	 * Connect the socket (e.g. entering Control Mode).  Ensure the socket instance
	 * exists and create it if not.
	 */
	void connect();
	/**
	 * Disconnect the socket (e.g. destruction, entering Monitor Mode.).  Delete the
	 * socket instance and set to NULL so that a subsequent connect() call will create
	 * a new socket.  This is a work-around for problems in the socket close() method.
	 * 
	 */
	void disconnect();
	/**
	 * Handle Online Update parameter in a thread-safe manner.  Close and re-connect
	 * the current connection if the new address is different to the current.
	 * @param primaryServerAddress
	 * 
	 */
	void updateServerAddress(const std::string& primaryServerAddress);
	/**
	 * Handle Online Update parameter in a thread-safe manner.  Close and re-connect
	 * the current connection if the new port is different to the current.
	 * @param primaryServerPort
	 * 
	 */
	void updateServerPort(const std::string& primaryServerPort);
	/**
	 * Enable Control Mode operations.
	 * Reconnect the socket.
	 */
	void onControlMode();
	/**
	 * Disable Control Mode operations.
	 * Disconnect the socket.
	 */
	void onMonitorMode();
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
	void write(AbstractOutgoingIasMessage& data);

protected:
	/**
	 * Observed socket instance.  Note that when refreshing socket connections, it is
	 * best to delete the socket and construct a new one.  
	 */
	TcpObservedSocket* m_socket;
	/**
	 * The IAS Server's IP address or hostname.
	 */
	const std::string& m_serverAddress;
	/**
	 * IAS server's port
	 */
	const std::string& m_serverPort;
	/**
	 * Used to disable operations when in Monitor Mode.  Toggle this flag to
	 * enable/disable Control mode functionality.
	 */
	bool m_isEnabled;
	SingletonQueue<AbstractIncomingIasMessage>* m_incomingMessageQ;

};
#endif // !defined(IasCommunicationsHandler_E69ADF8C_A309_430b_85BF_43155F75E4BC__INCLUDED_)