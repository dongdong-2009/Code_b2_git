/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PATableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for parsing PA data from a raw data.
  */

#ifndef _PA_TABLE_PARSER_H_
#define _PA_TABLE_PARSER_H_

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/BaseTableParser.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	class PATableParser : public BaseTableParser
	{
	public:
		/**
		 * Construcor
		 */
		PATableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		/**
		 * Destructor
		 */
		virtual ~PATableParser();

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
		PATableParser(const PATableParser&);
		PATableParser& operator= (const PATableParser&);
		TA_IRS_Bus::PAStationInfoVec m_paStationInfoVec;

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
		void readPATable(TA_IRS_Bus::RawTable& rawTable);

		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : RawTable data
		 * @param recordByteOffset : raw table offset
		 * @param platformId       : Platform ID
		 */
		void importRecord(TA_IRS_Bus::RawTable& rawTable, unsigned int recordByteOffset, TA_IRS_Bus::PlatformIdType platformId);

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