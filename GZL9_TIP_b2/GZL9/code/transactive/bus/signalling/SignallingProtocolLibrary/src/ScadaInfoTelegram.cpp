/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\ScandaInfoTelegram.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This telegram is used to send data to ATS for the Scada Info
  */

#include "bus/signalling/SignallingProtocolLibrary/src/ScadaInfoTelegram.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 ScadaInfoTelegram::MSG_ID_POSITION          = 2;
	const ta_uint8 ScadaInfoTelegram::SECTION_NUMBER_POSITION  = 8;  //plus header
	const ta_uint8 ScadaInfoTelegram::SECTION_ID_POSITION      = 10; // plus header
	const ta_uint8 ScadaInfoTelegram::POWER_STATUS_POSITION    = 12; // plus header
	const ta_uint8 ScadaInfoTelegram::SCADA_INFO_TELEGRAM_SIZE = 9;
	const ta_uint8 ScadaInfoTelegram::SCADA_MSG_LENGTH         = 7;
	
	const ta_uint8 ScadaInfoTelegram::LEN_MSG_SECTION_NUMBER_SIZE = 6;
	const ta_uint8 ScadaInfoTelegram::SCADA_DATA_CONTENT_SIZE  = 3;
	
	ScadaInfoTelegram::ScadaInfoTelegram(ta_uint16 numberOfSections)
		: Telegram(LEN_MSG_SECTION_NUMBER_SIZE + (numberOfSections*SCADA_DATA_CONTENT_SIZE),ISCS_ATS_SCADA_INFO)	  
	{
		FUNCTION_ENTRY("ScadaInScadaInfoTelegram");
		//setSectionNumber(numberOfSections);
		FUNCTION_EXIT;
	}

	void ScadaInfoTelegram::setSectionNumber(ta_uint16 sectionNumber)
	{
		FUNCTION_ENTRY("setSectionNumber");

		setWord(SECTION_NUMBER_POSITION,sectionNumber);

		FUNCTION_EXIT;
	}

	void ScadaInfoTelegram::setSectionId(ta_uint16 sectionNumber)
	{
		FUNCTION_ENTRY("setSectionNumber");

		setWord(SECTION_ID_POSITION,sectionNumber);

		FUNCTION_EXIT;
	}

	void ScadaInfoTelegram::setPowerStatus(ta_uint8 powerStatus)
	{
		FUNCTION_ENTRY("setPowerStatus");

		setByte(POWER_STATUS_POSITION,powerStatus);

		FUNCTION_EXIT;
	}

	ta_uint16 ScadaInfoTelegram::getSectionNumber() const
	{
		FUNCTION_ENTRY("getSectionNumber");
		FUNCTION_EXIT;
		return getWord(SECTION_NUMBER_POSITION);
	}

	ta_uint16 ScadaInfoTelegram::getSectionId() const
	{
		FUNCTION_ENTRY("getSectionId");
		FUNCTION_EXIT;
		return getWord(SECTION_ID_POSITION);
	}

	ta_uint8 ScadaInfoTelegram::getPowerStatus() const
	{
		FUNCTION_ENTRY("getPowerStatus");
		FUNCTION_EXIT;
		return getByte(POWER_STATUS_POSITION);
	}

	void ScadaInfoTelegram::setDataContent(const std::vector<ta_uint8>& dataContent)
	{
		FUNCTION_ENTRY("setDataContent");

		setBytes(SECTION_ID_POSITION,dataContent);

		FUNCTION_EXIT;
	}
}