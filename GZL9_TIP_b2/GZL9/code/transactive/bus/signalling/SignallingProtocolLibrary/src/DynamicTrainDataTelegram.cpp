/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\DynamicTrainDataTelegram.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Dynamic Train Data.
  */
#include "bus/signalling/SignallingProtocolLibrary/src/DynamicTrainDataTelegram.h"
#include "core/utilities/src/DebugUtil.h"
#include <vector>

namespace TA_IRS_Bus
{
	const ta_uint8 DynamicTrainDataTelegram::GROUP_ID_POSITION       = 4;
	const ta_uint8 DynamicTrainDataTelegram::TRAIN_ID_POSITION       = 6;
	const ta_uint8 DynamicTrainDataTelegram::TRAIN_ID_WORD_SIZE      = 2;
	const ta_uint8 DynamicTrainDataTelegram::SERVICE_ID_POSITION     = 10;
	const ta_uint8 DynamicTrainDataTelegram::DESTINATION_ID_POSITION = 12;
	const ta_uint8 DynamicTrainDataTelegram::DIR_POSITION            = 14;
	const ta_uint8 DynamicTrainDataTelegram::TYPE_POSITION           = 15;
	const ta_uint8 DynamicTrainDataTelegram::STATION_ID_POSITION     = 16;
	const ta_uint8 DynamicTrainDataTelegram::PLATFORM_ID_POSITION    = 17;
	const ta_uint8 DynamicTrainDataTelegram::TIME_POSITION           = 19;
	const ta_uint8 DynamicTrainDataTelegram::TIME_WORD_SIZE          = 2;
	const ta_uint8 DynamicTrainDataTelegram::FLAG_POSITION           = 23;

	DynamicTrainDataTelegram::DynamicTrainDataTelegram( const Telegram& incomingData )
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("DynamicTrainDataTelegram");
		FUNCTION_EXIT
	}

	ta_uint16 DynamicTrainDataTelegram::getGroupId() const
	{
		FUNCTION_ENTRY("getGroupId");
		FUNCTION_EXIT;
		return getWord(GROUP_ID_POSITION);
	}

	ta_uint32 DynamicTrainDataTelegram::getTrainId() const
	{
		FUNCTION_ENTRY("getTrainId");
	
		std::vector<ta_uint16> trainId = getWords(TRAIN_ID_POSITION,TRAIN_ID_WORD_SIZE);
		ta_uint32 _ret = 0;
		ta_uint8 count = 0;
		std::vector<ta_uint16>::const_iterator iter = trainId.begin();
		for ( iter; iter != trainId.end(); ++iter)
		{
			if ( 0 == count )
			{
				_ret = *iter << 16;
				count++;
			}
			else
				_ret += *iter;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint16 DynamicTrainDataTelegram::getServiceId() const
	{
		FUNCTION_ENTRY("getServiceId");
		FUNCTION_EXIT;
		return getWord(SERVICE_ID_POSITION);
	}

	ta_uint16 DynamicTrainDataTelegram::getDestinationId() const
	{
		FUNCTION_ENTRY("getDestinationId");
		FUNCTION_EXIT;
		return getWord(DESTINATION_ID_POSITION);
	}

	ta_uint8 DynamicTrainDataTelegram::getDir() const
	{
		FUNCTION_ENTRY("getDir");
		FUNCTION_EXIT;
		return getByte(DIR_POSITION);
	}

	ta_uint8 DynamicTrainDataTelegram::getType() const
	{
		FUNCTION_ENTRY("getType");
		FUNCTION_EXIT;
		return getByte(TYPE_POSITION);
	}

	ta_uint8 DynamicTrainDataTelegram::getStationId() const
	{
		FUNCTION_ENTRY("getStationId");
		FUNCTION_EXIT;
		return getByte(STATION_ID_POSITION);
	}

	ta_uint16 DynamicTrainDataTelegram::getPlatformId() const
	{
		FUNCTION_ENTRY("getPlatformId");
		FUNCTION_EXIT;
		return getWord(PLATFORM_ID_POSITION);
	}

	ta_uint32 DynamicTrainDataTelegram::getTime() const
	{
		FUNCTION_ENTRY("getTime");

		std::vector<ta_uint16> _time = getWords(TIME_POSITION,TIME_WORD_SIZE);
		ta_uint32 _ret = 0;
		ta_uint8 count = 0;
		std::vector<ta_uint16>::const_iterator iter = _time.begin();
		for ( iter; iter != _time.end(); ++iter)
		{
			if ( 0 == count )
			{
				_ret = *iter << 16;
				count++;
			}
			else
				_ret += *iter;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint8 DynamicTrainDataTelegram::getFlag() const
	{
		FUNCTION_ENTRY("getFlag");
		FUNCTION_EXIT;
		return getByte(FLAG_POSITION);
	}

}