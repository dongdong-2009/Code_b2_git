
#ifndef _GENA_PIS_TABLE_PARSER_H_
#define _GENA_PIS_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{

	typedef std::vector<AttributePtr>	T_pidDataAttrVec;
	typedef T_pidDataAttrVec::iterator	T_pidDataAttrVecItr;
	typedef std::vector<std::string>	T_trainArrTimeVec;

	class GenaPISTableParser : public GenaBaseTableParser
	{
	public:
		/**
		 * Constructor
		 */
		GenaPISTableParser();
		/**
		 * Destructor
		 */
		virtual ~GenaPISTableParser();

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
		GenaPISTableParser(const GenaPISTableParser&);
		GenaPISTableParser& operator= (const GenaPISTableParser&);
		TA_IRS_Bus::PISInfoVec	m_pisStationInfoVec;


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : Gena RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);
		void constructEmptyPisInfo(TA_IRS_Bus::PISInfo& paInfo);
		std::string getErlierTime(const std::string & strArrTime1,const std::string & strArrTime2);
		bool convertTimeFormat(const std::string & strTime,struct time& tmTime);

		/**
		 * timeCompare
		 *
		 * Method to use to compare time
		 *
		 * @return bool : true - tmTime1 is earlier than tmTime2, otherwise is false
		 * 
		 */
		bool timeCompare(struct time& tmTime1,struct time& tmTime2);
		std::string get1stTrainArrTime(const T_trainArrTimeVec & trainArrTimeVec);
		std::string getAttributeValue(const std::string& uri, AttributePtr attr);
		std::vector<AttributePtr> getPidDataAttrVec(const std::string& attrbName, T_AttribValueMap& pidDataMap);

	};
}
#endif //_GENA_PIS_TABLE_PARSER_H_