/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\TrainCommonPisInfoTelegram.cpp$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used access the common data of TrainPisInfo from incoming packets
  */

#include "bus/signalling/SignallingProtocolLibrary/src/TrainCommonPisInfoTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 TrainCommonPisInfoTelegram::DATA_REPEAT_TRAIN_PIS_INFO_SIZE = 79;

	TrainCommonPisInfoTelegram::TrainCommonPisInfoTelegram(const Telegram& incomingData)
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("TrainCommonPisInfoTelegram");
		FUNCTION_EXIT;
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainValidityFieldPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const 
	{
		FUNCTION_ENTRY("getTrainValidityField");
		FUNCTION_EXIT;
		return getByte(startPosition + nDataRepeat);
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainUnitNumberPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainUnitNumber");
		FUNCTION_EXIT;
		return getWord(startPosition + nDataRepeat);
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainServiceNumberPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainServiceNumberPos");
		FUNCTION_EXIT;
		return getWord(startPosition + nDataRepeat);
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainDestinationIdPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainDestinationIdPos");
		FUNCTION_EXIT;
		return getWord(startPosition + nDataRepeat);
	}

	ta_uint32 TrainCommonPisInfoTelegram::getTrainScheduleArrivalTimePos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainScheduleArrivalTimePos");
		FUNCTION_EXIT;
		return getDWord(startPosition + nDataRepeat);
	}

	ta_uint32 TrainCommonPisInfoTelegram::getTrainScheduleDepartureTimePos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainScheduleDepartureTimePos");
		FUNCTION_EXIT;
		return getDWord(startPosition + nDataRepeat);
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainSkipStatusPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainSkipStatusPos");
		FUNCTION_EXIT;
		return getByte(startPosition + nDataRepeat);
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainOutOfServicePos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainOutOfServicePos");
		FUNCTION_EXIT;
		return getByte(startPosition + nDataRepeat);
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainLastTrainPos(ETrainPosition startPosition, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainLastTrainPos");
		FUNCTION_EXIT;
		return getByte(startPosition + nDataRepeat);
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainValidityField(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainValidityField");
		
		ta_uint8 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainValidityFieldPos(TRAIN1_VALIDITYFIELD_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainValidityFieldPos(TRAIN2_VALIDITYFIELD_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainValidityFieldPos(TRAIN3_VALIDITYFIELD_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainValidityFieldPos(TRAIN4_VALIDITYFIELD_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainUnitNumber(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainUnitNumber");

		ta_uint16 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainUnitNumberPos(TRAIN1_UNIT_NUMBER_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainUnitNumberPos(TRAIN2_UNIT_NUMBER_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainUnitNumberPos(TRAIN3_UNIT_NUMBER_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainUnitNumberPos(TRAIN4_UNIT_NUMBER_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainServiceNumber(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainUnitNumber");

		ta_uint16 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainServiceNumberPos(TRAIN1_SERVICE_NUMBER_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainServiceNumberPos(TRAIN2_SERVICE_NUMBER_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainServiceNumberPos(TRAIN3_SERVICE_NUMBER_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainServiceNumberPos(TRAIN4_SERVICE_NUMBER_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint16 TrainCommonPisInfoTelegram::getTrainDestinationId(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainDestinationId");

		ta_uint16 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainDestinationIdPos(TRAIN1_DESTINATION_ID_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainDestinationIdPos(TRAIN2_DESTINATION_ID_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainDestinationIdPos(TRAIN3_DESTINATION_ID_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainDestinationIdPos(TRAIN4_DESTINATION_ID_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint32 TrainCommonPisInfoTelegram::getTrainScheduleArrivalTime(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/ ) const
	{
		FUNCTION_ENTRY("getTrainScheduleArrivalTime");

		ta_uint32 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainScheduleArrivalTimePos(TRAIN1_SCHEDULE_ARRIVAL_TIME_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainScheduleArrivalTimePos(TRAIN2_SCHEDULE_ARRIVAL_TIME_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainScheduleArrivalTimePos(TRAIN3_SCHEDULE_ARRIVAL_TIME_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainScheduleArrivalTimePos(TRAIN4_SCHEDULE_ARRIVAL_TIME_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint32 TrainCommonPisInfoTelegram::getTrainScheduleDepartureTime(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/ ) const
	{
		FUNCTION_ENTRY("getTrainScheduleDepartureTime");

		ta_uint32 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainScheduleDepartureTimePos(TRAIN1_SCHEDULE_DEPARTURE_TIME_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainScheduleDepartureTimePos(TRAIN2_SCHEDULE_DEPARTURE_TIME_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainScheduleDepartureTimePos(TRAIN3_SCHEDULE_DEPARTURE_TIME_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainScheduleDepartureTimePos(TRAIN4_SCHEDULE_DEPARTURE_TIME_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainSkipStatus(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainSkipStatus");

		ta_uint8 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainSkipStatusPos(TRAIN1_SKIP_STATUS_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainSkipStatusPos(TRAIN2_SKIP_STATUS_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainSkipStatusPos(TRAIN3_SKIP_STATUS_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainSkipStatusPos(TRAIN4_SKIP_STATUS_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainOutOfService(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainSkipStatus");

		ta_uint8 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainOutOfServicePos(TRAIN1_OUT_OF_SERVICE_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainOutOfServicePos(TRAIN2_OUT_OF_SERVICE_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainOutOfServicePos(TRAIN3_OUT_OF_SERVICE_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainOutOfServicePos(TRAIN4_OUT_OF_SERVICE_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}

	ta_uint8 TrainCommonPisInfoTelegram::getTrainLastTrain(ETrainNumber trainNumber, ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getTrainLastTrain");

		ta_uint8 _ret = 0;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		switch (trainNumber)
		{
		case TRAIN_1:
			_ret = getTrainLastTrainPos(TRAIN1_LAST_TRAIN_POSITION, nDataRepeat);
			break;

		case  TRAIN_2:
			_ret = getTrainLastTrainPos(TRAIN2_LAST_TRAIN_POSITION, nDataRepeat);
			break;

		case TRAIN_3:
			_ret = getTrainLastTrainPos(TRAIN3_LAST_TRAIN_POSITION, nDataRepeat);
			break;

		case TRAIN_4:
			_ret = getTrainLastTrainPos(TRAIN4_LAST_TRAIN_POSITION, nDataRepeat);
			break;
		}
		FUNCTION_EXIT;
		return _ret;
	}
}