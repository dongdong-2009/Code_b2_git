
#ifndef _GENA_TRAIN_TABLE_PARSER_H_
#define _GENA_TRAIN_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class GenaTrainTableParser : public GenaBaseTableParser
	{
	public:
		GenaTrainTableParser();
		virtual ~GenaTrainTableParser();

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
		GenaTrainTableParser(const GenaTrainTableParser&);
		GenaTrainTableParser& operator= (const GenaTrainTableParser&);


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 *
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);

		/*
		 * constructEmptyValue
		 * Method to initialize trainInfo
		 *
		 * @param trainInfo         
		 *
		*/
		void constructEmptyValue(TA_IRS_Bus::TrainInfo& emptyRecord);

		void findBranchID(TA_Base_Bus::GENATopicDataPtr TrainData, TA_IRS_Bus::TrainInfo& newRecord);

		//unsigned short m_numberOfTrains;
		TA_IRS_Bus::TrainInfoVec m_trainInfoVec;
		
		TA_IRS_Bus::BranchMap m_BranchMap;

	};
}
#endif //_GENA_TRAIN_TABLE_PARSER_H_