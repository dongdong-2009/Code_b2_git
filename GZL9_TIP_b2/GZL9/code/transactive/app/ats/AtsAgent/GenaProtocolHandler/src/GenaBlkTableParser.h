
#ifndef _GENA_BLK_TABLE_PARSER_H_
#define _GENA_BLK_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class GenaBlkTableParser : public GenaBaseTableParser
	{
	public:
		GenaBlkTableParser();
		virtual ~GenaBlkTableParser();
		/**
		 * parseAtsTable
		 *
		 * Method to use to process Ats Table
		 *
		 * @param rawTable : Gena Rawtable data
		 */
		void parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable);	

	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		GenaBlkTableParser(const GenaBlkTableParser&);
		GenaBlkTableParser& operator= (const GenaBlkTableParser&);
		TA_IRS_Bus::BlkInfoVec m_blkInfoVec;

		
		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);
	};
}
#endif //_GENA_PIS_TABLE_PARSER_H_
