/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/AbstractIasMessage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  AbstractIasMessage.h
//  Implementation of the Class AbstractIasMessage
//  Created on:      04-Mar-2005 08:23:55 AM
///////////////////////////////////////////////////////////

#if !defined(AbstractIasMessage_906E704D_509C_441c_99CB_EBEDA3C4E35B__INCLUDED_)
#define AbstractIasMessage_906E704D_509C_441c_99CB_EBEDA3C4E35B__INCLUDED_

/**
 * The AbstractIasMessage class is a placeholder for fields common to all IAS
 * Messages.  All IAS Messages are derived from the AbstractIasMessage class.
 * Internally, all states are kept as basic types.
 * 
 * Note that despite its name, through the evolution of the design this is no
 * longer an abstract class.
 * @version 1.0
 * @created 04-Mar-2005 08:23:55 AM
 */
class AbstractIasMessage
{

public:
	AbstractIasMessage();
	virtual ~AbstractIasMessage();

public:
	/**
	 * This constructor is used to construct the message using explicit fields.
	 * 
	 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
	 * 
	 * @param transactionNumber
	 * @param repeatNumber
	 * @param hostId
	 * @param systemCode
	 * @param dateTime    Number of seconds since dd mm 1970.
	 * @param receiveParty
	 * 
	 */
	AbstractIasMessage(int transactionNumber, int repeatNumber, int hostId, const std::string& systemCode, time_t dateTime, int receiveParty);
	int getTransactionNumber();
	int getRepeatNumber();
	void incrementRepeatNumber();
	int getHostId();
	time_t getDateTime();
	int getReceiveParty();

protected:
	int m_transactionNumber;
	int m_repeatNumber;
	/**
	 * Note that hostId and receiveParty will be swapped between incoming and outgoing
	 * messages.
	 */
	int m_hostId;
	std::string m_systemCode;
	time_t m_dateTime;
	/**
	 * Note that hostId and receiveParty will be swapped between incoming and outgoing
	 * messages.
	 */
	int m_receiveParty;

};
#endif // !defined(AbstractIasMessage_906E704D_509C_441c_99CB_EBEDA3C4E35B__INCLUDED_)