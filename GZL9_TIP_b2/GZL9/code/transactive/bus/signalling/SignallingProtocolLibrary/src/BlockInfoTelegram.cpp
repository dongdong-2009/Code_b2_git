/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\BlockInfoTelegram.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Train Data.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/BlockInfoTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 BlockInfoTelegram::DIRECTION_POSITION        = 9;
	const ta_uint8 BlockInfoTelegram::STATION_ID_UP_POSITION    = 10;
	const ta_uint8 BlockInfoTelegram::STATION_ID_DOWN_POSITION  = 11;
	const ta_uint8 BlockInfoTelegram::STOP_FLAG_POSITION        = 12;
	const ta_uint8 BlockInfoTelegram::DATA_REPEAT_BLOCK_INFO_SIZE = 8;

	BlockInfoTelegram::BlockInfoTelegram(const Telegram& incomingData)
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("BlockInfoTelegram");
		FUNCTION_EXIT;
	}

	ta_uint8 BlockInfoTelegram::getDirection(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getDirection");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getByte(DIRECTION_POSITION + nDataRepeat);
	}

	ta_uint8 BlockInfoTelegram::getStationID_UpSide(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getStationID_UpSide");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getByte(STATION_ID_UP_POSITION + nDataRepeat);
	}

	ta_uint8 BlockInfoTelegram::getStationID_DownSide(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getStationID_DownSide");
		FUNCTION_EXIT ;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getByte(STATION_ID_DOWN_POSITION + nDataRepeat);
	}

	ta_uint8 BlockInfoTelegram::getStopFlag(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getStopFlag");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getByte(STOP_FLAG_POSITION + nDataRepeat);
	}

	std::vector<unsigned char> BlockInfoTelegram::getDataRepeatBlockInfo() const
	{
		FUNCTION_ENTRY("getDataRepeatBlockInfo");
		FUNCTION_EXIT;
		return getDataRepeat(TRAIN_UNIT_NUMBER_POSITION, DATA_REPEAT_BLOCK_INFO_SIZE * getTrainCnt());
	}

	ta_uint8 BlockInfoTelegram::getTrainCnt() const
	{
		FUNCTION_ENTRY("getTrainCnt");
		FUNCTION_EXIT;
		return getByte(TRAIN_CNT_POSITION);
	}

	ta_uint16 BlockInfoTelegram::getTrainUnitNumber(int nDataRepeat) const
	{
		FUNCTION_ENTRY("getTrainUnitNumber");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getWord(TRAIN_UNIT_NUMBER_POSITION + nDataRepeat);
	}

	ta_uint16 BlockInfoTelegram::getTrainServiceNumber(int nDataRepeat) const
	{
		FUNCTION_ENTRY("getTrainServiceNumber");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_BLOCK_INFO_SIZE;
		return getWord(TRAIN_SERVICE_NUMBER_POSITION + nDataRepeat);
	}
}
