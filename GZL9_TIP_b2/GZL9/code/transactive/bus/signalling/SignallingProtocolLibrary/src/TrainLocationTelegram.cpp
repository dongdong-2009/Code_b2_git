/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\TrainLocationTelegram.cpp$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * These telegrams are used to communicate Train Data.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/TrainLocationTelegram.h"
#include "core/utilities/src/DebugUtil.h"
#include <vector>
#include <sstream>

namespace TA_IRS_Bus
{
	
	const ta_uint8 TrainLocationTelegram::DESTINATION_ID_POSITION   = 9;
	const ta_uint8 TrainLocationTelegram::DIRECTION_POSITION        = 11;
	const ta_uint8 TrainLocationTelegram::LOCATION_WINDOW_POSITION  = 12;
	const ta_uint8 TrainLocationTelegram::POSITION_WINDOW_POSITION  = 14;
	const ta_uint8 TrainLocationTelegram::STATION_ID_POSITION       = 15;
	const ta_uint8 TrainLocationTelegram::SECTION_NAME_POSITION     = 16;
	const ta_uint8 TrainLocationTelegram::SECTION_NAME_SIZE         = 10;
	const ta_uint8 TrainLocationTelegram::STATION_ID_UP_POSITION    = 26;
	const ta_uint8 TrainLocationTelegram::STATION_ID_DOWN_POSITION  = 27;
	const ta_uint8 TrainLocationTelegram::STATUS_POSITION           = 28;
	const ta_uint8 TrainLocationTelegram::DATA_REPEAT_TRAIN_LOCATION_SIZE = 24;


	TrainLocationTelegram::TrainLocationTelegram(const Telegram& incomingData)
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("Constructor: TrainLocationTelegram");
		FUNCTION_EXIT;
	}

	ta_uint16 TrainLocationTelegram::getDestinationId(long lTrainNumber)
	{
		FUNCTION_ENTRY("getDestinationId");
		FUNCTION_EXIT;

		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getWord(DESTINATION_ID_POSITION + nDataRepeat);
	}

	ta_uint8 TrainLocationTelegram::getDirection(long lTrainNumber)
	{
		FUNCTION_ENTRY("getDirection");
		FUNCTION_EXIT;	
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getByte(DIRECTION_POSITION + nDataRepeat);
	}

	ta_uint16 TrainLocationTelegram::getLocationWindowId(long lTrainNumber)
	{
		FUNCTION_ENTRY("getLocationWindowId");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getWord(LOCATION_WINDOW_POSITION + nDataRepeat);
	}

	ta_uint8 TrainLocationTelegram::getPositionTrainWindow(long lTrainNumber) 
	{
		FUNCTION_ENTRY("getPositionTrainWindow");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getByte(POSITION_WINDOW_POSITION + nDataRepeat);
	}

	ta_uint8 TrainLocationTelegram::getStationId(long lTrainNumber)
	{
		FUNCTION_ENTRY("getStationId");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getByte(STATION_ID_POSITION + nDataRepeat);
	}

	std::string TrainLocationTelegram::getSectionName(long lTrainNumber)
	{
		FUNCTION_ENTRY("getSectionName");

		std::string ret = "";
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;

		std::vector<unsigned char> setionName = getBytes(SECTION_NAME_POSITION + nDataRepeat,SECTION_NAME_SIZE);
		std::vector<unsigned char>::const_iterator iter = setionName.begin();
		for ( iter; iter != setionName.end(); ++iter)
		{
			if (*iter != 0)
				ret += *iter;
		}
		FUNCTION_EXIT;
		return ret;
	}

	ta_uint8 TrainLocationTelegram::getStationID_UpSide(long lTrainNumber)
	{
		FUNCTION_ENTRY("getStationID_UpSide");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;

		return getByte(STATION_ID_UP_POSITION + nDataRepeat);
	}

	ta_uint8 TrainLocationTelegram::getStationID_DownSide(long lTrainNumber)
	{
		FUNCTION_ENTRY("getStationID_DownSide");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber  * DATA_REPEAT_TRAIN_LOCATION_SIZE;

		return getByte(STATION_ID_DOWN_POSITION + nDataRepeat);
	}

	ta_uint8 TrainLocationTelegram::getStatus(long lTrainNumber)
	{
		FUNCTION_ENTRY("getStatus");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		
		return getByte(STATUS_POSITION + nDataRepeat);
	}

	std::vector<unsigned char> TrainLocationTelegram::getDataRepeatTrainLocation()
	{
		FUNCTION_ENTRY("getDataRepeatTrainLocation");
		FUNCTION_EXIT;
		return getDataRepeat(TRAIN_UNIT_NUMBER_POSITION,DATA_REPEAT_TRAIN_LOCATION_SIZE * getTrainCnt());
	}

	std::string TrainLocationTelegram::getLogs()
	{
		FUNCTION_ENTRY("getLogs");
		
		ta_uint8 trainCnt = getTrainCnt();
		std::ostringstream dump;
	
		dump << std::hex << std::endl;
		dump << "Length                  : " << (int)getLength() << std::endl;
		dump << "MsgID                   : " << getMsgId() << std::endl;
		dump << "TrainCnt                : " << (int)getTrainCnt() << std::endl;
	
// 		for ( int offset=0; offset<trainCnt; offset++)
// 		{
// 			//Print the 1st 11 Data
// 			dump << "\n\nTRAIN COUNT # " << offset + 1 << std::endl;
// 			dump << "===================================" << std::endl;
// 			dump << "Train Unit Number       : " << getTrainUnitNumber(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Train Service Number    : " << getTrainServiceNumber( offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Destination ID          : " << getDestinationId(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Direction               : " << (int)getDirection(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl; 
// 			dump << "Location Train Window ID: " << getLocationWindowId(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Position in Train Window: " << (int)getPositionTrainWindow(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Station ID              : " << (int)getStationId(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Section Name            : " << getSectionName(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE).c_str() << std::endl;
// 			dump << "StationID in Up Side    : " << (int)getStationID_UpSide(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "StationID in Down Side  : " << (int)getStationID_DownSide(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl;
// 			dump << "Status                  : " << (int)getStatus(offset * DATA_REPEAT_TRAIN_LOCATION_SIZE) << std::endl << std::endl;
// 		}

		FUNCTION_EXIT;
		return dump.str();
	}

	ta_uint16 TrainLocationTelegram::getTrainUnitNumber(long lTrainNumber) const
	{
		FUNCTION_ENTRY("getTrainUnitNumber");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getWord(TRAIN_UNIT_NUMBER_POSITION + nDataRepeat);
	}

	ta_uint16 TrainLocationTelegram::getTrainServiceNumber(long lTrainNumber) const
	{
		FUNCTION_ENTRY("getTrainServiceNumber");
		FUNCTION_EXIT;
		
		lTrainNumber -=1;
		long nDataRepeat = lTrainNumber * DATA_REPEAT_TRAIN_LOCATION_SIZE;
		return getWord(TRAIN_SERVICE_NUMBER_POSITION + nDataRepeat);
	}
	ta_uint8 TrainLocationTelegram::getTrainCnt() const
	{
		FUNCTION_ENTRY("getTrainCnt");
		FUNCTION_EXIT;
		return getByte(TRAIN_CNT_POSITION);
	}

}
