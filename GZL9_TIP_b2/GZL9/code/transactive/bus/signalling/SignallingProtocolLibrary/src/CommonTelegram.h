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
  * This telegram is used access the common data from incoming packets
  */

#ifndef _COMMON_TELEGRAM_H_
#define _COMMON_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include <vector>

namespace TA_IRS_Bus
{
	class CommonTelegram : public Telegram
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
			CommonTelegram(const Telegram& incomingData);

			CommonTelegram(ta_uint16 msgLength, ETelegramIdentification identification, ta_uint16 telegramSize);
		 /**
		  * Destructor
		  */
			virtual ~CommonTelegram(){};

		/**
		 * getMsgId
		 *
		 * Gets the Message Id of the incoming packets
		 */
		ta_uint16 getMsgId() const;

	

		protected:
			
			// Packet Data Message (Common for all SIG Data)
			static const ta_uint8	TRAIN_LENGTH_WORD_POSITION;
			static const ta_uint8	TRAIN_MSG_ID_POSITION;
			static const ta_uint8	TRAIN_UNIT_NUMBER_POSITION;
			static const ta_uint8	TRAIN_SERVICE_NUMBER_POSITION;
			static const ta_uint8	TRAIN_CNT_POSITION;

		/**
		 * getDataRepeat
		 *
		 * Gets the Data Repeat of the incoming telegram
		 */
		std::vector<unsigned char> getDataRepeat(ta_uint16 position, ta_uint16 length) const;
	   
	};
}

#endif