/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\SignalStatusTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Signal Status.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/SignalStatusTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 SignalStatusTelegram::RTU_ID_POSITION         = 5;
	const ta_uint8 SignalStatusTelegram::SIGNAL_NAME_POSITION    = 7;
	const ta_uint8 SignalStatusTelegram::SIGNAL_NAME_SIZE        = 24;
	const ta_uint8 SignalStatusTelegram::SIGNAL_STATUS_POSITION  = 31;
	const ta_uint8 SignalStatusTelegram::SIGNAL_STATUS_WORD_SIZE = 2;
	const ta_uint8 SignalStatusTelegram::DATA_REPEAT_SIGNAL_STATUS_SIZE = 30;
	


	SignalStatusTelegram::SignalStatusTelegram( const Telegram& incomingData )
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("SignalStatusTelegram");
		FUNCTION_EXIT;
	}

	ta_uint8 SignalStatusTelegram::getSignalCnt() const
	{
		FUNCTION_ENTRY("getSignalCnt");
		FUNCTION_EXIT;
		return getByte(TRAIN_CNT_POSITION);
	}

	ta_uint16 SignalStatusTelegram::getRtuId(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getRtuId");
		FUNCTION_EXIT;
		return getWord(RTU_ID_POSITION + nDataRepeat);
	}

	std::string SignalStatusTelegram::getSignalName(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getSignalName");

		std::string _ret = "";
		nDataRepeat = nDataRepeat * DATA_REPEAT_SIGNAL_STATUS_SIZE;
		std::vector<ta_uint8> _signalName = getBytes(SIGNAL_NAME_POSITION + nDataRepeat,SIGNAL_NAME_SIZE);
		std::vector<ta_uint8>::const_iterator iter = _signalName.begin();
		for ( iter; iter != _signalName.end(); ++iter )
		{
			if (*iter != 0)
				_ret += *iter;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint32 SignalStatusTelegram::getSignalStaus(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getSignalStaus");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_SIGNAL_STATUS_SIZE;
		return getWord32(SIGNAL_STATUS_POSITION + nDataRepeat);
		
		
	}

	std::vector<unsigned char> SignalStatusTelegram::getDataRepeatSignalStatus() const
	{
		FUNCTION_ENTRY("getDataRepeatSignalStatus");
		FUNCTION_EXIT;
		return getDataRepeat(TRAIN_UNIT_NUMBER_POSITION, DATA_REPEAT_SIGNAL_STATUS_SIZE * getSignalCnt());
	}

}