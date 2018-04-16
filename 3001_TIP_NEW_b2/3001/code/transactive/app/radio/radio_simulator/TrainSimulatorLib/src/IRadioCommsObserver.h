/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\MotorolaProtocolLib\src\IProtocolHelper.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/08 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface to use to sendData to socket
  */

#ifndef _I_RADIO_COMMS_OBSERVER_H_
#define _I_RADIO_COMMS_OBSERVER_H_
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "boost/shared_ptr.hpp"

namespace TA_IRS_App
{
	class IRadioCommsObserver
	{
	public:
		/**
		 * sendData
		 *
		 * This method is used to send a data to a socket
		 *
		 * @param : const std::vector<unsigned char>& pData
		 */
		virtual void sendData( const std::vector<unsigned char>& pData) = 0;

		/**
		 * printData
		 */
		virtual void printData(const std::vector<unsigned char>& rawMessage, bool isReceived = false, bool display = false) = 0;

	};
}
#endif