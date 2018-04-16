/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/TestMessageReport.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  TestMessageReport.h
//  Implementation of the Class TestMessageReport
//  Created on:      04-Mar-2005 08:24:16 AM
///////////////////////////////////////////////////////////

#if !defined(TestMessageReport_78BE395C_A150_4629_84AC_DED4698076B4__INCLUDED_)
#define TestMessageReport_78BE395C_A150_4629_84AC_DED4698076B4__INCLUDED_

#include "AbstractIncomingIasMessage.h"

/**
 * The TestMessageReport is constructed to represent an incoming test message
 * report in response to a TestMessageRequest sent earlier.
 * @version 1.0
 * @created 04-Mar-2005 08:24:16 AM
 */
class TestMessageReport : public AbstractIncomingIasMessage 
{

public:
	TestMessageReport();
	virtual ~TestMessageReport();

public:
	/**
	 * This constructor is used to construct a Test Message Report using a raw stream
	 * of bytes.  When the message is successfully interpreted, the bytes must be
	 * removed from the original stream.  Ensure that upon completion, all member
	 * variables from AbstactIasMessage are set.
	 * 
	 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
	 * 
	 * Throws IasInvalidMessageException if the packet size is not the right size (37
	 * bytes).
	 * @param rawData
	 * 
	 */
	void TestMessageReport(std::vector<unsigned char>& rawData);
	/**
	 * Return "#A71"
	 */
	static const std::string getMessageId();
	bool hasConnectionFault();

protected:
	bool m_hasConnectionFault;
	static const std::string m_messageId;

};
#endif // !defined(TestMessageReport_78BE395C_A150_4629_84AC_DED4698076B4__INCLUDED_)