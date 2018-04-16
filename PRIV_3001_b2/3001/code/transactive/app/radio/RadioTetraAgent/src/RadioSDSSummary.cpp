/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioSDSSummary.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioSDSSummary.h"
#include "core/data_access_interface/entity_access/src/RadioMFTAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;
const std::string RadioSDSSummary::SDTS_HOST = "SDTSHost";
const std::string RadioSDSSummary::SDTS_PORT = "SDTSPort";
const std::string RadioSDSSummary::SDTS_CLIENT_SSI = "SDTSClientSSI";
const std::string RadioSDSSummary::SDTS_PASS_NUMBER = "SDTSPassNumber";
const std::string RadioSDSSummary::MFT_NAME = "MFTName";

const std::string RadioSDSSummary::SDTS_SECONDARY_HOST = "SDTSSecondaryHost";
const std::string RadioSDSSummary::SDTS_SECONDARY_PORT = "SDTSSecondaryPort";
const std::string RadioSDSSummary::SDTS_LINK_DP_NAME = "SDTSLinkDpName";
const std::string RadioSDSSummary::SDTS_MAX_NUM_RETRY = "SDTSMaxNumRetry";

const int SDSMinNumRetry = 1;


RadioSDSSummary::RadioSDSSummary( TA_Base_Core::IEntityDataPtr agentConfiguration)
:	m_runtime(agentConfiguration),
m_SDTSHost(""),
m_SDTSPort(""),
m_SDTSClientSSI(""),
m_SDTSPassNumber(""),
m_MFTName(""),
m_SDTSSecondaryHost(""),
m_SDTSSecondaryPort(""),
m_SDTSLinkDpName("")
{
	FUNCTION_ENTRY("constructor");

	FUNCTION_EXIT;
}

RadioSDSSummary::~RadioSDSSummary()
{
	FUNCTION_ENTRY("destructor");

	FUNCTION_EXIT;
}


bool RadioSDSSummary::loadSDSParameters()
{
    try
    {
        m_SDTSHost = m_runtime.getConfigParameterValue(SDTS_HOST);
        m_SDTSPort = m_runtime.getConfigParameterValue(SDTS_PORT);
        m_SDTSClientSSI = m_runtime.getConfigParameterValue(SDTS_CLIENT_SSI);
        m_SDTSPassNumber = m_runtime.getConfigParameterValue(SDTS_PASS_NUMBER);
		m_MFTName = m_runtime.getConfigParameterValue(MFT_NAME);
		m_SDTSLinkDpName = m_runtime.getConfigParameterValue(SDTS_LINK_DP_NAME);
		m_SDSMaxNumRetry = m_runtime.getConfigParameterValue(SDTS_MAX_NUM_RETRY);

		unsigned long locationKey = getLocation();
		
		//Fixed: C955-RADIO-MultipleDeport
		//Check the location type if it is DEPOT or OCC location based on locationkey
		TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(locationKey);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LocationKey : %d, LocationType : %d", 
			           locationKey, static_cast<int>(locationType));

		//Secondary SDTS is only used by DEPOT or OCC
		if ( (locationType == TA_Base_Core::ILocation::DPT) ||
			 (locationType == TA_Base_Core::ILocation::OCC) )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Location Depot or OCC, so have secondary SDTS Server");
			m_SDTSSecondaryHost = m_runtime.getConfigParameterValue(SDTS_SECONDARY_HOST);
			m_SDTSSecondaryPort = m_runtime.getConfigParameterValue(SDTS_SECONDARY_PORT);
		}

		SDSConsoleData trainData;
		trainData.clientSSI = atoi(m_SDTSClientSSI.c_str()); // trainborne SSI
		trainData.passNumber = atoi(m_SDTSPassNumber.c_str()); // trainborne PassNumber

		m_consoleDataMap["TRN"] = trainData;

		// MFT Console SSI
		//std::vector<TA_Base_Core::IConsole*> consoleList = TA_Base_Core::ConsoleAccessFactory::getInstance().getAllConsoles();
		//for (std::vector<TA_Base_Core::IConsole*>::iterator it = consoleList.begin();
		//     it != consoleList.end(); it++)
		// {
		//	if (((*it)->getLocation() == TA_Base_Core::LocationAccessFactory::getInstance().getOccComLocationKey()) || 
		//	   ((*it)->getLocation() == TA_Base_Core::LocationAccessFactory::getInstance().getDepotSDLocationKey()) ||
		//	   ((*it)->getLocation() == TA_Base_Core::LocationAccessFactory::getInstance().getDepotMDLocationKey()) ||
		//	   ((*it)->getLocation() == TA_Base_Core::LocationAccessFactory::getInstance().getDepotNDLocationKey()))
		//	{
		//		std::string consoleName = (*it)->getAddress();

		//		SDSConsoleData consoleData = getMFTData(consoleName);

		//		if (consoleData.clientSSI != 0)
		//		{
		//			m_consoleDataMap[consoleName] = consoleData;
		//		}
		//	}

		//	delete (*it);
		// }
			 
        return true;
    }
    catch(TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for SDS");
    }
    return false;
}


