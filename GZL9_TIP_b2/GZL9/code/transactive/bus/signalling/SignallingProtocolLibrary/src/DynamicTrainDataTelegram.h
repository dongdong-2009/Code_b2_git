/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\DynamicTrainDataTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Dynamic Train Data.
  */

#ifndef _DYNAMIC_TRAIN_DATA_TELEGRAM_H_
#define _DYNAMIC_TRAIN_DATA_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"

namespace TA_IRS_Bus
{
	class DynamicTrainDataTelegram : public CommonTelegram
	{

	public:
	/**
	 * CommonTelegram
	 * 
	 * Constructor to build the packet from incoming data.
	 * 
	 * @return 
	 * @param : const Telegram& incomingData
	 */
		DynamicTrainDataTelegram( const Telegram& incomingData );

	 /**
	  * Destructor
	  */
		virtual ~DynamicTrainDataTelegram(){};

	 /**
	  * getGroupId
	  *
	  * Gets the Group Id of the incoming packets.
	  */
		ta_uint16 getGroupId() const;

	 /**
	  * getTrainId
	  *
	  * Gets the train Id of the incoming packets
	  */
		ta_uint32 getTrainId() const;

	 /**
	  * getServiceId
	  *
	  * Gets the Service Id of the incoming packets
	  */
		ta_uint16 getServiceId() const;

	 /**
	  * getDestinationId
	  *
	  * Gets the Destination Id of the incoming packets
	  */
		ta_uint16 getDestinationId() const;

	 /**
	  * getDir
	  *
	  * Gets the Direction of the incoming packets
	  */
		ta_uint8 getDir() const;

	 /**
	  * getType
	  *
	  * Gets the type of the incoming packets
	  */
		ta_uint8 getType() const;

	 /**
	  * getStationId
	  *
	  * Gets the station id of the incoming packets
	  */
		ta_uint8 getStationId() const;

	 /**
	  * getPlatformId
	  *
	  * Gets the platform id of the incoming packets
	  */
	    ta_uint16 getPlatformId() const;

	 /**
	  * getTime
	  *
	  * Gets the time of the incoming packets
	  */
		ta_uint32 getTime() const;

	  /**
	   * getFlag
	   *
	   * Gets the flag of the incoming packets
	   */
		ta_uint8 getFlag() const;
	  

	private:
		static const ta_uint8 GROUP_ID_POSITION;
		static const ta_uint8 TRAIN_ID_POSITION;
		static const ta_uint8 TRAIN_ID_WORD_SIZE;
		static const ta_uint8 SERVICE_ID_POSITION;
		static const ta_uint8 DESTINATION_ID_POSITION;
		static const ta_uint8 DIR_POSITION;
		static const ta_uint8 TYPE_POSITION;
		static const ta_uint8 STATION_ID_POSITION;
		static const ta_uint8 PLATFORM_ID_POSITION;
		static const ta_uint8 TIME_POSITION;
		static const ta_uint8 TIME_WORD_SIZE;
		static const ta_uint8 FLAG_POSITION;
	};
}

#endif