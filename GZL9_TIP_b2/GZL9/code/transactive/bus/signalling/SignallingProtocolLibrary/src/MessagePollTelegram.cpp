/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\MessagePollTelegram.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/05 11:49:53 $
  * Last modified by:  $Author: builder $
  *
  * The Signalling system sends this message if in 5 seconds there is no message sending to ATS.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/MessagePollTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint16 MessagePollTelegram::MSG_LENGTH = 2;
	const ta_uint16 MessagePollTelegram::TELEGRAM_SIZE = 4;

	MessagePollTelegram::MessagePollTelegram()
		:CommonTelegram(MSG_LENGTH,Telegram::MESSAGE_POLLING,TELEGRAM_SIZE)
	{
		FUNCTION_ENTRY("Constructor MessagePollTelegram");
		FUNCTION_EXIT
	}

	MessagePollTelegram::MessagePollTelegram(const Telegram& incomingData)
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("MessagePollTelegram");
		FUNCTION_EXIT;
	}
}