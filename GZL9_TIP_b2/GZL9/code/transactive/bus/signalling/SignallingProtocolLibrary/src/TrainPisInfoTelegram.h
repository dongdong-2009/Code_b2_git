/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\TrainPisInfoTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate the Train Pis Info Data.
  */

#ifndef _TRAIN_PIS_INFO_TELEGRAM_H_
#define _TRAIN_PIS_INFO_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/TrainCommonPisInfoTelegram.h"
#include <vector>
#include <ctime>
#include "boost/shared_ptr.hpp"

namespace TA_IRS_Bus
{
	class TrainPisInfoTelegram : public TrainCommonPisInfoTelegram
	{
		
	public:

	/**
	 * TrainPisInfoTelegram
	 * 
	 * Constructor to build the packet from incoming data.
	 * 
	 * @return 
	 * @param : const Telegram& incomingData
	 */
		TrainPisInfoTelegram( const Telegram& incomingData );

	/**
	 * Destructor
	 */
		virtual ~TrainPisInfoTelegram(){};

	 /**
	  * getPlatfromCnt
	  *
	  * Gets the Platform Cnt of the incoming packets
	  */
		ta_uint8 getPlatformCnt() const;

	 /**
	  * getStationId
	  *
	  * Gets the Station Id of the incoming packets
	  */
		ta_uint8 getStationId(ta_uint16 nDataRepeat=0) const;

	 /**
	  * getPlatformId
	  *
	  * Gets the Platform Id of the incoming packets
	  */
		ta_uint16 getPlatformId(ta_uint16 nDataRepeat=0) const;

	  /**
	   * getPreArrival
	   *
	   * Gets the PreArrival data of the incoming packets
	   */
		ta_uint8 getPreArrival(ta_uint16 nDataRepeat=0) const;

	   /**
	   * getArrivalStatus
	   *
	   * Gets the Arrival Status of the incoming packets
	   */
		ta_uint8 getArrivalStatus(ta_uint16 nDataRepeat=0) const;

	  /**
	   * getDepartureStatus
	   *
	   * Gets the Departure Status of the incoming packets
	   */
		ta_uint8 getDepartureStatus(ta_uint16 nDataRepeat=0) const;

	  /**
	   * getHoldStatus
	   *
	   * Gets the Hold Status of the incoming packets
	   */
		ta_uint8 getHoldStatus(ta_uint16 nDataRepeat=0) const;

	  /**
	   * getDataRepeatTrainPisInfo
	   *
	   * Gets the Data Repeat of TrainPisInfo of the incoming packets
	   */
		std::vector<unsigned char> getDataRepeatTrainPisInfo() const;

		/**
		 * getPlatformData
		 *
		 * Gets the platform data based on the specified data repeat
		 */
		std::vector<unsigned char> getPlatformData(ta_uint16 nDataRepeat=0) const;

		/**
		 * getPlatformDataRepeat
		 *
		 * Gets the platform data based on platform index and number of platform to retrieve
		 */
		std::vector<unsigned char> getPlatformDataRepeat(int platformIndex, unsigned short numberOfplatform) const;

		/**
		 * getNewTrainPISInfoTelegram
		 *
		 * Gets the newly constructed TrainPISInfo Telegram
		 */
		boost::shared_ptr<TrainPisInfoTelegram> getNewTrainPISInfoTelegram(int platformIndex, unsigned short numberOfplatform) const;

		/**
		 * getPacketDataWithHeader
		 *
		 * Gets the packetData with Header included
		 */
		std::vector<unsigned char> getPacketDataWithHeader();


	private:
		static const ta_uint8 PLATFORM_CNT_POSITION;
		static const ta_uint8 STATION_ID_POSITION;
		static const ta_uint8 PLATFORM_ID_POSITION;
		static const ta_uint8 PRE_ARRIVAL_POSITION;
		static const ta_uint8 ARRIVAL_STATUS_POSITION;
		static const ta_uint8 DEPARTURE_STATUS_POSITION;
		static const ta_uint8 HOLD_STATUS_POSITION;
		

	};
}

#endif