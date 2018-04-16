/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\AgentOperationMode.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * A object helper class responsible for printing raw data logs and converting int to HexStr.
  */

#ifndef _ATS_HELPER_H_
#define _ATS_HELPER_H_

#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include <string>
#include <ctime>
#include "core/types/public_types/Constants.h"

namespace TA_IRS_Bus
{
	class AtsHelper
	{
	public:
		 AtsHelper();
		 virtual ~AtsHelper();

		 /**
		 * printRawData
		 *
		 * This will print the raw data information in the logs
		 *
		 * @param : const std::vector<unsigned char>& rawdata  - Raw data information
		 * @param : const std::string& sourceMsg               - The source of the logs
		 *
		 */
		static void printRawData(const std::vector<unsigned char>& rawdata, const std::string& sourceMsg);

		/**
		 * printRawData
		 *
		 * This will print the raw data information in the logs
		 *
		 * @param : const TA_Base_Bus::DataBlock<WORD>& rawData - Raw data information
		 * @param : const std::string& sourceMsg                - The source of the logs
		 *
		 */
		static void printRawData(const TA_Base_Bus::DataBlock<WORD>& rawData, const std::string& sourceMsg);

		 static std::string IntToStr(int val);
		 static std::string IntToHexStr(int val);
		 static TrackInfo getAllTrackInformation();
		 static BranchMap getAllBranchInformation();
		 static std::string getLocalTimeFormat(unsigned long arrivalTime);

		 /**
		  * isGoodDatapointQuality()
		  *
		  * Method used to check if the Datapoint Quality is Good or Bad
		  *
		  * @return bool : true if GOOD quality else false for BAD Quality
		  */
		 static bool isGoodDatapointQuality(TA_Base_Core::EDataPointQuality quality); 

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		AtsHelper(const AtsHelper&);
		AtsHelper& operator= (const AtsHelper& );
	};
}
#endif