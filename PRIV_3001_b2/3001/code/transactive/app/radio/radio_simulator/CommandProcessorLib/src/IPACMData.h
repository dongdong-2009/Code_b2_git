/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: g:\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\IPACMData.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface to use to expose the CMData.
  */

#ifndef _IPACM_DATA_H_
#define _IPACM_DATA_H_
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMData.h"
#include <string>

namespace TA_IRS_App
{
	class IPACMData
	{
	public:
		/**
		 * getCMdata
		 *
		 * This method is used to retrive the CMData
		 */
		virtual CMData& getCMData() = 0;

		/**
		 * getTLHeader
		 *
		 * This method is to retrive TLHeader Data
		 */
		virtual std::string& getTLHeader() = 0;

		virtual void logger(const std::vector<unsigned char>& data) = 0;

		/**
		 * getTTISLibVerstion
		 *
		 * This method is used to retrive the CMData
		 */
		virtual std::string& getTTISLibVersion() = 0;
	};
}
#endif