/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\SignalStatusTelegram.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to communicate Signal Status.
  */

#ifndef _SIGNAL_STATUS_TELEGRAM_H_
#define _SIGNAL_STATUS_TELEGRAM_H_
#include "bus/signalling/SignallingProtocolLibrary/src/CommonTelegram.h"
#include <string>
#include <vector>

namespace TA_IRS_Bus
{
	class SignalStatusTelegram : public CommonTelegram
	{

	public:
	/**
	 * SignalStatusTelegram
	 * 
	 * Constructor to build the packet from incoming data.
	 * 
	 * @return 
	 * @param : const Telegram& incomingData
	 */
		SignalStatusTelegram( const Telegram& incomingData );

	/**
	 * Destruction
	 */
		virtual ~SignalStatusTelegram(){};

	/**
	 * getSignalCnt
	 *
	 * Gets the Signal Cnt of the incoming packets
	 */
		ta_uint8 getSignalCnt() const;

	/**
	 * getRtuId
	 *
	 * Gets the Rtu id of the incoming packets
	 */
		ta_uint16 getRtuId(ta_uint16 nDataRepeat=0) const;

	/**
	 * getSignalName
	 *
	 * Gets the Signal Name of the incoming packets
	 */
		std::string getSignalName(ta_uint16 nDataRepeat=0) const;

	/**
	 * getSignalStatus
	 *
	 * Gets the Signal Status of the incoming packets
	 */
		ta_uint32 getSignalStaus(ta_uint16 nDataRepeat=0) const;

	 /**
	  * getDataRepeatSignalStatus
	  *
	  * Gets the Data Repeat Signal Status of the incoming packets
	  */
		std::vector<unsigned char> getDataRepeatSignalStatus() const;

	private:
		static const ta_uint8 RTU_ID_POSITION;
		static const ta_uint8 SIGNAL_NAME_POSITION;
		static const ta_uint8 SIGNAL_NAME_SIZE;
		static const ta_uint8 SIGNAL_STATUS_POSITION;
		static const ta_uint8 SIGNAL_STATUS_SIZE;
		static const ta_uint8 SIGNAL_STATUS_WORD_SIZE;
		static const ta_uint8 DATA_REPEAT_SIGNAL_STATUS_SIZE;
	};
}
#endif