/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\MessagePollTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/05 11:49:53 $
  * Last modified by:  $Author: builder $
  *
  * The Signalling system sends this message if in 5 seconds there is no message sending to ATS.
  */

#ifndef _MESSAGE_POLLING_H
#define _MESSAGE_POLLING_H
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include <vector>

namespace TA_IRS_Bus
{
	class MessagePollTelegram : public CommonTelegram
	{
	public:

	 /**
	  * TrainLocationTelegram
	  * 
	  * Constructor to build the packet from incoming data.
	  * 
	  * @return 
	  * @param : const Telegram& incomingData
	  */
		MessagePollTelegram(const Telegram& incomingData);

		/**
		 * Constructor
		 */
		MessagePollTelegram();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		MessagePollTelegram (const MessagePollTelegram&);
		MessagePollTelegram& operator= (const MessagePollTelegram&);

		static const ta_uint16 MSG_LENGTH;
		static const ta_uint16 TELEGRAM_SIZE;

	};
}
#endif