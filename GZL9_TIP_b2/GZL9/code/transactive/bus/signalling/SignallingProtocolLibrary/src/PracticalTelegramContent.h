/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\PracticalTelegramContent.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef _PRACTICAL_TELEGRAM_CONTENT_H_
#define _PRACTICAL_TELEGRAM_CONTENT_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"

namespace TA_IRS_Bus
{
	class PracticalTelegramContent : public CommonTelegram
	{
	
	public:
		/**
		 * PracticalTelegramContent
		 * 
		 * Constructor to build the packet from incoming data.
		 * 
		 * @return 
		 * @param : const Telegram& incomingData
		 */
		PracticalTelegramContent( const Telegram& incomingData );

		/**
		 * Destructor
		 */
		virtual ~PracticalTelegramContent();

		/**
		 * getSubID
		 *
		 * Gets the SUB ID from the incoming Telegram
		 */
		ta_uint16 getSubID() const;

		/**
		 * getGroupID
		 *
		 * Gets the Group ID from the incoming Telegram
		 */
		ta_uint16 getGroupID() const;

		/**
		 * getRecCnt
		 *
		 * Gets the Record Count from the incoming Telegram
		 */
		ta_uint16 getRecCnt() const;

		//Repetition

		/**
		 * getStationID
		 *
		 * Gets the SUB ID from the incoming Telegram
		 */
		ta_uint8 getStationID(int nDataRepeat=0) const;

		/**
		 * getPlatformID
		 *
		 * Gets the Platform ID from the incoming Telegram
		 */
		ta_uint16 getPlatformID(int nDataRepeat=0) const;

		/**
		 * getA_Time
		 *
		 * Gets the Arrival Time from the incoming Telegram
		 */
		ta_uint32 getA_Time(int nDataRepeat=0) const;

		/**
		 * getD_Time
		 *
		 * Gets the Destination Time from the incoming Telegram
		 */
		ta_uint32 getD_Time(int nDataRepeat=0) const;

		/**
		 * getFlag
		 *
		 * Gets the Flag from the incoming Telegram
		 */
		ta_uint8 getFlag(int nDataRepeat=0) const;

		/**
		 * getTrainID
		 *
		 * Gets the Train ID from the incoming Telegram
		 */
		ta_uint32 getTrainID(int nDataRepeat=0) const;

		/**
		 * getTrainID
		 *
		 * Gets the Service ID from the incoming Telegram
		 */
		ta_uint16 getServiceID(int nDataRepeat=0) const;

		/**
		 * getDestinationID
		 *
		 * Gets the Destination ID from the incoming Telegram
		 */
		ta_uint16 getDestinationID(int nDataRepeat=0) const;

		/**
		 * getType
		 *
		 * Gets the Type from the incoming Telegram
		 */
		ta_uint8 getType(int nDataRepeat=0) const;

		/**
		 * getReserve
		 *
		 * Gets the Reserve from the incoming Telegram
		 */
		ta_uint32 getReserve(int nDataRepeat=0) const;

		private:
			static const ta_uint8 SUB_ID_POSITION;
			static const ta_uint8 GROUP_ID_POSITION;
			static const ta_uint8 REC_CNT_POSITION;

			//REPETITION
			static const ta_uint8 STATION_ID_POSITION;
			static const ta_uint8 PLATFORM_ID_POSITION;
			static const ta_uint8 A_TIME_POSITION;
			static const ta_uint8 D_TIME_POSITION;
			static const ta_uint8 FLAG_POSITION;
			static const ta_uint8 TRAIN_ID_POSITION;
			static const ta_uint8 SERVICE_ID_POSITION;
			static const ta_uint8 DESTINATION_ID_POSITION;
			static const ta_uint8 TYPE_POSITION;
			static const ta_uint8 RESERVE_POSITION;
			static const ta_uint8 REPETITION_DATA_SIZE;


	};
}

#endif