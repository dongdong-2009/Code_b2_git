/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.cpp$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used access the common data from incoming packets
  */

#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TelegramException.h"
namespace TA_IRS_Bus
{
	const ta_uint8 CommonTelegram::TRAIN_LENGTH_WORD_POSITION		= 0;
	const ta_uint8 CommonTelegram::TRAIN_MSG_ID_POSITION            =  2;
	const ta_uint8 CommonTelegram::TRAIN_UNIT_NUMBER_POSITION		= 5;
	const ta_uint8 CommonTelegram::TRAIN_SERVICE_NUMBER_POSITION	= 7;
	const ta_uint8 CommonTelegram::TRAIN_CNT_POSITION				= 4;
	
	CommonTelegram::CommonTelegram(const Telegram& incomingData)
		: Telegram(incomingData)
	{
		FUNCTION_ENTRY("CommonTelegram");
		
		// check if message length is valid or not
		
		unsigned short statedMsgLen = getMsgLength();
		unsigned short actualMessageLength = getPacketData().size();
		
		if (statedMsgLen > actualMessageLength)
		{
		
			TA_THROW(TelegramException	
					(TelegramException::IncompletePacket, "Incorrect Packet Length.", 
					"Incorrect Packet Length"));    
		}
		
		FUNCTION_EXIT
	}

	CommonTelegram::CommonTelegram(ta_uint16 msgLength, ETelegramIdentification identification, ta_uint16 telegramSize)
		: Telegram(msgLength,identification,telegramSize)
	{
		FUNCTION_ENTRY("CommonTelegram");
		FUNCTION_EXIT;
	}

	ta_uint16 CommonTelegram::getMsgId() const
	{
		FUNCTION_ENTRY("getMsgId");
		FUNCTION_EXIT;
		return getWord(TRAIN_MSG_ID_POSITION);
	}

	std::vector<unsigned char> CommonTelegram::getDataRepeat(ta_uint16 position, ta_uint16 length) const
	{
		FUNCTION_ENTRY("getDataRepeat");
		FUNCTION_EXIT;
		return getMessageBytes(position,length);
		
	}
}