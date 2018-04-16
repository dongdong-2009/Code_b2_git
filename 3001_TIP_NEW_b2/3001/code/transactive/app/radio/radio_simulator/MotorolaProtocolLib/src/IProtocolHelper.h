
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
  * Interface to expose the Helper methods to encode/decode SDTS messages
  */

#ifndef _IPROTOCOL_HELPER_H
#define _IPROTOCOL_HELPER_H
#include <vector>

namespace TA_IRS_App
{
	class IProtocolHelper
	{
	public:
		/**
		 * processIncomingSDS
		 *
		 * This method is used process the incoming SDS whether it is registration or a sds data
		 *
		 * @param : std::vector<unsigned char> pData
		 */
		virtual void processIncomingSDS( std::vector<unsigned char>& pData ) = 0;

		/**
		 * execAddISSI
		 *
		 * This method is used to add the ISSI
		 */
		virtual bool execAddISSI(const std::string& cmdValue) = 0;

		/**
		 * execListISSIPassNum
		 *
		 * This method is used to list the ISSI Pass Number
		 */
		virtual bool execListISSIPassNum() = 0;

	};
}
#endif