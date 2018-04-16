/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\PracticalTelegramStart.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef _PRACTICAL_TELEGRAM_START_H_
#define _PRACTICAL_TELEGRAM_START_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"

namespace TA_IRS_Bus
{
	class PracticalTelegramStart : public CommonTelegram
	{
	public:
		/**
		 * PracticalTelegramStart
		 * 
		 * Constructor to build the packet from incoming data.
		 * 
		 * @return 
		 * @param : const Telegram& incomingData
		 */
		PracticalTelegramStart( const Telegram& incomingData );

		/**
		 * Destructor
		 */
		virtual ~PracticalTelegramStart();

		/**
		 * getSub_id
		 *
		 * Gets the Sub ID from the incoming packets
		 */
		ta_uint16 getSubID() const;

	private:
		static const ta_uint8 SUB_ID_POSITION;

	};
}

#endif