/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\BlockInfoTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Train Data.
  */

#ifndef _BLOCK_INFO_TELEGRAM_H_
#define _BLOCK_INFO_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include <vector>

namespace TA_IRS_Bus
{
	class BlockInfoTelegram : public  CommonTelegram
	{

	public:
	 /**
	  * BlockInfoTelegram
	  * 
	  * Constructor to build the packet from incoming data.
	  * 
	  * @return 
	  * @param : const Telegram& incomingData
	  */
		BlockInfoTelegram(const Telegram& incomingData);

	  /**
	   * Destructor
	   */
		virtual ~BlockInfoTelegram(){};

		/**
		 * getDirection
		 *
		 * Gets the Direction of the incoming packets
		 */
		ta_uint8 getDirection(ta_uint16 nDataRepeat=0) const;

		/**
		 * getSationID_UpSide
		 *
		 * Gets the station id up side of the incoming packets
		 */
		ta_uint8 getStationID_UpSide(ta_uint16 nDataRepeat=0) const;

		/**
		 * getSationID_DownSide
		 *
		 * Gets the station id down side of the incoming packets
		 */
		ta_uint8 getStationID_DownSide(ta_uint16 nDataRepeat=0) const;

		/**
		 * getStopFlag
		 *
		 * Gets the Stop Flag of the incoming packets
		 */
		ta_uint8 getStopFlag(ta_uint16 nDataRepeat=0) const;

		/**
		 * getDataRepeatBlockInfo
		 *
		 * Gets the Data Repeat of the incoming packets
		 */
		std::vector<unsigned char> getDataRepeatBlockInfo() const;

		ta_uint8 getTrainCnt() const;
		ta_uint16 getTrainUnitNumber(int nDataRepeat = 0) const;
		ta_uint16 getTrainServiceNumber(int nDataRepeat = 0) const;


	private:

		static const ta_uint8 DIRECTION_POSITION;
		static const ta_uint8 STATION_ID_UP_POSITION;
		static const ta_uint8 STATION_ID_DOWN_POSITION;
		static const ta_uint8 STOP_FLAG_POSITION;
		static const ta_uint8 DATA_REPEAT_BLOCK_INFO_SIZE;
	};
}
#endif