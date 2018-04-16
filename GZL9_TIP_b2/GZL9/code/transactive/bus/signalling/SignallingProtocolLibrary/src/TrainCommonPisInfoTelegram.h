/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\CommonTelegram.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used access the common data of TrainPisInfo from incoming packets
  */

#ifndef _TRAIN_COMMON_PIS_INFO_TELEGRAM_H_
#define _TRAIN_COMMON_PIS_INFO_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TrainPisInfoProtocol.h"

namespace TA_IRS_Bus
{
	class TrainCommonPisInfoTelegram : public CommonTelegram
	{
	public:
	/**
	 * TrainCommonPisInfoTelegram
	 * 
	 * Constructor to build the packet from incoming data.
	 * 
	 * @return 
	 * @param : const Telegram& incomingData
	 */
		TrainCommonPisInfoTelegram(const Telegram& incomingData);

	 /**
	  * Destructor
	  */
		virtual ~TrainCommonPisInfoTelegram(){};

	private:

	/**
	  * getTrainValidityField
	  *
	  * Gets the Train Validity Field of the incoming packets
	  */
		ta_uint8 getTrainValidityFieldPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	  /**
	  * getTrainUnitNumber
	  *
	  * Gets the Train1 Unit Number of the incoming packets
	  */
		ta_uint16 getTrainUnitNumberPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	  /**
	  * getTrainServiceNumber
	  *
	  * Gets the Train Service Number of the incoming packets
	  */
		ta_uint16 getTrainServiceNumberPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainDestinationId
	  *
	  * Gets the Train Destination Id of the incoming packets
	  */
		ta_uint16 getTrainDestinationIdPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainScheduleArrivalTime
	  *
	  * Gets the Train Schedule Arrival Time of the incoming packets
	  */
		ta_uint32 getTrainScheduleArrivalTimePos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainScheduleDepartureTime
	  *
	  * Gets the Train Schedule Departure Time of the incoming packets
	  */
		ta_uint32 getTrainScheduleDepartureTimePos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainSkipStatus
	  *
	  * Gets the Train Skip Status of the incoming packets
	  */
		ta_uint8 getTrainSkipStatusPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainOutOfService
	  *
	  * Gets the Train Out of Service of the incoming packets
	  */
		ta_uint8 getTrainOutOfServicePos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainLastTrain
	  *
	  * Gets the Train Last Train of the incoming packets
	  */
		ta_uint8 getTrainLastTrainPos(ETrainPosition startPosition, ta_uint16 nDataRepeat=0) const;


	public:


	 /**
	  * getTrainValidityField
	  *
	  * Gets the Train Validity Field of the incoming packets
	  */
		ta_uint8 getTrainValidityField(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	  /**
	  * getTrainUnitNumber
	  *
	  * Gets the Train1 Unit Number of the incoming packets
	  */
		ta_uint16 getTrainUnitNumber(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	  /**
	  * getTrainServiceNumber
	  *
	  * Gets the Train Service Number of the incoming packets
	  */
		ta_uint16 getTrainServiceNumber(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainDestinationId
	  *
	  * Gets the Train Destination Id of the incoming packets
	  */
		ta_uint16 getTrainDestinationId(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainScheduleArrivalTime
	  *
	  * Gets the Train Schedule Arrival Time of the incoming packets
	  */
		ta_uint32 getTrainScheduleArrivalTime(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainScheduleDepartureTime
	  *
	  * Gets the Train Schedule Departure Time of the incoming packets
	  */
		ta_uint32 getTrainScheduleDepartureTime(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainSkipStatus
	  *
	  * Gets the Train Skip Status of the incoming packets
	  */
		ta_uint8 getTrainSkipStatus(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainOutOfService
	  *
	  * Gets the Train Out of Service of the incoming packets
	  */
		ta_uint8 getTrainOutOfService(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

	 /**
	  * getTrainLastTrain
	  *
	  * Gets the Train Last Train of the incoming packets
	  */
		ta_uint8 getTrainLastTrain(ETrainNumber trainNumber, ta_uint16 nDataRepeat=0) const;

		protected:
			static const ta_uint8 DATA_REPEAT_TRAIN_PIS_INFO_SIZE;

	};
}
#endif