
#ifndef _GENA_SIG_TABLE_PARSER_H_
#define _GENA_SIG_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class SignalStatusTableParser : public GenaBaseTableParser
	{
	public:
		/**
		 * Constructor
		 */
		SignalStatusTableParser();
		/**
		 * Destructor
		 */
		virtual ~SignalStatusTableParser();

		/**
		 * parseAtsTable
		 *
		 * Method to use to process Ats Table
		 *
		 * @param rawTable : Gena RawTable data
		 */
		void parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable);

	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		SignalStatusTableParser(const SignalStatusTableParser&);
		SignalStatusTableParser& operator= (const SignalStatusTableParser&);
		TA_IRS_Bus::SignalInfoVec m_signalInfoVec;


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : Gena RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);

		void constructEmptySigInfo(TA_IRS_Bus::SignalInfo& sigInfo);

	};
}
#endif //_GENA_SIG_TABLE_PARSER_H_