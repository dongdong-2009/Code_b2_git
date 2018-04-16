/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\PlannedTelegramContent.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#include "bus/signalling/SignallingProtocolLibrary/src/PlannedTelegramContent.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 PlannedTelegramContent::SUB_ID_POSITION          = 4;
	const ta_uint8 PlannedTelegramContent::SERVICE_ID_POSITION      = 6;
	const ta_uint8 PlannedTelegramContent::TRIP_CNT_POSITION        = 8;

	//REPETITION
	const ta_uint8 PlannedTelegramContent::TRAIN_ID_POSITION        = 9;
	const ta_uint8 PlannedTelegramContent::DESTINATION_ID_POSITION  = 13;
	const ta_uint8 PlannedTelegramContent::REC_CNT_POSITION         = 15;

	//REPETITION
	const ta_uint8 PlannedTelegramContent::STATION_ID_POSITION      = 17;
	const ta_uint8 PlannedTelegramContent::PLATFORM_ID_POSITION     = 18;
	const ta_uint8 PlannedTelegramContent::A_TIME_POSITION          = 20;
	const ta_uint8 PlannedTelegramContent::D_TIME_POSITION          = 24;
	const ta_uint8 PlannedTelegramContent::FLAG_POSITION            = 28;
	const ta_uint8 PlannedTelegramContent::RESERVE_POSITION         = 29;
	const ta_uint8 PlannedTelegramContent::INNER_REPETITION_SIZE    = 16;

	PlannedTelegramContent::PlannedTelegramContent( const Telegram& incomingData )
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("PlannedTelegramContent");

		m_repetitionContainerSize.clear();
		initializeRepetitionSize();

		FUNCTION_EXIT;
	}

	PlannedTelegramContent::~PlannedTelegramContent()
	{
		FUNCTION_ENTRY("~PlannedTelegramContent");
		FUNCTION_EXIT;
	}

	void PlannedTelegramContent::initializeRepetitionSize()
	{
		FUNCTION_ENTRY("initializeRepetitionSize");

		for (int count =0 ; count < getTripCnt(); count++ )
		{
			unsigned short _repetitionSize =  ( getRecCnt(count) * INNER_REPETITION_SIZE ) + 8;
			m_repetitionContainerSize.insert(std::make_pair(count,_repetitionSize));

		}

		FUNCTION_EXIT;
	}

	ta_uint16 PlannedTelegramContent::getSubID() const
	{
		FUNCTION_ENTRY("getSubID");
		FUNCTION_EXIT;
		return getWord(SUB_ID_POSITION);
	}

	ta_uint16 PlannedTelegramContent::getServiceID() const
	{
		FUNCTION_ENTRY("getServiceID");
		FUNCTION_EXIT;
		return getWord(SERVICE_ID_POSITION);
	}

	ta_uint8 PlannedTelegramContent::getTripCnt() const
	{
		FUNCTION_ENTRY("getTrinCnt");
		FUNCTION_EXIT;
		return getByte(TRIP_CNT_POSITION);
	}

	ta_uint32 PlannedTelegramContent::getTrainID(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getTrainID");

		int nDataRepeatSize = getRepetitionSize(nDataRepeat);

		FUNCTION_EXIT;
		return getWord32(TRAIN_ID_POSITION + nDataRepeatSize);
	}

	ta_uint16 PlannedTelegramContent::getDestinationID(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getDestinationID");

		int nDataRepeatSize = getRepetitionSize(nDataRepeat);

		FUNCTION_EXIT;
		return getWord(DESTINATION_ID_POSITION + nDataRepeatSize);
	}

	ta_uint16 PlannedTelegramContent::getRecCnt(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getRecCnt");

		int nDataRepeatSize = getRepetitionSize(nDataRepeat);

		FUNCTION_EXIT;
		return getWord(REC_CNT_POSITION + nDataRepeatSize);
	}

	ta_uint8 PlannedTelegramContent::getStationID(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getStationID");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getByte(STATION_ID_POSITION + nDataRepeat);
	}

	ta_uint16 PlannedTelegramContent::getPlatformID(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getPlatformID");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getWord(PLATFORM_ID_POSITION + nDataRepeat);
	}

	ta_uint32 PlannedTelegramContent::getA_Time(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getA_Time");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getWord32(A_TIME_POSITION + nDataRepeat);
	}

	ta_uint32 PlannedTelegramContent::getD_Time(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getA_Time");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getWord32(D_TIME_POSITION + nDataRepeat);
	}

	ta_uint8 PlannedTelegramContent::getFlag(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getFlag");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getByte(FLAG_POSITION + nDataRepeat);
	}

	ta_uint32 PlannedTelegramContent::getReserve(ta_uint16 nDataRepeat/* =0 */, ta_uint16 outerLoop/* =0 */) const
	{
		FUNCTION_ENTRY("getReserve");

		nDataRepeat =  ( nDataRepeat * INNER_REPETITION_SIZE ) + getRepetitionSize(outerLoop);

		FUNCTION_EXIT;
		return getWord32(RESERVE_POSITION + nDataRepeat);
	}

	ta_uint16 PlannedTelegramContent::getRepetitionSize(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getRepetitionSize");

		ta_uint16 _sizeRet = 0;

		for ( int count = 0; count < nDataRepeat; count++ )
		{
			_sizeRet += ( const_cast<PlannedTelegramContent*>(this) )->m_repetitionContainerSize[count];
 		}

		FUNCTION_EXIT;
		return _sizeRet;
	}

}