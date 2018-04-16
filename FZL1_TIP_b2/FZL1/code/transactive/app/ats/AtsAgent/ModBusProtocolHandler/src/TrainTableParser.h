/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\TrainTableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing Train data from a raw data.
  */

#ifndef _TRAIN_TABLE_PARSER_H_
#define _TRAIN_TABLE_PARSER_H_

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/BaseTableParser.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class TrainTableParser : public BaseTableParser
	{
	public:
		TrainTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainTableParser();

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
		TrainTableParser(const TrainTableParser&);
		TrainTableParser& operator= (const TrainTableParser&);

		/**
		 * getTotalNumberOfPlatform
		 *
		 * Method to use to get Total number of platforms
		 *
		 * @return long : Total number of platform
		 */
		unsigned long getTotalNumberOfTrains();

		/**
		 * getAtsModbusStartAddress
		 *
		 * Method to use to get AtsModbus Start Address
		 */
		unsigned long getAtsModbusStartAddress();

		/**
		 * getAtsModbusEndAddress
		 *
		 * Method to use to get AtsModbus End Address
		 */
		unsigned long getAtsModbusEndAddress();

		/**
		 * readTrainTable
		 *
		 * Method to use to readTrainTable data
		 *
		 * @param rawTable : RawTable object
		 */
		void readTrainTable(TA_IRS_Bus::RawTable& rawTable);

		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 * @param recordByteOffset : raw table offset
		 * @param platformId       : Platform ID
		 */
		void importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset, bool lastTrainRecord);
		unsigned short m_numberOfTrains;
		TA_IRS_Bus::TrainInfoVec m_trainInfoVec;

	};
}
#endif