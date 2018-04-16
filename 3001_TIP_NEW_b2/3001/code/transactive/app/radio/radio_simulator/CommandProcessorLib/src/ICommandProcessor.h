/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\ISDSMessageObserver.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/08 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface use for SDS messages
  */

#ifndef _ICOMMAND_PROCESSOR_H_
#define _ICOMMAND_PROCESSOR_H_
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "boost/shared_ptr.hpp"

namespace TA_IRS_App
{
	class ICommandProcessor
	{
	public:
		/**
		 * sendSDSMessage
		 *
		 * This method is used to process the SDS Message 
		 *
		 * @param: const std::vector<unsigned char>& pData
		 * @parma: std::vector<unsigned char>& resp
		 */
		virtual void sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage) = 0;
	};
}
#endif