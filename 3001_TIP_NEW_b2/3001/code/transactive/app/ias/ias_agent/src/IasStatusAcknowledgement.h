/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/IasStatusAcknowledgement.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  IasStatusAcknowledgement.h
//  Implementation of the Class IasStatusAcknowledgement
//  Created on:      04-Mar-2005 08:24:09 AM
///////////////////////////////////////////////////////////

#if !defined(IasStatusAcknowledgement_DF83AE9B_7D73_4350_A1C2_687D86CBB213__INCLUDED_)
#define IasStatusAcknowledgement_DF83AE9B_7D73_4350_A1C2_687D86CBB213__INCLUDED_

#include "AbstractOutgoingIasMessage.h"

/**
 * The IasStatusAcknowledgement message is realised and sent as a response to a
 * received and processed IasStatusReport message.
 * @version 1.0
 * @created 04-Mar-2005 08:24:10 AM
 */
class IasStatusAcknowledgement : public AbstractOutgoingIasMessage 
{

public:
	IasStatusAcknowledgement();
	virtual ~IasStatusAcknowledgement();

public:
	/**
	 * Construct by calling AbstractOutgoingIasMessage()
	 * - Use m_messageId for messageId.
	 * - Use iasStatusReport for transactionNumber and repeatNumber.
	 * 
	 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
	 * 
	 * @param iasStatusReport    The recently received IasStatusReport packet to
	 * obtain the transactionNumber and repeatNumber from.
	 * @param myHostId
	 * @param receiveParty
	 * 
	 */
	IasStatusAcknowledgement(IasStatusReport& iasStatusReport, int myHostId, int receiveParty);
	/**
	 * Return "#A99"
	 */
	static const std::string getMessageId();
	/**
	 * This pure virtual method must be implemented by derived classes so that a raw
	 * byte stream can be produced ready for sending through a socket stream.
	 */
	virtual std::vector<unsigned char> getRawData();

protected:
	static const std::string m_messageId;
	static const std::string m_acknowledgement;

};
#endif // !defined(IasStatusAcknowledgement_DF83AE9B_7D73_4350_A1C2_687D86CBB213__INCLUDED_)