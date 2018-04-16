/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/AbstractOutgoingIasMessage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  AbstractOutgoingIasMessage.h
//  Implementation of the Class AbstractOutgoingIasMessage
//  Created on:      04-Mar-2005 08:23:56 AM
///////////////////////////////////////////////////////////

#if !defined(AbstractOutgoingIasMessage_4C81AE26_EABD_4e56_8B15_72139798FF9B__INCLUDED_)
#define AbstractOutgoingIasMessage_4C81AE26_EABD_4e56_8B15_72139798FF9B__INCLUDED_

#include "AbstractIasMessage.h"

/**
 * The AbstractOutgoingIasMessage consolidates features common to all IAS Messages
 * that can be sent to the IAS servers.
 * 
 * Note that despite its name, through the evolution of the design this is no
 * longer an abstract class.
 * @version 1.0
 * @created 04-Mar-2005 08:23:56 AM
 */
class AbstractOutgoingIasMessage : public AbstractIasMessage 
{

public:
	AbstractOutgoingIasMessage();
	virtual ~AbstractOutgoingIasMessage();

public:
	/**
	 * Construct by calling AbstractIasMessage()
	 * - Use the current time for the dateTime parameter.
	 * - USe "COM" for the systemCode parameter.
	 * @param transactionNumber
	 * @param repeatNumber
	 * @param myHostId
	 * @param receiveParty
	 * 
	 */
	AbstractOutgoingIasMessage(int transactionNumber, int repeatNumber, int myHostId, int receiveParty);
	/**
	 * This pure virtual method must be implemented by derived classes so that a raw
	 * byte stream can be produced ready for sending through a socket stream.
	 */
	virtual std::vector<unsigned char> getRawData() =0;

};
#endif // !defined(AbstractOutgoingIasMessage_4C81AE26_EABD_4e56_8B15_72139798FF9B__INCLUDED_)