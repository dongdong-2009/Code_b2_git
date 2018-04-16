/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\TrainPISTableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PIS data from  raw data.
  * Update the PIDS data point in ISCS
  */

#ifndef _TRAIN_PIS_TABLE_PARSER_H_
#define _TRAIN_PIS_TABLE_PARSER_H_

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/BaseTableParser.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class TrainPISTableParser : public BaseTableParser
	{
	public:
		TrainPISTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainPISTableParser();
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
		TrainPISTableParser(const TrainPISTableParser&);
		TrainPISTableParser& operator= (const TrainPISTableParser&);
		TA_IRS_Bus::PISInfoVec m_pisInfoVec;

		
		/**
		 * getTotalNumberOfPlatform
		 *
		 * Method to use to get Total number of platforms
		 *
		 * @return long : Total number of platform
		 */
		unsigned long getTotalNumberOfPlatform();

		/**
		 * readPATable
		 *
		 * Method to use to readPATable data
		 *
		 * @param rawTable : RawTable object
		 */
		void readPISTable(TA_IRS_Bus::RawTable& rawTable);

		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 * @param recordByteOffset : raw table offset
		 * @param platformId       : Platform ID
		 */
		void importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset,TA_IRS_Bus::PlatformIdType platformId);

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

		unsigned short m_numberOfPlatforms; 
	};
}
#endif
