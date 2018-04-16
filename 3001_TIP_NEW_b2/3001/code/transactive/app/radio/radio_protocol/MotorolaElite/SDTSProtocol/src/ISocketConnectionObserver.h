/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: 
  * @author:  Ye Zhaohua
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 
  * Last modified by:  $Author: 
  *
  */

#if !defined(COESWITCH_ICONNECTIONOBSERVER_H)
#define COESWITCH_ICONNECTIONOBSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class ISocketConnectionObserver  
{
	public:

		/**
 		* processReceivedData
 		*
 		* This method is called whenever data is received from the socket.
 		* Typically the implementation of this method will determine whether
 		* a complete message is stored in the received data, and remove it if
 		* there is.  The remaining data is left in the buffer.
 		*
 		* @param std::string & pData - received data
 		*
 		* @returns nothing
 		*
 		*/
		virtual void processReceivedData( std::vector<unsigned char> & pData ) = 0;

		/**
 		* writeFailed
 		*
 		* This method is called whenever writing to the socket fails.
 		*
 		* @returns nothing
 		*/
		virtual void writeFailed() = 0;

		/**
 		* connectionLost
 		*
 		* This method is called whenever the observed socket loses the 
 		* connection to the remote socket.
 		*
 		*/
		virtual void connectionLost() = 0;

		/**
 		* connectionEstablished
 		*
 		* This method is called whenever the observed socket establishes a
 		* connection to the remote socket.  It is used in conjunction with
 		* the connectionLost method in order to handle socket reconnection
 		* at runtime.
 		*
 		* @returns nothing
 		*/
		virtual void connectionEstablished() = 0;

};

#endif // !defined(COESWITCH_ICONNECTIONOBSERVER_H)
