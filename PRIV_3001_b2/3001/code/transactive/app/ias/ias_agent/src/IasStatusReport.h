/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/IasStatusReport.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  IasStatusReport.h
//  Implementation of the Class IasStatusReport
//  Created on:      04-Mar-2005 08:24:10 AM
///////////////////////////////////////////////////////////

#if !defined(IasStatusReport_8ED5FD5D_199B_40cf_908A_B569E98F3E9F__INCLUDED_)
#define IasStatusReport_8ED5FD5D_199B_40cf_908A_B569E98F3E9F__INCLUDED_

#include "AbstractIncomingIasMessage.h"

/**
 * The IasStatusReport is constructed to represent an incoming status report
 * message.
 * @version 1.0
 * @created 04-Mar-2005 08:24:10 AM
 */
class IasStatusReport : public AbstractIncomingIasMessage 
{

public:
	IasStatusReport();
	virtual ~IasStatusReport();

public:
	/**
	 * This constructor is used to construct an IAS Status Report using a raw stream
	 * of bytes.  When the message is successfully interpreted, the bytes must be
	 * removed from the original stream.  Ensure that upon completion, all member
	 * variables from AbstactIasMessage are set.
	 * 
	 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
	 * 
	 * Throws IasInvalidMessageException if the packet size is not the right size (40
	 * bytes).
	 * @param rawData
	 * 
	 */
	void IasStatusReport(std::vector<unsigned char>& rawData);
	/**
	 * Return "#A70"
	 */
	static const std::string getMessageId();
	bool hasFault();
	bool hasIntruderAlarm();
	std::string getStation();

protected:
	bool m_hasFault;
	bool m_hasIntruderAlarm;
	std::string m_stationId;
	static const std::string m_messageId;

};
#endif // !defined(IasStatusReport_8ED5FD5D_199B_40cf_908A_B569E98F3E9F__INCLUDED_)