std::string RadioSDSSummary::getSDTSHost()
{
    return m_SDTSHost;
}

std::string RadioSDSSummary::getSDTSPort()
{
    return m_SDTSPort;
}

unsigned long RadioSDSSummary::getSDTSClientSSI()
{
    return atol(m_SDTSClientSSI.c_str());
}

unsigned long RadioSDSSummary::getSDTSPassNumber()
{
    return atol(m_SDTSPassNumber.c_str());
}


std::string RadioSDSSummary::getSDTSSecondaryHost()
{
    return m_SDTSSecondaryHost;
}

std::string RadioSDSSummary::getSDTSSecondaryPort()
{
    return m_SDTSSecondaryPort;
}


std::string RadioSDSSummary::getMFTName()
{
	return m_MFTName;
}

RadioSDSSummary::SDSConsoleDataMap RadioSDSSummary::getConsoleDataMap()
{
	return m_consoleDataMap;
}

unsigned long RadioSDSSummary::getEntityKey()
{
	return m_runtime.getMyEntityKey();
}

unsigned long RadioSDSSummary::getSubsystem()
{
	return m_runtime.getMyEntitySubsystemKey();
}

unsigned long RadioSDSSummary::getLocation()
{
	return m_runtime.getMyEntityLocationKey();
}

unsigned long RadioSDSSummary::getEntityTypeKey()
{
	return m_runtime.getMyEntityTypeKey();
}

std::string RadioSDSSummary::getEntityName()
{
	return m_runtime.getMyEntityName();
}

std::string RadioSDSSummary::getSDTSLinkDpName()
{
	return m_SDTSLinkDpName;
}

unsigned int RadioSDSSummary::getSDSMaxNumRetry()
{
	int _SDSMaxRetry = atoi(m_SDSMaxNumRetry.c_str());
	if(_SDSMaxRetry < SDSMinNumRetry)
		return SDSMinNumRetry;
	else
		return _SDSMaxRetry;
}

//RadioSDSSummary::SDSConsoleData RadioSDSSummary::getMFTData(const std::string& console)
//{
//	SDSConsoleData consoleData;
//	consoleData.clientSSI = 0;
//	consoleData.passNumber = 0;
//
//	std::string entityName = console + "RadioMFTAgent";
//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get RadioMFTAgent entity = %s",entityName.c_str());
//
//	try
//	{
//	
//		TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
//		
//		TA_Base_Core::RadioMFTAgentEntityData * pData = new TA_Base_Core::RadioMFTAgentEntityData(pEntity->getKey());
//		if (pData != NULL)
//		{
//			consoleData.clientSSI  = pData->getClientSSI();
//			consoleData.passNumber = pData->getPassNumber();
//			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ClientSSI: %d , PassNumber: %d",
//					consoleData.clientSSI,consoleData.passNumber);
//		}
//		delete pData;
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error getting data for entity = %s",entityName.c_str());		
//	}
//
//	return consoleData;
//}
