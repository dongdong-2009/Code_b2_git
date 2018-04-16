/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/TrainLocationTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Train Data.
  */

#ifndef _TRAIN_LOCATION_TELEGRAM_H_
#define _TRAIN_LOCATION_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include <vector>
#include <string>
#include <sstream>

namespace TA_IRS_Bus
{
	class TrainLocationTelegram : public CommonTelegram
	{
	public:
	 /**
	  * TrainLocationTelegram
	  * 
	  * Constructor to build the packet from incoming data.
	  * 
	  * @return 
	  * @param : const Telegram& incomingData
	  */
		TrainLocationTelegram(const Telegram& incomingData);

	  /**
	   * Destructor
	   */
		virtual ~TrainLocationTelegram(){};

	/**
	 * getDestinationId
	 *
	 * Gets the Destination ID of the incoming packets
	 *
	 * @return ta_uint16
	 */
		ta_uint16 getDestinationId(long lTrainNumber);

	/**
	 * getDirection
	 *
	 * Gets the Direction of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getDirection(long lTrainNumber);

	 /**
	 * getLocationWindowId
	 *
	 * Gets the Location Train Window ID of the incoming packets
	 *
	 * @return ta_uint16
	 */
		ta_uint16 getLocationWindowId(long lTrainNumber);

	/**
	 * getPositionTrainWindow
	 *
	 * Gets the Position Train Window of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getPositionTrainWindow(long lTrainNumber);

	/**
	 * getStationId
	 *
	 * Gets the station id of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getStationId(long lTrainNumber);

	/**
	 * getSectionName
	 *
	 * Gets the Section Name of the incoming packets
	 *
	 * @return std::string
	 */
		std::string getSectionName(long lTrainNumber);

	/**
	 * getStationId_UpSide
	 *
	 * Gets the Station ID Up Side of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getStationID_UpSide(long lTrainNumber);

	/**
	 * getStationId_DownSide
	 *
	 * Gets the Station ID Down Side of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getStationID_DownSide(long lTrainNumber);

	/**
	 * getStatus
	 *
	 * Gets the Status of the incoming packets
	 *
	 * @return ta_uint8
	 */
		ta_uint8 getStatus(long lTrainNumber);

	/**
	 * getDataRepeatTrainLocation
	 *
	 * Gets the Data Repeat of the incoming packets
	 */
		std::vector<unsigned char> getDataRepeatTrainLocation();

	/**
	 * getLogs
	 * Gets the Data Repeat logs
	 */
		std::string getLogs();
	
	ta_uint16 getTrainUnitNumber(long lTrainNumber) const;
	ta_uint16 getTrainServiceNumber(long lTrainNumber) const;
	ta_uint8 getTrainCnt() const;

	private:
		static const ta_uint8 DESTINATION_ID_POSITION;
		static const ta_uint8 DIRECTION_POSITION;
		static const ta_uint8 LOCATION_WINDOW_POSITION;
		static const ta_uint8 POSITION_WINDOW_POSITION;
		static const ta_uint8 STATION_ID_POSITION;
		static const ta_uint8 SECTION_NAME_POSITION;
		static const ta_uint8 SECTION_NAME_SIZE;
		static const ta_uint8 STATION_ID_UP_POSITION;
		static const ta_uint8 STATION_ID_DOWN_POSITION;
		static const ta_uint8 STATUS_POSITION;
		static const ta_uint8 DATA_REPEAT_TRAIN_LOCATION_SIZE;
	};
}
#endif