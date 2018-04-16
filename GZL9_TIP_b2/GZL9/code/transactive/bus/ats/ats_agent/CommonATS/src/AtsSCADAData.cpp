

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_Bus
{
	AtsSCADAData::AtsSCADAData(TA_Base_Bus::ItaScada* tScada, std::string dataName)
		:m_itScada(tScada), m_dataName(dataName)	 
	{		 
	};

	AtsSCADAData::~AtsSCADAData()
	{		 
	} 

	void AtsSCADAData::setLocalDataPoint(const std::string& dpAddress, unsigned long dpValue)
	{
		FUNCTION_ENTRY("setLocalDataPoint");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SetLocalDataPoint, %s Address : %s, Value : %d",
			m_dataName.c_str(),dpAddress.c_str(),dpValue );

		try
		{
			time_t timestamp;
			time(&timestamp);

			//Update the Datapoint
			m_itScada->setLocalDataPointStateByAddress(dpAddress,dpValue,
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
				<< dpAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}


	void AtsSCADAData::setLocalDataPoint(const std::string& dpAddress, bool dpValue, TA_Base_Core::EDataPointQuality dpQuality)
	{
		FUNCTION_ENTRY("setLocalDataPoint");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SetLocalDataPoint, %s Address : %s, Value : %d, Quality %d",
			m_dataName.c_str(),dpAddress.c_str(),dpValue,static_cast<int>(dpQuality));

		try
		{
			time_t timestamp;
			time(&timestamp);

			//Update the Datapoint
			m_itScada->setLocalDataPointStateByAddress(dpAddress,dpValue,
				dpQuality,timestamp); 
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
				<< dpAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

	
 
	void AtsSCADAData::setLocalDataPoint(const std::string& dpAddress, const std::string& dpValue)
	{
		FUNCTION_ENTRY("setLocalDataPoint");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SetLocalDataPoint, %s Address : %s, Value : %s",
			m_dataName.c_str() ,dpAddress.c_str(),dpValue.c_str());

		try
		{
			time_t timestamp;
			time(&timestamp);

			std::string strValue = " ";
			if(TA_IRS_Bus::EMPTY_VAL != dpValue)
				strValue = dpValue;

			//Update the Datapoint
			m_itScada->setLocalDataPointStateByAddress(dpAddress,strValue,
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
				<< dpAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

	void AtsSCADAData::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const std::string& value, TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");

		try
		{
			time_t timestamp;
			time(&timestamp);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Update Datapoint Quality with Matching %s address %s to quality %d",
				m_dataName.c_str(), addressSubString.c_str(), static_cast<int>(quality));

			m_itScada->setAllLocalDataPointStateMatchingAddress(addressSubString,value,
				quality,timestamp); 

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
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}
		FUNCTION_EXIT;
	}

	void AtsSCADAData::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const unsigned long value, TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");

		try
		{
			time_t timestamp;
			time(&timestamp);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Update Datapoint Quality with Matching %s address %s to quality %d",
				m_dataName.c_str(), addressSubString.c_str(), static_cast<int>(quality));

			m_itScada->setAllLocalDataPointStateMatchingAddress(addressSubString,value,
				quality,timestamp); 

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
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}
		FUNCTION_EXIT;
	}

	void AtsSCADAData::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const bool value, TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");

		try
		{
			time_t timestamp;
			time(&timestamp);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Update Datapoint Quality with Matching %s address %s to quality %d",
				m_dataName.c_str(), addressSubString.c_str(), static_cast<int>(quality));

			m_itScada->setAllLocalDataPointStateMatchingAddress(addressSubString,value,
				quality,timestamp); 

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
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

	void AtsSCADAData::setAllLocalDataPointQualitiesMatchingAddress (const std::string& addressSubString, TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("setAllLocalDataPointQualitiesMatchingAddress");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Update Datapoint Quality :%d, with Emergency Stop Button Address matching:%s",
			static_cast<int>(quality),addressSubString.c_str());

		m_itScada->setAllLocalDataPointQualitiesMatchingAddress(addressSubString,quality);

		FUNCTION_EXIT;
	}

}
