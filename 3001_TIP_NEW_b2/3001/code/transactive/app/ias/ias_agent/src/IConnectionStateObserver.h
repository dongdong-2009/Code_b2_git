/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/IConnectionStateObserver.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  IConnectionStateObserver.h
//  Implementation of the Interface IConnectionStateObserver
//  Created on:      04-Mar-2005 08:24:11 AM
///////////////////////////////////////////////////////////

#if !defined(IConnectionStateObserver_12D7FB8B_FD43_49fe_AC9B_3BA8377DB02F__INCLUDED_)
#define IConnectionStateObserver_12D7FB8B_FD43_49fe_AC9B_3BA8377DB02F__INCLUDED_

/**
 * Interface allowing an interested party to be notified when an IAS Server
 * connection is down.
 * @version 1.0
 * @created 04-Mar-2005 08:24:11 AM
 */
class IConnectionStateObserver
{

public:
	/**
	 * Informs the observer when the connection state has been changed.  Call on
	 * ConnectionTester::isConnectionDown()  to obtain the state.
	 * @param whichConnectionTester    Which ConnectionTester instance changing state.
	 * 
	 */
	void notifyConnectionState(ConnectionTester* whichConnectionTester) =0;

};
#endif // !defined(IConnectionStateObserver_12D7FB8B_FD43_49fe_AC9B_3BA8377DB02F__INCLUDED_)