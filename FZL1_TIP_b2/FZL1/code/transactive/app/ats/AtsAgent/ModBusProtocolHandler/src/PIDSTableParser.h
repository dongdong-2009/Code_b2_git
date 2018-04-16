/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PIDSTableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PIDS data from a raw data.
  */

#ifndef _PIDS_TABLE_PARSER_H_
#define _PIDS_TABLE_PARSER_H_

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/BaseTableParser.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"

namespace TA_IRS_App
{
	class PIDSTableParser : public BaseTableParser
	{
	public:
		PIDSTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~PIDSTableParser();
		/**
		 * parseAtsTable
		 *
		 * Method to use to process Ats Table
		 *
		 * @param rawTable : Rawtable data
		 */
		void parseAtsTable(TA_IRS_Bus::RawTable& rawTable);		
	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		PIDSTableParser(const PIDSTableParser&);
		PIDSTableParser& operator= (const PIDSTableParser&);
	};
}
#endif