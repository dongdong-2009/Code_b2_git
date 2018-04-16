/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\TrainPisInfoTelegram.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate the Train Pis Info Data.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/TrainPisInfoTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 TrainPisInfoTelegram::PLATFORM_CNT_POSITION     = 4;
	const ta_uint8 TrainPisInfoTelegram::STATION_ID_POSITION       = 5;
	const ta_uint8 TrainPisInfoTelegram::PLATFORM_ID_POSITION      = 6;
	const ta_uint8 TrainPisInfoTelegram::PRE_ARRIVAL_POSITION      = 15;
	const ta_uint8 TrainPisInfoTelegram::ARRIVAL_STATUS_POSITION   = 24;
	const ta_uint8 TrainPisInfoTelegram::DEPARTURE_STATUS_POSITION = 25;
	const ta_uint8 TrainPisInfoTelegram::HOLD_STATUS_POSITION      = 26;
	
	TrainPisInfoTelegram::TrainPisInfoTelegram( const Telegram& incomingData )
		: TrainCommonPisInfoTelegram(incomingData)
	{
		FUNCTION_ENTRY("TrainPisInfoTelegram");
		FUNCTION_EXIT;
	}

	ta_uint8 TrainPisInfoTelegram::getPlatformCnt() const
	{
		FUNCTION_ENTRY("getPlatformCnt");
		FUNCTION_EXIT;
		return getByte(PLATFORM_CNT_POSITION);
	}

	ta_uint8 TrainPisInfoTelegram::getStationId(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getStationId");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getByte(STATION_ID_POSITION + nDataRepeat);
	}

	ta_uint16 TrainPisInfoTelegram::getPlatformId(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getPlatformId");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getWord(PLATFORM_ID_POSITION + nDataRepeat);
	}

	ta_uint8 TrainPisInfoTelegram::getPreArrival(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getPreArrival");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getByte(PRE_ARRIVAL_POSITION + nDataRepeat);
	}

	ta_uint8 TrainPisInfoTelegram::getArrivalStatus(ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getArrivalStatus");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getByte(ARRIVAL_STATUS_POSITION + nDataRepeat);
	}

	ta_uint8 TrainPisInfoTelegram::getDepartureStatus(ta_uint16 nDataRepeat/*=0*/)const
	{
		FUNCTION_ENTRY("getDepartureStatus");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getByte(DEPARTURE_STATUS_POSITION + nDataRepeat);
	}

	ta_uint8 TrainPisInfoTelegram::getHoldStatus(ta_uint16 nDataRepeat/*=0*/) const
	{
		FUNCTION_ENTRY("getHoldStatus");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

		return getByte(HOLD_STATUS_POSITION + nDataRepeat);
	}

	std::vector<unsigned char> TrainPisInfoTelegram::getDataRepeatTrainPisInfo()const
	{
		FUNCTION_ENTRY("getDataRepeatTrainPisInfo");
		FUNCTION_EXIT;
		return getDataRepeat(STATION_ID_POSITION,DATA_REPEAT_TRAIN_PIS_INFO_SIZE * getPlatformCnt());
	}

	std::vector<unsigned char> TrainPisInfoTelegram::getPlatformData(ta_uint16 nDataRepeat/* =0 */) const
	{
		FUNCTION_ENTRY("getPlatformData");
		FUNCTION_EXIT;
		nDataRepeat = nDataRepeat * DATA_REPEAT_TRAIN_PIS_INFO_SIZE;
		return getDataRepeat(STATION_ID_POSITION + nDataRepeat, DATA_REPEAT_TRAIN_PIS_INFO_SIZE );
	}
	std::vector<unsigned char> TrainPisInfoTelegram::getPlatformDataRepeat(int platformIndex, unsigned short numberOfplatform) const
	{
		FUNCTION_ENTRY("getPlatformDataRepeat");
		FUNCTION_EXIT;
		std::vector<unsigned char> platformDataCache;
		int totalPlatformCount = numberOfplatform + platformIndex;

		for ( int count = platformIndex; count < totalPlatformCount; count++ )
		{
			std::vector<unsigned char> platformDataRepeat = getPlatformData(count);
			platformDataCache.insert(platformDataCache.begin(),platformDataRepeat.begin(), platformDataRepeat.end());
		}
		return platformDataCache;
	}

	boost::shared_ptr<TrainPisInfoTelegram> TrainPisInfoTelegram::getNewTrainPISInfoTelegram(int platformIndex, unsigned short numberOfplatform) const
	{
		FUNCTION_ENTRY("getNewTrainPISInfoTelegram");
		FUNCTION_EXIT;
		std::vector<unsigned char> platformData = getPlatformDataRepeat(platformIndex,numberOfplatform);
		unsigned short msgLength = platformData.size() + 3; // msg_id(2Bytes) + platorm_cnt(1Byte)
		
		//MsgLength
		unsigned char cMsb = msgLength >> 8;
		unsigned char lLsb = msgLength & 0xff;

		std::vector<unsigned char> _retData;

		//Construct message
		_retData.push_back(cMsb); //MsgLength
		_retData.push_back(lLsb);
		_retData.push_back(0x10); //MsgID
		_retData.push_back(0x07);
		_retData.push_back(numberOfplatform); //Platform Count

		//Insert Platform Data Repetition
		_retData.insert(_retData.end(),platformData.begin(),platformData.end());

		Telegram newTelegram(_retData,false);
		boost::shared_ptr<TrainPisInfoTelegram> newTrainPisInfo(new TrainPisInfoTelegram(newTelegram));

		return newTrainPisInfo;
	}

	std::vector<unsigned char> TrainPisInfoTelegram::getPacketDataWithHeader()
	{
		FUNCTION_ENTRY("getPacketDataWithHeader");

		std::vector<unsigned char> _rawData = getPacketData();
		std::vector<unsigned char> _retData;

		//TotalLength
		unsigned short _totalLength = _rawData.size() + 1; //Plus 1 for the MultiFlag 
		unsigned char MSBTotalLength =  _totalLength >> 8;
		unsigned char LSBTotalLength = _totalLength & 0xff;

		//Construct header
		_retData.push_back(0xff);           // System ID
		_retData.push_back(MSBTotalLength); // TotalLength
		_retData.push_back(LSBTotalLength); 
		_retData.push_back(0);             // MultiFlag was set to complete telegram

		//Merge the Header and Packet Data
		_retData.insert(_retData.end(),_rawData.begin(),_rawData.end());

		FUNCTION_EXIT;

		return _retData;
	}
}
