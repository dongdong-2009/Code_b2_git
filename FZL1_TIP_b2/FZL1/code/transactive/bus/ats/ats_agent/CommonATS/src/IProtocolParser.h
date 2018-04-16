/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IProtocolParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * A interface to be used for Protocol parsing
  */

#ifndef _I_PROCOTOL_PARSER_H_
#define _I_PROCOTOL_PARSER_H_
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"

namespace TA_IRS_Bus
{
	class IProtocolParser
	{
	public:
		/**
		 * processAtsTable
		 *
		 * Method used to process Ats table
		 * 
		 * @param rawTable  : rawTable
		 */
		virtual void parseAtsTable(RawTable& rawTable) = 0;

		/**
		 * setAtsDataNotification
		 *
		 * Method used to set Data notification
		 * 
		 * @param atsDataNofic  : Data notification object
		 */
		virtual void setAtsDataNotification(IAtsDataNotification* atsDataNofic) = 0;
	};
}
#endif