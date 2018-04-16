
#ifndef _GENA_ESB_TABLE_PARSER_H_
#define _GENA_ESB_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class GenaEsbTableParser : public GenaBaseTableParser
	{
	public:
		GenaEsbTableParser();
		virtual ~GenaEsbTableParser();
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
		GenaEsbTableParser(const GenaEsbTableParser&);
		GenaEsbTableParser& operator= (const GenaEsbTableParser&);

		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);

	private:
		TA_IRS_Bus::EsbInfoVec m_esbInfoVec;

	};
}
#endif //_GENA_PIS_TABLE_PARSER_H_
