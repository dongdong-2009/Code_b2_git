/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\PracticalTelegramContent.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#include "bus/signalling/SignallingProtocolLibrary/src/PracticalTelegramContent.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 PracticalTelegramContent::SUB_ID_POSITION         = 4;
	const ta_uint8 PracticalTelegramContent::GROUP_ID_POSITION       = 6;
	const ta_uint8 PracticalTelegramContent::REC_CNT_POSITION        = 8;
	//REPETITION
	const ta_uint8 PracticalTelegramContent::STATION_ID_POSITION     = 10;
	const ta_uint8 PracticalTelegramContent::PLATFORM_ID_POSITION    = 11;
	const ta_uint8 PracticalTelegramContent::A_TIME_POSITION         = 13;
	const ta_uint8 PracticalTelegramContent::D_TIME_POSITION         = 17;
	const ta_uint8 PracticalTelegramContent::FLAG_POSITION           = 21;
	const ta_uint8 PracticalTelegramContent::TRAIN_ID_POSITION       = 22;
	const ta_uint8 PracticalTelegramContent::SERVICE_ID_POSITION     = 26;
	const ta_uint8 PracticalTelegramContent::DESTINATION_ID_POSITION = 28;
	const ta_uint8 PracticalTelegramContent::TYPE_POSITION           = 30;
	const ta_uint8 PracticalTelegramContent::RESERVE_POSITION        = 31;
	const ta_uint8 PracticalTelegramContent::REPETITION_DATA_SIZE    = 25;

	PracticalTelegramContent::PracticalTelegramContent( const Telegram& incomingData )
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("PracticalTelegramContent");
		FUNCTION_EXIT;
	}

	PracticalTelegramContent::~PracticalTelegramContent()
	{
		FUNCTION_ENTRY("~PracticalTelegramContent");
		FUNCTION_EXIT;
	}

	ta_uint16 PracticalTelegramContent::getSubID() const
	{
		FUNCTION_ENTRY("getSubID");
		FUNCTION_EXIT;
		return getWord(SUB_ID_POSITION);
	}

	ta_uint16 PracticalTelegramContent::getGroupID() const
	{
		FUNCTION_ENTRY("getSubID");
		FUNCTION_EXIT;
		return getWord(GROUP_ID_POSITION);
	}

	ta_uint16 PracticalTelegramContent::getRecCnt() const
	{
		FUNCTION_ENTRY("getSubID");
		FUNCTION_EXIT;
		return getWord(REC_CNT_POSITION);
	}

	ta_uint8 PracticalTelegramContent::getStationID(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getStationID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getByte(STATION_ID_POSITION + nDataRepeat);
	}

	ta_uint16 PracticalTelegramContent::getPlatformID(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getPlatformID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord(PLATFORM_ID_POSITION + nDataRepeat);
	}

	ta_uint32 PracticalTelegramContent::getA_Time(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getA_Time");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord32(A_TIME_POSITION + nDataRepeat);
	}

	ta_uint32 PracticalTelegramContent::getD_Time(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getA_Time");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord32(D_TIME_POSITION + nDataRepeat);
	}

	ta_uint8 PracticalTelegramContent::getFlag(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getFlag");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getByte(FLAG_POSITION + nDataRepeat);
	}

	ta_uint32 PracticalTelegramContent::getTrainID(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getTrainID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord32(TRAIN_ID_POSITION + nDataRepeat);
	}

	ta_uint16 PracticalTelegramContent::getServiceID(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getServiceID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord(SERVICE_ID_POSITION + nDataRepeat);
	}

	ta_uint16 PracticalTelegramContent::getDestinationID(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getDestinationID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord(DESTINATION_ID_POSITION + nDataRepeat);
	}
	
	ta_uint8 PracticalTelegramContent::getType(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getType");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getByte(TYPE_POSITION + nDataRepeat);
	}

	ta_uint32 PracticalTelegramContent::getReserve(int nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getTrainID");

		nDataRepeat = nDataRepeat * REPETITION_DATA_SIZE;

		FUNCTION_EXIT;
		return getWord32(RESERVE_POSITION + nDataRepeat);
	}

}