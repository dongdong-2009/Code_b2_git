/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPisData.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * An object which is responsible for handling Train PIS Datapoint
  */
#include "bus/ats/ats_agent/CommonATS/src/TrainPisData.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include <sstream>

namespace TA_IRS_Bus
{
	//Constant value based on Datapoint Address
	const std::string TrainPisData::ZERO_PADDING                  = "0";
	const std::string TrainPisData::STATION_ST                    = "ST";
	const std::string TrainPisData::TIS_PLATFORM_SEPARATOR        = "_AITPI0";

	TrainPisData::TrainPisData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_itScada(itScada),
		  m_trainPisAddress(EntityData->getTrainPisDpAddress())
	{
		FUNCTION_ENTRY("TrainPisData");

		initialize();

		FUNCTION_EXIT;
	}

	TrainPisData::~TrainPisData()
	{
		FUNCTION_ENTRY("TrainPisData");

		deInitialize();

		FUNCTION_EXIT;
	}

	void TrainPisData::updateTrainPisData(const PISInfoVec& pisInfo)
	{
		FUNCTION_ENTRY("updateTrainPisData");

		PISInfoVec::const_iterator iterPis = pisInfo.begin();
		for (iterPis; iterPis != pisInfo.end(); iterPis++)
		{
			updateSchedArrTimeDp((*iterPis));
		}

		FUNCTION_EXIT;
	}

	void TrainPisData::updateSchedArrTimeDp(const PISInfo& pisInfo)
	{
		FUNCTION_ENTRY("updateSchedArrTimeDp");
		
		//Convert to local time format
		std::string arrivalTimeStr = AtsHelper::getLocalTimeFormat(pisInfo.Train1st_ArrivalTime);
		//Get the Train PIS Datapoint Address
		std::string trainPisAddress = getAddressByPlatformID(pisInfo.PlatformId);

		//Update Datapoint for Train Arrival Time
		updateDataPoint(arrivalTimeStr,trainPisAddress);

		FUNCTION_EXIT;
	}

	void TrainPisData::initialize()
	{
		FUNCTION_ENTRY("initialize");

		//Get all platform information
		m_platforms = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAllAtsPlatforms();

		FUNCTION_EXIT;
	}

	void TrainPisData::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		TA_IRS_Core::IAtsPlatformsIt platformIdIter = m_platforms.begin();
		for ( platformIdIter; platformIdIter != m_platforms.end(); platformIdIter++ )
		{
			if ( NULL != (*platformIdIter))
			{
				delete (*platformIdIter);
				(*platformIdIter) = NULL;
			}
		}

		FUNCTION_EXIT;
	}

	std::string TrainPisData::getStationId(unsigned long platformId)
	{
		FUNCTION_ENTRY("getStationId");

		std::ostringstream _strData;

		TA_IRS_Core::IAtsPlatformsIt iter = m_platforms.begin();
		for ( iter; iter != m_platforms.end(); iter++ )
		{
			if ( (*iter)->getPlatformId() == platformId )
			{
				_strData << STATION_ST;

				if ( (*iter)->getStationId() <=9 && (*iter)->getStationId() >= 1 )
				{
					_strData << ZERO_PADDING;
				}

				_strData << (*iter)->getStationId();
				break;
			}
		}

		FUNCTION_EXIT;
		return _strData.str();
	}

	std::string TrainPisData::getTISPlatformID(unsigned long platformID)
	{
		FUNCTION_ENTRY("getStationID");

		std::ostringstream _strData;

		TA_IRS_Core::IAtsPlatformsIt iter = m_platforms.begin();
		for ( iter; iter != m_platforms.end(); iter++ )
		{
			if ( (*iter)->getPlatformId() == platformID )
			{
				_strData << TIS_PLATFORM_SEPARATOR;
				_strData << static_cast<int>( (*iter)->getTisPlatformId() );
				break;
			}
		}

		FUNCTION_EXIT;
		return _strData.str();
	}

	std::string TrainPisData::getAddressByPlatformID(unsigned long platformID)
	{
		FUNCTION_ENTRY("getAddressByPlatformID");

		//Concatenate Address Name with Station ID and TISPlatformID
		std::ostringstream _strAddress;
		_strAddress << m_trainPisAddress
			<< getStationId(platformID)
			<< getTISPlatformID(platformID);

		std::string _ret = _strAddress.str();

		FUNCTION_EXIT;
		return _ret;
	}

	void TrainPisData::updateDataPoint(const std::string& trainArrivalTime, const std::string& trainPisAddress)
	{
		FUNCTION_ENTRY("updateDataPoint");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Update datapoint, Address : %s, Value : %d",
			trainPisAddress.c_str(), trainArrivalTime.c_str() );

		try
		{
			time_t timestamp;
			time(&timestamp);

			//Update the Datapoint
			m_itScada->setLocalDataPointStateByAddress(trainPisAddress,trainArrivalTime,
				TA_Base_Core::ITA_QUALITY_GOOD,timestamp); 
		}
		catch(TA_Base_Core::CtaScadaInvalidConfigurationException& mssge)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"%s", mssge.what());
		}

		catch(TA_Base_Core::CtaScadaWriteException& e)
		{
			std::ostringstream reason;
			reason << "Setting Local Data Point Error "
				<< trainPisAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}


		FUNCTION_EXIT;
	}

	void TrainPisData::initializeDatapoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		try
		{
			time_t timestamp;
			time(&timestamp);

			//Update the Datapoint
			m_itScada->setAllLocalDataPointStateMatchingAddress(m_trainPisAddress,EMPTY_VAL,
				TA_Base_Core::ITA_QUALITY_GOOD,timestamp); 
		}
		catch(TA_Base_Core::CtaScadaInvalidConfigurationException& mssge)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"%s", mssge.what());
		}

		catch(TA_Base_Core::CtaScadaWriteException& e)
		{
			std::ostringstream reason;
			reason << "Setting Local Data Point Error "
				<< m_trainPisAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

}