/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/TestMessageRequest.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  TestMessageRequest.h
//  Implementation of the Class TestMessageRequest
//  Created on:      04-Mar-2005 08:24:17 AM
///////////////////////////////////////////////////////////

#if !defined(TestMessageRequest_D5432CAD_E1F1_4a06_8907_878F2B848E28__INCLUDED_)
#define TestMessageRequest_D5432CAD_E1F1_4a06_8907_878F2B848E28__INCLUDED_

#include "AbstractOutgoingIasMessage.h"

/**
 * The TestMessageRequest is constructed to represent a request message for the
 * IAS server to return a server report.
 * @version 1.0
 * @created 04-Mar-2005 08:24:17 AM
 */
class TestMessageRequest : public AbstractOutgoingIasMessage 
{

public:
	TestMessageRequest();
	virtual ~TestMessageRequest();

public:
	/**
	 * Construct by calling AbstractOutgoingIasMessage()
	 * - Use m_messageId for messageId.
	 * - Use 0 for repeatNumber.
	 * 
	 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
	 * 
	 * @param transactionNumber
	 * @param myHostId
	 * @param receiveParty
	 * 
	 */
	TestMessageRequest(int transactionNumber, int myHostId, int receiveParty);
	/**
	 * Return "#M71"
	 */
	static const std::string getMessageId();
	bool isMatchingResponseFound();
	/**
	 * @param value
	 * 
	 */
	void setIsMatchingReponseFound(bool value);
	/**
	 * This pure virtual method must be implemented by derived classes so that a raw
	 * byte stream can be produced ready for sending through a socket stream.
	 */
	virtual std::vector<unsigned char> getRawData();

protected:
	int m_isMatchingResponseFound;
	static const std::string m_stationId;
	static const std::string m_messageId;

};
#endif // !defined(TestMessageRequest_D5432CAD_E1F1_4a06_8907_878F2B848E28__INCLUDED_)