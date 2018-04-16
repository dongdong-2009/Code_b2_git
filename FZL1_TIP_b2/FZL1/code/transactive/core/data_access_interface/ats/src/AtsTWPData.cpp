/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsTWPData.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsTWPDataHelper is an object that is held by AtsTWPData and ConfigAtsTWPData objects. 
  * It contains all data used by AtsTWPDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsTWPData and ConfigAtsTWPData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsTWPData.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

	const std::string AtsTWPData::INPUT_TYPE			= "Input";
	const std::string AtsTWPData::OUTPUT_TYPE			= "Output";

	// Output commands
	const std::string AtsTWPData::NO_WASH								= "<Selection of No Wash mode>";
	const std::string AtsTWPData::DAILY_RINSE							= "<Selection of Daily Rinse wash>";
	const std::string AtsTWPData::DAILY_DETERGENT						= "<Selection of Daily Detergent wash>";
	const std::string AtsTWPData::WEEKLY_WASH							= "<Selection of Weekly wash>";
	const std::string AtsTWPData::CAB_END_WASH							= "<Train stopped for cab end wash>";
	const std::string AtsTWPData::STOP_WP								= "<Stop WP request>";
	const std::string AtsTWPData::RESTART_WP							= "<Restart WP request>";

	// Input commands
	const std::string AtsTWPData::WP_STATUS								= "<WP Status>";
	const std::string AtsTWPData::NO_WASH_SELECTED						= "<No wash selected>";
	const std::string AtsTWPData::DAILY_RINSE_SELECTED					= "<Daily wash rinse mode selected>";
	const std::string AtsTWPData::DAILY_DETERGENT_SELECTED				= "<Daily wash detergent mode selected>";
	const std::string AtsTWPData::WEEKLY_WASH_SELECTED					= "<Weekly wash selected>";
	const std::string AtsTWPData::CAB_END_WASH_COMPLETED				= "<Cab end wash completed>";
	const std::string AtsTWPData::TRAIN_WASH_CYCLE_COMPLETED			= "<Train Wash cycle fully completed>";
	const std::string AtsTWPData::TRAIN_SPEED_OUT_OF_RANGE				= "<Train Speed out of range>";
	const std::string AtsTWPData::TRAIN_STALLED_IN_TWP					= "<Train stalled in TWP (detected by TWP or by ATS)>";
	const std::string AtsTWPData::TRAIN_INCORRECTLY_POSITIONED			= "<Train incorrectly Positioned>";
	const std::string AtsTWPData::WASH_CYCLE_START_FAILURE				= "<Wash Cyle Start Failure>";
	const std::string AtsTWPData::PH_CHECK								= "<pH check (<6 or >9)>";
	const std::string AtsTWPData::MAINS_WATER_LOW						= "<Mains Water Low>";
	const std::string AtsTWPData::SECOND_BRUSH_RINSE					= "<Second Brush Rinse and Final Rinse Spray>";
	const std::string AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_HOR		= "<Cab End Wash Gantry Movement Failure (hor.)>";
	const std::string AtsTWPData::CAB_END_WASH_GANTRY_FAILURE_VER		= "<Cab End Wash Gantry Movement Failure (ver.)>";
	const std::string AtsTWPData::CAB_END_WASH_BRUSH_FAILURE			= "<Cab End Wash Brush Movement Failure>";
	const std::string AtsTWPData::EMERGENCY_STOP_ACTIVATED				= "<EMERGENCY STOP ACTIVATED>";
	const std::string AtsTWPData::PNEUMATIC_FAILURE						= "<Pneumatic Failure>";
	const std::string AtsTWPData::BRUSH_ACTUATION_FAILURE				= "<Brush Actuation (individual) Failure>";
	const std::string AtsTWPData::SIDE_ROOF_BRUSHING_FAILURE			= "<Side/roof Brushing Failure>";
	const std::string AtsTWPData::END_BRUSHING_FAILURE					= "<End Brushing Failure>";
	const std::string AtsTWPData::AIR_BLOW_FAILURE						= "<Air Blow Failure>";
	const std::string AtsTWPData::PRE_WET_FAILURE						= "<Pre-wet Failure>";
	const std::string AtsTWPData::RECYCLED_RINSE_FAILURE				= "<Recycled Rinse failure>";
	const std::string AtsTWPData::WATER_RECYCLING_FAILURE				= "<Water Recycling Failure>";
	const std::string AtsTWPData::DETERGENT_FAILURE						= "<Detergent failure>";
	const std::string AtsTWPData::DETERGENT_LOW_ALARMS					= "<Detergent Low Alarms>";
	const std::string AtsTWPData::AIR_PRESSURE_LOW						= "<Air Pressure Low>";
	const std::string AtsTWPData::NON_CRITICAL_SUMMARY_ALARM			= "<Non Critical Summary Alarm>";

	AtsTWPData::AtsTWPData(const unsigned long key)
    {
        m_atsTWPDataHelper = new AtsTWPDataHelper(key);
    }

    AtsTWPData::AtsTWPData(unsigned long row, TA_Base_Core::IData& data)
    {
		m_atsTWPDataHelper = new AtsTWPDataHelper(row, data);
    }

    AtsTWPData::~AtsTWPData() 
	{
        delete m_atsTWPDataHelper;
        m_atsTWPDataHelper=NULL;
    }

	unsigned long AtsTWPData::getTWPDataPointEntityKey()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPDataPointEntityKey();
	}

	std::string AtsTWPData::getTWPDataPointType()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPDataPointType();
	}

	std::string AtsTWPData::getTWPCommand()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPCommand();
	}
	
	unsigned long AtsTWPData::getKey()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getKey();
	}

	std::string AtsTWPData::getName()
    {
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
        std::ostringstream name;
		name << m_atsTWPDataHelper->getTWPCommand();
        name << " - ";
		name << m_atsTWPDataHelper->getTWPDataPointType();

        return name.str();
    }

	void AtsTWPData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AtsTWPData::invalidate");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");

        m_atsTWPDataHelper->invalidate();

        LOG( SourceInfo, DebugUtil::FunctionExit, "AtsTWPData::invalidate");
    }

} // TA_Core
