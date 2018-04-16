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

#ifndef _PLANNED_TELEGRAM_CONTENT_H_
#define _PLANNED_TELEGRAM_CONTENT_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include <vector>
#include <map>

namespace TA_IRS_Bus
{
	class PlannedTelegramContent : public CommonTelegram
	{
	public:
		/**
		 * PlannedTelegramContent
		 * 
		 * Constructor to build the packet from incoming data.
		 * 
		 * @return 
		 * @param : const Telegram& incomingData
		 */
		PlannedTelegramContent( const Telegram& incomingData );

		typedef unsigned short TripCount;
		typedef unsigned short RepetitionSize;
		typedef std::map< TripCount, RepetitionSize > RepetitionContainer;

		/**
		 * Destructor
		 */
		virtual ~PlannedTelegramContent();

		/**
		 * getSubID
		 *
		 * Gets the SUB ID from the incoming Telegram
		 */
		ta_uint16 getSubID() const;

		/**
		 * getServiceID() const
		 */
		ta_uint16 getServiceID() const;

		/**
		 * getTripCnt
		 *
		 * Get the TripCnt from the incoming Telegram
		 */
		ta_uint8 getTripCnt() const;


		//Repetition

		/**
		 * getTrainID
		 *
		 * Gets the TrainID from the incoming Telegram
		 */
		ta_uint32 getTrainID(ta_uint16 nDataRepeat=0) const;

		/**
		 * getDestinationID
		 *
		 * Gets the DestinationID form the incoming Telegram
		 */
		ta_uint16 getDestinationID(ta_uint16 nDataRepeat=0) const;

		/**
		 * getRecCnt()
		 *
		 * Gets the Rec_Cnt from the incoming Telegram
		 */
		ta_uint16 getRecCnt(ta_uint16 nDataRepeat=0) const;

		//Repetition

		/**
		 * getStationID
		 *
		 * Gets the getStationID from the incoming Telegram
		 */
		ta_uint8 getStationID(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		/**
		 * getPlatformID
		 *
		 * Gets the Platform ID from the incoming Telegram
		 */
		ta_uint16 getPlatformID(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		/**
		 * getA_Time
		 *
		 * Gets the Arrival Time from the incoming Telegram
		 */
		ta_uint32 getA_Time(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		/**
		 * getD_Time
		 *
		 * Gets the Destination Time from the incoming Telegram
		 */
		ta_uint32 getD_Time(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		/**
		 * getFlag
		 *
		 * Gets the Flag from the incoming Telegram
		 */
		ta_uint8 getFlag(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		/**
		 * getReserved
		 *
		 * Gets the Reserve from the incoming Telegram
		 */
		ta_uint32 getReserve(ta_uint16 nDataRepeat=0, ta_uint16 outerLoop=0 ) const;

		private:
			static const ta_uint8 SUB_ID_POSITION;
			static const ta_uint8 SERVICE_ID_POSITION;
			static const ta_uint8 TRIP_CNT_POSITION;

			//REPETITION
			static const ta_uint8 TRAIN_ID_POSITION;
			static const ta_uint8 DESTINATION_ID_POSITION;
			static const ta_uint8 REC_CNT_POSITION;

			static const ta_uint8 STATION_ID_POSITION;
			static const ta_uint8 PLATFORM_ID_POSITION;
			static const ta_uint8 A_TIME_POSITION;
			static const ta_uint8 D_TIME_POSITION;
			static const ta_uint8 FLAG_POSITION;
			static const ta_uint8 RESERVE_POSITION;
			static const ta_uint8 INNER_REPETITION_SIZE;

			RepetitionContainer m_repetitionContainerSize;
			void initializeRepetitionSize();

		/**
		 * getRepetitionSize()
		 *
		 * Gets the number of TripCntRepetition
		 */
		ta_uint16 getRepetitionSize(ta_uint16 nDataRepeat/* =0 */) const;

	};
}
#endif