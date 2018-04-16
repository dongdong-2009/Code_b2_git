
#ifndef _GENA_PLATFORM_TABLE_PARSER_H_
#define _GENA_PLATFORM_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class GenaPlatformTableParser : public GenaBaseTableParser
	{
	public:
		/**
		 * Constructor
		 */
		GenaPlatformTableParser();
		/**
		 * Destructor
		 */
		virtual ~GenaPlatformTableParser();

		/**
		 * parseAtsTable
		 *
		 * Method to use to process Ats Table
		 *
		 * @param rawTable : Gena RawTable data
		 */
		virtual void parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable);

	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		GenaPlatformTableParser(const GenaPlatformTableParser&);
		GenaPlatformTableParser& operator= (const GenaPlatformTableParser&);
		TA_IRS_Bus::PlatformInfoVec m_platformInfoVec;


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : Gena RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);
		void constructEmptyPisInfo(TA_IRS_Bus::PlatformInfo& platformInfo);

	};
}
#endif //_GENA_PLATFORM_TABLE_PARSER_H_