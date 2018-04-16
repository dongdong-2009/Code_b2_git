
#ifndef _GENA_PLATFORM_DOOR_TABLE_PARSER_H_
#define _GENA_PLATFORM_DOOR_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class GenaPlatformDoorTableParser : public GenaBaseTableParser
	{
	public:
		/**
		 * Constructor
		 */
		GenaPlatformDoorTableParser();
		/**
		 * Destructor
		 */
		virtual ~GenaPlatformDoorTableParser();

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
		GenaPlatformDoorTableParser(const GenaPlatformDoorTableParser&);
		GenaPlatformDoorTableParser& operator= (const GenaPlatformDoorTableParser&);
		TA_IRS_Bus::PlatformDoorInfoVec m_platformDoorInfoVec;


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : Gena RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);
		void constructEmptyInfo(TA_IRS_Bus::PlatformDoorInfo& platformInfo);
	};
}
#endif //_GENA_PLATFORM_TABLE_PARSER_H_