/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/AtsAgentEntityData.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/12/23 17:32:14 $
  * Last modified by:  $Author: peter.wong $
  * 
  * AtsAgentEntityData provide read-only access to AtsAgent entity data.
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	
    const std::string AtsAgentEntityData::ENTITY_TYPE = "AtsAgent";
    const std::string AtsAgentEntityData::ATS1_IP_ADDRESS = "AtsServer1IpAddress";
	const std::string AtsAgentEntityData::ATS1_SERVICE_PORT = "AtsServer1ServicePort";
	const std::string AtsAgentEntityData::ATS2_IP_ADDRESS = "AtsServer2IpAddress";
	const std::string AtsAgentEntityData::ATS2_SERVICE_PORT = "AtsServer2ServicePort";
	const std::string AtsAgentEntityData::AGENT_ATS_LOCATION_TYPE = "AgentAtsLocationType";
	const std::string AtsAgentEntityData::ATS_TABLES_BASE_ADDRESS = "AtsTablesBaseAddress";
	const std::string AtsAgentEntityData::ISCS_TABLES_BASE_ADDRESS = "IscsTablesBaseAddress";
	const std::string AtsAgentEntityData::MAX_COMMS_RETRIES = "MaxCommsRetries";
	const std::string AtsAgentEntityData::MAX_WATCHDOG_INTERVAL = "MaxWatchdogInterval";

	const std::string AtsAgentEntityData::ATS_LOC_TYPE_OCC = "OCC";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_DEPOT = "DPT";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_MAIN_STATION = "MSS";		
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_SECONDARY_STATION_1 = "SSS1";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_SECONDARY_STATION_2 = "SSS2";

	const std::string AtsAgentEntityData::OA1_TIMEOUT = "OA1TableTimeout";
	const std::string AtsAgentEntityData::ATC_TIMEOUT = "ATCDataTimeout";
	const std::string AtsAgentEntityData::CONNECTION_FAILED_TIMEOUT = "ConnectionFailedTimeout";
	const std::string AtsAgentEntityData::DEFAULT_VALUE = "0";

	const std::string AtsAgentEntityData::ASSET_NAME = "AssetName";

    AtsAgentEntityData::AtsAgentEntityData() :
        m_isValidData(false),  m_atsServer1IpAddress (""), m_atsServer1ServicePort (0),
		m_atsServer2IpAddress (""), m_atsServer2ServicePort (0), m_agentAtsLocationType (Unknown), 
		m_atsTablesBaseAddress (0), m_iscsTablesBaseAddress (0), 
		m_oa1Timeout(0),m_atcTimeout(0), m_assetName(""),m_connectionFailedTimeout(0)
    {}

    AtsAgentEntityData::AtsAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() ),
        m_isValidData(false)
    {
    }

    void AtsAgentEntityData::invalidate()
    {
        getHelper()->invalidate();

        m_isValidData = false;
    }

    AtsAgentEntityData::~AtsAgentEntityData()
    {
    }

    std::string AtsAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string AtsAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
    
	std::string AtsAgentEntityData::getAtsServer1IpAddress()
    {
		m_atsServer1IpAddress = getHelper()->getParameter(ATS1_IP_ADDRESS);

        return m_atsServer1IpAddress;
    }

	std::string AtsAgentEntityData::getAtsServer1ServicePort()
    {
		m_atsServer1ServicePort = getHelper()->getParameter(ATS1_SERVICE_PORT);

        return m_atsServer1ServicePort;
    }

	std::string AtsAgentEntityData::getAtsServer2IpAddress()
    {
		m_atsServer2IpAddress = getHelper()->getParameter(ATS2_IP_ADDRESS);

        return m_atsServer2IpAddress;
    }

	std::string AtsAgentEntityData::getAtsServer2ServicePort()
    {
		m_atsServer2ServicePort = getHelper()->getParameter(ATS2_SERVICE_PORT);

        return m_atsServer2ServicePort;
    }

	std::string AtsAgentEntityData::getAssetName()
    {
		m_assetName = getHelper()->getParameter(ASSET_NAME);

        return m_assetName;
    }

	AtsAgentEntityData::EAtsLocationType AtsAgentEntityData::getAgentAtsLocationType()
	{
		std::string value = getHelper()->getParameter(AGENT_ATS_LOCATION_TYPE);
		if (ATS_LOC_TYPE_OCC == value)
		{
			m_agentAtsLocationType = Occ;
		}
		else if (ATS_LOC_TYPE_DEPOT == value)
		{
			m_agentAtsLocationType = Depot;
		}
		else if (ATS_LOC_TYPE_MAIN_STATION == value)
		{
			m_agentAtsLocationType = MainStation;
		}
		else if (ATS_LOC_TYPE_SECONDARY_STATION_1 == value)
		{
			m_agentAtsLocationType = SecondaryStation1;
		}
		else if (ATS_LOC_TYPE_SECONDARY_STATION_2 == value)
		{
			m_agentAtsLocationType = SecondaryStation2;
		}
		else
		{
			m_agentAtsLocationType = Unknown;
		}

        return m_agentAtsLocationType;
	}

	unsigned long AtsAgentEntityData::getAtsTablesBaseAddress()
    {
		std::string value = getHelper()->getParameter(ATS_TABLES_BASE_ADDRESS);
		m_atsTablesBaseAddress = EntityHelper::getUnsignedLongData(value);

        return m_atsTablesBaseAddress;
    }

	unsigned long AtsAgentEntityData::getIscsTablesBaseAddress()
    {
		std::string value = getHelper()->getParameter(ISCS_TABLES_BASE_ADDRESS);
		m_iscsTablesBaseAddress = EntityHelper::getUnsignedLongData(value);

        return m_iscsTablesBaseAddress;
    }

	unsigned long AtsAgentEntityData::getMaxCommsRetries()
    {
		std::string value = getHelper()->getParameter(MAX_COMMS_RETRIES);
		m_maxCommsRetries = EntityHelper::getUnsignedLongData(value);

        return m_maxCommsRetries;
    }

	unsigned long AtsAgentEntityData::getMaxWatchdogInterval()
    {
		std::string value = getHelper()->getParameter(MAX_WATCHDOG_INTERVAL);
		m_maxWatchdogInterval = EntityHelper::getUnsignedLongData(value);

        return m_maxWatchdogInterval;
    }
	
	//td 13947
	//zhou yuan++
	//start the AtsTrainDataTimer for ATC table
	long AtsAgentEntityData::getAtcTimeout()
	{
		std::string value = getHelper()->getParameter(ATC_TIMEOUT);
		m_atcTimeout = EntityHelper::getIntegerData(value);

		return m_atcTimeout;
	}

	long AtsAgentEntityData::getOa1Timeout()
	{
		std::string value = getHelper()->getParameter(OA1_TIMEOUT);
		m_oa1Timeout = EntityHelper::getIntegerData(value);

		return m_oa1Timeout;		
	}
	//++zhou yuan

    TA_Base_Core::IEntityData* AtsAgentEntityData::clone(unsigned long key)
    {
        return( new AtsAgentEntityData(key) );
    }

	long AtsAgentEntityData::getConnectionFailedTimeout()
	{
		std::string value = getHelper()->getParameter(CONNECTION_FAILED_TIMEOUT);
		long connectionTimeout;

		//Use DEFAULT_VALUE if the value is empty
		if (value.empty())
			connectionTimeout = EntityHelper::getIntegerData(DEFAULT_VALUE);
		else
			connectionTimeout = EntityHelper::getIntegerData(value);

		return connectionTimeout;
	}

}
